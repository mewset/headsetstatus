#include "TrayIconController.h"
#include <QAction>
#include <QFontDatabase>
#include <QPainter>
#include <QMessageBox>
#include <QApplication>
#include <QDesktopServices>
#include <QUrl>
#include <QTimer>
#include <QKeyEvent>

TrayIconController::TrayIconController(QObject *parent) : QObject(parent) {
    m_trayIcon = new QSystemTrayIcon();
    m_trayMenu = new QMenu();

    QAction *infoAction = new QAction("Information");
    QObject::connect(infoAction, &QAction::triggered, this, &TrayIconController::informationRequested);
    m_trayMenu->addAction(infoAction);

    QAction *aboutAction = new QAction("About");
    QObject::connect(aboutAction, &QAction::triggered, this, &TrayIconController::aboutRequested);
    m_trayMenu->addAction(aboutAction);

    QAction *quitAction = new QAction("Quit");
    QObject::connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
    m_trayMenu->addAction(quitAction);

    m_trayIcon->setContextMenu(m_trayMenu);

    int fontId = QFontDatabase::addApplicationFont("/usr/share/fonts/TTF/fa-solid-900.ttf");
    if (fontId == -1) {
        qWarning() << "Could not load Font Awesome font";
    } else {
        QString family = QFontDatabase::applicationFontFamilies(fontId).at(0);
        qDebug() << "Font Awesome loaded with family name:" << family;
        faFont = QFont(family);
        faFont.setPointSize(16);
        QApplication::setFont(faFont);
    }

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
    if (devices.isEmpty()) {
        setTrayIconIconText("", 0);
        setTooltip("No headset found");
        return;
    }

    QStringList tooltips;
    QString iconText;
    bool anyWarning = false;
    bool anyCharging = false;
    bool anyUSB = false;

    for (const HeadsetDevice &device : devices) {
        QString status;
        if (!device.isPresent) {
            status = "(Not present)";
            anyWarning = true;
        } else if (device.battery < 20) {
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

    if (anyWarning) {
        iconText = "⚠"; // warning icon
    } else if (anyCharging) {
        iconText = "⚡"; // bolt icon (charging)
    } else if (anyUSB) {
        iconText = "🔌"; // USB icon
    } else {
        iconText = "🎧"; // headphones icon
    }

    QString tooltip = tooltips.join("\n\n");
    setTooltip(tooltip);
    setTrayIconIconText(iconText, deviceCount);
}

void TrayIconController::setTooltip(const QString& text) {
    m_trayIcon->setToolTip(text);
}

QSystemTrayIcon* TrayIconController::trayIcon() const {
    return m_trayIcon;
}

QMenu* TrayIconController::trayMenu() const {
    return m_trayMenu;
}

void TrayIconController::setTrayIconIconText(const QString &text, int deviceCount) {
    QSize size(24,24);
    QPixmap pixmap(size);
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setFont(faFont);
    painter.setPen(Qt::white);
    painter.drawText(pixmap.rect(), Qt::AlignCenter, text);
    if (deviceCount > 1) {
        QFont countFont = faFont;
        countFont.setPointSize(10);
        painter.setFont(countFont);
        QRect countRect(size.width() - 14, size.height() - 16, 16, 16);
        painter.setPen(Qt::yellow);
        painter.drawText(countRect, Qt::AlignRight | Qt::AlignBottom, QString::number(deviceCount));
    }
    painter.end();
    m_trayIcon->setIcon(QIcon(pixmap));
} 