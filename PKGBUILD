# Maintainer: Pablo G. Gallardo <pggllrd@gmail.com>
# Contributor: Gabriel Moura <g@srmoura.com.br>
# Após baixado o PKGBUILD lance $makepkg e #pacman -U <pacote_gerado>
pkgname=livrenfe
pkgver=0.1.0
pkgrel=1
pkgdesc="Project open for the issuance of Brazilian Electronic Tax Notes"
arch=('i686' 'x86_64')
url="livrenfe.org"
license=('LGPL')
depends=('libxml2' 'xmlsec' 'curl' 'openssl' 'sqlite' 'gtk3')
provides=('livrenfe')
replaces=('livrenfe')
source=("https://github.com/pablogallardo/livrenfe/releases/download/v0.1.1-alpha/livrenfe-0.1.1-alpha.tar.gz")
sha512sums=('aeefc4b60d9a479b115a52a94d0eb80c99b4cde9406373f14ffb433cf7a8c0007c16b6b61e57ce23e357757413d41265fe6e2b5367250d3d0d7cd9a9ff28afba')

build() {
  cd "$pkgname-master"
  mkdir m4
  autoreconf --install
  ./configure && sudo make
}

package() {
  cd "$pkgname-master"
  sudo make install
}
