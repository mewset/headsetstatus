#include <QApplication>
#include <QCommandLineParser>
#include <QtDBus>
#include <QDebug>
#include <QMessageBox>
#include <QTimer>
#include "version.h"
#include "src/HeadsetManager.h"
#include "src/TrayIconController.h"
#include "src/NotificationManager.h"
#include "src/ConfigManager.h"
#include "src/SettingsDialog.h"

/**
 * @class DBusListener
 * @brief Listens for D-Bus property changes from UPower
 */
class DBusListener : public QObject {
    Q_OBJECT
public:
    explicit DBusListener(QObject *parent = nullptr) : QObject(parent) {}

signals:
    void statusRelevantEvent();

public slots:
    void propertiesChanged(const QString& interfaceName,
                           const QVariantMap& changedProperties,
                           const QStringList& invalidatedProperties) {
        Q_UNUSED(invalidatedProperties)

        if (interfaceName != "org.freedesktop.UPower.Device") {
            return;
        }

        if (changedProperties.contains("Percentage") ||
            changedProperties.contains("IsCharging") ||
            changedProperties.contains("IsPresent")) {
            emit statusRelevantEvent();
        }
    }

    void deviceAdded(const QDBusObjectPath&) { emit statusRelevantEvent(); }
    void deviceRemoved(const QDBusObjectPath&) { emit statusRelevantEvent(); }
};

/**
 * @class HeadsetStatusApp
 * @brief Main application class coordinating all components
 *
 * Supports both GUI mode (system tray) and headless mode (notifications only).
 */
class HeadsetStatusApp : public QObject {
    Q_OBJECT
public:
    explicit HeadsetStatusApp(bool headless = false, bool debug = false)
        : m_headless(headless), m_debug(debug) {

        // Initialize managers
        configManager = new ConfigManager(this);
        headsetManager = new HeadsetManager(this);
        notificationManager = new NotificationManager(this);
        listener = new DBusListener(this);

        // Apply config to notification manager
        notificationManager->setNotificationsEnabled(configManager->notificationsEnabled());
        notificationManager->setLowBatteryThreshold(configManager->lowBatteryThreshold());

        // Only create tray controller in GUI mode
        if (!m_headless) {
            trayController = new TrayIconController(this);
            trayController->setLowBatteryThreshold(configManager->lowBatteryThreshold());

            // Connect tray signals
            connect(trayController, &TrayIconController::informationRequested, this, &HeadsetStatusApp::showInformation);
            connect(trayController, &TrayIconController::settingsRequested, this, &HeadsetStatusApp::showSettings);
            connect(trayController, &TrayIconController::aboutRequested, this, &HeadsetStatusApp::showAbout);
            connect(trayController, &TrayIconController::deviceDetailsRequested, this, &HeadsetStatusApp::showDeviceDetails);
        }

        // Connect to D-Bus for property changes
        bool connected = QDBusConnection::systemBus().connect(
            "org.freedesktop.UPower", QString(),
            "org.freedesktop.DBus.Properties",
            "PropertiesChanged",
            listener,
            SLOT(propertiesChanged(QString,QVariantMap,QStringList))
        );

        if (!connected) {
            qWarning() << "Failed to connect to D-Bus PropertiesChanged signal";
        }

        bool addedConnected = QDBusConnection::systemBus().connect(
            "org.freedesktop.UPower", "/org/freedesktop/UPower",
            "org.freedesktop.UPower", "DeviceAdded",
            listener, SLOT(deviceAdded(QDBusObjectPath))
        );

        if (!addedConnected) {
            qWarning() << "Failed to connect to UPower DeviceAdded signal";
        }

        bool removedConnected = QDBusConnection::systemBus().connect(
            "org.freedesktop.UPower", "/org/freedesktop/UPower",
            "org.freedesktop.UPower", "DeviceRemoved",
            listener, SLOT(deviceRemoved(QDBusObjectPath))
        );

        if (!removedConnected) {
            qWarning() << "Failed to connect to UPower DeviceRemoved signal";
        }

        m_updateDebounceTimer = new QTimer(this);
        m_updateDebounceTimer->setSingleShot(true);
        m_updateDebounceTimer->setInterval(120);
        connect(m_updateDebounceTimer, &QTimer::timeout, this, &HeadsetStatusApp::updateStatus);

        m_fallbackPollTimer = new QTimer(this);
        m_fallbackPollTimer->setSingleShot(false);
        connect(m_fallbackPollTimer, &QTimer::timeout, this, &HeadsetStatusApp::scheduleStatusUpdate);
        applyPollingInterval(configManager->updateInterval());

        // Connect signals
        connect(listener, &DBusListener::statusRelevantEvent, this, &HeadsetStatusApp::scheduleStatusUpdate);
        connect(configManager, &ConfigManager::configChanged, this, &HeadsetStatusApp::onConfigChanged);

        if (m_debug) {
            qDebug() << "HeadsetStatus started in" << (m_headless ? "headless" : "GUI") << "mode";
        }

        // Initial status update
        updateStatus();
    }

private slots:
    void scheduleStatusUpdate() {
        if (!m_updateDebounceTimer->isActive()) {
            m_updateDebounceTimer->start();
        }
    }

