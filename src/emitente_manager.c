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

#include "emitente_manager.h"
#include "lnfe_window.h"
#include "db_interface.h"
#include "utils.h"
#include <gtk/gtk.h>
#include <stdlib.h>


struct _EmitenteManagerClass{
	GtkDialogClass parent_class;
};

typedef struct _EmitenteManagerPrivate EmitenteManagerPrivate;

struct _EmitenteManagerPrivate{
	GtkEntry *cnpj;
	GtkEntry *razao_social;
	GtkEntry *ie;
	GtkEntry *rua;
	GtkEntry *num;
	GtkEntry *complemento;
	GtkEntry *bairro;
	GtkEntry *cep;

	GtkComboBox *crt;
	GtkComboBox *uf;
	GtkComboBox *municipio;
};

G_DEFINE_TYPE_WITH_PRIVATE(EmitenteManager, emitente_manager, GTK_TYPE_DIALOG)

static void list_uf(GtkComboBox *uf){
	GtkCellRenderer *r_uf;

	r_uf = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(uf), r_uf, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(uf),
				r_uf, "text", 1, NULL);
	gtk_combo_box_set_model(uf, GTK_TREE_MODEL(db_list_uf()));
	gtk_combo_box_set_id_column(uf, 0);
}

static void list_municipios(GtkComboBox *uf, GtkComboBox *municipio){
	GtkCellRenderer *r_mun;

	gtk_cell_layout_clear(GTK_CELL_LAYOUT(municipio));
	r_mun = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(municipio), r_mun, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(municipio),
				r_mun, "text", 1, NULL);
	char *active_uf = gtk_combo_box_get_active_id(uf);
	gtk_combo_box_set_model(municipio, GTK_TREE_MODEL(db_list_municipios(active_uf)));
	gtk_combo_box_set_id_column(municipio, 0);
}

static void list_crt(GtkComboBox *fp){
	GtkListStore *list_store;
	GtkTreeIter iter;

	enum{
		ID,
		TEXT,
		N_COLS
	};

	list_store = gtk_list_store_new(N_COLS, G_TYPE_STRING, G_TYPE_STRING);
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, ID, itoa(CRT_SNAC), 
			TEXT, "Simples nacional", -1);
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, ID, itoa(CRT_SNAC_EXC), 
			TEXT, "Simples nacional - Exceso de sublimite de receita bruta", -1);
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, ID, itoa(CRT_NORMAL), 
			TEXT, "Régime normal", -1);

	GtkCellRenderer *r_fp;

	r_fp = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(fp), r_fp, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(fp),
				r_fp, "text", TEXT, NULL);
	gtk_combo_box_set_model(fp, GTK_TREE_MODEL(list_store));
	gtk_combo_box_set_id_column(fp, ID);
}

static void emitente_manager_dispose(GObject *object){
	EmitenteManagerPrivate *priv;

	priv = emitente_manager_get_instance_private(EMITENTE_MANAGER(object));
	G_OBJECT_CLASS(emitente_manager_parent_class)->dispose(object);
}

static void inst_emitente(gpointer p, EmitenteManager *eman){
	EmitenteManagerPrivate *priv;
	priv = emitente_manager_get_instance_private(eman);
	EMITENTE *e = get_emitente(1);
	if(e){
		gtk_entry_set_text(priv->cnpj, e->cnpj);
		gtk_entry_set_text(priv->razao_social, e->nome);
		gtk_entry_set_text(priv->ie, e->inscricao_estadual);
		gtk_entry_set_text(priv->rua, e->endereco->rua);
		gtk_entry_set_text(priv->num, itoa(e->endereco->num));
		gtk_entry_set_text(priv->complemento, 
			e->endereco->complemento);
		gtk_entry_set_text(priv->bairro, e->endereco->bairro);
		gtk_entry_set_text(priv->cep, itoa(e->endereco->cep));

		gtk_combo_box_set_active_id(priv->uf, 
			itoa(e->endereco->municipio->cod_uf));
		gtk_combo_box_set_active_id(priv->municipio, 
			itoa(e->endereco->municipio->codigo));
		gtk_combo_box_set_active_id(priv->crt, 
			itoa(e->crt));
	}
}

static void emitente_manager_init(EmitenteManager *eman){
	EmitenteManagerPrivate *priv;

	priv = emitente_manager_get_instance_private(eman);
	gtk_widget_init_template(GTK_WIDGET(eman));
	g_signal_connect(eman, "show", G_CALLBACK(inst_emitente), eman);
	g_signal_connect(G_OBJECT(priv->uf), "changed",
		G_CALLBACK(list_municipios), priv->municipio);
	list_uf(priv->uf);
	list_crt(priv->crt);
}



static void emitente_manager_class_init(EmitenteManagerClass *class){
	G_OBJECT_CLASS (class)->dispose = emitente_manager_dispose;
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class),
		"/br/com/lapagina/livrenfe/emitente_manager.ui");
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), 
		EmitenteManager, cnpj);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), 
		EmitenteManager, razao_social);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), 
		EmitenteManager, ie);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), 
		EmitenteManager, rua);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), 
		EmitenteManager, num);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), 
		EmitenteManager, complemento);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), 
		EmitenteManager, bairro);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), 
		EmitenteManager, cep);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), 
		EmitenteManager, crt);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), 
		EmitenteManager, uf);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), 
		EmitenteManager, municipio);
}

EmitenteManager *emitente_manager_new(LivrenfeWindow *win){
	return g_object_new (EMITENTE_MANAGER_TYPE, "transient-for", win,
		"use-header-bar", FALSE, "skip-taskbar-hint", TRUE, NULL);
}
