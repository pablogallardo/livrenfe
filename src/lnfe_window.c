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
#include "emitente_manager.h"
#include "livrenfe.h"
#include "nfe.h"
#include "db_interface.h"
#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>

struct _LivrenfeWindow{
	GtkApplicationWindow parent;
	GtkTreeView *treeview;
	GtkButton *new_nfe;
	GtkMenuItem *emitente_manager_btn;
	GtkMenu *menu_nf;
	GtkMenuItem *abrir_nfe;
	GtkMenuItem *emitir_nfe;
};

struct _LivrenfeWindowClass{
	GtkApplicationWindowClass parent_class;
};

G_DEFINE_TYPE(LivrenfeWindow, livrenfe_window, GTK_TYPE_APPLICATION_WINDOW);

static void nfe_manager_activate(GtkButton *b, gpointer win){
	NFEManager *nman;

	nman = nfe_manager_new(LIVRENFE_WINDOW(win));
	nman->nfe = new_nfe();
	gtk_window_present(GTK_WINDOW(nman));
}

static void emitente_manager_activate(GtkMenuItem *i, gpointer win){
	EmitenteManager *eman;
	eman = emitente_manager_new(LIVRENFE_WINDOW(win));
	gtk_window_present(GTK_WINDOW(eman));
}

void view_on_row_activated(GtkTreeView *t, GtkTreePath *path, 
		GtkTreeViewColumn *col, gpointer win){
	GtkTreeModel *model;
	GtkTreeIter iter;
	model = gtk_tree_view_get_model(t);
	if(gtk_tree_model_get_iter(model, &iter, path)){
		int idnfe;
		gtk_tree_model_get(model, &iter, 0, &idnfe, -1);
		NFEManager *nman;
		nman = nfe_manager_new(LIVRENFE_WINDOW(win));
		nman->nfe = get_nfe(idnfe);
		gtk_window_present(GTK_WINDOW(nman));
	}
}

static gint nfe_context_menu_show(GtkTreeView *t, GdkEventButton *e, 
		gpointer win){
	if(e->type == GDK_BUTTON_PRESS){
		if(e->button == GDK_BUTTON_SECONDARY){
			GtkTreeSelection *s;
			GtkTreePath *p;
			s = gtk_tree_view_get_selection(t);
			if(gtk_tree_view_get_path_at_pos(t, e->x, e->y, &p,
					NULL, NULL, NULL)){
				gtk_tree_selection_unselect_all(s);
				gtk_tree_selection_select_path(s, p);
				gtk_tree_path_free(p);
			}
			gtk_menu_popup((LIVRENFE_WINDOW(win))->menu_nf, NULL, 
				NULL, NULL, NULL, e->button, e->time);
			return TRUE;
		}
	}
	return FALSE;
}

static void livrenfe_window_init(LivrenfeWindow *win){
	gtk_widget_init_template(GTK_WIDGET(win));
	g_signal_connect(win, "visibility-notify-event", G_CALLBACK(list_nfe),
			NULL);
	g_signal_connect((LIVRENFE_WINDOW(win))->new_nfe, "clicked", G_CALLBACK(nfe_manager_activate),
			win);
	g_signal_connect((LIVRENFE_WINDOW(win))->treeview, "row-activated",
			G_CALLBACK(view_on_row_activated), win);
	g_signal_connect((LIVRENFE_WINDOW(win))->treeview, "button-press-event",
			G_CALLBACK(nfe_context_menu_show), win);
	g_signal_connect((LIVRENFE_WINDOW(win))->emitente_manager_btn, "activate",
			G_CALLBACK(emitente_manager_activate), win);
}


static void livrenfe_window_class_init(LivrenfeWindowClass *class){
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS (class),
			"/br/com/lapagina/livrenfe/window.ui");
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), LivrenfeWindow,
		       	treeview);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), LivrenfeWindow,
		       	new_nfe);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), LivrenfeWindow,
		       	emitente_manager_btn);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), LivrenfeWindow,
		       	menu_nf);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), LivrenfeWindow,
		       	abrir_nfe);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), LivrenfeWindow,
		       	emitir_nfe);
}

void list_nfe(LivrenfeWindow *win){
	GtkCellRenderer *r_num_nfe;
	GtkCellRenderer *r_serie;
	GtkCellRenderer *r_dh_emis;
	GtkCellRenderer *r_destinatario;
	GtkTreeViewColumn *col_num_nfe;
	GtkTreeViewColumn *col_serie;
	GtkTreeViewColumn *col_dh_emis;
	GtkTreeViewColumn *col_destinatario;
	GtkListStore *ls = get_list_nfe();

	r_num_nfe = gtk_cell_renderer_text_new();
	r_serie = gtk_cell_renderer_text_new();
	r_dh_emis = gtk_cell_renderer_text_new();
	r_destinatario = gtk_cell_renderer_text_new();
	col_num_nfe = gtk_tree_view_column_new_with_attributes ("NFE", r_num_nfe,
		       	"text", 1, NULL);
	col_serie = gtk_tree_view_column_new_with_attributes ("Série", r_serie, 
			"text", 2, NULL);
	col_dh_emis = gtk_tree_view_column_new_with_attributes ("Emissão", r_dh_emis,
		       	"text", 3, NULL);
	col_destinatario = gtk_tree_view_column_new_with_attributes ("Destinatário",
		       	r_destinatario, "text", 4, NULL);
	gtk_tree_view_set_model(win->treeview, GTK_TREE_MODEL (ls));
	if(!gtk_tree_view_get_n_columns(win->treeview)){
		gtk_tree_view_append_column (GTK_TREE_VIEW (win->treeview), col_num_nfe);
		gtk_tree_view_append_column (GTK_TREE_VIEW (win->treeview), col_serie);
		gtk_tree_view_append_column (GTK_TREE_VIEW (win->treeview), col_dh_emis);
		gtk_tree_view_append_column (GTK_TREE_VIEW (win->treeview), col_destinatario);
	}
}

LivrenfeWindow *livrenfe_window_new(Livrenfe *app){
	return g_object_new(LIVRENFE_WINDOW_TYPE, "application", app, NULL);
}

