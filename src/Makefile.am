AM_CFLAGS = `pkg-config --cflags gtk+-3.0`
AM_LDFLAGS = -lsqlite3 `pkg-config --libs gtk+-3.0`
bin_PROGRAMS = livrenfe
livrenfe_SOURCES = main.c db.c db_create.c db_init.c livrenfe.c lnfe_window.c resources.c \
		   db_nfe.c nfe_manager.c item_manager.c utils.c

resources.c: livrenfe.gresource.xml window.ui nfe_manager.ui item_manager.ui
	glib-compile-resources --target=$@ --generate-source $<

clean-local:
	rm -f resources.c
