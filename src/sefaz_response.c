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

#include "sefaz_response.h"
#include "lnfe_window.h"
#include "sefaz.h"
#include <gtk/gtk.h>
#include <pthread.h>
#include <stdlib.h>


struct _SefazResponseClass {
	GtkDialogClass parent_class;
};

typedef struct _SefazResponsePrivate SefazResponsePrivate;

struct _SefazResponsePrivate{
	GtkSpinner *spinner;
	GtkLabel *resposta;
	GtkButton *ok_btn;
	GtkOverlay *overlay;
};

G_DEFINE_TYPE_WITH_PRIVATE(SefazResponse, sefaz_response, GTK_TYPE_DIALOG)

static void sefaz_thread(void *arg){
	SefazResponse *sr = SEFAZ_RESPONSE(arg);
	SefazResponsePrivate *priv;
	pthread_t tid;

	priv = sefaz_response_get_instance_private(sr);
	char *msg = malloc(sizeof(char) * 255);
	int rc = get_status_servico(2, 35, sr->password, &msg);
	gtk_spinner_stop(priv->spinner);
	gtk_label_set_text(priv->resposta, msg);
	gtk_overlay_reorder_overlay(priv->overlay, priv->resposta, -1);
	free(msg);
}

static void get_response(SefazResponse *sr){
	SefazResponsePrivate *priv;
	pthread_t tid;

	priv = sefaz_response_get_instance_private(sr);
	gtk_spinner_start(priv->spinner);
	pthread_create(&tid, NULL, &sefaz_thread, sr);
}

static void ok_click(GtkButton *b, SefazResponse *sr){
	gtk_widget_destroy(sr);
}

static void sefaz_response_dispose(GObject *object){
	SefazResponsePrivate *priv;

	priv = sefaz_response_get_instance_private(SEFAZ_RESPONSE(object));
	G_OBJECT_CLASS(sefaz_response_parent_class)->dispose(object);
}


static void sefaz_response_init(SefazResponse *sr){
	SefazResponsePrivate *priv;

	priv = sefaz_response_get_instance_private(sr);
	gtk_widget_init_template(GTK_WIDGET(sr));
	g_signal_connect(sr, "show", G_CALLBACK(get_response),
			NULL);
	g_signal_connect(priv->ok_btn, "clicked", G_CALLBACK(ok_click),
			sr);

}



static void sefaz_response_class_init(SefazResponseClass *class){
	G_OBJECT_CLASS (class)->dispose = sefaz_response_dispose;

	gtk_widget_class_set_template_from_resource(GTK_WIDGET_CLASS(class),
                               "/br/com/lapagina/livrenfe/sefaz_response.ui");
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), 
		SefazResponse, spinner);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), 
		SefazResponse, resposta);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), 
		SefazResponse, ok_btn);
	gtk_widget_class_bind_template_child_private(GTK_WIDGET_CLASS(class), 
		SefazResponse, overlay);
}

SefazResponse *sefaz_response_new(LivrenfeWindow *win){
	return g_object_new (SEFAZ_RESPONSE_TYPE, "transient-for", win, 
		"use-header-bar", FALSE, "skip-taskbar-hint", TRUE, NULL);
}
