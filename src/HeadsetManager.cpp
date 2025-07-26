#include "HeadsetManager.h"
#include <QDBusInterface>
#include <QDBusReply>
#include <QVariant>

HeadsetManager::HeadsetManager(QObject *parent) : QObject(parent) {}

QList<HeadsetDevice> HeadsetManager::getDevices() {
    QList<HeadsetDevice> devices;
    QDBusInterface upower("org.freedesktop.UPower", "/org/freedesktop/UPower", "org.freedesktop.UPower", QDBusConnection::systemBus());
    if (!upower.isValid()) {
        return devices;
    }
    QDBusReply<QList<QDBusObjectPath>> reply = upower.call("EnumerateDevices");
    if (!reply.isValid()) {
        return devices;
    }
    for (const QDBusObjectPath &path : reply.value()) {
        QDBusInterface deviceIf("org.freedesktop.UPower", path.path(), "org.freedesktop.UPower.Device", QDBusConnection::systemBus());
        if (!deviceIf.isValid())
            continue;
        QVariant modelVar = deviceIf.property("Model");
        if (!modelVar.isValid())
            continue;
        QString model = modelVar.toString().toLower();
        if (model.contains("headset") || model.contains("bluetooth") || path.path().contains("headset")) {
            HeadsetDevice dev;
            dev.model = deviceIf.property("Model").toString();
            dev.connectionType = deviceIf.property("NativePath").toString().contains("usb", Qt::CaseInsensitive) ? "USB" : "Bluetooth";
            dev.battery = deviceIf.property("Percentage").toDouble();
            dev.isCharging = deviceIf.property("IsCharging").toBool();
            dev.isPresent = deviceIf.property("IsPresent").toBool();
            dev.nativePath = deviceIf.property("NativePath").toString();
            dev.dbusPath = path.path();
            devices.append(dev);
        }
    }
    return devices;
} 