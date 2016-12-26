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

static void emitente_manager_dispose(GObject *object){
	EmitenteManagerPrivate *priv;

	priv = emitente_manager_get_instance_private(EMITENTE_MANAGER(object));
	G_OBJECT_CLASS(emitente_manager_parent_class)->dispose(object);
}


static void emitente_manager_init(EmitenteManager *eman){
	EmitenteManagerPrivate *priv;

	priv = emitente_manager_get_instance_private(eman);
	gtk_widget_init_template(GTK_WIDGET(eman));
}



static void emitente_manager_class_init(EmitenteManagerClass *class){
	G_OBJECT_CLASS (class)->dispose = emitente_manager_dispose;

	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class),
		"/br/com/lapagina/livrenfe/emitente_manager.ui");
}

EmitenteManager *emitente_manager_new(LivrenfeWindow *win){
	return g_object_new (EMITENTE_MANAGER_TYPE, "transient-for", win,
		"use-header-bar", FALSE, "skip-taskbar-hint", TRUE, NULL);
}
