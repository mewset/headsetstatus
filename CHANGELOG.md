# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [1.2.1] - 2026-02-15

### Changed
- Reduced unnecessary status updates by tightening UPower D-Bus filtering and adding update coalescing.
- Added fallback polling via `general/updateInterval` so missed D-Bus events recover automatically.
- Improved tray update efficiency by skipping redundant tooltip/icon/menu rebuilds when state is unchanged.

### Fixed
- Settings save now batches writes into one sync instead of multiple disk writes.
- Unit tests for `ConfigManager` now use isolated temporary config files instead of user config paths.

### Technical
- Release builds now define `QT_NO_DEBUG_OUTPUT` to strip debug output strings.
- General cleanup for object lifetimes and simpler UI dialog ownership.

## [1.2.0] - 2025-12-06

### Added
- **Headless mode** (`--no-tray`) - Run without system tray, notifications only
- **CLI arguments**: `--version`, `--help`, `--no-tray`, `--debug`
- **Systemd user service** for auto-start on login
- Device disconnect notifications (previously broken, now working)

### Changed
- Binary size reduced by 91% (431 KB → 39 KB) via demoscene-inspired optimizations
- Consolidated D-Bus includes for cleaner code
- Removed unused QMessageBox include from TrayIconController

### Fixed
- Device disconnect notification was implemented but never triggered
- Signal-slot connection leak when opening settings dialog repeatedly

### Technical
- Demoscene-inspired size optimizations:
  - Link Time Optimization (LTO)
  - `-fvisibility=hidden` for symbol hiding
  - `-ffunction-sections -fdata-sections` with `--gc-sections`
  - Automatic binary stripping
  - UPX LZMA compression (if available)
- Cleaner include structure with forward declarations

## [1.1.0] - 2025-09-30

### Added
- Desktop notifications for low battery, charging complete, and device disconnect events
- Settings dialog for configuring notification preferences and thresholds
- Persistent configuration file (~/.config/headsetstatus/config.ini)
- **Multi-device submenu** with individual device status and details (when 2+ devices connected)
- Per-device "Show Details" action in submenu with technical information
- Improved device detection with vendor-specific keywords (Jabra, Bose, Sony, etc.)
- Comprehensive inline documentation (Doxygen-style) for all classes
- Version management system with auto-generated version header from CMake
- Installation targets in CMakeLists.txt for proper system integration
- Low battery emoji (🪫) for better visual indication

### Changed
- **BREAKING**: Removed FontAwesome dependency, now uses native emoji rendering
- Enhanced headset detection algorithm for better multi-vendor support
- Improved error handling and logging throughout the application
- Settings menu item added to tray context menu
- Better code organization with professional inline documentation

### Fixed
- FontAwesome font loading issues on different distributions
- Global font application bug that affected entire application
- Hardcoded version number in About dialog

### Technical Improvements
- D-Bus filtering optimized for better performance
- Notification system compatible with libnotify, swaync, mako, and dunst
- Configuration changes are now persisted across sessions
- Better memory management with proper Qt parent-child relationships

## [1.0.3] - 2025-09-30

Previous stable release.

## [1.0.2] - 2025-09-30

### Added
- Information and About dialogs
- Konami code easter egg

### Fixed
- QMessageBox parent issue

## [1.0.1] - Initial release

### Added
- Basic system tray icon with FontAwesome symbols
- Multiple headset support with device count display
- Real-time updates via D-Bus (UPower)
- Support for Bluetooth and USB headsets
- Battery and charging status in tooltip
