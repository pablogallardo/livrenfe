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

#include "lnfe_window.h"
#include "nfe_manager.h"
#include "item_manager.h"
#include "db_interface.h"
#include "livrenfe.h"
#include "gtk_common.h"
#include "libnfe/utils.h"
#include <libnfe/errno.h>
#include <libnfe/nfe.h>
#include <libnfe/libnfe.h>
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
	GtkComboBox *t_doc;
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
	GtkEntry *chave_nfe;
	GtkEntry *inf_ad_fisco;
	GtkEntry *inf_ad_contrib;
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
	gtk_list_store_set(list_store, &iter, ID, itoa(OUTROS), 
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

	enum{ COD_PRODUTO, DESCRICAO, QTD, VALOR, N_COLS };

	list_store = gtk_list_store_new(N_COLS, G_TYPE_STRING, G_TYPE_STRING,
		G_TYPE_INT, G_TYPE_FLOAT);
	i = nfe->itens;
	while(i){
		gtk_list_store_append(list_store, &iter);
		gtk_list_store_set(list_store, &iter, 
			COD_PRODUTO, i->produto->codigo, 
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
	gtk_list_store_set(list_store, &iter, ID, itoa(IE_CONTRIBUINTE_PAG_AVISTA), 
			TEXT, "Contrinuinte ICMS", -1);
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, ID, itoa(IE_CONTRIBUINTE_ISENTO), 
			TEXT, "Contribuinte isento", -1);
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, ID, itoa(IE_NAO_CONTRIBUINTE), 
			TEXT, "Não contribuinte", -1);

	GtkCellRenderer *r;

	r = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(t), r, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(t),
				r, "text", TEXT, NULL);
	gtk_combo_box_set_model(t, GTK_TREE_MODEL(list_store));
	gtk_combo_box_set_id_column(t, ID);
}

static void list_tipo_doc(GtkComboBox *t){
	GtkListStore *list_store;
	GtkTreeIter iter;

	enum{
		ID,
		N_COLS
	};

	list_store = gtk_list_store_new(N_COLS, G_TYPE_STRING);
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, ID, "CNPJ", -1);
	gtk_list_store_append(list_store, &iter);
	gtk_list_store_set(list_store, &iter, ID, "CPF", -1);

	GtkCellRenderer *r_td;

	r_td = gtk_cell_renderer_text_new();
	gtk_cell_layout_pack_start(GTK_CELL_LAYOUT(t), r_td, TRUE);
	gtk_cell_layout_set_attributes(GTK_CELL_LAYOUT(t),
				r_td, "text", ID, NULL);
	gtk_combo_box_set_model(t, GTK_TREE_MODEL(list_store));
	gtk_combo_box_set_id_column(t, ID);
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
	char *active_uf = (char *)gtk_combo_box_get_active_id(uf);
	gtk_combo_box_set_model(municipio, GTK_TREE_MODEL(db_list_municipios(active_uf)));
	gtk_combo_box_set_id_column(municipio, 0);
}

