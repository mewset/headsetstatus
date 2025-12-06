#pragma once
#include <QObject>
#include <QList>
#include <QSet>
#include "HeadsetDevice.h"

/**
 * @class HeadsetManager
 * @brief Manages detection and tracking of headset devices via UPower
 *
 * This class queries UPower over D-Bus to discover and monitor connected
 * headset devices, supporting both Bluetooth and USB connections.
 */
class HeadsetManager : public QObject {
    Q_OBJECT
public:
    explicit HeadsetManager(QObject *parent = nullptr);

    /**
     * @brief Retrieves all currently connected headset devices
     * @return List of HeadsetDevice objects representing connected headsets
     */
    QList<HeadsetDevice> getDevices();

    /**
     * @brief Checks if a device model name matches known headset patterns
     * @param model Device model string from UPower
     * @param path Device D-Bus path
     * @return True if device is likely a headset
     */
    bool isHeadsetDevice(const QString& model, const QString& path) const;

signals:
    /**
     * @brief Emitted when the list of connected devices changes
     */
    void devicesChanged();

private:
    // Known headset vendor keywords for improved detection
    static const QSet<QString> s_headsetKeywords;
}; 