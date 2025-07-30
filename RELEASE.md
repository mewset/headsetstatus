# HeadsetStatus Release Guide

## Prerequisites
- Git repository cloned and up to date
- AUR repository cloned at `~/headsetstatus/`
- SSH key configured for AUR access

## Release Process

### 1. Update Version Numbers

**Update PKGBUILD:**
```bash
# Edit PKGBUILD and change:
pkgver=1.0.X  # Replace X with new version
```

**Update main.cpp About dialog:**
```cpp
// In main.cpp, update the version in showAbout():
"Version 1.0.X<br>"  // Replace X with new version
```

### 2. Commit and Push to GitHub

```bash
git add .
git commit -m "Bump to v1.0.X: [description of changes]"
git push origin main
```

### 3. Create GitHub Release

```bash
git tag v1.0.X
git push origin v1.0.X
```

### 4. Generate SHA256 Checksum

```bash
# Download the new tarball and compute checksum
curl -Ls "https://github.com/mewset/headsetstatus/archive/refs/tags/v1.0.X.tar.gz" -o /tmp/headsetstatus-1.0.X.tar.gz
sha256sum /tmp/headsetstatus-1.0.X.tar.gz
```

### 5. Update PKGBUILD with Checksum

**Edit PKGBUILD:**
```bash
# Replace the sha256sums line with the new checksum:
sha256sums=('NEW_CHECKSUM_HERE')
```

### 6. Generate .SRCINFO

```bash
makepkg --printsrcinfo > .SRCINFO
```

### 7. Update AUR Repository

```bash
# Copy updated files to AUR repo
cp PKGBUILD .SRCINFO HeadsetStatus.desktop ~/headsetstatus/

# Commit and push to AUR
cd ~/headsetstatus
git add PKGBUILD .SRCINFO HeadsetStatus.desktop
git commit -m "Update to v1.0.X"
git push
```

### 8. Test Locally (Optional)

```bash
# Test the package build
makepkg -si
```

## Version History

- **v1.0.3**: Refaktorerat till objektorienterad C++ (OOCPP)
- **v1.0.2**: Add Information/About dialogs and fix QMessageBox parent issue
- **v1.0.1**: Add HeadsetStatus.desktop to repo for packaging
- **v1.0.0**: Initial release

## Important Notes

- Always update both PKGBUILD and main.cpp version numbers
- Generate SHA256 checksum AFTER creating GitHub release
- Test the package locally before pushing to AUR
- Keep this file updated with new versions and changes 