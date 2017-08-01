SUBDIRS = libnfe icons
AM_CFLAGS = -I../include -I$(top_srcdir)/include `pkg-config --cflags gtk+-3.0`\
    	`xml2-config --cflags` `pkg-config --cflags xmlsec1-openssl`\
       	`curl-config --cflags`
AM_LDFLAGS = -L/usr/local/lib -Llibnfe -lnfe -lsqlite3 -lssl -lcrypto -lp11 \
	`xml2-config --libs` `pkg-config --libs gtk+-3.0` \
	`pkg-config --libs xmlsec1-openssl` `curl-config --libs`
bin_PROGRAMS = livrenfe
livrenfe_SOURCES = main.c db.c db_create.c db_init.c livrenfe.c lnfe_window.c \
		   resources.c  db_nfe.c nfe_manager.c item_manager.c \
		   crypto_interface.c \
		   db_prefs.c emitente_manager.c sefaz_response.c \
		   prefs.c prefs_dialog.c gtk_common.c

livrenfe_LDADD = libnfe/libnfe.la


resources.c: livrenfe.gresource.xml window.ui nfe_manager.ui item_manager.ui \
		emitente_manager.ui sefaz_response.ui icons/livrenfe.png \
		icons/livrenfe_128x128.png prefs_dialog.ui
	glib-compile-resources --target=$@ --generate-source $<

clean-local:
	rm -f resources.c

