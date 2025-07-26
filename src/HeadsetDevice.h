#pragma once
#include <QString>

class HeadsetDevice {
public:
    QString model;
    QString connectionType;
    double battery = 0.0;
    bool isCharging = false;
    bool isPresent = false;
    QString nativePath;
    QString dbusPath;
}; 