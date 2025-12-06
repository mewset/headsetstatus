<p align="center">
  <h1 align="center">🎧 HeadsetStatus</h1>
  <p align="center">
    <strong>Lightweight headset battery monitor for Linux</strong>
  </p>
  <p align="center">
    <a href="https://github.com/mewset/headsetstatus/releases"><img src="https://img.shields.io/github/v/release/mewset/headsetstatus?style=flat-square" alt="Release"></a>
    <a href="https://aur.archlinux.org/packages/headsetstatus"><img src="https://img.shields.io/aur/version/headsetstatus?style=flat-square" alt="AUR"></a>
    <img src="https://img.shields.io/badge/size-39%20KB-brightgreen?style=flat-square" alt="Binary size">
    <img src="https://img.shields.io/badge/Qt6-blue?style=flat-square" alt="Qt6">
    <img src="https://img.shields.io/badge/license-MIT-green?style=flat-square" alt="License">
  </p>
</p>

---

A fast, minimal system tray app that shows battery and connection status for wireless/USB headsets. Real-time updates via D-Bus, desktop notifications, and only **39 KB** thanks to demoscene-inspired optimizations.

## Features

| Feature | Description |
|---------|-------------|
| 🎧 **System Tray** | Native emoji icons (🎧⚡⚠️🔌🪫) with device count badge |
| 🔔 **Notifications** | Low battery, charging complete, device disconnect |
| 👻 **Headless Mode** | Run without tray (`--no-tray`) for servers/scripts |
| ⚙️ **Systemd Service** | Auto-start on login with user service |
| 📱 **Multi-Device** | Submenu with individual status per device |
| ⚡ **Real-time** | Instant updates via D-Bus/UPower |
| 🪶 **Lightweight** | 39 KB binary, minimal resource usage |

## Installation

### Arch Linux (AUR)

```bash
yay -S headsetstatus
```

### From Source

```bash
git clone https://github.com/mewset/headsetstatus.git
cd headsetstatus
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
sudo cmake --install build --prefix /usr
```

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

Access via tray menu → **Settings**, or edit directly:

```ini
[notifications]
enabled=true
lowBatteryThreshold=20
notifyOnLowBattery=true
notifyOnChargingComplete=true
notifyOnDisconnect=true
```

## Supported Headsets

Auto-detection for 20+ brands:

> Jabra, Bose, Sony, Sennheiser, JBL, Beats, HyperX, SteelSeries, Razer, Logitech, Corsair, Plantronics, Audio-Technica, Beyerdynamic, AKG, Skullcandy, Anker, AirPods, Galaxy Buds, Pixel Buds, Surface Headphones

Missing your headset? [Open an issue](https://github.com/mewset/headsetstatus/issues).

## Requirements

- Linux with D-Bus
- Qt6 (Core, Widgets, DBus)
- UPower
- Notification daemon (optional): libnotify, dunst, mako, swaync

## Tech Stack

| Component | Technology |
|-----------|------------|
| Language | C++17 |
| Framework | Qt6 |
| IPC | D-Bus / UPower |
| Build | CMake |
| Size | 39 KB (LTO + UPX) |

## Contributing

PRs welcome! Please:
- Follow existing code style
- Test before submitting
- Use clear commit messages

## License

MIT © [mewset](https://github.com/mewset)

---

<p align="center">
  <sub>39 KB of pure battery anxiety relief</sub>
</p>
