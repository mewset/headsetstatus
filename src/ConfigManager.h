#pragma once
#include <QObject>
#include <QSettings>
#include <QString>

/**
 * @class ConfigManager
 * @brief Manages persistent configuration settings
 *
 * This class handles loading and saving user preferences to a configuration file
 * located at ~/.config/headsetstatus/config.ini
 */
class ConfigManager : public QObject {
    Q_OBJECT
public:
    explicit ConfigManager(QObject *parent = nullptr);

    /**
     * @brief Loads configuration from disk
     */
    void load();

    /**
     * @brief Saves current configuration to disk
     */
    void save();

    // Getters
    bool notificationsEnabled() const { return m_notificationsEnabled; }
    int lowBatteryThreshold() const { return m_lowBatteryThreshold; }
    bool notifyOnLowBattery() const { return m_notifyOnLowBattery; }
    bool notifyOnChargingComplete() const { return m_notifyOnChargingComplete; }
    bool notifyOnDisconnect() const { return m_notifyOnDisconnect; }
    int updateInterval() const { return m_updateInterval; }

    // Setters
    void setNotificationsEnabled(bool enabled);
    void setLowBatteryThreshold(int threshold);
    void setNotifyOnLowBattery(bool notify);
    void setNotifyOnChargingComplete(bool notify);
    void setNotifyOnDisconnect(bool notify);
    void setUpdateInterval(int interval);

signals:
    void configChanged();

private:
    QSettings *m_settings;

    // Configuration values
    bool m_notificationsEnabled;
    int m_lowBatteryThreshold;
    bool m_notifyOnLowBattery;
    bool m_notifyOnChargingComplete;
    bool m_notifyOnDisconnect;
    int m_updateInterval; // in milliseconds
};