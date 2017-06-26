/* Copyright (c) 2016 Pablo G. Gallardo <pggllrd@gmail.com>
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

#ifndef	ITEM_MANAGER_H
#define	ITEM_MANAGER_H

#include "nfe_manager.h"
#include <libnfe/libnfe.h>
#include <gtk/gtk.h>

#define ITEM_MANAGER_TYPE	(item_manager_get_type())
#define	ITEM_MANAGER(obj)	(G_TYPE_CHECK_INSTANCE_CAST((obj), ITEM_MANAGER_TYPE, ItemManager))

typedef struct _ItemManager ItemManager;
typedef struct _ItemManagerClass ItemManagerClass;

GType item_manager_get_type(void);
ItemManager *item_manager_new(NFEManager *win);

struct _ItemManager{
	GtkDialog parent;
	NFE *nfe;
};

#endif
