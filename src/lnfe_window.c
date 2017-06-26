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

#include "lnfe_window.h"
#include "nfe_manager.h"
#include "emitente_manager.h"
#include "sefaz_response.h"
#include "prefs_dialog.h"
#include "livrenfe.h"
#include "db_interface.h"
#include <libnfe/nfe.h>
#include <libnfe/libnfe.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <string.h>
#include <locale.h>

struct _LivrenfeWindow{
	GtkApplicationWindow parent;
	GtkTreeView *treeview;
	GtkMenuItem *emitente_manager_btn;
	GtkMenuItem *status_servico_btn;
	GtkMenuItem *new_nfe_btn;
	GtkMenuItem *about_btn;
	GtkMenuItem *pref_btn;
	GtkMenu *menu_nf;
	GtkMenuItem *abrir_nfe;
	GtkMenuItem *emitir_nfe;
	GtkMenuItem *cancel_nfe;
	GtkDialog *password_modal;
	GtkDialog *just_modal;
	GtkEntry *password;
	GtkEntry *justificativa;
	GtkButton *pw_cancel_btn;
	GtkButton *pw_ok_btn;
	GtkButton *just_cancel_btn;
	GtkButton *just_ok_btn;
	gulong passwd_click_signal_handler;
	gulong passwd_key_signal_handler;
};

struct _LivrenfeWindowClass{
	GtkApplicationWindowClass parent_class;
};

G_DEFINE_TYPE(LivrenfeWindow, livrenfe_window, GTK_TYPE_APPLICATION_WINDOW)

static void on_nfe_manager_destroy(gpointer p, LivrenfeWindow *win){
	list_nfe(win);
}

static void password_modal_dismiss(GtkWidget *p, LivrenfeWindow *win){
	gtk_entry_set_text(win->password, "");
	gtk_widget_set_visible(GTK_WIDGET(win->password_modal), FALSE);
}

static void just_modal_dismiss(GtkWidget *p, LivrenfeWindow *win){
	gtk_entry_set_text(win->justificativa, "");
	gtk_widget_set_visible(GTK_WIDGET(win->just_modal), FALSE);
}

static void on_abrir_nfe_click(GtkMenuItem *m, LivrenfeWindow *win){
	GtkTreeView *t = win->treeview;
	GtkTreeSelection *s;
	s = gtk_tree_view_get_selection(t);
	GtkTreeModel *model;
	GtkTreeIter iter;

	if(gtk_tree_selection_get_selected(s, &model, &iter)){
		int idnfe;
		gtk_tree_model_get(model, &iter, 0, &idnfe, -1);
		NFEManager *nman;
		nman = nfe_manager_new(LIVRENFE_WINDOW(win));
		nman->nfe = get_nfe(idnfe);
		gtk_window_present(GTK_WINDOW(nman));
		g_signal_connect(nman, "destroy", 
			G_CALLBACK(on_nfe_manager_destroy), win);
	}
}

static void sefaz_status(gpointer *b, LivrenfeWindow *win){
	char *password = strdup(gtk_entry_get_text(win->password));
	password_modal_dismiss(NULL, win);
	SefazResponse *sr;
	sr = sefaz_response_new(LIVRENFE_WINDOW(win));
	sr->password = password;
	gtk_window_present(GTK_WINDOW(sr));
}

static void sefaz_emitir(gpointer *b, LivrenfeWindow *win){
	GtkTreeView *t = win->treeview;
	GtkTreeSelection *s;
	s = gtk_tree_view_get_selection(t);
	GtkTreeModel *model;
	GtkTreeIter iter;
	LOTE *lote = NULL;

	if(gtk_tree_selection_get_selected(s, &model, &iter)){
		int idnfe;
		gtk_tree_model_get(model, &iter, 0, &idnfe, -1);
		NFE *nfe = get_nfe(idnfe);
		lote = new_lote(get_lote_id());
		add_nfe(lote, nfe);
	}
	char *password = strdup(gtk_entry_get_text(win->password));
	password_modal_dismiss(NULL, win);
	SefazResponse *sr;
	sr = sefaz_response_new(LIVRENFE_WINDOW(win));
	sr->password = password;
	sr->lote = lote;
	gtk_window_present(GTK_WINDOW(sr));
}

