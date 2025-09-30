#pragma once
#include <QString>

/**
 * @struct HeadsetDevice
 * @brief Represents a single headset device with its properties
 *
 * This structure holds all relevant information about a connected headset,
 * including battery status, connection type, and system paths.
 */
struct HeadsetDevice {
    QString model;           ///< Device model name
    QString connectionType;  ///< Connection type: "USB" or "Bluetooth"
    double battery = 0.0;    ///< Battery percentage (0-100)
    bool isCharging = false; ///< True if device is currently charging
    bool isPresent = false;  ///< True if device is physically present
    QString nativePath;      ///< System native path (e.g., /sys/...)
    QString dbusPath;        ///< D-Bus object path for this device

    /**
     * @brief Equality operator for device comparison
     */
    bool operator==(const HeadsetDevice& other) const {
        return dbusPath == other.dbusPath;
    }
}; 