#pragma once
#include <QObject>
#include <QDBusInterface>
#include <QString>
#include "HeadsetDevice.h"

/**
 * @class NotificationManager
 * @brief Manages desktop notifications for headset events
 *
 * This class handles sending desktop notifications via D-Bus for events such as
 * low battery warnings and charging completion. Supports both libnotify and
 * Wayland notification systems (swaync, mako, dunst).
 */
class NotificationManager : public QObject {
    Q_OBJECT
public:
    explicit NotificationManager(QObject *parent = nullptr);

    /**
     * @brief Sends a low battery notification
     * @param device The headset device with low battery
     */
    void notifyLowBattery(const HeadsetDevice& device);

    /**
     * @brief Sends a charging complete notification
     * @param device The headset device that finished charging
     */
    void notifyChargingComplete(const HeadsetDevice& device);

    /**
     * @brief Sends a device disconnected notification
     * @param device The headset device that was disconnected
     */
    void notifyDeviceDisconnected(const HeadsetDevice& device);

    /**
     * @brief Sets whether notifications are enabled
     * @param enabled True to enable notifications, false to disable
     */
    void setNotificationsEnabled(bool enabled);

    /**
     * @brief Sets the low battery threshold percentage
     * @param threshold Battery percentage threshold (0-100)
     */
    void setLowBatteryThreshold(int threshold);

    bool isNotificationsEnabled() const { return m_notificationsEnabled; }
    int getLowBatteryThreshold() const { return m_lowBatteryThreshold; }

private:
    /**
     * @brief Sends a notification via D-Bus
     * @param summary Notification title
     * @param body Notification body text
     * @param urgency Urgency level (0=low, 1=normal, 2=critical)
     */
    void sendNotification(const QString& summary, const QString& body, int urgency = 1);

    QDBusInterface *m_notificationInterface;
    bool m_notificationsEnabled;
    int m_lowBatteryThreshold;
    QString m_appName;
};