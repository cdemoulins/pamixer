# Maintainer: Clément Démoulins <clement@archivel.fr>

pkgname=pamixer-git
pkgver=20120804
pkgrel=1
pkgdesc="Pulseaudio command-line mixer like amixer"
arch=('i686' 'x86_64')
url="https://github.com/cdemoulins/pamixer"
license=('GPL3')
depends=('libpulse' 'boost-libs')
makedepends=('git' 'boost')

source=()
md5sums=()

_gitroot=https://github.com/cdemoulins/pamixer.git
_gitname=pamixer

build() {
  cd "$srcdir"
  msg "Connecting to GIT server...."

  if [[ -d "$_gitname" ]]; then
    cd "$_gitname" && git pull origin
    msg "The local files are updated."
  else
    git clone "$_gitroot" "$_gitname"
  fi

  msg "GIT checkout done or server timeout"
  msg "Starting build..."

  rm -rf "$srcdir/$_gitname-build"
  git clone "$srcdir/$_gitname" "$srcdir/$_gitname-build"
  cd "$srcdir/$_gitname-build"

  #
  # BUILD HERE
  #
  make
}

package() {
  cd "$srcdir/$_gitname-build"
  install -D -m755 pamixer $pkgdir/usr/bin/pamixer
}

# vim:set ts=2 sw=2 et:
