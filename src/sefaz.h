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

#ifndef	SEFAZ_H
#define	SEFAZ_H

#include "livrenfe.h"

#define	SEFAZ_STATUS_OK	107

extern int get_status_servico(int ambiente, int cuf, char *passwd, char **msg);

extern int send_lote(LOTE *lote, int ambiente, char *passwd, char **msg);

extern int cons_lote(LOTE *, int ambiente);

extern int canc_nfe(NFE *, int ambiente);

#endif
