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

#include "lnfe_window.h"
#include "nfe_manager.h"
#include "item_manager.h"
#include <gtk/gtk.h>

struct _NFEManager{
	GtkDialog parent;
};

struct _NFEManagerClass{
	GtkDialogClass parent_class;
};

typedef struct _NFEManagerPrivate NFEManagerPrivate;

struct _NFEManagerPrivate{
	GtkButton *novo_item;
	GtkButton *salvar_nfe;
};

G_DEFINE_TYPE_WITH_PRIVATE(NFEManager, nfe_manager, GTK_TYPE_DIALOG)

static void item_manager_activate(GtkButton *b, gpointer win){
	ItemManager *iman;

	iman = item_manager_new(NFE_MANAGER(win));
	gtk_window_present(GTK_WINDOW(iman));
}

static void nfe_manager_init(NFEManager *nman){
	NFEManagerPrivate *priv;

	priv = nfe_manager_get_instance_private(nman);
	gtk_widget_init_template(GTK_WIDGET(nman));
	g_signal_connect(priv->novo_item, "clicked", G_CALLBACK(item_manager_activate),
			nman);
}


static void nfe_manager_dispose(GObject *object){
	NFEManagerPrivate *priv;

	priv = nfe_manager_get_instance_private(NFE_MANAGER(object));
	G_OBJECT_CLASS(nfe_manager_parent_class)->dispose(object);
}

static void nfe_manager_class_init(NFEManagerClass *class){
	G_OBJECT_CLASS (class)->dispose = nfe_manager_dispose;

	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class),
                                               "/br/com/lapagina/livrenfe/nfe_manager.ui");
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), NFEManager,
		       	novo_item);
}

NFEManager *nfe_manager_new(LivrenfeWindow *win){
	return g_object_new (NFE_MANAGER_TYPE, "transient-for", win, "use-header-bar", TRUE, NULL);
}
