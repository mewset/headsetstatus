#include "HeadsetManager.h"
#include <QDBusInterface>
#include <QDBusReply>
#include <QVariant>

// Known headset vendor and model keywords for better detection
const QSet<QString> HeadsetManager::s_headsetKeywords = {
    "headset", "headphone", "earphone", "earbud",
    "jabra", "bose", "sony", "sennheiser", "jbl", "beats",
    "hyperx", "steelseries", "razer", "logitech", "corsair",
    "plantronics", "poly", "audio-technica", "beyerdynamic",
    "akg", "skullcandy", "anker", "soundcore", "airpods",
    "galaxy buds", "pixel buds", "surface headphones",
    "wh-", "wf-", "qc", "quietcomfort", "evolve"
};

HeadsetManager::HeadsetManager(QObject *parent) : QObject(parent) {}

bool HeadsetManager::isHeadsetDevice(const QString& model, const QString& path) const {
    QString modelLower = model.toLower();
    QString pathLower = path.toLower();

    // Check if model or path contains any headset keywords
    for (const QString& keyword : s_headsetKeywords) {
        if (modelLower.contains(keyword) || pathLower.contains(keyword)) {
            return true;
        }
    }

    return false;
}

QList<HeadsetDevice> HeadsetManager::getDevices() {
    QList<HeadsetDevice> devices;

    // Connect to UPower service
    QDBusInterface upower("org.freedesktop.UPower",
                          "/org/freedesktop/UPower",
                          "org.freedesktop.UPower",
                          QDBusConnection::systemBus());

    if (!upower.isValid()) {
        qWarning() << "Failed to connect to UPower service";
        return devices;
    }

    // Enumerate all power devices
    QDBusReply<QList<QDBusObjectPath>> reply = upower.call("EnumerateDevices");
    if (!reply.isValid()) {
        qWarning() << "Failed to enumerate UPower devices:" << reply.error().message();
        return devices;
    }

    // Check each device to see if it's a headset
    for (const QDBusObjectPath &path : reply.value()) {
        QDBusInterface deviceIf("org.freedesktop.UPower",
                                path.path(),
                                "org.freedesktop.UPower.Device",
                                QDBusConnection::systemBus());

        if (!deviceIf.isValid()) {
            continue;
        }

        // Get device model
        QVariant modelVar = deviceIf.property("Model");
        if (!modelVar.isValid()) {
            continue;
        }

        QString model = modelVar.toString();

        // Check if this device is a headset
        if (isHeadsetDevice(model, path.path())) {
            HeadsetDevice dev;
            dev.model = model;

            // Determine connection type (USB or Bluetooth)
            QString nativePath = deviceIf.property("NativePath").toString();
            dev.connectionType = nativePath.contains("usb", Qt::CaseInsensitive) ? "USB" : "Bluetooth";

            // Get battery information
            dev.battery = deviceIf.property("Percentage").toDouble();
            dev.isCharging = deviceIf.property("IsCharging").toBool();
            dev.isPresent = deviceIf.property("IsPresent").toBool();

            // Store paths for future reference
            dev.nativePath = nativePath;
            dev.dbusPath = path.path();

            devices.append(dev);

        }
    }

    return devices;
} 
