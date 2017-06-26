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

#ifndef	SEFAZ_RESPONSE_H	
#define	SEFAZ_RESPONSE_H	

#include "lnfe_window.h"
#include <libnfe/libnfe.h>
#include <gtk/gtk.h>

#define SEFAZ_RESPONSE_TYPE	(sefaz_response_get_type())
#define	SEFAZ_RESPONSE(obj)	(G_TYPE_CHECK_INSTANCE_CAST((obj), SEFAZ_RESPONSE_TYPE, SefazResponse))

typedef struct _SefazResponse SefazResponse;
typedef struct _SefazResponseClass SefazResponseClass;

GType sefaz_response_get_type(void);
SefazResponse *sefaz_response_new(LivrenfeWindow *win);

struct _SefazResponse{
	GtkDialog parent;
	char *password;
	LOTE *lote;
	LOTE_EVENTO *lote_evento;
};

#endif
