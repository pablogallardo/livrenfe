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

#include "item_manager.h"
#include "nfe_manager.h"
#include "nfe.h"
#include <gtk/gtk.h>
#include <stdlib.h>


struct _ItemManagerClass{
	GtkDialogClass parent_class;
};

typedef struct _ItemManagerPrivate ItemManagerPrivate;

struct _ItemManagerPrivate{
	GtkComboBox *icms_regime;
	GtkComboBox *icms_situacao_tributaria;
	GtkComboBox *icms_origem;
	GtkComboBox *cofins_situacao_tributaria;
	GtkComboBox *pis_situacao_tributaria;
	GtkEntry *codigo;
	GtkEntry *ncm;
	GtkEntry *descricao;
	GtkEntry *cfop;
	GtkEntry *unidade;
	GtkEntry *quantidade;
	GtkEntry *valor;
	GtkEntry *icms_aliquota;
	GtkEntry *icms_credito_aproveitado;
	GtkButton *ok_btn;
	GtkButton *cancel_btn;
};

G_DEFINE_TYPE_WITH_PRIVATE(ItemManager, item_manager, GTK_TYPE_DIALOG)

static void item_manager_dispose(GObject *object){
	ItemManagerPrivate *priv;

	priv = item_manager_get_instance_private(ITEM_MANAGER(object));
	G_OBJECT_CLASS(item_manager_parent_class)->dispose(object);
}

static void set_item(GtkButton *b, GtkWidget *iman){
	ItemManagerPrivate *priv = item_manager_get_instance_private(ITEM_MANAGER(iman));
	PRODUTO *p = new_produto(atoi(gtk_entry_get_text(priv->codigo)),
			gtk_entry_get_text(priv->descricao),
			atoi(gtk_entry_get_text(priv->ncm)),
			atoi(gtk_entry_get_text(priv->cfop)),
			gtk_entry_get_text(priv->unidade),
			atof(gtk_entry_get_text(priv->valor)));
	ICMS *i = new_icms(1, 1, atof(gtk_entry_get_text(priv->icms_aliquota)),
			atof(gtk_entry_get_text(priv->icms_credito_aproveitado)));
	IMPOSTO *imp = new_imposto(i, NULL, NULL);
	ITEM *item = new_item(p,
		       	imp, atof(gtk_entry_get_text(priv->valor)),
			atof(gtk_entry_get_text(priv->quantidade)),
		       	++(ITEM_MANAGER(iman))->nfe->q_itens);
	add_item((ITEM_MANAGER(iman))->nfe, item);
	gtk_widget_destroy(iman);
}

static void list_icms_regime(GtkComboBox *t){
	GtkListStore *list_store;
	GtkTreeIter iter;

	enum{
		ID,
		TEXT,
		N_COLS
	};

	list_store = gtk_list_store_new(N_COLS, G_TYPE_INT, G_TYPE_STRING);
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, ID, 1, 
			TEXT, "Simples Nacional", -1);
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, ID, 2, 
			TEXT, "Simples Nacional - excesso de sublimite de receita bruta", -1);
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, ID, 3, 
			TEXT, "Regime Normal", -1);

	GtkCellRenderer *r;

	r = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(t), r, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(t),
				r, "text", TEXT, NULL);
	gtk_combo_box_set_model(t, GTK_TREE_MODEL(list_store));
	gtk_combo_box_set_id_column(t, ID);
}

static void list_icms_st(GtkComboBox *t){
	GtkListStore *list_store;
	GtkTreeIter iter;

	enum{
		ID,
		TEXT,
		N_COLS
	};

	list_store = gtk_list_store_new(N_COLS, G_TYPE_INT, G_TYPE_STRING);
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, ID, 101, 
			TEXT, "Tributada pelo Simples Nacional com permissão de crédito", -1);

	GtkCellRenderer *r;

	r = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(t), r, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(t),
				r, "text", TEXT, NULL);
	gtk_combo_box_set_model(t, GTK_TREE_MODEL(list_store));
	gtk_combo_box_set_id_column(t, ID);
}

static void list_icms_origem(GtkComboBox *t){
	GtkListStore *list_store;
	GtkTreeIter iter;

	enum{
		ID,
		TEXT,
		N_COLS
	};

	list_store = gtk_list_store_new(N_COLS, G_TYPE_INT, G_TYPE_STRING);
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, ID, NACIONAL, 
			TEXT, "Nacional", -1);
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, ID, E_ID, 
			TEXT, "Estrangeira - Importação direta", -1);
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, ID, E_AI, 
			TEXT, "Estrangeira - Adquirida no mercado interno", -1);

	GtkCellRenderer *r;

	r = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(t), r, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(t),
				r, "text", TEXT, NULL);
	gtk_combo_box_set_model(t, GTK_TREE_MODEL(list_store));
	gtk_combo_box_set_id_column(t, ID);
}

static void list_cofins_st(GtkComboBox *t){
	GtkListStore *list_store;
	GtkTreeIter iter;

	enum{
		ID,
		TEXT,
		N_COLS
	};

	list_store = gtk_list_store_new(N_COLS, G_TYPE_INT, G_TYPE_STRING);
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, ID, 8, 
			TEXT, "Operação sem incidência da contribuição", -1);

	GtkCellRenderer *r;

	r = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(t), r, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(t),
				r, "text", TEXT, NULL);
	gtk_combo_box_set_model(t, GTK_TREE_MODEL(list_store));
	gtk_combo_box_set_id_column(t, ID);
}

static void item_manager_init(ItemManager *iman){
	ItemManagerPrivate *priv;

	priv = item_manager_get_instance_private(iman);
	gtk_widget_init_template(GTK_WIDGET(iman));
	list_icms_regime(priv->icms_regime);
	list_icms_origem(priv->icms_origem);
	list_icms_st(priv->icms_situacao_tributaria);
	list_cofins_st(priv->cofins_situacao_tributaria);
	list_cofins_st(priv->pis_situacao_tributaria);
	g_signal_connect(priv->ok_btn, "clicked", G_CALLBACK(set_item),
			iman);
}



static void item_manager_class_init(ItemManagerClass *class){
	G_OBJECT_CLASS (class)->dispose = item_manager_dispose;

	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class),
                                               "/br/com/lapagina/livrenfe/item_manager.ui");
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), ItemManager,
		       	icms_regime);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), ItemManager,
		       	icms_situacao_tributaria);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), ItemManager,
		       	icms_origem);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), ItemManager,
		       	cofins_situacao_tributaria);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), ItemManager,
		       	pis_situacao_tributaria);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), ItemManager,
		       	ok_btn);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), ItemManager,
		       	cancel_btn);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), ItemManager,
		       	codigo);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), ItemManager,
		       	ncm);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), ItemManager,
		       	descricao);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), ItemManager,
		       	cfop);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), ItemManager,
		       	unidade);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), ItemManager,
		       	quantidade);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), ItemManager,
		       	valor);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), ItemManager,
		       	icms_aliquota);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), ItemManager,
		       	icms_credito_aproveitado);
}

ItemManager *item_manager_new(NFEManager *win){
	return g_object_new (ITEM_MANAGER_TYPE, "transient-for", win, "use-header-bar", FALSE, "skip-taskbar-hint", TRUE, NULL);
}
