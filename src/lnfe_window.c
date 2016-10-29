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
#include "livrenfe.h"
#include "nfe.h"
#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>

struct _LivrenfeWindow{
	GtkApplicationWindow parent;
	GtkTreeView *treeview;
	GtkButton *new_nfe;
};

struct _LivrenfeWindowClass{
	GtkApplicationWindowClass parent_class;
};

G_DEFINE_TYPE(LivrenfeWindow, livrenfe_window, GTK_TYPE_APPLICATION_WINDOW);

static void livrenfe_window_init(LivrenfeWindow *win){
	gtk_widget_init_template(GTK_WIDGET(win));
}

static void nfe_manager_activate(GtkButton *b, gpointer win){
	NFEManager *nman;

	nman = nfe_manager_new(LIVRENFE_WINDOW(win));
	nman->nfe = new_nfe();
	gtk_window_present(GTK_WINDOW(nman));
}

static void livrenfe_window_class_init(LivrenfeWindowClass *class){
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS (class),
			"/br/com/lapagina/livrenfe/window.ui");
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), LivrenfeWindow,
		       	treeview);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), LivrenfeWindow,
		       	new_nfe);
}

void list_nfe(LivrenfeWindow *win, GtkListStore *ls){
	GtkCellRenderer *r_num_nfe;
	GtkCellRenderer *r_serie;
	GtkCellRenderer *r_dh_emis;
	GtkCellRenderer *r_destinatario;
	GtkTreeViewColumn *col_num_nfe;
	GtkTreeViewColumn *col_serie;
	GtkTreeViewColumn *col_dh_emis;
	GtkTreeViewColumn *col_destinatario;

	r_num_nfe = gtk_cell_renderer_text_new();
	r_serie = gtk_cell_renderer_text_new();
	r_dh_emis = gtk_cell_renderer_text_new();
	r_destinatario = gtk_cell_renderer_text_new();
	col_num_nfe = gtk_tree_view_column_new_with_attributes ("NFE", r_num_nfe,
		       	"text", 1, NULL);
	col_serie = gtk_tree_view_column_new_with_attributes ("Serie", r_serie, 
			"text", 2, NULL);
	col_dh_emis = gtk_tree_view_column_new_with_attributes ("Emissão", r_dh_emis,
		       	"text", 3, NULL);
	col_destinatario = gtk_tree_view_column_new_with_attributes ("Destinatário",
		       	r_destinatario, "text", 4, NULL);
	gtk_tree_view_set_model(win->treeview, GTK_TREE_MODEL (ls));
	gtk_tree_view_append_column (GTK_TREE_VIEW (win->treeview), col_num_nfe);
	gtk_tree_view_append_column (GTK_TREE_VIEW (win->treeview), col_serie);
	gtk_tree_view_append_column (GTK_TREE_VIEW (win->treeview), col_dh_emis);
	gtk_tree_view_append_column (GTK_TREE_VIEW (win->treeview), col_destinatario);
	g_signal_connect(win->new_nfe, "clicked", G_CALLBACK(nfe_manager_activate),
			win);
}

LivrenfeWindow *livrenfe_window_new(Livrenfe *app){
	return g_object_new(LIVRENFE_WINDOW_TYPE, "application", app, NULL);
}

