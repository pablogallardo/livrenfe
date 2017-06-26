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

#ifndef	NFE_MANAGER_H
#define	NFE_MANAGER_H

#include "livrenfe.h"
#include "lnfe_window.h"
#include <libnfe/libnfe.h>
#include <gtk/gtk.h>

#define NFE_MANAGER_TYPE	(nfe_manager_get_type())
#define	NFE_MANAGER(obj)	(G_TYPE_CHECK_INSTANCE_CAST((obj), NFE_MANAGER_TYPE, NFEManager))

typedef struct _NFEManager NFEManager;
typedef struct _NFEManagerClass NFEManagerClass;

struct _NFEManager{
	GtkDialog parent;
	NFE *nfe;
};

GType nfe_manager_get_type(void);
NFEManager *nfe_manager_new(LivrenfeWindow *win);

#endif
