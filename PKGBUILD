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
depends=('xml2' 'xmlsec' 'curl' 'openssl' 'sqlite' 'gtk3')
provides=('livrenfe')
replaces=('livrenfe')
source=("https://github.com/pablogallardo/livrenfe/archive/master.tar.gz")
sha512sums=('5823db5f58374fc387b0e992f8c148ae5f57085256629a71b064b4bde555fa4ee668da4c100292691f23953f93491dc8bcbc176588694c89544ee8ccdf00a895')

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
