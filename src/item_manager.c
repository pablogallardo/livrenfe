/* Copyright (c) 2016, 2017 Pablo G. Gallardo <pggllrd@gmail.com>
 *
 * This file is part of LivreNFE.
 *
 * LivreNFE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LivreNFE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with LivreNFE.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "item_manager.h"
#include "nfe_manager.h"
#include "libnfe/utils.h"
#include "gtk_common.h"
#include <libnfe/errno.h>
#include <libnfe/nfe.h>
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
	GtkComboBox *ipi_situacao_tributaria;
	GtkEntry *codigo;
	GtkEntry *ncm;
	GtkEntry *descricao;
	GtkEntry *cfop;
	GtkEntry *unidade;
	GtkEntry *quantidade;
	GtkEntry *valor;
	GtkEntry *icms_aliquota;
	GtkEntry *icms_credito_aproveitado;
	GtkEntry *ipi_classe;
	GtkEntry *ipi_codigo;
	GtkButton *ok_btn;
	GtkButton *cancel_btn;
	GtkLabel *subtotal;
};

G_DEFINE_TYPE_WITH_PRIVATE(ItemManager, item_manager, GTK_TYPE_DIALOG)

static void item_manager_dispose(GObject *object){
	G_OBJECT_CLASS(item_manager_parent_class)->dispose(object);
}

static int check_fields(GtkWidget *iman){
	ItemManagerPrivate *priv;
	int rc;

	priv = item_manager_get_instance_private(ITEM_MANAGER(iman));
	if(validate_integer(priv->codigo, "Código de produto", iman, FALSE))
		return -EINVFIELD;
	if(validate_integer(priv->ncm, "Número de ncm inválido", iman, FALSE))
		return -EINVFIELD;
	if(validate_integer(priv->cfop, "Número de CFOP inválido", iman, FALSE))
		return -EINVFIELD;
	if(validate_integer(priv->quantidade, "Quantidade inválida", iman, FALSE))
		return -EINVFIELD;
	if(validate_integer(priv->ipi_classe, "Classe IPI inválida", iman, TRUE))
		return -EINVFIELD;
	if(validate_integer(priv->ipi_codigo, "Código IPI inválido", iman, TRUE))
		return -EINVFIELD;
	if(validate_float(priv->valor, "Valor inválido", iman, FALSE))
		return -EINVFIELD;
	if(validate_float(priv->icms_aliquota, "Aliquota ICMS inválida", iman, TRUE))
		return -EINVFIELD;
	if(validate_float(priv->icms_credito_aproveitado, 
			"Crédito aproveitado ICMS inválido", iman, TRUE))
		return -EINVFIELD;

	return 0;
}

static int set_item(GtkButton *b, GtkWidget *iman){
	ItemManagerPrivate *priv = item_manager_get_instance_private(ITEM_MANAGER(iman));
	char *valor = str_replace(",",".",(char*)gtk_entry_get_text(priv->valor));
	if(check_fields(iman))
		return -EINVFIELD;

	ITEM *item = ITEM_MANAGER(iman)->item?
		ITEM_MANAGER(iman)->item : new_item();

	NFE *nfe = (ITEM_MANAGER(iman))->nfe;
	inst_produto(0, gtk_entry_get_text(priv->codigo),
		gtk_entry_get_text(priv->descricao),
		atoi(gtk_entry_get_text(priv->ncm)),
		atoi(gtk_entry_get_text(priv->cfop)),
		gtk_entry_get_text(priv->unidade),
		atof(valor),
		item->produto);
	
	int icms_situacao_tributaria = atoi(gtk_combo_box_get_active_id(priv->icms_situacao_tributaria));
	if(icms_situacao_tributaria){
		inst_icms(atoi(gtk_combo_box_get_active_id(priv->icms_origem)),
			icms_situacao_tributaria,
			atof(gtk_entry_get_text(priv->icms_aliquota)),
			atof(gtk_entry_get_text(priv->icms_credito_aproveitado)),
			item->imposto->icms);
	}

	int ipi_situacao_tributaria = atoi(gtk_combo_box_get_active_id(priv->ipi_situacao_tributaria));
	if(ipi_situacao_tributaria){
		inst_ipi(ipi_situacao_tributaria,
			gtk_entry_get_text(priv->ipi_classe),
			gtk_entry_get_text(priv->ipi_codigo),
			item->imposto->ipi);
	}
	item->valor = atof(valor);
	free(valor);

	item->quantidade = atoi(gtk_entry_get_text(priv->quantidade));
	if(ITEM_MANAGER(iman)->item == NULL){
		item->ordem = (ITEM_MANAGER(iman))->nfe->q_itens + 1; 
		add_item(nfe, item);
	}
	gtk_widget_destroy(iman);
	return 0;
}

static void set_read_only(ItemManagerPrivate *priv){
	gtk_widget_set_sensitive(GTK_WIDGET(priv->codigo), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(priv->ncm), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(priv->descricao), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(priv->cfop), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(priv->unidade), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(priv->quantidade), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(priv->valor), FALSE);

	gtk_widget_set_sensitive(GTK_WIDGET(priv->icms_regime), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(priv->icms_situacao_tributaria), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(priv->icms_origem), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(priv->icms_aliquota), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(priv->icms_credito_aproveitado), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(priv->cofins_situacao_tributaria), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(priv->pis_situacao_tributaria), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(priv->ipi_situacao_tributaria), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(priv->ipi_classe), FALSE);
	gtk_widget_set_sensitive(GTK_WIDGET(priv->ipi_codigo), FALSE);
}

static void set_icms_fields(ItemManagerPrivate *priv, gboolean sensitive){
	gtk_widget_set_sensitive(GTK_WIDGET(priv->icms_regime), sensitive);
	gtk_widget_set_sensitive(GTK_WIDGET(priv->icms_origem), sensitive);
	gtk_widget_set_sensitive(GTK_WIDGET(priv->icms_aliquota), sensitive);
	gtk_widget_set_sensitive(GTK_WIDGET(priv->icms_credito_aproveitado), 
		sensitive);
}

static void set_ipi_fields(ItemManagerPrivate *priv, gboolean sensitive){
	gtk_widget_set_sensitive(GTK_WIDGET(priv->ipi_classe), sensitive);
	gtk_widget_set_sensitive(GTK_WIDGET(priv->ipi_codigo), sensitive);
}

static void on_icms_type_change(GtkComboBox *c, ItemManager *iman){
	ItemManagerPrivate *priv;

	priv = item_manager_get_instance_private(iman);
	int id = atoi(gtk_combo_box_get_active_id(c));
	gboolean sensitivity = id == 0? FALSE : TRUE;
	set_icms_fields(priv, sensitivity);
}

static void on_ipi_type_change(GtkComboBox *c, ItemManager *iman){
	ItemManagerPrivate *priv;

	priv = item_manager_get_instance_private(iman);
	int id = atoi(gtk_combo_box_get_active_id(c));
	gboolean sensitivity = id == 0? FALSE : TRUE;
	set_ipi_fields(priv, sensitivity);
}

static void calc_subtotal(gpointer p, GdkEvent *e, ItemManager *iman){
	ItemManagerPrivate *priv;

	priv = item_manager_get_instance_private(iman);
	int qtd = atoi(gtk_entry_get_text(priv->quantidade));
	char *valor_str = (char*) gtk_entry_get_text(priv->valor);
	valor_str = str_replace(",", ".", valor_str);
	double valor = atof(valor_str);
	char *subtotal = dtoa(qtd * valor);
	gtk_label_set_text(priv->subtotal, subtotal);
	free(subtotal);
}

static void inst_item_manager(gpointer p, ItemManager *iman){
	ItemManagerPrivate *priv;
	priv = item_manager_get_instance_private(iman);

	ITEM *i = iman->item;
	NFE *n = iman->nfe;
	if(i){
		PRODUTO *p = i->produto;
		IMPOSTO *imp = i->imposto;
		ICMS *icms = imp->icms;
		PIS *pis = imp->pis;
		COFINS *cofins = imp->cofins;
		IPI *ipi = imp->ipi;

		gtk_entry_set_text(priv->codigo, p->codigo);
		gtk_entry_set_text(priv->descricao, p->descricao);
		gtk_entry_set_text(priv->unidade, p->unidade_comercial);
		gtk_entry_set_text(priv->valor, dtoa(p->valor));
		gtk_entry_set_text(priv->quantidade, itoa(i->quantidade));
		gtk_entry_set_text(priv->ncm, itoa(p->ncm));
		gtk_entry_set_text(priv->cfop, itoa(p->cfop));

		//ICMS
		if(icms->tipo != 0){
			gtk_combo_box_set_active_id(priv->icms_regime, 
				"1");
			gtk_combo_box_set_active_id(priv->icms_origem, 
				itoa(icms->origem));
			gtk_combo_box_set_active_id(priv->icms_situacao_tributaria, 
				itoa(icms->tipo));
			gtk_entry_set_text(priv->icms_aliquota, dtoa(icms->aliquota));
			gtk_entry_set_text(priv->icms_credito_aproveitado, 
				dtoa(icms->valor));
		}

		//COFINS
		gtk_combo_box_set_active_id(priv->cofins_situacao_tributaria, 
			"8");

		//PIS
		gtk_combo_box_set_active_id(priv->pis_situacao_tributaria, 
			"8");

		//IPI
		if(ipi->sit_trib != 0){
			gtk_combo_box_set_active_id(priv->ipi_situacao_tributaria, 
				itoa(ipi->sit_trib));
			gtk_entry_set_text(priv->ipi_classe, ipi->classe);
			gtk_entry_set_text(priv->ipi_codigo, ipi->codigo);
		}

		calc_subtotal(NULL, NULL, iman);
		if(n->protocolo->numero)
			set_read_only(priv);

	}
}

static void list_icms_regime(GtkComboBox *t){
	GtkListStore *list_store;
	GtkTreeIter iter;

	enum{
		ID,
		TEXT,
		N_COLS
	};

	list_store = gtk_list_store_new(N_COLS, G_TYPE_STRING, G_TYPE_STRING);
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, ID, "1", 
			TEXT, "Simples Nacional", -1);
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, ID, "2", 
			TEXT, "Simples Nacional - excesso de sublimite de receita bruta", -1);
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, ID, "3", 
			TEXT, "Regime Normal", -1);

	GtkCellRenderer *r;

	r = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(t), r, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(t),
				r, "text", TEXT, NULL);
	gtk_combo_box_set_model(t, GTK_TREE_MODEL(list_store));
	gtk_combo_box_set_id_column(t, ID);
}

static void list_icms_st(GtkComboBox *t, ItemManager *iman){
	GtkListStore *list_store;
	GtkTreeIter iter;
	ItemManagerPrivate *priv;

	priv = item_manager_get_instance_private(iman);

	enum{
		ID,
		TEXT,
		N_COLS
	};

	list_store = gtk_list_store_new(N_COLS, G_TYPE_STRING, G_TYPE_STRING);
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, ID, "0", 
			TEXT, "Sem ICMS", -1);
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, ID, "101", 
			TEXT, "Tributada pelo Simples Nacional com permissão de crédito", -1);
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, ID, "102", 
			TEXT, "Tributada pelo Simples Nacional sem permissão de crédito", -1);

	GtkCellRenderer *r;

	r = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(t), r, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(t),
				r, "text", TEXT, NULL);
	gtk_combo_box_set_model(t, GTK_TREE_MODEL(list_store));
	gtk_combo_box_set_id_column(t, ID);
	gtk_combo_box_set_active_id(t, "0");
	on_icms_type_change(t, iman);
}

static void list_ipi_st(GtkComboBox *t, ItemManager *iman){
	GtkListStore *list_store;
	GtkTreeIter iter;
	ItemManagerPrivate *priv;

	priv = item_manager_get_instance_private(iman);

	enum{
		ID,
		TEXT,
		N_COLS
	};

	list_store = gtk_list_store_new(N_COLS, G_TYPE_STRING, G_TYPE_STRING);
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, ID, "0", 
			TEXT, "Sem IPI", -1);
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, ID, "53", 
			TEXT, "Saída não tributada", -1);

	GtkCellRenderer *r;

	r = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(t), r, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(t),
				r, "text", TEXT, NULL);
	gtk_combo_box_set_model(t, GTK_TREE_MODEL(list_store));
	gtk_combo_box_set_id_column(t, ID);
	gtk_combo_box_set_active_id(t, "0");
	on_ipi_type_change(t, iman);
}

static void list_icms_origem(GtkComboBox *t){
	GtkListStore *list_store;
	GtkTreeIter iter;

	enum{
		ID,
		TEXT,
		N_COLS
	};

	list_store = gtk_list_store_new(N_COLS, G_TYPE_STRING, G_TYPE_STRING);
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, ID, itoa(ORIGEM_NACIONAL), 
			TEXT, "Nacional", -1);
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, ID, itoa(ORIGEM_ESTRANGEIRA_IMP_DIRETA), 
			TEXT, "Estrangeira - Importação direta", -1);
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, ID, itoa(ORIGEM_ESTRANGEIRA_MERC_INTERNO), 
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

	list_store = gtk_list_store_new(N_COLS, G_TYPE_STRING, G_TYPE_STRING);
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, ID, "8", 
			TEXT, "Operação sem incidência da contribuição", -1);

	GtkCellRenderer *r;

	r = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(t), r, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(t),
				r, "text", TEXT, NULL);
	gtk_combo_box_set_model(t, GTK_TREE_MODEL(list_store));
	gtk_combo_box_set_id_column(t, ID);
	gtk_combo_box_set_active_id(t, "8");
}

static void on_item_manager_destroy(gpointer btn, GtkWidget *iman){
	gtk_widget_destroy(GTK_WIDGET(iman));
}

static void item_manager_init(ItemManager *iman){
	ItemManagerPrivate *priv;

	priv = item_manager_get_instance_private(iman);
	gtk_widget_init_template(GTK_WIDGET(iman));
	list_icms_regime(priv->icms_regime);
	list_icms_origem(priv->icms_origem);
	list_icms_st(priv->icms_situacao_tributaria, iman);
	list_cofins_st(priv->cofins_situacao_tributaria);
	list_cofins_st(priv->pis_situacao_tributaria);
	list_ipi_st(priv->ipi_situacao_tributaria, iman);
	g_signal_connect(priv->ok_btn, "clicked", G_CALLBACK(set_item),
			iman);
	g_signal_connect(iman, "show", G_CALLBACK(inst_item_manager),
			iman);
	g_signal_connect(priv->cancel_btn, "clicked", G_CALLBACK(on_item_manager_destroy),
			iman);
	g_signal_connect(priv->quantidade, "focus-out-event", 
		G_CALLBACK(calc_subtotal), iman);
	g_signal_connect(priv->valor, "focus-out-event", 
		G_CALLBACK(calc_subtotal), iman);
	g_signal_connect(priv->icms_situacao_tributaria, "changed", 
		G_CALLBACK(on_icms_type_change), iman);
	g_signal_connect(priv->ipi_situacao_tributaria, "changed", 
		G_CALLBACK(on_ipi_type_change), iman);
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
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), ItemManager,
		       	ipi_situacao_tributaria);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), ItemManager,
		       	ipi_classe);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), ItemManager,
		       	ipi_codigo);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), ItemManager,
		       	subtotal);
}

ItemManager *item_manager_new(NFEManager *win){
	return g_object_new (ITEM_MANAGER_TYPE, "transient-for", win, "use-header-bar", FALSE, "skip-taskbar-hint", TRUE, NULL);
}
