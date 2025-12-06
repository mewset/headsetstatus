#include <QtTest/QtTest>
#include "../src/HeadsetManager.h"

/**
 * @class TestHeadsetManager
 * @brief Unit tests for HeadsetManager device detection logic
 */
class TestHeadsetManager : public QObject {
    Q_OBJECT

private:
    HeadsetManager *manager;

private slots:
    void initTestCase() {
        manager = new HeadsetManager(this);
    }

    void cleanupTestCase() {
        // Qt parent-child cleanup handles deletion
    }

    // Test keyword detection for various headset brands
    void testJabraDetection() {
        QVERIFY(manager->isHeadsetDevice("Jabra Evolve2 75", "/some/path"));
        QVERIFY(manager->isHeadsetDevice("JABRA ELITE", "/some/path"));
    }

    void testSonyDetection() {
        QVERIFY(manager->isHeadsetDevice("Sony WH-1000XM5", "/some/path"));
        QVERIFY(manager->isHeadsetDevice("Sony WF-1000XM4", "/some/path"));
    }

    void testBoseDetection() {
        QVERIFY(manager->isHeadsetDevice("Bose QuietComfort 45", "/some/path"));
        QVERIFY(manager->isHeadsetDevice("bose qc35", "/some/path"));
    }

    void testGamingHeadsetDetection() {
        QVERIFY(manager->isHeadsetDevice("HyperX Cloud II", "/some/path"));
        QVERIFY(manager->isHeadsetDevice("SteelSeries Arctis 7", "/some/path"));
        QVERIFY(manager->isHeadsetDevice("Razer Kraken", "/some/path"));
        QVERIFY(manager->isHeadsetDevice("Logitech G Pro X", "/some/path"));
        QVERIFY(manager->isHeadsetDevice("Corsair HS70", "/some/path"));
    }

    void testGenericKeywords() {
        QVERIFY(manager->isHeadsetDevice("Generic Headset", "/some/path"));
        QVERIFY(manager->isHeadsetDevice("Bluetooth Headphone", "/some/path"));
        QVERIFY(manager->isHeadsetDevice("Wireless Earphone", "/some/path"));
        QVERIFY(manager->isHeadsetDevice("TWS Earbud", "/some/path"));
    }

    void testPathBasedDetection() {
        // Should detect from path even if model doesn't match
        QVERIFY(manager->isHeadsetDevice("Unknown Device", "/dev/headset/device1"));
        QVERIFY(manager->isHeadsetDevice("XYZ-123", "/bluetooth/jabra/evolve"));
    }

    void testNonHeadsetDevices() {
        // These should NOT be detected as headsets
        // Note: Logitech is a headset keyword (for Logitech G Pro headsets),
        // so we use brand names not associated with audio
        QVERIFY(!manager->isHeadsetDevice("Dell Keyboard", "/usb/keyboard"));
        QVERIFY(!manager->isHeadsetDevice("Samsung Phone", "/usb/phone"));
        QVERIFY(!manager->isHeadsetDevice("Generic Battery", "/power/battery"));
        QVERIFY(!manager->isHeadsetDevice("USB Hub", "/usb/hub"));
        QVERIFY(!manager->isHeadsetDevice("Microsoft Mouse", "/usb/mouse"));
        QVERIFY(!manager->isHeadsetDevice("Lenovo ThinkPad", "/laptop/battery"));
    }

    void testCaseInsensitivity() {
        QVERIFY(manager->isHeadsetDevice("SONY WH-1000XM5", "/path"));
        QVERIFY(manager->isHeadsetDevice("sony wh-1000xm5", "/path"));
        QVERIFY(manager->isHeadsetDevice("SoNy Wh-1000Xm5", "/path"));
    }

    void testEmptyInputs() {
        QVERIFY(!manager->isHeadsetDevice("", ""));
        QVERIFY(!manager->isHeadsetDevice("", "/some/path"));
        QVERIFY(!manager->isHeadsetDevice("Unknown", ""));
    }

    void testPartialMatches() {
        // Partial keyword matches should work
        QVERIFY(manager->isHeadsetDevice("WH-900N", "/path")); // Sony WH- prefix
        QVERIFY(manager->isHeadsetDevice("WF-SP800N", "/path")); // Sony WF- prefix
        QVERIFY(manager->isHeadsetDevice("QC25", "/path")); // Bose QC prefix
    }
};

QTEST_MAIN(TestHeadsetManager)
#include "test_HeadsetManager.moc"
