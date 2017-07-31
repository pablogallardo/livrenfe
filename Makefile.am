ACLOCAL_AMFLAGS = -I m4
if ENABLE_GTK_DOC
GTK_DOC_SUBDIR = docs/reference/libnfe
endif

SUBDIRS = include src $(GTK_DOC_SUBDIR)

README: README.md

desktopdir = $(datadir)/applications
dist_desktop_DATA = livrenfe.desktop
