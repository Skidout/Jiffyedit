# Maintainer: Skidout <Skidout@proton.me>

pkgname=jiffyedit
pkgver=1.5.8.23
pkgrel=1
pkgdesc="Automate video editing tasks for Shotcut, Pitivi, Losslesscut, and Vidcutter."
arch=(x86_64)
url="https://github.com/Skidout/Jiffyedit"
license=('custom')
depends=(ffmpeg)
makedepends=(clang, llvm, lld, libc++)
optdepends=(
	'mlt: for direct export with mlt filters'
)
source=("$pkgname::git+$url")
md5sums=('SKIP')

build() {
	cd "$pkgname"
	clang++ -Ofast -std=c++17 -stdlib=libc++ -fuse-ld=lld jiffyedit-master.cpp -o jiffyedit
	clang++ -Ofast -std=c++17 -stdlib=libc++ -fuse-ld=lld jiffyedit-sr.cpp -o jiffyedit-sr
	clang++ -Ofast -std=c++17 -stdlib=libc++ -fuse-ld=lld jiffyedit-st.cpp -o jiffyedit-st
}

package() {
	cd "$pkgname"
	install -D jiffyedit-sr -t ${pkgdir}/usr/lib/jiffyedit
	install -D jiffyedit-st -t ${pkgdir}/usr/lib/jiffyedit
	install -D jiffyedit-sr.dat -t ${pkgdir}/usr/lib/jiffyedit
	install -D jiffyedit-st.dat -t ${pkgdir}/usr/lib/jiffyedit
	install -D jiffyedit -t ${pkgdir}/usr/bin
}
