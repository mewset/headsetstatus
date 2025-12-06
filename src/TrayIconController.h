#pragma once
#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QKeyEvent>
#include <QTimer>
#include <QDesktopServices>
#include <QUrl>
#include "HeadsetDevice.h"

/**
 * @class TrayIconController
 * @brief Controls the system tray icon and menu for headset status display
 *
 * This class manages the system tray icon, including emoji rendering,
 * tooltip updates, context menu, and easter egg functionality.
 */
class TrayIconController : public QObject {
    Q_OBJECT
public:
    explicit TrayIconController(QObject *parent = nullptr);

    /**
     * @brief Updates the tray icon based on current device status
     * @param devices List of currently connected headset devices
     */
    void updateIcon(const QList<HeadsetDevice>& devices);

    /**
     * @brief Sets the tooltip text for the tray icon
     * @param text Tooltip text to display
     */
    void setTooltip(const QString& text);

    /**
     * @brief Sets the low battery threshold for warning indicators
     * @param threshold Battery percentage threshold (0-100)
     */
    void setLowBatteryThreshold(int threshold);

    QSystemTrayIcon* trayIcon() const;
    QMenu* trayMenu() const;
    bool eventFilter(QObject *obj, QEvent *event) override;

signals:
    void informationRequested();
    void aboutRequested();
    void settingsRequested();
    void deviceDetailsRequested(const QString& dbusPath);

private slots:
    void resetKonamiCode();

private:
    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayMenu;
    QMenu *m_devicesMenu;
    QTimer *konamiTimer;
    int m_lowBatteryThreshold = 20;
    QList<int> konamiSequence;
    int konamiIndex;

    /**
     * @brief Creates a tray icon from emoji text
     * @param emoji Emoji character(s) to display
     * @param deviceCount Number of devices (shows count badge if > 1)
     */
    void setTrayIconFromEmoji(const QString &emoji, int deviceCount);

    /**
     * @brief Rebuilds the devices submenu with current device list
     * @param devices List of connected headset devices
     */
    void rebuildDevicesMenu(const QList<HeadsetDevice>& devices);

    /**
     * @brief Gets emoji for device state
     * @param device Headset device to get emoji for
     * @return Emoji string representing device state
     */
    QString getDeviceEmoji(const HeadsetDevice& device) const;

    void checkKonamiCode(int key);
}; 