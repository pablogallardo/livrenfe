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
#include <gtk/gtk.h>

struct _LivrenfeWindow{
	GtkApplicationWindow parent;
	GtkTreeView *treeview;
};

struct _LivrenfeWindowClass{
	GtkApplicationWindowClass parent_class;
};

G_DEFINE_TYPE(LivrenfeWindow, livrenfe_window, GTK_TYPE_APPLICATION_WINDOW);

static void livrenfe_window_init(LivrenfeWindow *win){
	gtk_widget_init_template(GTK_WIDGET(win));
}

static void livrenfe_window_class_init(LivrenfeWindowClass *class){
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS (class),
			"/br/com/lapagina/livrenfe/window.ui");
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), LivrenfeWindow,
		       	treeview);
}

void list_nfe(LivrenfeWindow *win, GtkListStore *ls){
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;

	renderer = gtk_cell_renderer_text_new ();
	column = gtk_tree_view_column_new_with_attributes ("NFE",
							   renderer,
							   "text", 1,
							   NULL);
	gtk_tree_view_set_model(win->treeview, GTK_TREE_MODEL (ls));
	gtk_tree_view_append_column (GTK_TREE_VIEW (win->treeview), column);
}

LivrenfeWindow *livrenfe_window_new(Livrenfe *app){
	return g_object_new(LIVRENFE_WINDOW_TYPE, "application", app, NULL);
}
