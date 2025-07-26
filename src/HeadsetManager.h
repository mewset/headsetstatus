#pragma once
#include <QObject>
#include <QList>
#include "HeadsetDevice.h"

class HeadsetManager : public QObject {
    Q_OBJECT
public:
    explicit HeadsetManager(QObject *parent = nullptr);
    QList<HeadsetDevice> getDevices();

signals:
    void devicesChanged();
}; 