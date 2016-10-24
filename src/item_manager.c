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
	GtkComboBoxText *icms_regime;
	GtkComboBoxText *icms_situacao_tributaria;
	GtkComboBoxText *icms_origem;
	GtkComboBoxText *cofins_situacao_tributaria;
	GtkComboBoxText *pis_situacao_tributaria;
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

static void list_icms_regime(GtkComboBoxText *c){
	gtk_combo_box_text_append(c, "1", "Simples Nacional");
	gtk_combo_box_text_append(c, "2", "Simples Nacional – excesso de sublimite de receita bruta");
	gtk_combo_box_text_append(c, "3", "Regime Normal");
}

static void list_icms_st(GtkComboBoxText *c){
	gtk_combo_box_text_append(c, "101", "101 - Tributada pelo Simples Nacional com permissão de crédito");
}

static void list_icms_origem(GtkComboBoxText *fp){
	char *a;
	a = malloc(20);
	sprintf(a, "%d", NACIONAL);
	gtk_combo_box_text_append(fp, a, "Nacional");
	sprintf(a, "%d", E_ID);
	gtk_combo_box_text_append(fp, a, "Estrangeira - Importação direta");
	sprintf(a, "%d", E_AI);
	gtk_combo_box_text_append(fp, a, "Estrangeira - Adquirida no mercado interno");
}

static void list_cofins_st(GtkComboBoxText *c){
	gtk_combo_box_text_append(c, "08", "Operação sem incidência da contribuição");
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
