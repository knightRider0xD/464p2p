# Maintainer: 
# Contributor: Ian Knight <ian@knightly.xyz>

pkgname=uperf-cvs
pkgver=1.13
pkgrel=1
pkgdesc="A network performance tool that supports modelling and replay of various networking patterns"
arch=('i686' 'x86_64')
url="http://www.uperf.org/"
license=('GPL3')
depends=('lksctp-tools')
makedepends=('cvs')
provides=('uperf')
conflicts=('uperf')

_cvsroot=":pserver:anonymous@uperf.cvs.sourceforge.net:/cvsroot/uperf" 
_cvsmod="uperf"

build() {
  cd "$srcdir"
  msg "Connecting to $_cvsmod.sourceforge.net CVS server...."

  if [[ -d "$_cvsmod/CVS" ]]; then
    cd "$_cvsmod"
    cvs -z3 update -d
  else
    cvs -z3 -d "$_cvsroot" co -D "$pkgver" -f "$_cvsmod"
    cd "$_cvsmod"
  fi

  msg "CVS checkout done or server timeout"
  msg "Starting build..."

  rm -rf "$srcdir/$_cvsmod-build"
  cp -r "$srcdir/$_cvsmod" "$srcdir/$_cvsmod-build"
  cd "$srcdir/$_cvsmod-build"

  ./configure --prefix=/usr \
  --datadir=/usr/share/uperf

  make 
}

package() {
  cd "$srcdir/$_cvsmod-build"
  make DESTDIR="$pkgdir/" install
}