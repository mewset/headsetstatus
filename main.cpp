#include <QApplication>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QVariant>
#include <QDebug>
#include <QFontDatabase>
#include <QObject>
#include <QDBusReply>
#include <QPainter>

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

class HeadsetStatusApp : public QObject {
    Q_OBJECT
public:
    HeadsetStatusApp() {
        trayIcon = new QSystemTrayIcon();
        trayMenu = new QMenu();

        QAction *quitAction = new QAction("Quit");
        QObject::connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
        trayMenu->addAction(quitAction);

        trayIcon->setContextMenu(trayMenu);

        int fontId = QFontDatabase::addApplicationFont("/usr/share/fonts/TTF/fa-solid-900.ttf");
    if (fontId == -1) {
        qWarning() << "Could not load Font Awesome font";
    } else {
        QString family = QFontDatabase::applicationFontFamilies(fontId).at(0);
        qDebug() << "Font Awesome loaded with family name:" << family;
        faFont = QFont(family); // Store in faFont
        faFont.setPointSize(16);
        QApplication::setFont(faFont);
    }

        trayIcon->show();

        listener = new DBusListener(this);
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

        connect(listener, &DBusListener::headsetStatusChanged, this, &HeadsetStatusApp::updateStatus);

        updateStatus();
    }

    ~HeadsetStatusApp() {
        trayIcon->hide();
        delete trayIcon;
        delete trayMenu;
    }

private slots:
    void updateStatus() {
        QStringList devices = getHeadsetDevices();
        int deviceCount = devices.size();
        if (devices.isEmpty()) {
            QString tooltip = "No headset found";
            QString iconText = "";
            if (tooltip == lastTooltip && iconText == lastIconText && deviceCount == lastDeviceCount) {
                return;
            }
            lastTooltip = tooltip;
            lastIconText = iconText;
            lastDeviceCount = deviceCount;
            trayIcon->setToolTip(tooltip);
            setTrayIconIconText(iconText, 0);
            return;
        }

        QStringList tooltips;
        QString iconText;
        bool anyWarning = false;
        bool anyCharging = false;
        bool anyUSB = false;
        for (const QString &devicePath : devices) {
            QDBusInterface device("org.freedesktop.UPower", devicePath, "org.freedesktop.UPower.Device", QDBusConnection::systemBus());
            if (!device.isValid()) {
                tooltips << QString("Invalid headset device at %1").arg(devicePath);
                anyWarning = true;
                continue;
            }

            double percentage = device.property("Percentage").toDouble();
            bool isCharging = device.property("IsCharging").toBool();
            bool isPresent = device.property("IsPresent").toBool();
            QString nativePath = device.property("NativePath").toString();
            QString connectionType = nativePath.contains("usb", Qt::CaseInsensitive) ? "USB" : "Bluetooth";
            QString model = device.property("Model").toString();

            QString status;
            if (!isPresent) {
                status = "(Not present)";
                anyWarning = true;
            } else if (percentage < 20) {
                status = QString("%1% (Low)").arg(int(percentage));
                anyWarning = true;
            } else if (isCharging) {
                status = QString("%1% (Charging)").arg(int(percentage));
                anyCharging = true;
            } else {
                status = QString("%1%").arg(int(percentage));
            }
            if (connectionType == "USB") anyUSB = true;
            tooltips << QString("%1\nConnection: %2\nBattery: %3").arg(model).arg(connectionType).arg(status);
        }

        if (anyWarning) {
            iconText = ""; // warning icon
        } else if (anyCharging) {
            iconText = ""; // bolt icon (charging)
        } else if (anyUSB) {
            iconText = ""; // USB icon
        } else {
            iconText = ""; // headphones icon
        }

        QString tooltip = tooltips.join("\n\n");
        if (tooltip == lastTooltip && iconText == lastIconText && deviceCount == lastDeviceCount) {
            return;
        }
        lastTooltip = tooltip;
        lastIconText = iconText;
        lastDeviceCount = deviceCount;
        trayIcon->setToolTip(tooltip);
        setTrayIconIconText(iconText, deviceCount);
    }

private:
    QString lastTooltip;
    QString lastIconText;
    int lastDeviceCount = 0;
    QStringList getHeadsetDevices() {
        QDBusInterface upower("org.freedesktop.UPower", "/org/freedesktop/UPower", "org.freedesktop.UPower", QDBusConnection::systemBus());
        if (!upower.isValid()) {
            // qWarning() << "UPower interface not valid";
            return {};
        }

        QDBusReply<QList<QDBusObjectPath>> reply = upower.call("EnumerateDevices");
        if (!reply.isValid()) {
            // qWarning() << "Failed to enumerate devices";
            return {};
        }

        QStringList headsetDevices;
        for (const QDBusObjectPath &path : reply.value()) {
            QDBusInterface device("org.freedesktop.UPower", path.path(), "org.freedesktop.UPower.Device", QDBusConnection::systemBus());
            if (!device.isValid())
                continue;

            QVariant modelVar = device.property("Model");
            if (!modelVar.isValid())
                continue;

            QString model = modelVar.toString().toLower();
            // qDebug() << "Found device model:" << model << "at path:" << path.path(); // Debug print

            // Match any device containing 'headset' or 'bluetooth' in the model name, or 'headset' in the device path
            if (model.contains("headset") || model.contains("bluetooth") || path.path().contains("headset")) {
                headsetDevices.append(path.path());
            }
        }
        return headsetDevices;
    }

    void setTrayIconIconText(const QString &text, int deviceCount) {
        QSize size(24,24);
        QPixmap pixmap(size);
        pixmap.fill(Qt::transparent);
        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setFont(faFont);
        painter.setPen(Qt::white);
        painter.drawText(pixmap.rect(), Qt::AlignCenter, text);
        // Draw device count if more than 1
        if (deviceCount > 1) {
            QFont countFont = faFont;
            countFont.setPointSize(10);
            painter.setFont(countFont);
            QRect countRect(size.width() - 14, size.height() - 16, 16, 16); // bottom-right
            painter.setPen(Qt::yellow);
            painter.drawText(countRect, Qt::AlignRight | Qt::AlignBottom, QString::number(deviceCount));
        }
        painter.end();
        trayIcon->setIcon(QIcon(pixmap));
    }

    QSystemTrayIcon *trayIcon;
    QMenu *trayMenu;
    DBusListener *listener;
    QFont faFont;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    HeadsetStatusApp headsetStatus;
    return app.exec();
}

#include "main.moc"