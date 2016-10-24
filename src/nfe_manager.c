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
#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>

struct _NFEManager{
	GtkDialog parent;
	NFE *nfe;
};

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
	GtkComboBoxText *forma_pagamento;
	GtkComboBoxText *t_doc;
	GtkComboBoxText *tipo_contribuinte;
};

G_DEFINE_TYPE_WITH_PRIVATE(NFEManager, nfe_manager, GTK_TYPE_DIALOG)

static void list_forma_pagamento(GtkComboBoxText *fp){
	char *a;
	a = malloc(20);
	sprintf(a, "%d", A_VISTA);
	gtk_combo_box_text_append(fp, a, "A vista");
	sprintf(a, "%d", A_PRAZO);
	gtk_combo_box_text_append(fp, a, "A prazo");
	sprintf(a, "%d", OUTRO);
	gtk_combo_box_text_append(fp, a, "Outro");
}

static void list_tipo_contribuinte(GtkComboBoxText *t){
	char *a;
	a = malloc(20);
	sprintf(a, "%d", CONT_AV);
	gtk_combo_box_text_append(t, a, "Contribuinte ICMS");
	sprintf(a, "%d", CONT_IS);
	gtk_combo_box_text_append(t, a, "Contribuinte isento");
	sprintf(a, "%d", NAO_CONT);
	gtk_combo_box_text_append(t, a, "Não contribuinte");
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

static void item_manager_activate(GtkButton *b, gpointer win){
	ItemManager *iman;

	iman = item_manager_new(NFE_MANAGER(win));
	NFE *n = malloc(sizeof(NFE));
	n->q_itens;
	n->itens = NULL;
	(NFE_MANAGER(win))->nfe = n;
	iman->nfe = n;
	gtk_window_present(GTK_WINDOW(iman));
}

static void nfe_manager_init(NFEManager *nman){
	NFEManagerPrivate *priv;

	priv = nfe_manager_get_instance_private(nman);
	gtk_widget_init_template(GTK_WIDGET(nman));
	g_signal_connect(priv->novo_item, "clicked", G_CALLBACK(item_manager_activate),
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
}

NFEManager *nfe_manager_new(LivrenfeWindow *win){
	return g_object_new (NFE_MANAGER_TYPE, "transient-for", win, "use-header-bar", FALSE, "skip-taskbar-hint", TRUE, NULL);
}
