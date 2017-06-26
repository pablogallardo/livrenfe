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

#ifndef	EMITENTE_MANAGER_H
#define	EMITENTE_MANAGER_H

#include "lnfe_window.h"
#include <gtk/gtk.h>

#define EMITENTE_MANAGER_TYPE	(emitente_manager_get_type())
#define	EMITENTE_MANAGER(obj)	(G_TYPE_CHECK_INSTANCE_CAST((obj), EMITENTE_MANAGER_TYPE, EmitenteManager))

typedef struct _EmitenteManager EmitenteManager;
typedef struct _EmitenteManagerClass EmitenteManagerClass;

GType emitente_manager_get_type(void);
EmitenteManager *emitente_manager_new(LivrenfeWindow *win);

struct _EmitenteManager{
	GtkDialog parent;
};

#endif