static void list_items(gpointer p, NFEManager *win){
	NFEManagerPrivate *priv;
	GtkListStore *l;

	priv = nfe_manager_get_instance_private(NFE_MANAGER(win));
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

static int check_fields(GtkWidget *nman){
	NFEManagerPrivate *priv;
	int rc;

	priv = nfe_manager_get_instance_private(NFE_MANAGER(nman));
	if(validate_integer(priv->num, "Número de NF inválido", nman, FALSE))
		return -EINVFIELD;
	if(validate_integer(priv->serie, "Número de série inválido", nman, FALSE))
		return -EINVFIELD;
	if(validate_integer(priv->doc, "Número de CNPJ/CPF inválido", nman, FALSE))
		return -EINVFIELD;
	if(validate_integer(priv->ie, "Número de inscrição estadual inválido", nman, TRUE))
		return -EINVFIELD;
	if(validate_integer(priv->numero_endereco, "Número de endereço inválido", nman, FALSE))
		return -EINVFIELD;
	if(validate_integer(priv->cep, "Número de CEP inválido", nman, FALSE))
		return -EINVFIELD;
	if(validate_date(priv->dh_emis, "Data de emissão inválida\nFormato: DD/MM/AAAA HH:MM:SS", nman, FALSE))
		return -EINVFIELD;
	if(validate_date(priv->dh_saida, "Data de saída inválida\nFormato: DD/MM/AAAA HH:MM:SS", nman, TRUE))
		return -EINVFIELD;

	return 0;
}

static int save_nfe(GtkButton *b, GtkWidget *win){
	NFEManagerPrivate *priv;
	if(check_fields(win))
		return -EINVFIELD;

	priv = nfe_manager_get_instance_private(NFE_MANAGER(win));
	NFE *nfe = (NFE_MANAGER(win))->nfe;
	IDNFE *idnfe = nfe->idnfe;
	DESTINATARIO *destinatario = nfe->destinatario;
	ENDERECO *endereco = destinatario->endereco;
	idnfe->serie = atoi(gtk_entry_get_text(priv->serie));
	idnfe->num_nf = atoi(gtk_entry_get_text(priv->num));
	idnfe->nat_op = gtk_entry_get_text(priv->nat_op);
	idnfe->ind_pag = atoi(gtk_combo_box_get_active_id(priv->forma_pagamento));
	idnfe->mod = MOD_NFe;
	idnfe->dh_emis = strtotime(gtk_entry_get_text(priv->dh_emis));
	time_t saida = strtotime(gtk_entry_get_text(priv->dh_saida));
	idnfe->dh_saida = saida == -1? NULL:&saida;
	idnfe->municipio->cod_uf = atoi(gtk_combo_box_get_active_id(priv->uf));
	idnfe->municipio->codigo = atoi(gtk_combo_box_get_active_id(priv->municipio));

	destinatario->tipo_doc = gtk_combo_box_get_active_id(priv->t_doc);
	destinatario->cnpj = gtk_entry_get_text(priv->doc);
	destinatario->nome = gtk_entry_get_text(priv->razao_social);
	destinatario->tipo_ie = atoi(gtk_combo_box_get_active_id(priv->tipo_contribuinte));
	destinatario->inscricao_estadual= gtk_entry_get_text(priv->ie);

	endereco->rua = gtk_entry_get_text(priv->logradouro);
	endereco->num = atoi(gtk_entry_get_text(priv->numero_endereco));
	endereco->complemento = gtk_entry_get_text(priv->complemento);
	endereco->bairro = gtk_entry_get_text(priv->bairro);
	endereco->cep = atoi(gtk_entry_get_text(priv->cep));
	endereco->municipio->cod_uf = atoi(gtk_combo_box_get_active_id(priv->uf_destinatario));
	endereco->municipio->codigo = atoi(gtk_combo_box_get_active_id(priv->municipio_destinatario));

	nfe->inf_ad_fisco = gtk_entry_get_text(priv->inf_ad_fisco);
	nfe->inf_ad_fisco = strlen(nfe->inf_ad_fisco) == 0? 
		NULL : nfe->inf_ad_fisco;
	nfe->inf_ad_contrib = gtk_entry_get_text(priv->inf_ad_contrib);
	nfe->inf_ad_contrib = strlen(nfe->inf_ad_contrib) == 0? 
		NULL : nfe->inf_ad_contrib;
	nfe->emitente->id = 1;
	register_nfe(nfe);
	gtk_widget_destroy(win);
	return 0;
}

static void inst_nfe_destinatario(gpointer p, NFEManager *nman){
	NFEManagerPrivate *priv;

	priv = nfe_manager_get_instance_private(nman);
	NFE *nfe = nman->nfe;
	DESTINATARIO *destinatario = nfe->destinatario;
	ENDERECO *endereco = destinatario->endereco;

	gtk_combo_box_set_active_id(priv->t_doc, 
		destinatario->tipo_doc);
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

static void lookup_destinatario(gpointer p,  NFEManager *nman){
	NFEManagerPrivate *priv;

	priv = nfe_manager_get_instance_private(nman);
	const char *doc = gtk_entry_get_text(priv->doc);
	DESTINATARIO *d = get_destinatario_by_doc(doc);
	
	if(d){
		free_destinatario(nman->nfe->destinatario);
		nman->nfe->destinatario = d;
		inst_nfe_destinatario(NULL, nman);
	}
	// Set id to 0 to save it with another id
	if(d)
		d->id = 0;
}

static void lookup_destinatario_focusout(gpointer p, GdkEvent *e,
		NFEManager *nman){
	lookup_destinatario(p, nman);
}

static void inst_nfe_manager(gpointer p, NFEManager *nman){
	NFEManagerPrivate *priv;

	priv = nfe_manager_get_instance_private(nman);
	NFE *nfe = nman->nfe;
	if(nfe->idnfe->id_nfe != 0){
		IDNFE *idnfe = nfe->idnfe;

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

		inst_nfe_destinatario(NULL, nman);
		gtk_widget_set_sensitive(GTK_WIDGET(priv->chave_nfe), TRUE);
		gtk_entry_set_text(priv->chave_nfe, nfe->idnfe->chave);
		if(nfe->inf_ad_fisco)
			gtk_entry_set_text(priv->inf_ad_fisco, nfe->inf_ad_fisco);
		if(nfe->inf_ad_contrib)
			gtk_entry_set_text(priv->inf_ad_contrib, nfe->inf_ad_contrib);
	}
}

static void default_emis(NFEManagerPrivate *priv){
	time_t now;
	time(&now);
	char *today = timef(now, "%d/%m/%Y %H:%M:%S", 19);
	gtk_entry_set_text(priv->dh_emis, today);
	free(today);
}

static void default_nfe_number(NFEManagerPrivate *priv){
	int num, serie;
	get_next_nfe_number(&num, &serie);
	if(num == 0){
		num = 1;
		serie = 1;
	}
	gtk_entry_set_text(priv->num, itoa(num));
	gtk_entry_set_text(priv->serie, itoa(serie));
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
	g_signal_connect(priv->doc, "focus-out-event", 
		G_CALLBACK(lookup_destinatario_focusout), nman);
	g_signal_connect(priv->doc, "activate", 
		G_CALLBACK(lookup_destinatario), nman);
	g_signal_connect(G_OBJECT(priv->uf), "changed", G_CALLBACK(list_municipios), priv->municipio);
	g_signal_connect(G_OBJECT(priv->uf_destinatario), "changed", G_CALLBACK(list_municipios),
		priv->municipio_destinatario);
	list_uf(priv->uf);
	list_uf(priv->uf_destinatario);
	list_forma_pagamento(priv->forma_pagamento);
	list_tipo_doc(priv->t_doc);
	list_tipo_contribuinte(priv->tipo_contribuinte);
	default_emis(priv);
	default_nfe_number(priv);
}


static void nfe_manager_dispose(GObject *object){
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
		       	chave_nfe);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), NFEManager,
		       	inf_ad_fisco);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), NFEManager,
		       	inf_ad_contrib);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), NFEManager,
		       	treeview);
}

NFEManager *nfe_manager_new(LivrenfeWindow *win){
	return g_object_new (NFE_MANAGER_TYPE, "transient-for", win, "use-header-bar", FALSE, "skip-taskbar-hint", TRUE, NULL);
}
