#pragma once
#include <QDialog>
#include <QCheckBox>
#include <QSpinBox>
#include <QPushButton>
#include "ConfigManager.h"

/**
 * @class SettingsDialog
 * @brief Settings dialog for configuring notification preferences
 *
 * This dialog allows users to configure notification settings, battery thresholds,
 * and other application preferences.
 */
class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(ConfigManager *configManager, QWidget *parent = nullptr);

private slots:
    void saveSettings();
    void restoreDefaults();

private:
    void setupUi();
    void loadSettings();

    ConfigManager *m_configManager;

    // UI elements
    QCheckBox *m_notificationsEnabledCheckBox;
    QCheckBox *m_notifyLowBatteryCheckBox;
    QCheckBox *m_notifyChargingCompleteCheckBox;
    QCheckBox *m_notifyDisconnectCheckBox;
    QSpinBox *m_lowBatteryThresholdSpinBox;
    QPushButton *m_saveButton;
    QPushButton *m_cancelButton;
    QPushButton *m_defaultsButton;
};