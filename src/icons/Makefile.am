iconsdir = $(datadir)/icons

nobase_dist_icons_DATA = \
	livrenfe.png

gtk_update_icon_cache = gtk-update-icon-cache -f -t $(iconsdir)

install-data-hook: update-icon-cache
uninstall-hook: update-icon-cache
update-icon-cache:
	@if test -z "$(DESTDIR)"; then \
		echo "Atualizar cache de ícones GTK." \
		$(gtk_update_icon_cache);\
	fi
