<p align="center">
  <h1 align="center">HeadsetStatus</h1>
  <p align="center">
    <strong>Lightweight headset battery monitor for Linux</strong>
  </p>
  <p align="center">
    <a href="https://github.com/mewset/headsetstatus/releases"><img src="https://img.shields.io/github/v/release/mewset/headsetstatus?style=flat-square&color=blue" alt="Release"></a>
    <a href="https://aur.archlinux.org/packages/headsetstatus"><img src="https://img.shields.io/aur/version/headsetstatus?style=flat-square&color=1793d1" alt="AUR"></a>
    <a href="https://github.com/mewset/headsetstatus/stargazers"><img src="https://img.shields.io/github/stars/mewset/headsetstatus?style=flat-square&color=yellow" alt="Stars"></a>
    <img src="https://img.shields.io/badge/size-39%20KB-brightgreen?style=flat-square" alt="Binary size">
    <img src="https://img.shields.io/badge/Qt6-41cd52?style=flat-square&logo=qt&logoColor=white" alt="Qt6">
    <img src="https://img.shields.io/badge/C%2B%2B17-00599C?style=flat-square&logo=cplusplus&logoColor=white" alt="C++17">
    <a href="LICENSE"><img src="https://img.shields.io/badge/license-MIT-green?style=flat-square" alt="License"></a>
    <a href="https://ko-fi.com/R6R21I53PD"><img src="https://img.shields.io/badge/Ko--fi-Support-ff5e5b?style=flat-square&logo=ko-fi&logoColor=white" alt="Ko-fi"></a>
  </p>
</p>

---

A fast, minimal system tray app that shows battery and connection status for wireless/USB headsets. Real-time updates via D-Bus, desktop notifications, and only **39 KB**

## Features

| Feature | Description |
|---------|-------------|
| **System Tray** | Native emoji icons with device count badge |
| **Notifications** | Low battery, charging complete, device disconnect |
| **Headless Mode** | Run without tray (`--no-tray`) for servers/scripts |
| **Systemd Service** | Auto-start on login with user service |
| **Multi-Device** | Submenu with individual status per device |
| **Real-time** | Instant updates via D-Bus/UPower |
| **Lightweight** | 39 KB binary, minimal resource usage |
| **Settings GUI** | Configure notification preferences and thresholds |

## Screenshots

<details>
<summary>Click to expand</summary>

*System tray with battery status and multi-device menu*

</details>

## Installation

### Arch Linux (AUR)

```bash
yay -S headsetstatus
```

### From Source

**Dependencies:**
- CMake >= 3.16
- Qt6 (Core, Widgets, DBus)
- UPower
- C++17 compiler

```bash
git clone https://github.com/mewset/headsetstatus.git
cd headsetstatus
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
sudo cmake --install build --prefix /usr
```

<details>
<summary>Build with unit tests</summary>

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
cmake --build build
ctest --test-dir build --output-on-failure
```

</details>

## Usage

```bash
# GUI mode (system tray)
HeadsetStatus

# Headless mode (notifications only)
HeadsetStatus --no-tray

# Show version
HeadsetStatus --version
```

### CLI Options

| Option | Description |
|--------|-------------|
| `-h, --help` | Show help |
| `-v, --version` | Show version |
| `-n, --no-tray` | Headless mode (no system tray) |
| `-d, --debug` | Enable debug output |

## Auto-start

### Systemd (recommended)

```bash
systemctl --user enable --now headsetstatus.service
```

### Hyprland / Sway

```bash
# ~/.config/hypr/hyprland.conf
exec-once = HeadsetStatus

# ~/.config/sway/config
exec HeadsetStatus
```

### XDG Autostart

```bash
cp /usr/share/applications/HeadsetStatus.desktop ~/.config/autostart/
```

## Configuration

Settings are stored in `~/.config/headsetstatus/config.ini`.

Access via tray menu > **Settings**, or edit directly:

```ini
[notifications]
enabled=true
lowBatteryThreshold=20
notifyOnLowBattery=true
notifyOnChargingComplete=true
notifyOnDisconnect=true

[general]
updateInterval=30000
```

## Supported Headsets

Auto-detection for 20+ brands:

> Jabra, Bose, Sony, Sennheiser, JBL, Beats, HyperX, SteelSeries, Razer, Logitech, Corsair, Plantronics, Audio-Technica, Beyerdynamic, AKG, Skullcandy, Anker, AirPods, Galaxy Buds, Pixel Buds, Surface Headphones

Missing your headset? [Open an issue](https://github.com/mewset/headsetstatus/issues).

## Requirements

| Requirement | Notes |
|-------------|-------|
| **Linux** | D-Bus support required |
| **Qt6** | Core, Widgets, DBus modules |
| **UPower** | Battery status provider |
| **Notification daemon** | Optional: libnotify, dunst, mako, swaync |

## Architecture

```
HeadsetStatus/
├── main.cpp              # Application entry, CLI parsing, D-Bus listener
├── src/
│   ├── HeadsetManager    # UPower D-Bus device discovery and filtering
│   ├── TrayIconController# System tray icon, menu, emoji rendering
│   ├── NotificationManager# D-Bus notification sending
│   ├── ConfigManager     # Persistent settings (QSettings)
│   ├── SettingsDialog    # Qt GUI for preferences
│   └── HeadsetDevice     # Device data struct
└── tests/                # Qt Test unit tests
```

### Tech Stack

| Component | Technology |
|-----------|------------|
| Language | C++17 |
| Framework | Qt6 |
| IPC | D-Bus / UPower |
| Build | CMake 3.16+ |
| Size | 39 KB (LTO + UPX) |
| Tests | Qt Test |

## Contributing

PRs welcome! Please:

- Follow existing code style
- Test before submitting
- Use clear commit messages
- Add tests for new functionality

### Development

```bash
# Debug build with tests
cmake -B build -DCMAKE_BUILD_TYPE=Debug -DBUILD_TESTS=ON
cmake --build build

# Run tests
ctest --test-dir build --output-on-failure

# Run with debug output
./build/HeadsetStatus --debug
```

## Changelog

See [CHANGELOG.md](CHANGELOG.md) for version history.

### Recent Changes (v1.2.1)

- Tighter UPower D-Bus filtering + coalesced status updates
- Fallback polling via `general/updateInterval` for missed events
- Batched settings writes (single sync per save)
- Tray update cache to skip redundant redraws

## Support

<a href="https://ko-fi.com/R6R21I53PD"><img src="https://ko-fi.com/img/githubbutton_sm.svg" alt="Support on Ko-fi"></a>

## License

MIT © [mewset](https://github.com/mewset)

---

<p align="center">
  <sub>39 KB of pure battery anxiety relief</sub>
</p>
