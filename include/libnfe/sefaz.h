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

#ifndef	SEFAZ_H
#define	SEFAZ_H

#include <libnfe/libnfe.h>
#include <openssl/x509.h>
#include <openssl/evp.h>

#define	SEFAZ_STATUS_OK	107

extern int get_status_servico(int ambiente, char *URL, int cuf, EVP_PKEY *, 
	X509 *, char **msg);

extern int send_lote(LOTE *lote, char *URL, int ambiente, int cuf, EVP_PKEY *, 
	X509 *, char **msg);

extern int send_lote_evento(LOTE_EVENTO *lote, char *URL, int ambiente, int cuf,
	EVP_PKEY *, X509 *, char **msg);

extern int cons_lote(LOTE *, char *URL, int ambiente, int cuf, EVP_PKEY *, 
	X509 *, char **msg);

#endif
