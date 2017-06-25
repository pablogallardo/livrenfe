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

#include "gtk_common.h"
#include <libnfe/errno.h>
#include <glib.h>
#include <gtk/gtk.h>

int is_integer(GtkEntry *e, gpointer user_data){
	return g_regex_match_simple("^\\d+$", gtk_entry_get_text(e), 0, 0);
}

int is_float(GtkEntry *e, gpointer user_data){
	return g_regex_match_simple("^(\\d+,)?\\d+$", gtk_entry_get_text(e), 0, 0);
}

int is_date(GtkEntry *e, gpointer user_data){
	return g_regex_match_simple("^\\d{2}/\\d{2}/\\d{4} \\d{2}:\\d{2}:\\d{2}$", 
		gtk_entry_get_text(e), 0, 0);
}

int is_alpha(GtkEntry *e, gpointer user_data){
	return g_regex_match_simple("^\\w$", 
		gtk_entry_get_text(e), 0, 0);
}

void show_msg(char *msg, GtkWidget *parent){
	GtkDialogFlags flags = GTK_DIALOG_DESTROY_WITH_PARENT;
	GtkWidget *dialog = gtk_message_dialog_new (GTK_WINDOW(parent),
		flags, GTK_MESSAGE_ERROR, GTK_BUTTONS_CLOSE,
		msg);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
}

int validate_integer(GtkEntry *e, char *msg, GtkWidget *w, gboolean empty_valid){
	if(empty_valid){
		GtkEntryBuffer *b = gtk_entry_get_buffer(e);
		int len = gtk_entry_buffer_get_length(b);
		if(len == 0)
			return 0;
	}
	if(!is_integer(e, NULL)){
		show_msg(msg, w);
		gtk_widget_grab_focus(GTK_WIDGET(e));
		return -EINVFIELD;
	}
	return 0;
}

int validate_float(GtkEntry *e, char *msg, GtkWidget *w, gboolean empty_valid){
	if(empty_valid){
		GtkEntryBuffer *b = gtk_entry_get_buffer(e);
		int len = gtk_entry_buffer_get_length(b);
		if(len == 0)
			return 0;
	}
	if(!is_float(e, NULL)){
		show_msg(msg, w);
		gtk_widget_grab_focus(GTK_WIDGET(e));
		return -EINVFIELD;
	}
	return 0;
}

int validate_date(GtkEntry *e, char *msg, GtkWidget *w, gboolean empty_valid){
	if(empty_valid){
		GtkEntryBuffer *b = gtk_entry_get_buffer(e);
		int len = gtk_entry_buffer_get_length(b);
		if(len == 0)
			return 0;
	}
	if(!is_date(e, NULL)){
		show_msg(msg, w);
		gtk_widget_grab_focus(GTK_WIDGET(e));
		return -EINVFIELD;
	}
	return 0;
}