    void updateStatus() {
        QList<HeadsetDevice> currentDevices = headsetManager->getDevices();
        QSet<QString> currentPaths;
        currentPaths.reserve(currentDevices.size());

        for (const HeadsetDevice& device : currentDevices) {
            currentPaths.insert(device.dbusPath);
        }

        if (m_debug) {
            qDebug() << "Status update: found" << currentDevices.size() << "devices";
        }

        // Check for disconnected devices
        if (configManager->notifyOnDisconnect()) {
            for (auto it = m_knownDevices.constBegin(); it != m_knownDevices.constEnd(); ++it) {
                if (!currentPaths.contains(it.key())) {
                    notificationManager->notifyDeviceDisconnected(it.value());
                }
            }
        }

        for (auto it = m_lowBatteryNotified.begin(); it != m_lowBatteryNotified.end();) {
            if (!currentPaths.contains(*it)) {
                it = m_lowBatteryNotified.erase(it);
            } else {
                ++it;
            }
        }

        for (auto it = m_previouslyCharging.begin(); it != m_previouslyCharging.end();) {
            if (!currentPaths.contains(*it)) {
                it = m_previouslyCharging.erase(it);
            } else {
                ++it;
            }
        }

        // Update device cache
        m_knownDevices.clear();
        m_knownDevices.reserve(currentDevices.size());
        for (const HeadsetDevice& device : currentDevices) {
            m_knownDevices[device.dbusPath] = device;
        }

        // Check for low battery and send notifications
        if (configManager->notifyOnLowBattery()) {
            for (const HeadsetDevice& device : currentDevices) {
                if (device.isPresent && !device.isCharging &&
                    device.battery <= configManager->lowBatteryThreshold()) {
                    if (!m_lowBatteryNotified.contains(device.dbusPath)) {
                        notificationManager->notifyLowBattery(device);
                        m_lowBatteryNotified.insert(device.dbusPath);
                    }
                } else {
                    m_lowBatteryNotified.remove(device.dbusPath);
                }
            }
        }

        // Check for charging complete
        if (configManager->notifyOnChargingComplete()) {
            for (const HeadsetDevice& device : currentDevices) {
                if (device.battery >= 95 && !device.isCharging) {
                    if (m_previouslyCharging.contains(device.dbusPath)) {
                        notificationManager->notifyChargingComplete(device);
                        m_previouslyCharging.remove(device.dbusPath);
                    }
                } else if (device.isCharging) {
                    m_previouslyCharging.insert(device.dbusPath);
                }
            }
        }

        // Update tray icon (GUI mode only)
        if (trayController) {
            trayController->updateIcon(currentDevices);
        }
    }

    void showInformation() {
        if (trayController) {
            QMessageBox::information(nullptr, "Headset Information",
                trayController->trayIcon()->toolTip());
        }
    }

    void onConfigChanged() {
        notificationManager->setNotificationsEnabled(configManager->notificationsEnabled());
        notificationManager->setLowBatteryThreshold(configManager->lowBatteryThreshold());
        applyPollingInterval(configManager->updateInterval());
        if (trayController) {
            trayController->setLowBatteryThreshold(configManager->lowBatteryThreshold());
        }
    }

