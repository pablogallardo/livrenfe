/* Copyright (c) 2016, 2017 Pablo G. Gallardo <pggllrd@gmail.com>
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

#include "about.h"
#include "lnfe_window.h"
#include "livrenfe.h"
#include <gtk/gtk.h>
#include <stdlib.h>


struct _AboutLivrenfeClass{
	GtkAboutDialogClass parent_class;
};

typedef struct _AboutLivrenfePrivate AboutLivrenfePrivate;

struct _AboutLivrenfePrivate{
	GtkButton *close_btn;
};

G_DEFINE_TYPE_WITH_PRIVATE(AboutLivrenfe, about_livrenfe, GTK_TYPE_ABOUT_DIALOG)


static void close_about_dialog(GtkButton *b, GtkWidget *win){
	gtk_widget_destroy(win);
}

static void about_livrenfe_dispose(GObject *object){
	AboutLivrenfePrivate *priv;

	priv = about_livrenfe_get_instance_private(ABOUT_LIVRENFE(object));
	G_OBJECT_CLASS(about_livrenfe_parent_class)->dispose(object);
}

static void about_livrenfe_init(AboutLivrenfe *about_livrenfe){
	AboutLivrenfePrivate *priv;

	priv = about_livrenfe_get_instance_private(about_livrenfe);
	gtk_widget_init_template(GTK_WIDGET(about_livrenfe));
}



static void about_livrenfe_class_init(AboutLivrenfeClass *class){
	G_OBJECT_CLASS (class)->dispose = about_livrenfe_dispose;
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class),
		"/br/com/lapagina/livrenfe/about.ui");
}

AboutLivrenfe *about_livrenfe_new(LivrenfeWindow *win){
	return g_object_new (ABOUT_LIVRENFE_TYPE, "transient-for", win,
		"use-header-bar", FALSE, "skip-taskbar-hint", TRUE, NULL);
}
