# HeadsetStatus

A lightweight, fast, and modern Linux system tray application that displays the battery and connection status of your wireless and USB headsets. Supports multiple devices, desktop notifications, and D-Bus integration for real-time updates.

## Features
- **System tray icon** with native emoji rendering (🎧⚡⚠️🔌🪫)
- **Desktop notifications** for low battery, charging complete, and device disconnect
- **Configurable settings** with persistent configuration file
- **Multiple headset support**:
  - Shows device count badge on icon
  - **Dedicated submenu** with individual device status (when 2+ devices)
  - Per-device details with battery, connection type, and technical info
- **Real-time updates** via D-Bus (UPower)
- **Supports Bluetooth and USB headsets** from major vendors (Jabra, Bose, Sony, etc.)
- **Battery and charging status** in tooltip
- **Fast and efficient** (minimal resource usage)
- **Works with Hyprland and swaybar** (Wayland compositors/status bars)
- **Notification daemon support**: libnotify, swaync, mako, dunst


## Requirements
- Linux (tested on Arch Linux, should work on most modern distros)
- Qt6 (Core, Widgets, DBus)
- CMake >= 3.16
- UPower running and accessible on the system bus
- (Optional) Notification daemon for desktop notifications (libnotify, swaync, mako, dunst)

## Compatibility
HeadsetStatus works with most Linux desktop environments and status bars, including:
- Hyprland (Wayland)
- swaybar (Wayland)
- KDE Plasma
- GNOME
- XFCE
- ...and more

## Installation

You can install HeadsetStatus from the AUR using your favorite AUR helper:

```bash
# With yay:
yay -S headsetstatus

# Or with paru:
paru -S headsetstatus
```

If HeadsetStatus is added to the official repositories in the future, you will be able to install it with:

```bash
sudo pacman -S headsetstatus
```

## Autostart on Boot
To start HeadsetStatus automatically when you log in:

### For most desktop environments (KDE, GNOME, XFCE, etc.):
1. Create a `.desktop` file (see below) and place it in `~/.config/autostart/`:

```bash
mkdir -p ~/.config/autostart
cp HeadsetStatus.desktop ~/.config/autostart/
```

2. Make sure the `Exec` path in the `.desktop` file points to the installed location of your HeadsetStatus binary.

### For swaybar/Hyprland (Wayland):
- Add `HeadsetStatus &` to your sway or Hyprland config's `exec` section, e.g.:
  - For sway: add to `~/.config/sway/config`:
    ```
    exec HeadsetStatus
    ```
  - For Hyprland: add to `~/.config/hypr/hyprland.conf`:
    ```
    exec = HeadsetStatus
    ```

This will ensure HeadsetStatus starts with your session.

### Example `HeadsetStatus.desktop` (This is also in the repo)
```ini
[Desktop Entry]
Type=Application
Name=HeadsetStatus
Exec=HeadsetStatus
Icon=audio-headset
Categories=Audio;Utility;
StartupNotify=false
```

## Contributing
Pull requests and issues are welcome! Please:
- Use clear commit messages
- Follow the existing code style
- Test on your system before submitting
- AI generated code is FINE, but i welcome actual human code more

## Configuration

HeadsetStatus stores its configuration in `~/.config/headsetstatus/config.ini`. You can configure:

- **Notifications**: Enable/disable desktop notifications
- **Low battery threshold**: Set the battery percentage for low battery warnings (default: 20%)
- **Notification types**: Choose which events trigger notifications
  - Low battery warnings
  - Charging complete notifications
  - Device disconnect notifications

Access settings via the tray icon context menu → **Settings**.

## Supported Headsets

HeadsetStatus automatically detects headsets from major vendors including:
- Jabra (Evolve, Elite series)
- Bose (QuietComfort, etc.)
- Sony (WH-, WF- series)
- Sennheiser, JBL, Beats
- HyperX, SteelSeries, Razer, Logitech, Corsair
- AirPods, Galaxy Buds, Pixel Buds
- And many more...

If your headset isn't detected, please open an issue on GitHub.

## Why

I created this project because I wanted a fast way of seeing the battery status of my Jabra Evolve2 85. The code has been refactored to follow modern C++ best practices with comprehensive documentation.

---

**Enjoy fast, reliable headset status in your Linux tray!** 
