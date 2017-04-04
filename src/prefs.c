/* Copyright (c) 2017 Pablo G. Gallardo <pggllrd@gmail.com>
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

#include "prefs.h"
#include "lnfe_window.h"
#include "db_interface.h"
#include "utils.h"
#include "nfe.h"
#include "livrenfe.h"
#include <gtk/gtk.h>
#include <stdlib.h>


struct _PrefsClass{
	GtkDialogClass parent_class;
};

typedef struct _PrefsPrivate PrefsPrivate;

struct _PrefsPrivate{
	GtkRadioButton *ambiente_p;
	GtkRadioButton *ambiente_h;
	GtkRadioButton *cert_a1;
	GtkRadioButton *cert_a2;

	GtkEntry *public_key;
	GtkEntry *private_key;
	GtkEntry *card_reader_lib;
	GtkEntry *recepcaoevento_prod;
	GtkEntry *recepcaoevento_cert;
	GtkEntry *nfeconsultacadastro_prod;
	GtkEntry *nfeconsultacadastro_cert;
	GtkEntry *nfeinutilizacao_prod;
	GtkEntry *nfeinutilizacao_cert;
	GtkEntry *nfeconsultaprotocolo_prod;
	GtkEntry *nfeconsultaprotocolo_cert;
	GtkEntry *nfestatusservico_prod;
	GtkEntry *nfestatusservico_cert;
	GtkEntry *nferetautorizacao_prod;
	GtkEntry *nferetautorizacao_cert;
	GtkEntry *nfeautorizacao_prod;
	GtkEntry *nfeautorizacao_cert;
};

G_DEFINE_TYPE_WITH_PRIVATE(Prefs, prefs, GTK_TYPE_DIALOG)


static void prefs_dispose(GObject *object){
	PrefsPrivate *priv;

	priv = prefs_get_instance_private(PREFS(object));
	G_OBJECT_CLASS(prefs_parent_class)->dispose(object);
}

static void prefs_init(Prefs *p){
	PrefsPrivate *priv;

	priv = prefs_get_instance_private(p);
	gtk_widget_init_template(GTK_WIDGET(p));
}



static void prefs_class_init(PrefsClass *class){
	G_OBJECT_CLASS (class)->dispose = prefs_dispose;
	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class),
		"/br/com/lapagina/livrenfe/prefs.ui");
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), 
		Prefs, ambiente_p);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), 
		Prefs, ambiente_h);
}

Prefs *prefs_new(LivrenfeWindow *win){
	return g_object_new (PREFS_TYPE, "transient-for", win,
		"use-header-bar", FALSE, "skip-taskbar-hint", TRUE, NULL);
}
