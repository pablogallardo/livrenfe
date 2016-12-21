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
#include "db_interface.h"
#include "nfe.h"
#include "livrenfe.h"
#include "utils.h"
#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>


struct _NFEManagerClass{
	GtkDialogClass parent_class;
};

typedef struct _NFEManagerPrivate NFEManagerPrivate;

struct _NFEManagerPrivate{
	GtkButton *novo_item;
	GtkButton *save_nfe;
	GtkComboBox *uf;
	GtkComboBox *municipio;
	GtkComboBox *uf_destinatario;
	GtkComboBox *municipio_destinatario;
	GtkComboBox *forma_pagamento;
	GtkComboBoxText *t_doc;
	GtkComboBox *tipo_contribuinte;
	GtkEntry *num;
	GtkEntry *serie;
	GtkEntry *dh_emis;
	GtkEntry *dh_saida;
	GtkEntry *nat_op;
	GtkEntry *doc;
	GtkEntry *razao_social;
	GtkEntry *ie;
	GtkEntry *logradouro;
	GtkEntry *numero_endereco;
	GtkEntry *complemento;
	GtkEntry *bairro;
	GtkEntry *cep;
	GtkTreeView *treeview;
};

G_DEFINE_TYPE_WITH_PRIVATE(NFEManager, nfe_manager, GTK_TYPE_DIALOG)

static void list_forma_pagamento(GtkComboBox *fp){
	GtkListStore *list_store;
	GtkTreeIter iter;

	enum{
		ID,
		TEXT,
		N_COLS
	};

	list_store = gtk_list_store_new(N_COLS, G_TYPE_STRING, G_TYPE_STRING);
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, ID, itoa(A_VISTA), 
			TEXT, "A vista", -1);
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, ID, itoa(A_PRAZO), 
			TEXT, "A prazo", -1);
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, ID, itoa(OUTRO), 
			TEXT, "Outro", -1);

	GtkCellRenderer *r_fp;

	r_fp = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(fp), r_fp, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(fp),
				r_fp, "text", TEXT, NULL);
	gtk_combo_box_set_model(fp, GTK_TREE_MODEL(list_store));
	gtk_combo_box_set_id_column(fp, ID);
}

static GtkListStore *get_item_list(NFE *nfe){
	GtkListStore *list_store;
	GtkTreeIter iter;
	ITEM *i;

	enum{ ID_PRODUTO, DESCRICAO, QTD, VALOR, N_COLS };

	list_store = gtk_list_store_new(N_COLS, G_TYPE_INT, G_TYPE_STRING,
		G_TYPE_FLOAT, G_TYPE_FLOAT);
	i = nfe->itens;
	while(i){
		gtk_list_store_append(list_store, &iter);
		gtk_list_store_set(list_store, &iter, ID_PRODUTO, i->produto->codigo, 
				DESCRICAO, i->produto->descricao,
				QTD, i->quantidade,
				VALOR, i->valor, -1);
		
		i = i->pointer;
	}
	return list_store;
}


static void list_tipo_contribuinte(GtkComboBox *t){
	GtkListStore *list_store;
	GtkTreeIter iter;

	enum{
		ID,
		TEXT,
		N_COLS
	};

	list_store = gtk_list_store_new(N_COLS, G_TYPE_STRING, G_TYPE_STRING);
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, ID, itoa(CONT_AV), 
			TEXT, "Contrinuinte ICMS", -1);
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, ID, itoa(CONT_IS), 
			TEXT, "Contribuinte isento", -1);
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, ID, itoa(NAO_CONT), 
			TEXT, "Não contribuinte", -1);

	GtkCellRenderer *r;

	r = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(t), r, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(t),
				r, "text", TEXT, NULL);
	gtk_combo_box_set_model(t, GTK_TREE_MODEL(list_store));
	gtk_combo_box_set_id_column(t, ID);
}

static void list_tipo_doc(GtkComboBoxText *t){
	gtk_combo_box_text_append(t, "CNPJ", "CNPJ");
	gtk_combo_box_text_append(t, "CPF", "CPF");
}

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

