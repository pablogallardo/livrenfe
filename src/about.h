/* Copyright (c) 2016, 2017 Pablo G. Gallardo <pggllrd@gmail.com>
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

#ifndef	ABOUT_H
#define	ABOUT_H

#include "lnfe_window.h"
#include <gtk/gtk.h>

#define ABOUT_LIVRENFE_TYPE	(about_livrenfe_get_type())
#define	ABOUT_LIVRENFE(obj)	(G_TYPE_CHECK_INSTANCE_CAST((obj), ABOUT_LIVRENFE_TYPE, AboutLivrenfe))

typedef struct _AboutLivrenfe AboutLivrenfe;
typedef struct _AboutLivrenfeClass AboutLivrenfeClass;

GType about_livrenfe_get_type(void);
AboutLivrenfe *about_livrenfe_new(LivrenfeWindow *win);

struct _AboutLivrenfe{
	GtkAboutDialog parent;
};

#endif
