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

## Why
I created this project because i wanted a fast way of seeing the batterystatus of my Jabra Evolve2 85. It was a VERY long time ago i coded in C/C++ (15+ years) so the source can most likely be optimized and worked upon and making it more readable and clearer.

---

**Enjoy fast, reliable headset status in your Linux tray!** 
