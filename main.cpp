#include <QApplication>
#include <QDBusInterface>
#include <QDBusConnection>
#include <QDBusMessage>
#include <QVariant>
#include <QDebug>
#include <QObject>
#include <QDBusReply>
#include <QMessageBox>
#include "src/HeadsetManager.h"
#include "src/TrayIconController.h"

class DBusListener : public QObject {
    Q_OBJECT
public:
    explicit DBusListener(QObject *parent = nullptr) : QObject(parent) {}

signals:
    void headsetStatusChanged();

public slots:
    void propertiesChanged(QString interfaceName, QVariantMap changedProperties, QStringList invalidatedProperties) {
        Q_UNUSED(interfaceName)
        Q_UNUSED(changedProperties)
        Q_UNUSED(invalidatedProperties)

        qDebug() << "PropertiesChanged signal received";
        emit headsetStatusChanged();
    }
};

class HeadsetStatusApp : public QObject {
    Q_OBJECT
public:
    HeadsetStatusApp() {
        headsetManager = new HeadsetManager(this);
        trayController = new TrayIconController(this);
        listener = new DBusListener(this);

        bool connected = QDBusConnection::systemBus().connect(
            QString(), QString(),
            "org.freedesktop.DBus.Properties",
            "PropertiesChanged",
            listener,
            SLOT(propertiesChanged(QString,QVariantMap,QStringList))
        );

        if (!connected) {
            qWarning() << "Failed to connect to D-Bus PropertiesChanged signal";
        }

        connect(listener, &DBusListener::headsetStatusChanged, this, &HeadsetStatusApp::updateStatus);
        connect(trayController, &TrayIconController::informationRequested, this, &HeadsetStatusApp::showInformation);
        connect(trayController, &TrayIconController::aboutRequested, this, &HeadsetStatusApp::showAbout);

        updateStatus();
    }

private slots:
    void updateStatus() {
        QList<HeadsetDevice> devices = headsetManager->getDevices();
        trayController->updateIcon(devices);
    }

    void showInformation() {
        QMessageBox::information(trayController->trayMenu(), "Headset Information", trayController->trayIcon()->toolTip());
    }

    void showAbout() {
        QMessageBox *aboutBox = new QMessageBox(trayController->trayMenu());
        aboutBox->setWindowTitle("About HeadsetStatus");
        aboutBox->setTextFormat(Qt::RichText);
        aboutBox->setText("<b>HeadsetStatus</b><br>"
            "Version 1.0.2<br>"
            "A fast Linux tray app for headset battery and connection status.<br>"
            "<a href='https://github.com/mewset/headsetstatus'>GitHub</a><br>"
            "License: MIT<br><br>"
            "&copy; 2025 mewset");
        aboutBox->setStandardButtons(QMessageBox::Ok);
        
        // Install event filter for Konami code detection
        aboutBox->installEventFilter(trayController);
        aboutBox->exec();
        
        aboutBox->deleteLater();
    }

private:
    HeadsetManager *headsetManager;
    TrayIconController *trayController;
    DBusListener *listener;
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    HeadsetStatusApp headsetStatus;
    return app.exec();
}

#include "main.moc"