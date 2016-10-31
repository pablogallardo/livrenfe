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

#ifndef	LIVRENFE_WINDOW_H
#define	LIVRENFE_WINDOW_H

#include "livrenfe.h"
#include <gtk/gtk.h>

#define LIVRENFE_WINDOW_TYPE	(livrenfe_window_get_type())
#define	LIVRENFE_WINDOW(obj) (G_TYPE_CHECK_INSTANCE_CAST((obj), LIVRENFE_WINDOW_TYPE, LivrenfeWindow))

typedef struct _LivrenfeWindow LivrenfeWindow;
typedef struct _LivrenfeWindowClass LivrenfeWindowClass;

GType livrenfe_window_get_type (void);
LivrenfeWindow *livrenfe_window_new (Livrenfe *app);

void list_nfe(LivrenfeWindow *win);

#endif
