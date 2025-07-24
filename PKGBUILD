# Maintainer: mewset <@mewset github>
pkgname=headsetstatus
pkgver=1.0.0
pkgrel=1
pkgdesc="A fast Linux tray app for headset battery and connection status (Qt6, UPower, FontAwesome)"
arch=('x86_64')
url="https://github.com/mewset/headsetstatus"
license=('MIT')
depends=('qt6-base' 'qt6-tools' 'qt6-declarative' 'upower' 'ttf-font-awesome')
makedepends=('cmake' 'git')
source=("$pkgname-$pkgver.tar.gz::$url/archive/refs/tags/v$pkgver.tar.gz")
sha256sums=('SKIP')

build() {
  cd "$srcdir/headsetstatus-$pkgver"
  cmake -B build -DCMAKE_BUILD_TYPE=Release
  cmake --build build
}

package() {
  cd "$srcdir/headsetstatus-$pkgver"
  install -Dm755 build/HeadsetStatus "$pkgdir/usr/bin/HeadsetStatus"
  install -Dm644 LICENSE "$pkgdir/usr/share/licenses/$pkgname/LICENSE"
  install -Dm644 HeadsetStatus.desktop "$pkgdir/usr/share/applications/HeadsetStatus.desktop"
} 