static void sefaz_cancelar(gpointer *b, LivrenfeWindow *win){
	GtkTreeView *t = win->treeview;
	GtkTreeSelection *s;
	s = gtk_tree_view_get_selection(t);
	GtkTreeModel *model;
	GtkTreeIter iter;
	LOTE_EVENTO *lote = NULL;

	if(gtk_tree_selection_get_selected(s, &model, &iter)){
		int idnfe;
		gtk_tree_model_get(model, &iter, 0, &idnfe, -1);
		NFE *nfe = get_nfe(idnfe);
		lote = new_lote_evento(get_lote_evento_id());
		EVENTO_CANCELAMENTO *ec = new_evento_cancelamento();
		ec->evento.nfe = nfe;
		char *j = strdup(gtk_entry_get_text(win->justificativa));
		gtk_entry_set_text(win->justificativa, "");
		ec->justificativa = j;
		add_evento(lote, (EVENTO*)ec);
	}
	char *password = strdup(gtk_entry_get_text(win->password));
	password_modal_dismiss(NULL, win);
	SefazResponse *sr;
	sr = sefaz_response_new(LIVRENFE_WINDOW(win));
	sr->password = password;
	sr->lote_evento = lote;
	gtk_window_present(GTK_WINDOW(sr));
}

static void on_emitir_nfe_click(GtkMenuItem *m, LivrenfeWindow *win){
	gtk_widget_set_visible(GTK_WIDGET(win->password_modal), TRUE);
	gtk_widget_grab_focus(GTK_WIDGET(win->password));
	if(win->passwd_click_signal_handler != 0){
		g_signal_handler_disconnect(win->pw_ok_btn, 
			win->passwd_click_signal_handler);
		g_signal_handler_disconnect(win->password, 
			win->passwd_key_signal_handler);
	}
	win->passwd_click_signal_handler =  g_signal_connect(win->pw_ok_btn, 
		"clicked", G_CALLBACK(sefaz_emitir), win);
	win->passwd_key_signal_handler =  g_signal_connect(win->password, 
		"activate", G_CALLBACK(sefaz_emitir), win);
}

static void on_just_ok_click(GtkButton *m, LivrenfeWindow *win){
	gtk_widget_set_visible(GTK_WIDGET(win->just_modal), FALSE);
	gtk_widget_set_visible(GTK_WIDGET(win->password_modal), TRUE);
	gtk_widget_grab_focus(GTK_WIDGET(win->password));
	if(win->passwd_click_signal_handler != 0){
		g_signal_handler_disconnect(win->pw_ok_btn, 
			win->passwd_click_signal_handler);
		g_signal_handler_disconnect(win->password, 
			win->passwd_key_signal_handler);
	}
	win->passwd_click_signal_handler =  g_signal_connect(win->pw_ok_btn, 
		"clicked", G_CALLBACK(sefaz_cancelar), win);
	win->passwd_key_signal_handler =  g_signal_connect(win->password, 
		"activate", G_CALLBACK(sefaz_cancelar), win);
}

static void on_cancel_nfe_click(GtkMenuItem *m, LivrenfeWindow *win){
	gtk_widget_set_visible(GTK_WIDGET(win->just_modal), TRUE);
	gtk_widget_grab_focus(GTK_WIDGET(win->justificativa));
}

void list_nfe(LivrenfeWindow *win){
	GtkCellRenderer *r_num_nfe;
	GtkCellRenderer *r_serie;
	GtkCellRenderer *r_dh_emis;
	GtkCellRenderer *r_destinatario;
	GtkCellRenderer *r_status;
	GtkTreeViewColumn *col_num_nfe;
	GtkTreeViewColumn *col_serie;
	GtkTreeViewColumn *col_dh_emis;
	GtkTreeViewColumn *col_destinatario;
	GtkTreeViewColumn *col_status;
	GtkListStore *ls = get_list_nfe();

	r_num_nfe = gtk_cell_renderer_text_new();
	r_serie = gtk_cell_renderer_text_new();
	r_dh_emis = gtk_cell_renderer_text_new();
	r_destinatario = gtk_cell_renderer_text_new();
	r_status = gtk_cell_renderer_text_new();
	col_num_nfe = gtk_tree_view_column_new_with_attributes ("NFE", r_num_nfe,
		       	"text", 1, NULL);
	col_serie = gtk_tree_view_column_new_with_attributes ("Série", r_serie, 
			"text", 2, NULL);
	col_dh_emis = gtk_tree_view_column_new_with_attributes ("Emissão", r_dh_emis,
		       	"text", 3, NULL);
	col_status= gtk_tree_view_column_new_with_attributes ("Status",
		       	r_status, "text", 7, NULL);
	col_destinatario = gtk_tree_view_column_new_with_attributes ("Destinatário",
		       	r_destinatario, "text", 4, NULL);
	gtk_tree_view_set_model(win->treeview, GTK_TREE_MODEL (ls));
	if(!gtk_tree_view_get_n_columns(win->treeview)){
		gtk_tree_view_append_column (GTK_TREE_VIEW (win->treeview), col_num_nfe);
		gtk_tree_view_append_column (GTK_TREE_VIEW (win->treeview), col_serie);
		gtk_tree_view_append_column (GTK_TREE_VIEW (win->treeview), col_dh_emis);
		gtk_tree_view_append_column (GTK_TREE_VIEW (win->treeview), col_status);
		gtk_tree_view_append_column (GTK_TREE_VIEW (win->treeview), col_destinatario);
	}
}

