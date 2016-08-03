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

#ifndef	DBI_H
#define	DBI_H

#define DB_VERSION	1

#include "livrenfe.h"
#include <gtk/gtk.h>

/**
 * Creates an empty app DB
 */
extern int create_db();

/**
 * Materialize NFE
 */
extern int register_nfe(t_nfe *);

/**
 * Set db path
 */
extern void set_db(char *);

/**
 * Get opened NFEs for GTKTreeView
 */
extern GtkListStore *get_list_nfe();

#endif
