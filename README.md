LivreNFE
========

Copyright (C) 2016, 2017 Pablo G. Gallardo <pggllrd@gmail.com>

**PROJETO EM CONSTRUÇÃO**

Descrição
---------
LivreNFE é um projeto opensource para a emissão de Notas Ficais Eletrónicas
orientado para pequenas e medianas empresas. Este programa se distribui baixo
a licença LGPLv3. Para mais informação veja o documento LICENSE.

Instalação
----------
```
$ ./autogen.sh
$ ./configure
$ make
# make install
```

Dependências
----------
- Gtk3
- cURL 7.51
- OpenSSL 1.0.2c
- libp11 0.4.2
- xmlsec 1.2.24
- libsqlite3

Exemplo de instalação em Debian
---------
* Instalar dependências desde repositório APT:
```
# apt-get install libsqlite3-dev curl libcurl4-openssl-dev libgtk-3-dev libssl-dev \
libxml2-dev libxslt1-dev git
```

* Instalar libp11 versao 0.4.2 que não está disponível no repositório APT:
```
$ wget https://github.com/OpenSC/libp11/releases/download/libp11-0.4.2/libp11-0.4.2.tar.gz
$ tar xzvf libp11-0.4.2.tar.gz
$ cd libp11-0.4.2
$ ./configure && make
$ sudo make install
```

* Instalar xmlsec versao 1.2.24 que não está disponível no repositorio APT:
```$ git clone https://github.com/lsh123/xmlsec
$ cd xmlsec
$ autoreconf --install
$ ./configure && make
$ sudo make install
```

* Instalar livrenfe:
```
$ cd livrenfe
$ ./autogen.sh
$ ./configure && make
$ sudo make install
```

Após a instalação
-------------
* Configurar preferências do sistema (ex. caminho do driver do leitor de cartão
de certificado A3. libaetpkss.so no caso de leitoras Gemalto)
* Cadastro de dados de emissor de NF-e

Erros
-----
Se encontrar um bug, por favor, envie a descrição do erro (qualquer output da
tela) junto com o passo-a-passo de como replicá-lo ao seguinte endereço:
pggllrd@gmail.com

Contribuições
-------------
Sinta-se à vontade de criar Pull Requests para contribuir com funcionalidades
novas ou correção de erros.
