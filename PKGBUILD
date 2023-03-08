# Maintainer: Skidout <Skidout@proton.me>

pkgname=jiffyedit-cli
pkgver=1.2.2.5
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
	g++ -Ofast jiffyedit-master.cpp -o jiffyedit-master
	g++ -Ofast jiffyedit-sr.cpp -o jiffyedit-sr
}

package() {
	cd "$pkgname"
	install -D jiffyedit -t ${pkgdir}/usr/bin
	install -D jiffyedit-sr -t ${pkgdir}/usr/lib/jiffyedit
	install -D jiffyedit-sr.dat -t ${pkgdir}/usr/lib/jiffyedit
	install -D jiffyedit-master -t ${pkgdir}/usr/lib/jiffyedit
}