    void applyPollingInterval(int intervalMs) {
        if (intervalMs <= 0) {
            m_fallbackPollTimer->stop();
            return;
        }

        if (m_fallbackPollTimer->interval() != intervalMs) {
            m_fallbackPollTimer->setInterval(intervalMs);
        }

        if (!m_fallbackPollTimer->isActive()) {
            m_fallbackPollTimer->start();
        }
    }

    void showSettings() {
        SettingsDialog dialog(configManager, nullptr);
        dialog.exec();
    }

    void showDeviceDetails(const QString& dbusPath) {
        QList<HeadsetDevice> devices = headsetManager->getDevices();
        HeadsetDevice targetDevice;
        bool found = false;

        for (const HeadsetDevice& device : devices) {
            if (device.dbusPath == dbusPath) {
                targetDevice = device;
                found = true;
                break;
            }
        }

        if (!found) {
            QMessageBox::warning(nullptr, "Device Not Found",
                "The selected device is no longer connected.");
            return;
        }

        QString details = QString(
            "<b>%1</b><br><br>"
            "<b>Connection Type:</b> %2<br>"
            "<b>Battery Level:</b> %3%<br>"
            "<b>Charging:</b> %4<br>"
            "<b>Present:</b> %5<br>"
            "<br><b>Technical Details:</b><br>"
            "<small>D-Bus Path: %6<br>"
            "Native Path: %7</small>")
            .arg(targetDevice.model)
            .arg(targetDevice.connectionType)
            .arg(int(targetDevice.battery))
            .arg(targetDevice.isCharging ? "Yes" : "No")
            .arg(targetDevice.isPresent ? "Yes" : "No")
            .arg(targetDevice.dbusPath)
            .arg(targetDevice.nativePath);

        QMessageBox::information(nullptr,
            QString("Device Details: %1").arg(targetDevice.model),
            details);
    }

    void showAbout() {
        QMessageBox aboutBox;
        aboutBox.setWindowTitle("About HeadsetStatus");
        aboutBox.setTextFormat(Qt::RichText);
        aboutBox.setText(QString("<b>HeadsetStatus</b><br>"
            "Version %1<br>"
            "A fast Linux tray app for headset battery and connection status.<br>"
            "<a href='https://github.com/mewset/headsetstatus'>GitHub</a><br>"
            "License: MIT<br><br>"
            "&copy; 2025 mewset").arg(HEADSETSTATUS_VERSION));
        aboutBox.setStandardButtons(QMessageBox::Ok);

        if (trayController) {
            aboutBox.installEventFilter(trayController);
        }
        aboutBox.exec();
    }

private:
    bool m_headless;
    bool m_debug;
    HeadsetManager *headsetManager;
    TrayIconController *trayController = nullptr;
    NotificationManager *notificationManager;
    ConfigManager *configManager;
    DBusListener *listener;
    QTimer *m_updateDebounceTimer = nullptr;
    QTimer *m_fallbackPollTimer = nullptr;

    // Track device and notification states
    QHash<QString, HeadsetDevice> m_knownDevices;
    QSet<QString> m_lowBatteryNotified;
    QSet<QString> m_previouslyCharging;

};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    app.setApplicationName("HeadsetStatus");
    app.setApplicationVersion(HEADSETSTATUS_VERSION);
    app.setOrganizationName("mewset");

    QCommandLineParser parser;
    parser.setApplicationDescription("Headset battery status monitor for Linux");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption noTrayOption(
        QStringList() << "n" << "no-tray",
        "Run in headless mode without system tray icon");
    parser.addOption(noTrayOption);

    QCommandLineOption debugOption(
        QStringList() << "d" << "debug",
        "Enable debug output");
    parser.addOption(debugOption);

    parser.process(app);

    bool headless = parser.isSet(noTrayOption);
    bool debug = parser.isSet(debugOption);

    if (debug) {
        qDebug() << "HeadsetStatus" << HEADSETSTATUS_VERSION;
    }

    HeadsetStatusApp headsetStatus(headless, debug);
    return app.exec();
}

#include "main.moc"