static void list_items(gpointer p, NFEManager *win){
	NFEManagerPrivate *priv;
	GtkListStore *l;

	priv = nfe_manager_get_instance_private(win);
	NFE *nfe = (NFE_MANAGER(win))->nfe;
	l = get_item_list(nfe);	
	GtkCellRenderer *r_id_prod;
	GtkCellRenderer *r_desc;
	GtkCellRenderer *r_qtd;
	GtkCellRenderer *r_valor;
	GtkTreeViewColumn *col_id_prod;
	GtkTreeViewColumn *col_desc;
	GtkTreeViewColumn *col_qtd;
	GtkTreeViewColumn *col_valor;

	r_id_prod = gtk_cell_renderer_text_new();
	r_desc = gtk_cell_renderer_text_new();
	r_qtd = gtk_cell_renderer_text_new();
	r_valor = gtk_cell_renderer_text_new();
	col_id_prod = gtk_tree_view_column_new_with_attributes ("ID", r_id_prod,
		       	"text", 0, NULL);
	col_desc = gtk_tree_view_column_new_with_attributes ("Descrição", r_desc, 
			"text", 1, NULL);
	col_qtd = gtk_tree_view_column_new_with_attributes ("Quantidade", r_qtd,
		       	"text", 2, NULL);
	col_valor = gtk_tree_view_column_new_with_attributes ("Valor",
		       	r_valor, "text", 3, NULL);
	gtk_tree_view_set_model(priv->treeview, GTK_TREE_MODEL (l));
	if(!gtk_tree_view_get_n_columns(priv->treeview)){
		gtk_tree_view_append_column (GTK_TREE_VIEW (priv->treeview), col_id_prod);
		gtk_tree_view_append_column (GTK_TREE_VIEW (priv->treeview), col_desc);
		gtk_tree_view_append_column (GTK_TREE_VIEW (priv->treeview), col_qtd);
		gtk_tree_view_append_column (GTK_TREE_VIEW (priv->treeview), col_valor);
	}
}

static void item_manager_activate(GtkButton *b, gpointer win){
	ItemManager *iman;

	iman = item_manager_new(NFE_MANAGER(win));
	iman->nfe = (NFE_MANAGER(win))->nfe;
	gtk_window_present(GTK_WINDOW(iman));
	g_signal_connect(iman, "destroy", G_CALLBACK(list_items), win);
}

static void save_nfe(GtkButton *b, GtkWidget *win){
	NFEManagerPrivate *priv;

	priv = nfe_manager_get_instance_private(win);
	NFE *nfe = (NFE_MANAGER(win))->nfe;
	IDNFE *idnfe = nfe->idnfe;
	DESTINATARIO *destinatario = nfe->destinatario;
	ENDERECO *endereco = destinatario->endereco;
	idnfe->serie = atoi(gtk_entry_get_text(priv->serie));
	idnfe->num_nf = atoi(gtk_entry_get_text(priv->num));
	idnfe->nat_op = gtk_entry_get_text(priv->nat_op);
	idnfe->ind_pag = atoi(gtk_combo_box_get_active_id(priv->forma_pagamento));
	idnfe->mod = MOD_NFE;
	idnfe->dh_emis = strtotime(gtk_entry_get_text(priv->dh_emis));
	time_t saida = strtotime(gtk_entry_get_text(priv->dh_saida));
	idnfe->dh_saida = saida == -1? NULL:&saida;
	idnfe->municipio->cod_uf = atoi(gtk_combo_box_get_active_id(priv->uf));
	idnfe->municipio->codigo = atoi(gtk_combo_box_get_active_id(priv->municipio));

	destinatario->tipo_doc = gtk_combo_box_text_get_active_text(priv->t_doc);
	destinatario->cnpj = gtk_entry_get_text(priv->doc);
	destinatario->nome = gtk_entry_get_text(priv->razao_social);
	destinatario->tipo_ie = atoi(gtk_combo_box_get_active_id(priv->tipo_contribuinte));
	destinatario->inscricao_estadual= gtk_entry_get_text(priv->ie);

	endereco->rua = gtk_entry_get_text(priv->logradouro);
	endereco->num = atoi(gtk_entry_get_text(priv->numero_endereco));
	endereco->complemento = gtk_entry_get_text(priv->complemento);
	endereco->bairro = gtk_entry_get_text(priv->bairro);
	endereco->cep = gtk_entry_get_text(priv->cep);
	endereco->municipio->cod_uf = atoi(gtk_combo_box_get_active_id(priv->uf_destinatario));
	endereco->municipio->codigo = atoi(gtk_combo_box_get_active_id(priv->municipio_destinatario));
	register_nfe(nfe);
	gtk_widget_destroy(win);
}

