AM_CFLAGS = `pkg-config --cflags gtk+-3.0` `xml2-config --cflags` \
	`pkg-config --cflags xmlsec1-openssl` `curl-config --cflags`
AM_LDFLAGS = -L/usr/local/lib -lsqlite3 -lssl -lcrypto -lp11 \
	`xml2-config --libs` `pkg-config --libs gtk+-3.0` \
	`pkg-config --libs xmlsec1-openssl` `curl-config --libs`
bin_PROGRAMS = livrenfe
livrenfe_SOURCES = main.c db.c db_create.c db_init.c livrenfe.c lnfe_window.c \
		   resources.c  db_nfe.c nfe_manager.c item_manager.c utils.c \
		   nfe.c gen_xml.c crypto_interface.c sign.c send.c xml.c \
		   db_prefs.c sefaz.c emitente_manager.c sefaz_response.c \
		   about.c

resources.c: livrenfe.gresource.xml window.ui nfe_manager.ui item_manager.ui \
		emitente_manager.ui sefaz_response.ui about.ui \
		icons/livrenfe.png icons/livrenfe_128x128.png
	glib-compile-resources --target=$@ --generate-source $<

clean-local:
	rm -f resources.c
