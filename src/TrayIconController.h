#pragma once
#include <QObject>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QKeyEvent>
#include <QTimer>
#include <QDesktopServices>
#include <QUrl>
#include "HeadsetDevice.h"

class TrayIconController : public QObject {
    Q_OBJECT
public:
    explicit TrayIconController(QObject *parent = nullptr);
    void updateIcon(const QList<HeadsetDevice>& devices);
    void setTooltip(const QString& text);
    QSystemTrayIcon* trayIcon() const;
    QMenu* trayMenu() const;
    bool eventFilter(QObject *obj, QEvent *event) override;

signals:
    void informationRequested();
    void aboutRequested();

private slots:
    void resetKonamiCode();

private:
    QSystemTrayIcon *m_trayIcon;
    QMenu *m_trayMenu;
    QFont faFont;
    QTimer *konamiTimer;
    QList<int> konamiSequence;
    int konamiIndex;
    void setTrayIconIconText(const QString &text, int deviceCount);
    void checkKonamiCode(int key);
}; 