static void emitente_manager_activate(GtkMenuItem *i, gpointer win){
	EmitenteManager *eman;
	eman = emitente_manager_new(LIVRENFE_WINDOW(win));
	gtk_window_present(GTK_WINDOW(eman));
}

static void nfe_manager_activate(GtkMenuItem *b, gpointer win){
	NFEManager *nman;

	EMITENTE *e = get_emitente(1);
	if(e){
		free_emitente(e);
		nman = nfe_manager_new(LIVRENFE_WINDOW(win));
		nman->nfe = new_nfe();
		g_signal_connect(nman, "destroy", 
			G_CALLBACK(on_nfe_manager_destroy), win);
		gtk_window_present(GTK_WINDOW(nman));
	} else {
		GtkWidget *dialog;
		dialog = gtk_message_dialog_new(win,
			GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_ERROR,
			GTK_BUTTONS_CLOSE,
			"Cadastre primeiro o emitente");
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(GTK_WIDGET(dialog));
		emitente_manager_activate(NULL, win);
	}
}

static void about_activate(GtkMenuItem *i, gpointer win){
	char version[40];
	sprintf(version, "%s (%s)", VERSION_NAME, VERSION_TITLE);
	gtk_show_about_dialog(win, "program-name", LIVRENFE_TITLE, 
		"version", version, 
		"copyright", LIVRENFE_COPYRIGHT,
		"comments", LIVRENFE_COMMENT,
		"website", LIVRENFE_WEBSITE,
		"website-label", LIVRENFE_WEBSITE,
		"license_type", GTK_LICENSE_GPL_3_0, NULL);
}

static void pref_activate(GtkMenuItem *i, gpointer win){
	Prefs *prefs;
	prefs = prefs_new(LIVRENFE_WINDOW(win));
	gtk_window_present(GTK_WINDOW(prefs));
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
		g_signal_connect(nman, "destroy", G_CALLBACK(on_nfe_manager_destroy), win);
	}
}

static gint popup_menu_nfe(GtkTreeView *t, GdkEventButton *e, LivrenfeWindow *win){
	GtkTreeSelection *s;
	s = gtk_tree_view_get_selection(t);
	GtkTreeModel *model;
	GtkTreeIter iter;

	if(gtk_tree_selection_get_selected(s, &model, &iter)){
		int cancelada;
		int emitida;
		gtk_tree_model_get(model, &iter, 5, &cancelada, -1);
		gtk_tree_model_get(model, &iter, 6, &emitida, -1);
		if(cancelada || !emitida){
			gtk_widget_set_sensitive(GTK_WIDGET(win->cancel_nfe),
				FALSE);
		} else {
			gtk_widget_set_sensitive(GTK_WIDGET(win->cancel_nfe),
				TRUE);
		}
	}

#if GTK_CHECK_VERSION(3,22,0)
	gtk_menu_popup_at_pointer((LIVRENFE_WINDOW(win))->menu_nf, 
		(GdkEvent*) e);
#else
	gtk_menu_popup((LIVRENFE_WINDOW(win))->menu_nf, NULL,
		NULL, NULL, NULL, e->button, e->time);
#endif
	return TRUE;
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
			popup_menu_nfe(t, e, win);
			return TRUE;
		}
	}
	return FALSE;
}

static gint nfe_on_popup(GtkTreeView *t, gpointer win){
	popup_menu_nfe(t, NULL, win);
	return TRUE;
}

static void on_status_servico_click(gpointer p, LivrenfeWindow *win){
	gtk_widget_set_visible(GTK_WIDGET(win->password_modal), TRUE);
	gtk_widget_grab_focus(GTK_WIDGET(win->password));
	if(win->passwd_click_signal_handler != 0){
		g_signal_handler_disconnect(win->pw_ok_btn, 
			win->passwd_click_signal_handler);
		g_signal_handler_disconnect(win->password, 
			win->passwd_key_signal_handler);
	}
	win->passwd_click_signal_handler =  g_signal_connect(win->pw_ok_btn, 
		"clicked", G_CALLBACK(sefaz_status), win);
	win->passwd_key_signal_handler =  g_signal_connect(win->password, 
		"activate", G_CALLBACK(sefaz_status), win);
}