static void inst_nfe_manager(gpointer p, NFEManager *nman){
	NFEManagerPrivate *priv;

	priv = nfe_manager_get_instance_private(nman);
	NFE *nfe = nman->nfe;
	if(nfe->idnfe->id_nfe != 0){
		IDNFE *idnfe = nfe->idnfe;
		DESTINATARIO *destinatario = nfe->destinatario;
		ENDERECO *endereco = destinatario->endereco;

		gtk_entry_set_text(priv->serie, itoa(idnfe->serie));
		gtk_entry_set_text(priv->num, itoa(idnfe->num_nf));
		gtk_entry_set_text(priv->nat_op, idnfe->nat_op);
		gtk_combo_box_set_active_id(priv->forma_pagamento, itoa(idnfe->ind_pag));

		gtk_entry_set_text(priv->dh_emis, timef(idnfe->dh_emis,
			"%d/%m/%Y %H:%M:%S", 19));
		if(idnfe->dh_saida != NULL){
			gtk_entry_set_text(priv->dh_saida, timef(*idnfe->dh_saida,
				"%d/%m/%Y %H:%M:%S", 19));
		}
		/**TODO idnfe->dh_emis = strtotime(gtk_entry_get_text(priv->dh_emis));
		time_t saida = strtotime(gtk_entry_get_text(priv->dh_saida));
		idnfe->dh_saida = saida == -1? NULL:&saida; */
		gtk_combo_box_set_active_id(priv->uf, 
			itoa(idnfe->municipio->cod_uf));
		gtk_combo_box_set_active_id(priv->municipio, 
			itoa(idnfe->municipio->codigo));

		/* TODO gtk_combo_box_set_active_id(priv->tipo_doc, 
			itoa(destinatario->tipo_doc));*/
		gtk_entry_set_text(priv->doc, destinatario->cnpj);
		gtk_entry_set_text(priv->razao_social, destinatario->nome);
		gtk_combo_box_set_active_id(priv->tipo_contribuinte, 
			itoa(destinatario->tipo_ie));
		if(destinatario->inscricao_estadual != NULL)
			gtk_entry_set_text(priv->ie, 
				destinatario->inscricao_estadual);

		gtk_entry_set_text(priv->logradouro, endereco->rua);
		gtk_entry_set_text(priv->numero_endereco, itoa(endereco->num));
		gtk_entry_set_text(priv->complemento, endereco->complemento);
		gtk_entry_set_text(priv->bairro, endereco->bairro);
		gtk_entry_set_text(priv->cep, itoa(endereco->cep));
		gtk_combo_box_set_active_id(priv->uf_destinatario, 
			itoa(endereco->municipio->cod_uf));
		gtk_combo_box_set_active_id(priv->municipio_destinatario, 
			itoa(endereco->municipio->codigo));
	}
}

static void nfe_manager_init(NFEManager *nman){
	NFEManagerPrivate *priv;

	priv = nfe_manager_get_instance_private(nman);
	gtk_widget_init_template(GTK_WIDGET(nman));
	g_signal_connect(nman, "show", G_CALLBACK(list_items), nman);
	g_signal_connect(nman, "show", G_CALLBACK(inst_nfe_manager), nman);
	g_signal_connect(priv->novo_item, "clicked", G_CALLBACK(item_manager_activate),
			nman);
	g_signal_connect(priv->save_nfe, "clicked", G_CALLBACK(save_nfe),
			nman);
	g_signal_connect(G_OBJECT(priv->uf), "changed", G_CALLBACK(list_municipios), priv->municipio);
	g_signal_connect(G_OBJECT(priv->uf_destinatario), "changed", G_CALLBACK(list_municipios),
		priv->municipio_destinatario);
	list_uf(priv->uf);
	list_uf(priv->uf_destinatario);
	list_forma_pagamento(priv->forma_pagamento);
	list_tipo_doc(priv->t_doc);
	list_tipo_contribuinte(priv->tipo_contribuinte);

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
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), NFEManager,
		       	save_nfe);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), NFEManager,
		       	uf);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), NFEManager,
		       	municipio);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), NFEManager,
		       	uf_destinatario);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), NFEManager,
		       	municipio_destinatario);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), NFEManager,
		       	forma_pagamento);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), NFEManager,
		       	t_doc);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), NFEManager,
		       	tipo_contribuinte);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), NFEManager,
		       	num);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), NFEManager,
		       	serie);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), NFEManager,
		       	dh_emis);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), NFEManager,
		       	dh_saida);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), NFEManager,
		       	nat_op);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), NFEManager,
		       	doc);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), NFEManager,
		       	razao_social);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), NFEManager,
		       	ie);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), NFEManager,
		       	logradouro);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), NFEManager,
		       	numero_endereco);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), NFEManager,
		       	complemento);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), NFEManager,
		       	bairro);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), NFEManager,
		       	cep);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), NFEManager,
		       	treeview);
}

NFEManager *nfe_manager_new(LivrenfeWindow *win){
	return g_object_new (NFE_MANAGER_TYPE, "transient-for", win, "use-header-bar", FALSE, "skip-taskbar-hint", TRUE, NULL);
}
