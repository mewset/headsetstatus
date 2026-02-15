#include "TrayIconController.h"
#include <QAction>
#include <QPainter>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QKeyEvent>

TrayIconController::TrayIconController(QObject *parent) : QObject(parent), m_devicesMenu(nullptr) {
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayMenu = new QMenu();

    // Devices submenu will be added dynamically in updateIcon()

    QAction *infoAction = new QAction("Information", m_trayMenu);
    QObject::connect(infoAction, &QAction::triggered, this, &TrayIconController::informationRequested);
    m_trayMenu->addAction(infoAction);

    QAction *settingsAction = new QAction("Settings", m_trayMenu);
    QObject::connect(settingsAction, &QAction::triggered, this, &TrayIconController::settingsRequested);
    m_trayMenu->addAction(settingsAction);

    m_trayMenu->addSeparator();

    QAction *aboutAction = new QAction("About", m_trayMenu);
    QObject::connect(aboutAction, &QAction::triggered, this, &TrayIconController::aboutRequested);
    m_trayMenu->addAction(aboutAction);

    QAction *quitAction = new QAction("Quit", m_trayMenu);
    QObject::connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
    m_trayMenu->addAction(quitAction);

    m_trayIcon->setContextMenu(m_trayMenu);

    // Konami code setup
    konamiTimer = new QTimer(this);
    konamiTimer->setSingleShot(true);
    konamiTimer->setInterval(3000); // 3 seconds timeout
    connect(konamiTimer, &QTimer::timeout, this, &TrayIconController::resetKonamiCode);

    // Konami code sequence: ↑↑↓↓←→←→BA
    konamiSequence = {Qt::Key_Up, Qt::Key_Up, Qt::Key_Down, Qt::Key_Down,
                      Qt::Key_Left, Qt::Key_Right, Qt::Key_Left, Qt::Key_Right,
                      Qt::Key_B, Qt::Key_A};
    konamiIndex = 0;

    m_trayIcon->show();
}

bool TrayIconController::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent*>(event);
        checkKonamiCode(keyEvent->key());
    }
    return QObject::eventFilter(obj, event);
}

void TrayIconController::checkKonamiCode(int key) {
    if (konamiIndex < konamiSequence.size() && key == konamiSequence[konamiIndex]) {
        konamiIndex++;
        konamiTimer->start(); // Restart timer
        
        if (konamiIndex == konamiSequence.size()) {
            // Konami code completed!
            QDesktopServices::openUrl(QUrl("https://www.youtube.com/watch?v=t5_eImEdWuQ"));
            resetKonamiCode();
        }
    } else {
        resetKonamiCode();
    }
}

void TrayIconController::resetKonamiCode() {
    konamiIndex = 0;
    konamiTimer->stop();
}

void TrayIconController::updateIcon(const QList<HeadsetDevice>& devices) {
    int deviceCount = devices.size();
    const quint64 devicesSignatureHash = buildDeviceStateSignatureHash(devices);

    if (devicesSignatureHash != m_lastDevicesSignatureHash) {
        rebuildDevicesMenu(devices);
        m_lastDevicesSignatureHash = devicesSignatureHash;
    }

    if (devices.isEmpty()) {
        setTrayIconFromEmoji("", 0);
        setTooltip("No headset found");
        return;
    }

    QStringList tooltips;
    QString emoji;
    bool anyWarning = false;
    bool anyCharging = false;
    bool anyUSB = false;

    for (const HeadsetDevice &device : devices) {
        QString status;
        if (!device.isPresent) {
            status = "(Not present)";
            anyWarning = true;
        } else if (device.battery < m_lowBatteryThreshold) {
            status = QString("%1% (Low)").arg(int(device.battery));
            anyWarning = true;
        } else if (device.isCharging) {
            status = QString("%1% (Charging)").arg(int(device.battery));
            anyCharging = true;
        } else {
            status = QString("%1%").arg(int(device.battery));
        }
        if (device.connectionType == "USB") anyUSB = true;
        tooltips << QString("%1\nConnection: %2\nBattery: %3").arg(device.model).arg(device.connectionType).arg(status);
    }

    // Select appropriate emoji based on device state
    if (anyWarning) {
        emoji = "⚠️";
    } else if (anyCharging) {
        emoji = "⚡";
    } else if (anyUSB) {
        emoji = "🔌";
    } else {
        emoji = "🎧";
    }

    QString tooltip = tooltips.join("\n\n");
    setTooltip(tooltip);
    setTrayIconFromEmoji(emoji, deviceCount);
}

void TrayIconController::setTooltip(const QString& text) {
    if (text == m_lastTooltip) {
        return;
    }

    m_lastTooltip = text;
    m_trayIcon->setToolTip(text);
}

void TrayIconController::setLowBatteryThreshold(int threshold) {
    m_lowBatteryThreshold = qBound(0, threshold, 100);
}

QSystemTrayIcon* TrayIconController::trayIcon() const {
    return m_trayIcon;
}

QMenu* TrayIconController::trayMenu() const {
    return m_trayMenu;
}

