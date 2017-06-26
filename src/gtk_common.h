/* Copyright (c) 2017 Pablo G. Gallardo <pggllrd@gmail.com>
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

#ifndef	GTK_COMMON_H
#define	GTK_COMMON_H

#include <gtk/gtk.h>

extern int is_integer(GtkEntry *e, gpointer user_data);

extern int is_float(GtkEntry *e, gpointer user_data);

extern int is_date(GtkEntry *e, gpointer user_data);

extern int is_alpha(GtkEntry *e, gpointer user_data);

extern void show_msg(char *msg, GtkWidget *parent);

extern int validate_integer(GtkEntry *e, char *msg, GtkWidget *win, gboolean empty_valid);

extern int validate_float(GtkEntry *e, char *msg, GtkWidget *win, gboolean empty_valid);

extern int validate_date(GtkEntry *e, char *msg, GtkWidget *win, gboolean empty_valid);

#endif
