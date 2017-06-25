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

#ifndef	PREFS_DIALOG_H
#define	PREFS_DIALOG_H

#include "lnfe_window.h"
#include <gtk/gtk.h>

#define PREFS_TYPE	(prefs_get_type())
#define	PREFS(obj)	(G_TYPE_CHECK_INSTANCE_CAST((obj), PREFS_TYPE, Prefs))

typedef struct _Prefs Prefs;
typedef struct _PrefsClass PrefsClass;

GType pregs_get_type(void);
Prefs *prefs_new(LivrenfeWindow *win);

struct _Prefs{
	GtkDialog parent;
};

#endif