static void livrenfe_window_init(LivrenfeWindow *win){
	gtk_widget_init_template(GTK_WIDGET(win));
	win->passwd_click_signal_handler = 0;
	win->passwd_key_signal_handler = 0;
	g_signal_connect(win, "show", G_CALLBACK(list_nfe),
			NULL);
	g_signal_connect((LIVRENFE_WINDOW(win))->new_nfe_btn, "activate", 
		G_CALLBACK(nfe_manager_activate), win);
	g_signal_connect((LIVRENFE_WINDOW(win))->treeview, "row-activated",
			G_CALLBACK(view_on_row_activated), win);
	g_signal_connect((LIVRENFE_WINDOW(win))->treeview, "button-press-event",
			G_CALLBACK(nfe_context_menu_show), win);
	g_signal_connect((LIVRENFE_WINDOW(win))->treeview, "popup-menu",
			G_CALLBACK(nfe_on_popup), win);
	g_signal_connect((LIVRENFE_WINDOW(win))->emitente_manager_btn, "activate",
			G_CALLBACK(emitente_manager_activate), win);
	g_signal_connect((LIVRENFE_WINDOW(win))->about_btn, "activate",
			G_CALLBACK(about_activate), win);
	g_signal_connect((LIVRENFE_WINDOW(win))->pref_btn, "activate",
			G_CALLBACK(pref_activate), win);
	g_signal_connect((LIVRENFE_WINDOW(win))->status_servico_btn, "activate",
			G_CALLBACK(on_status_servico_click), win);
	g_signal_connect((LIVRENFE_WINDOW(win))->pw_cancel_btn, "clicked",
			G_CALLBACK(password_modal_dismiss), win);
	g_signal_connect((LIVRENFE_WINDOW(win))->just_cancel_btn, "clicked",
			G_CALLBACK(just_modal_dismiss), win);
	g_signal_connect((LIVRENFE_WINDOW(win))->just_ok_btn, "clicked",
			G_CALLBACK(on_just_ok_click), win);
	g_signal_connect((LIVRENFE_WINDOW(win))->justificativa, "activate",
			G_CALLBACK(on_just_ok_click), win);
	g_signal_connect((LIVRENFE_WINDOW(win))->abrir_nfe, "activate",
			G_CALLBACK(on_abrir_nfe_click), win);
	g_signal_connect((LIVRENFE_WINDOW(win))->emitir_nfe, "activate",
			G_CALLBACK(on_emitir_nfe_click), win);
	g_signal_connect((LIVRENFE_WINDOW(win))->cancel_nfe, "activate",
			G_CALLBACK(on_cancel_nfe_click), win);

	GdkPixbuf *icon;
	GError *error = NULL;
	icon = gdk_pixbuf_new_from_resource("/br/com/lapagina/livrenfe/icons/livrenfe_128x128.png",
		&error);
	gtk_window_set_default_icon(icon);
	gtk_window_set_icon(GTK_WINDOW(win), icon);
}


static void livrenfe_window_class_init(LivrenfeWindowClass *class){
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS (class),
			"/br/com/lapagina/livrenfe/window.ui");

	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), LivrenfeWindow,
		       	treeview);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), LivrenfeWindow,
		       	new_nfe_btn);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), LivrenfeWindow,
		       	emitente_manager_btn);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), LivrenfeWindow,
		       	about_btn);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), LivrenfeWindow,
		       	pref_btn);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), LivrenfeWindow,
		       	menu_nf);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), LivrenfeWindow,
		       	abrir_nfe);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), LivrenfeWindow,
		       	emitir_nfe);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), LivrenfeWindow,
		       	cancel_nfe);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), LivrenfeWindow,
		       	status_servico_btn);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), LivrenfeWindow,
		       	password_modal);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), LivrenfeWindow,
		       	password);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), LivrenfeWindow,
		       	pw_cancel_btn);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), LivrenfeWindow,
		       	pw_ok_btn);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), LivrenfeWindow,
		       	just_modal);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), LivrenfeWindow,
		       	justificativa);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), LivrenfeWindow,
		       	just_ok_btn);
	gtk_widget_class_bind_template_child(GTK_WIDGET_CLASS(class), LivrenfeWindow,
		       	just_cancel_btn);
}

LivrenfeWindow *livrenfe_window_new(Livrenfe *app){
	setlocale(LC_ALL, "C");
	return g_object_new(LIVRENFE_WINDOW_TYPE, "application", app, NULL);
}

