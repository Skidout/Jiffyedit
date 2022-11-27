# Maintainer: Skidout <Skidout@proton.me>

pkgname=jiffyedit-cli
pkgver=1.0.1.1
pkgrel=1
pkgdesc="Automatically edit videos to MLT project file for Shotcut."
arch=(x86_64)
url="https://github.com/Skidout/Jiffyedit"
license=('GPL3')
depends=(ffmpeg)
makedepends=(gcc)
provides=(jiffyedit)
source=("$pkgname::git+$url")
md5sums=('SKIP')

build() {
	cd "$pkgname"
	g++ -fsanitize=address -fstack-usage -g jiffyedit.cpp -o jiffyedit
}

package() {
	cd "$pkgname"

	install -D jiffyedit -t ${pkgdir}/usr/bin
}
