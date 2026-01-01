#include "ConfigManager.h"
#include <QStandardPaths>
#include <QDir>
#include <QDebug>

ConfigManager::ConfigManager(QObject *parent)
    : QObject(parent)
    , m_notificationsEnabled(true)
    , m_lowBatteryThreshold(20)
    , m_notifyOnLowBattery(true)
    , m_notifyOnChargingComplete(true)
    , m_notifyOnDisconnect(false)
    , m_updateInterval(5000) // 5 seconds default
{
    // Set up config file path: ~/.config/headsetstatus/config.ini
    QString configPath = QStandardPaths::writableLocation(QStandardPaths::ConfigLocation);
    QString appConfigDir = configPath + "/headsetstatus";

    // Create directory if it doesn't exist
    QDir dir;
    if (!dir.exists(appConfigDir)) {
        dir.mkpath(appConfigDir);
    }

    m_settings = new QSettings(appConfigDir + "/config.ini", QSettings::IniFormat, this);
    load();
}

void ConfigManager::load() {
    m_notificationsEnabled = m_settings->value("notifications/enabled", true).toBool();
    m_lowBatteryThreshold = m_settings->value("notifications/lowBatteryThreshold", 20).toInt();
    m_notifyOnLowBattery = m_settings->value("notifications/notifyOnLowBattery", true).toBool();
    m_notifyOnChargingComplete = m_settings->value("notifications/notifyOnChargingComplete", true).toBool();
    m_notifyOnDisconnect = m_settings->value("notifications/notifyOnDisconnect", false).toBool();
    m_updateInterval = m_settings->value("general/updateInterval", 5000).toInt();

    qDebug() << "Configuration loaded from:" << m_settings->fileName();
}

void ConfigManager::save() {
    m_settings->setValue("notifications/enabled", m_notificationsEnabled);
    m_settings->setValue("notifications/lowBatteryThreshold", m_lowBatteryThreshold);
    m_settings->setValue("notifications/notifyOnLowBattery", m_notifyOnLowBattery);
    m_settings->setValue("notifications/notifyOnChargingComplete", m_notifyOnChargingComplete);
    m_settings->setValue("notifications/notifyOnDisconnect", m_notifyOnDisconnect);
    m_settings->setValue("general/updateInterval", m_updateInterval);

    m_settings->sync();
    qDebug() << "Configuration saved to:" << m_settings->fileName();

    emit configChanged();
}

void ConfigManager::setNotificationsEnabled(bool enabled) {
    if (m_notificationsEnabled != enabled) {
        m_notificationsEnabled = enabled;
        save();
    }
}

void ConfigManager::setLowBatteryThreshold(int threshold) {
    if (threshold >= 0 && threshold <= 100 && m_lowBatteryThreshold != threshold) {
        m_lowBatteryThreshold = threshold;
        save();
    }
}

void ConfigManager::setNotifyOnLowBattery(bool notify) {
    if (m_notifyOnLowBattery != notify) {
        m_notifyOnLowBattery = notify;
        save();
    }
}

void ConfigManager::setNotifyOnChargingComplete(bool notify) {
    if (m_notifyOnChargingComplete != notify) {
        m_notifyOnChargingComplete = notify;
        save();
    }
}

void ConfigManager::setNotifyOnDisconnect(bool notify) {
    if (m_notifyOnDisconnect != notify) {
        m_notifyOnDisconnect = notify;
        save();
    }
}

void ConfigManager::setUpdateInterval(int interval) {
    if (interval > 0 && m_updateInterval != interval) {
        m_updateInterval = interval;
        save();
    }
}