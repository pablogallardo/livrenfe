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

#ifndef	GEN_XML_H
#define	GEN_XML_H

/**
 * NFE Version used to generate XML
 */
#define NFE_VERSAO	"3.10"
/**
 * NFE prefix used on NF id
 */
#define	ID_PREFIX	"NFe"

#include "livrenfe.h"

/**
 * This function generate a basic XML for the NFE WITHOUT signature and
 * SEFAZ response (protocolo).
 */
extern char *generate_xml(NFE *);

/**
 * XML for consulting service status
 */
extern char *gen_cons_status(int ambiente, int cuf);

/**
 * XML for  consulting NFE status
 */
extern char *gen_cons_nfe(LOTE *, int ambiente);

#endif