void TrayIconController::setTrayIconFromEmoji(const QString &emoji, int deviceCount) {
    if (emoji == m_lastIconEmoji && deviceCount == m_lastDeviceCount) {
        return;
    }

    m_lastIconEmoji = emoji;
    m_lastDeviceCount = deviceCount;

    QSize size(32, 32);
    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);

    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setRenderHint(QPainter::TextAntialiasing);

    // Use system font for emoji rendering
    QFont emojiFont = QFont();
    emojiFont.setPointSize(18);
    painter.setFont(emojiFont);
    painter.setPen(Qt::white);

    // Draw emoji centered
    painter.drawText(pixmap.rect(), Qt::AlignCenter, emoji);

    // Draw device count badge if multiple devices
    if (deviceCount > 1) {
        QFont countFont = QFont();
        countFont.setPointSize(10);
        countFont.setBold(true);
        painter.setFont(countFont);

        QRect countRect(size.width() - 16, size.height() - 14, 16, 14);
        painter.setPen(Qt::yellow);
        painter.drawText(countRect, Qt::AlignRight | Qt::AlignBottom, QString::number(deviceCount));
    }

    painter.end();
    m_trayIcon->setIcon(QIcon(pixmap));
}

quint64 TrayIconController::buildDeviceStateSignatureHash(const QList<HeadsetDevice>& devices) const {
    quint64 aggregate = static_cast<quint64>(devices.size()) * 1099511628211ULL;

    for (const HeadsetDevice& device : devices) {
        const quint64 batteryBucket = static_cast<quint64>(device.battery * 10.0);
        aggregate ^= qHash(device.dbusPath);
        aggregate ^= (static_cast<quint64>(qHash(device.model)) << 1);
        aggregate ^= (batteryBucket << 2);
        aggregate ^= (device.isCharging ? 0x9e3779b185ebca87ULL : 0ULL);
        aggregate ^= (device.isPresent ? 0xc2b2ae3d27d4eb4fULL : 0ULL);
    }

    return aggregate;
}

QString TrayIconController::getDeviceEmoji(const HeadsetDevice& device) const {
    if (!device.isPresent) {
        return "⚠️";
    } else if (device.battery < m_lowBatteryThreshold && !device.isCharging) {
        return "🪫"; // Low battery
    } else if (device.isCharging) {
        return "⚡";
    } else if (device.connectionType == "USB") {
        return "🔌";
    } else {
        return "🎧";
    }
}

void TrayIconController::rebuildDevicesMenu(const QList<HeadsetDevice>& devices) {
    // Remove old devices menu if it exists
    if (m_devicesMenu) {
        m_trayMenu->removeAction(m_devicesMenu->menuAction());
        m_devicesMenu->deleteLater();
        m_devicesMenu = nullptr;
    }

    // Only create devices menu if we have multiple devices
    if (devices.size() > 1) {
        m_devicesMenu = new QMenu("Connected Devices");
        m_devicesMenu->setParent(m_trayMenu);

        for (const HeadsetDevice& device : devices) {
            // Create submenu for each device
            QMenu *deviceSubmenu = m_devicesMenu->addMenu(
                QString("%1 %2").arg(getDeviceEmoji(device)).arg(device.model)
            );

            // Battery status
            QString batteryStatus;
            if (!device.isPresent) {
                batteryStatus = "Not present";
            } else if (device.isCharging) {
                batteryStatus = QString("%1% (Charging)").arg(int(device.battery));
            } else if (device.battery < m_lowBatteryThreshold) {
                batteryStatus = QString("%1% (Low)").arg(int(device.battery));
            } else {
                batteryStatus = QString("%1%").arg(int(device.battery));
            }

            QAction *batteryAction = new QAction(QString("Battery: %1").arg(batteryStatus));
            batteryAction->setEnabled(false);
            deviceSubmenu->addAction(batteryAction);

            // Connection type
            QAction *connectionAction = new QAction(QString("Connection: %1").arg(device.connectionType));
            connectionAction->setEnabled(false);
            deviceSubmenu->addAction(connectionAction);

            // Charging status
            if (device.isPresent) {
                QAction *chargingAction = new QAction(
                    device.isCharging ? "Status: Charging" : "Status: On Battery"
                );
                chargingAction->setEnabled(false);
                deviceSubmenu->addAction(chargingAction);
            }

            // Add separator before action buttons
            deviceSubmenu->addSeparator();

            // Show details action
            QAction *detailsAction = new QAction("Show Details");
            QString dbusPath = device.dbusPath; // Capture by value
            connect(detailsAction, &QAction::triggered, [this, dbusPath]() {
                emit deviceDetailsRequested(dbusPath);
            });
            deviceSubmenu->addAction(detailsAction);
        }

        // Insert devices menu at the top of the menu
        QAction *firstAction = m_trayMenu->actions().first();
        m_trayMenu->insertMenu(firstAction, m_devicesMenu);
        m_trayMenu->insertSeparator(firstAction);
    }
} 
