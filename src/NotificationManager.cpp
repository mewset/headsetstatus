#include "NotificationManager.h"
#include <QDBusConnection>
#include <QDBusMessage>
#include <QVariantList>
#include <QVariantMap>
#include <QDebug>

NotificationManager::NotificationManager(QObject *parent)
    : QObject(parent)
    , m_notificationsEnabled(true)
    , m_lowBatteryThreshold(20)
    , m_appName("HeadsetStatus")
{
    // Connect to the freedesktop notifications service
    m_notificationInterface = new QDBusInterface(
        "org.freedesktop.Notifications",
        "/org/freedesktop/Notifications",
        "org.freedesktop.Notifications",
        QDBusConnection::sessionBus(),
        this
    );

    if (!m_notificationInterface->isValid()) {
        qWarning() << "Failed to connect to notification service:"
                   << m_notificationInterface->lastError().message();
    }
}

void NotificationManager::sendNotification(const QString& summary, const QString& body, int urgency) {
    if (!m_notificationsEnabled || !m_notificationInterface->isValid()) {
        return;
    }

    // Prepare notification parameters
    QVariantList args;
    args << m_appName;              // app_name
    args << uint(0);                // replaces_id
    args << QString("audio-headset"); // app_icon
    args << summary;                // summary
    args << body;                   // body
    args << QStringList();          // actions

    // Hints for urgency level
    QVariantMap hints;
    hints["urgency"] = urgency;
    args << hints;

    args << int(5000);              // timeout (5 seconds)

    // Send notification via D-Bus
    QDBusMessage reply = m_notificationInterface->callWithArgumentList(
        QDBus::NoBlock,
        "Notify",
        args
    );

    if (reply.type() == QDBusMessage::ErrorMessage) {
        qWarning() << "Failed to send notification:" << reply.errorMessage();
    }
}

void NotificationManager::notifyLowBattery(const HeadsetDevice& device) {
    if (!m_notificationsEnabled) {
        return;
    }

    QString summary = QString("Low Battery: %1").arg(device.model);
    QString body = QString("Battery level is at %1%. Please charge soon.")
                       .arg(int(device.battery));

    sendNotification(summary, body, 2); // Critical urgency
}

void NotificationManager::notifyChargingComplete(const HeadsetDevice& device) {
    if (!m_notificationsEnabled) {
        return;
    }

    QString summary = QString("Charging Complete: %1").arg(device.model);
    QString body = QString("Battery is now at %1%.")
                       .arg(int(device.battery));

    sendNotification(summary, body, 1); // Normal urgency
}

void NotificationManager::notifyDeviceDisconnected(const HeadsetDevice& device) {
    if (!m_notificationsEnabled) {
        return;
    }

    QString summary = QString("Headset Disconnected");
    QString body = QString("%1 has been disconnected.").arg(device.model);

    sendNotification(summary, body, 1); // Normal urgency
}

void NotificationManager::setNotificationsEnabled(bool enabled) {
    m_notificationsEnabled = enabled;
}

void NotificationManager::setLowBatteryThreshold(int threshold) {
    if (threshold >= 0 && threshold <= 100) {
        m_lowBatteryThreshold = threshold;
    }
}