#include "SettingsDialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDialogButtonBox>

SettingsDialog::SettingsDialog(ConfigManager *configManager, QWidget *parent)
    : QDialog(parent)
    , m_configManager(configManager)
{
    setWindowTitle("HeadsetStatus Settings");
    setModal(true);
    setupUi();
    loadSettings();
}

void SettingsDialog::setupUi() {
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    // Notifications group
    QGroupBox *notificationGroup = new QGroupBox("Notifications");
    QVBoxLayout *notificationLayout = new QVBoxLayout();

    m_notificationsEnabledCheckBox = new QCheckBox("Enable notifications");
    notificationLayout->addWidget(m_notificationsEnabledCheckBox);

    m_notifyLowBatteryCheckBox = new QCheckBox("Notify on low battery");
    notificationLayout->addWidget(m_notifyLowBatteryCheckBox);

    m_notifyChargingCompleteCheckBox = new QCheckBox("Notify when charging complete");
    notificationLayout->addWidget(m_notifyChargingCompleteCheckBox);

    m_notifyDisconnectCheckBox = new QCheckBox("Notify on device disconnect");
    notificationLayout->addWidget(m_notifyDisconnectCheckBox);

    // Low battery threshold
    QHBoxLayout *thresholdLayout = new QHBoxLayout();
    QLabel *thresholdLabel = new QLabel("Low battery threshold:");
    m_lowBatteryThresholdSpinBox = new QSpinBox();
    m_lowBatteryThresholdSpinBox->setRange(5, 50);
    m_lowBatteryThresholdSpinBox->setSuffix("%");
    thresholdLayout->addWidget(thresholdLabel);
    thresholdLayout->addWidget(m_lowBatteryThresholdSpinBox);
    thresholdLayout->addStretch();

    notificationLayout->addLayout(thresholdLayout);
    notificationGroup->setLayout(notificationLayout);
    mainLayout->addWidget(notificationGroup);

    // Buttons
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    m_defaultsButton = new QPushButton("Restore Defaults");
    m_cancelButton = new QPushButton("Cancel");
    m_saveButton = new QPushButton("Save");
    m_saveButton->setDefault(true);

    buttonLayout->addWidget(m_defaultsButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(m_cancelButton);
    buttonLayout->addWidget(m_saveButton);

    mainLayout->addLayout(buttonLayout);

    // Connect signals
    connect(m_saveButton, &QPushButton::clicked, this, &SettingsDialog::saveSettings);
    connect(m_cancelButton, &QPushButton::clicked, this, &QDialog::reject);
    connect(m_defaultsButton, &QPushButton::clicked, this, &SettingsDialog::restoreDefaults);

    // Enable/disable notification options based on main checkbox
    connect(m_notificationsEnabledCheckBox, &QCheckBox::toggled, [this](bool enabled) {
        m_notifyLowBatteryCheckBox->setEnabled(enabled);
        m_notifyChargingCompleteCheckBox->setEnabled(enabled);
        m_notifyDisconnectCheckBox->setEnabled(enabled);
        m_lowBatteryThresholdSpinBox->setEnabled(enabled);
    });

    setLayout(mainLayout);
}

void SettingsDialog::loadSettings() {
    m_notificationsEnabledCheckBox->setChecked(m_configManager->notificationsEnabled());
    m_notifyLowBatteryCheckBox->setChecked(m_configManager->notifyOnLowBattery());
    m_notifyChargingCompleteCheckBox->setChecked(m_configManager->notifyOnChargingComplete());
    m_notifyDisconnectCheckBox->setChecked(m_configManager->notifyOnDisconnect());
    m_lowBatteryThresholdSpinBox->setValue(m_configManager->lowBatteryThreshold());

    // Trigger enable/disable logic
    m_notificationsEnabledCheckBox->toggled(m_notificationsEnabledCheckBox->isChecked());
}

void SettingsDialog::saveSettings() {
    m_configManager->beginBatchUpdate();
    m_configManager->setNotificationsEnabled(m_notificationsEnabledCheckBox->isChecked());
    m_configManager->setNotifyOnLowBattery(m_notifyLowBatteryCheckBox->isChecked());
    m_configManager->setNotifyOnChargingComplete(m_notifyChargingCompleteCheckBox->isChecked());
    m_configManager->setNotifyOnDisconnect(m_notifyDisconnectCheckBox->isChecked());
    m_configManager->setLowBatteryThreshold(m_lowBatteryThresholdSpinBox->value());
    m_configManager->endBatchUpdate();

    accept();
}

void SettingsDialog::restoreDefaults() {
    m_notificationsEnabledCheckBox->setChecked(true);
    m_notifyLowBatteryCheckBox->setChecked(true);
    m_notifyChargingCompleteCheckBox->setChecked(true);
    m_notifyDisconnectCheckBox->setChecked(false);
    m_lowBatteryThresholdSpinBox->setValue(20);
}
