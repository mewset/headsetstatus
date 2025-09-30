#include <QApplication>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QVariant>
#include <QDebug>
#include <QObject>
#include <QDBusReply>
#include <QMessageBox>
#include "version.h"
#include "src/HeadsetManager.h"
#include "src/TrayIconController.h"
#include "src/NotificationManager.h"
#include "src/ConfigManager.h"
#include "src/SettingsDialog.h"

/**
 * @class DBusListener
 * @brief Listens for D-Bus property changes from UPower
 *
 * This class monitors D-Bus signals for property changes on power devices,
 * triggering headset status updates when changes are detected.
 */
class DBusListener : public QObject {
    Q_OBJECT
public:
    explicit DBusListener(QObject *parent = nullptr) : QObject(parent) {}

signals:
    void headsetStatusChanged();

public slots:
    void propertiesChanged(QString interfaceName, QVariantMap changedProperties, QStringList invalidatedProperties) {
        Q_UNUSED(interfaceName)
        Q_UNUSED(changedProperties)
        Q_UNUSED(invalidatedProperties)

        qDebug() << "PropertiesChanged signal received";
        emit headsetStatusChanged();
    }
};

/**
 * @class HeadsetStatusApp
 * @brief Main application class coordinating all components
 *
 * This class integrates the headset manager, tray controller, notification manager,
 * and configuration manager, handling D-Bus signals and user interactions.
 */
class HeadsetStatusApp : public QObject {
    Q_OBJECT
public:
    HeadsetStatusApp() {
        // Initialize managers
        configManager = new ConfigManager(this);
        headsetManager = new HeadsetManager(this);
        trayController = new TrayIconController(this);
        notificationManager = new NotificationManager(this);
        listener = new DBusListener(this);

        // Apply config to notification manager
        notificationManager->setNotificationsEnabled(configManager->notificationsEnabled());
        notificationManager->setLowBatteryThreshold(configManager->lowBatteryThreshold());

        // Connect to D-Bus for property changes
        bool connected = QDBusConnection::systemBus().connect(
            QString(), QString(),
            "org.freedesktop.DBus.Properties",
            "PropertiesChanged",
            listener,
            SLOT(propertiesChanged(QString,QVariantMap,QStringList))
        );

        if (!connected) {
            qWarning() << "Failed to connect to D-Bus PropertiesChanged signal";
        }

        // Connect signals
        connect(listener, &DBusListener::headsetStatusChanged, this, &HeadsetStatusApp::updateStatus);
        connect(trayController, &TrayIconController::informationRequested, this, &HeadsetStatusApp::showInformation);
        connect(trayController, &TrayIconController::settingsRequested, this, &HeadsetStatusApp::showSettings);
        connect(trayController, &TrayIconController::aboutRequested, this, &HeadsetStatusApp::showAbout);
        connect(trayController, &TrayIconController::deviceDetailsRequested, this, &HeadsetStatusApp::showDeviceDetails);

        // Initial status update
        updateStatus();
    }

private slots:
    void updateStatus() {
        QList<HeadsetDevice> currentDevices = headsetManager->getDevices();

        // Check for low battery and send notifications
        if (configManager->notifyOnLowBattery()) {
            for (const HeadsetDevice& device : currentDevices) {
                if (device.isPresent && !device.isCharging &&
                    device.battery <= configManager->lowBatteryThreshold()) {
                    // Check if we haven't notified about this recently
                    if (!m_lowBatteryNotified.contains(device.dbusPath)) {
                        notificationManager->notifyLowBattery(device);
                        m_lowBatteryNotified.insert(device.dbusPath);
                    }
                } else {
                    // Reset notification flag if battery is above threshold
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

        // Update tray icon
        trayController->updateIcon(currentDevices);
    }

    void showInformation() {
        QMessageBox::information(trayController->trayMenu(), "Headset Information", trayController->trayIcon()->toolTip());
    }

    void showSettings() {
        SettingsDialog *dialog = new SettingsDialog(configManager, trayController->trayMenu());
        connect(configManager, &ConfigManager::configChanged, [this]() {
            // Update notification manager when config changes
            notificationManager->setNotificationsEnabled(configManager->notificationsEnabled());
            notificationManager->setLowBatteryThreshold(configManager->lowBatteryThreshold());
        });
        dialog->exec();
        dialog->deleteLater();
    }

    void showDeviceDetails(const QString& dbusPath) {
        // Find device by dbus path
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
            QMessageBox::warning(trayController->trayMenu(), "Device Not Found",
                "The selected device is no longer connected.");
            return;
        }

        // Build detailed information
        QString details;
        details += QString("<b>%1</b><br><br>").arg(targetDevice.model);
        details += QString("<b>Connection Type:</b> %1<br>").arg(targetDevice.connectionType);
        details += QString("<b>Battery Level:</b> %1%<br>").arg(int(targetDevice.battery));
        details += QString("<b>Charging:</b> %1<br>").arg(targetDevice.isCharging ? "Yes" : "No");
        details += QString("<b>Present:</b> %1<br>").arg(targetDevice.isPresent ? "Yes" : "No");
        details += QString("<br><b>Technical Details:</b><br>");
        details += QString("<small>D-Bus Path: %1<br>").arg(targetDevice.dbusPath);
        details += QString("Native Path: %1</small>").arg(targetDevice.nativePath);

        QMessageBox::information(trayController->trayMenu(),
            QString("Device Details: %1").arg(targetDevice.model),
            details);
    }

    void showAbout() {
        QMessageBox *aboutBox = new QMessageBox(trayController->trayMenu());
        aboutBox->setWindowTitle("About HeadsetStatus");
        aboutBox->setTextFormat(Qt::RichText);
        aboutBox->setText(QString("<b>HeadsetStatus</b><br>"
            "Version %1<br>"
            "A fast Linux tray app for headset battery and connection status.<br>"
            "<a href='https://github.com/mewset/headsetstatus'>GitHub</a><br>"
            "License: MIT<br><br>"
            "&copy; 2025 mewset").arg(HEADSETSTATUS_VERSION));
        aboutBox->setStandardButtons(QMessageBox::Ok);

        // Install event filter for Konami code detection
        aboutBox->installEventFilter(trayController);
        aboutBox->exec();

        aboutBox->deleteLater();
    }

private:
    HeadsetManager *headsetManager;
    TrayIconController *trayController;
    NotificationManager *notificationManager;
    ConfigManager *configManager;
    DBusListener *listener;

    // Track notification states
    QSet<QString> m_lowBatteryNotified;
    QSet<QString> m_previouslyCharging;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    HeadsetStatusApp headsetStatus;
    return app.exec();
}

#include "main.moc"