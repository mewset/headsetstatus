# HeadsetStatus

A lightweight, fast, and modern Linux system tray application that displays the battery and connection status of your wireless and USB headsets. Supports multiple devices, FontAwesome tray icons, and D-Bus integration for real-time updates.

## Features
- **System tray icon** with FontAwesome symbols
- **Multiple headset support**: shows a count if more than one device is connected
- **Real-time updates** via D-Bus (UPower)
- **Supports Bluetooth and USB headsets**
- **Battery and charging status** in tooltip
- **Fast and efficient** (minimal resource usage)
- **Works with Hyprland and swaybar** (Wayland compositors/status bars)


## Requirements
- Linux (tested on Arch Linux, should work on most modern distros)
- Qt6 (Core, Widgets, DBus)
- CMake >= 3.16
- FontAwesome TTF font (e.g., `/usr/share/fonts/TTF/fa-solid-900.ttf`)
- UPower running and accessible on the system bus

## Compatibility
HeadsetStatus works with most Linux desktop environments and status bars, including:
- Hyprland (Wayland)
- swaybar (Wayland)
- KDE Plasma
- GNOME
- XFCE
- ...and more

## Build Instructions

```bash
# Install dependencies (example for Arch Linux)
sudo pacman -S qt6-base qt6-tools qt6-declarative cmake upower ttf-font-awesome

# Clone the repository
git clone https://github.com/mewset/headsetstatus.git
cd headsetstatus

# Build (Release mode recommended)
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release

# Run
./build/HeadsetStatus
```

## Install (Optional)
You can copy the binary to a directory in your PATH, or create a `.desktop` file for autostart.

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

## Packaging
- For distribution, consider creating a `.deb`, `.rpm`, or AppImage package.
- Ensure the FontAwesome font is present on the target system.
- Provide a `.desktop` file for desktop integration (see below for an example).

### Example `HeadsetStatus.desktop`
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

---

**Enjoy fast, reliable headset status in your Linux tray!** 