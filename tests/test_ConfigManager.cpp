#include <QtTest/QtTest>
#include <QSignalSpy>
#include <QTemporaryDir>
#include <QSettings>
#include "../src/ConfigManager.h"

/**
 * @class TestConfigManager
 * @brief Unit tests for ConfigManager configuration handling
 *
 * Note: ConfigManager uses the user's real config directory.
 * These tests focus on behavior rather than absolute values,
 * and reset values as needed for each test.
 */
class TestConfigManager : public QObject {
    Q_OBJECT

private:
    ConfigManager *config;

private slots:
    void init() {
        config = new ConfigManager(this);
    }

    void cleanup() {
        delete config;
        config = nullptr;
    }

    // Test setters change values correctly
    void testNotificationsEnabled() {
        QSignalSpy spy(config, &ConfigManager::configChanged);

        bool originalValue = config->notificationsEnabled();
        bool newValue = !originalValue;

        config->setNotificationsEnabled(newValue);
        QCOMPARE(config->notificationsEnabled(), newValue);
        QCOMPARE(spy.count(), 1);

        // Setting same value should not emit signal
        config->setNotificationsEnabled(newValue);
        QCOMPARE(spy.count(), 1);

        // Restore original
        config->setNotificationsEnabled(originalValue);
        QCOMPARE(config->notificationsEnabled(), originalValue);
        QCOMPARE(spy.count(), 2);
    }

    void testLowBatteryThreshold() {
        QSignalSpy spy(config, &ConfigManager::configChanged);
        int originalValue = config->lowBatteryThreshold();

        // Set to a known different value
        int testValue = (originalValue == 50) ? 60 : 50;
        config->setLowBatteryThreshold(testValue);
        QCOMPARE(config->lowBatteryThreshold(), testValue);
        QCOMPARE(spy.count(), 1);

        // Restore
        config->setLowBatteryThreshold(originalValue);
    }

    void testLowBatteryThresholdBoundaries() {
        int originalValue = config->lowBatteryThreshold();

        // Test boundary values - these should work
        config->setLowBatteryThreshold(0);
        QCOMPARE(config->lowBatteryThreshold(), 0);

        config->setLowBatteryThreshold(100);
        QCOMPARE(config->lowBatteryThreshold(), 100);

        // Restore
        config->setLowBatteryThreshold(originalValue);
    }

    void testLowBatteryThresholdValidation() {
        int originalValue = config->lowBatteryThreshold();

        // Set a valid value first
        config->setLowBatteryThreshold(50);
        QCOMPARE(config->lowBatteryThreshold(), 50);

        // Invalid values should be rejected
        config->setLowBatteryThreshold(-1);
        QCOMPARE(config->lowBatteryThreshold(), 50); // Should stay 50

        config->setLowBatteryThreshold(101);
        QCOMPARE(config->lowBatteryThreshold(), 50); // Should stay 50

        // Restore
        config->setLowBatteryThreshold(originalValue);
    }

    void testNotifyOnLowBattery() {
        QSignalSpy spy(config, &ConfigManager::configChanged);
        bool originalValue = config->notifyOnLowBattery();
        bool newValue = !originalValue;

        config->setNotifyOnLowBattery(newValue);
        QCOMPARE(config->notifyOnLowBattery(), newValue);
        QCOMPARE(spy.count(), 1);

        // Restore
        config->setNotifyOnLowBattery(originalValue);
    }

    void testNotifyOnChargingComplete() {
        QSignalSpy spy(config, &ConfigManager::configChanged);
        bool originalValue = config->notifyOnChargingComplete();
        bool newValue = !originalValue;

        config->setNotifyOnChargingComplete(newValue);
        QCOMPARE(config->notifyOnChargingComplete(), newValue);
        QCOMPARE(spy.count(), 1);

        // Restore
        config->setNotifyOnChargingComplete(originalValue);
    }

    void testNotifyOnDisconnect() {
        QSignalSpy spy(config, &ConfigManager::configChanged);
        bool originalValue = config->notifyOnDisconnect();
        bool newValue = !originalValue;

        config->setNotifyOnDisconnect(newValue);
        QCOMPARE(config->notifyOnDisconnect(), newValue);
        QCOMPARE(spy.count(), 1);

        // Restore
        config->setNotifyOnDisconnect(originalValue);
    }

    void testUpdateInterval() {
        QSignalSpy spy(config, &ConfigManager::configChanged);
        int originalValue = config->updateInterval();

        // Set to a known different value
        int testValue = (originalValue == 10000) ? 5000 : 10000;
        config->setUpdateInterval(testValue);
        QCOMPARE(config->updateInterval(), testValue);
        QCOMPARE(spy.count(), 1);

        // Restore
        config->setUpdateInterval(originalValue);
    }

    void testUpdateIntervalValidation() {
        int originalValue = config->updateInterval();

        // Set valid value
        config->setUpdateInterval(5000);

        // Zero or negative should be rejected
        config->setUpdateInterval(0);
        QCOMPARE(config->updateInterval(), 5000);

        config->setUpdateInterval(-100);
        QCOMPARE(config->updateInterval(), 5000);

        // Restore
        config->setUpdateInterval(originalValue);
    }

    void testNoSignalOnSameValue() {
        QSignalSpy spy(config, &ConfigManager::configChanged);

        // Get current value and set it again
        bool current = config->notificationsEnabled();
        config->setNotificationsEnabled(current);

        // Should not emit signal when value doesn't change
        QCOMPARE(spy.count(), 0);
    }

    void testConfigPersistence() {
        int originalThreshold = config->lowBatteryThreshold();
        int testValue = (originalThreshold == 35) ? 45 : 35;

        // Change value
        config->setLowBatteryThreshold(testValue);

        // Create new config instance - should load saved value
        ConfigManager *config2 = new ConfigManager(this);
        QCOMPARE(config2->lowBatteryThreshold(), testValue);
        delete config2;

        // Restore original
        config->setLowBatteryThreshold(originalThreshold);
    }

    void testMultipleChangesEmitMultipleSignals() {
        // Get original values
        int origThreshold = config->lowBatteryThreshold();
        int origInterval = config->updateInterval();

        QSignalSpy spy(config, &ConfigManager::configChanged);

        // Each setter that changes a value should emit exactly one signal
        int newThreshold = (origThreshold == 25) ? 35 : 25;
        int newInterval = (origInterval == 3000) ? 4000 : 3000;

        config->setLowBatteryThreshold(newThreshold);
        QCOMPARE(spy.count(), 1);

        config->setUpdateInterval(newInterval);
        QCOMPARE(spy.count(), 2);

        // Same values should not emit
        config->setLowBatteryThreshold(newThreshold);
        config->setUpdateInterval(newInterval);
        QCOMPARE(spy.count(), 2);

        // Restore
        config->setLowBatteryThreshold(origThreshold);
        config->setUpdateInterval(origInterval);
    }
};

QTEST_MAIN(TestConfigManager)
#include "test_ConfigManager.moc"
