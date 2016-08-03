/* Copyright (c) 2016 Pablo G. Gallardo <pggllrd@gmail.com>
 *
 * This file is part of LivreNFE.
 *
 * LivreNFE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LivreNFE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LivreNFE.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "livrenfe.h"
#include "lnfe_window.h"
#include <gtk/gtk.h>

struct _Livrenfe{
	GtkApplication parent;
};

struct _LivrenfeClass{
	GtkApplicationClass parent_class;
};

G_DEFINE_TYPE(Livrenfe, livrenfe, GTK_TYPE_APPLICATION);

static void livrenfe_init(Livrenfe *app){
}

static void livrenfe_activate(GApplication *app){
	LivrenfeWindow *win;

	win = livrenfe_window_new(LIVRENFE_APP (app));
	gtk_window_present(GTK_WINDOW(win));
}

static void livrenfe_open(GApplication *app, GFile **files, gint n_files,
	       	const gchar *hint){
	GList *windows;
	LivrenfeWindow *win;
	int i;

	windows = gtk_application_get_windows(GTK_APPLICATION(app));
	if(windows)
		win = LIVRENFE_WINDOW(windows->data);
	else
		win = livrenfe_window_new(LIVRENFE_APP(app));

	for(i = 0; i < n_files; i++)
		livrenfe_window_open(win, files[i]);

	gtk_window_present(GTK_WINDOW(win));
}

static void livrenfe_class_init(LivrenfeClass *class){
	G_APPLICATION_CLASS(class)->activate = livrenfe_activate;
	G_APPLICATION_CLASS(class)->open = livrenfe_open;
}

Livrenfe *livrenfe_new(void){
	return g_object_new(LIVRENFE_TYPE, "application-id", "br.com.lapagina.livrenfe",
			"flags", G_APPLICATION_HANDLES_OPEN, NULL);
}
