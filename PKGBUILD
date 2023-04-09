# Maintainer: Skidout <Skidout@proton.me>

pkgname=jiffyedit
pkgver=1.5.5.12
pkgrel=1
pkgdesc="Automate video editing tasks for Shotcut and Pitivi."
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
	g++ -Ofast jiffyedit-st.cpp -o jiffyedit-st
}

package() {
	cd "$pkgname"
	install -D jiffyedit -t ${pkgdir}/usr/bin
	install -D jiffyedit-sr -t ${pkgdir}/usr/lib/jiffyedit
	install -D jiffyedit-st -t ${pkgdir}/usr/lib/jiffyedit
	install -D jiffyedit-sr.dat -t ${pkgdir}/usr/lib/jiffyedit
	install -D jiffyedit-st.dat -t ${pkgdir}/usr/lib/jiffyedit
	install -D jiffyedit-master -t ${pkgdir}/usr/lib/jiffyedit
}
