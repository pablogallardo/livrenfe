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

#include <libnfe/libnfe.h>
#include <openssl/x509.h>
#include <openssl/evp.h>

/**
 * This function generates the XML for the NFE WITH signature 
 */
extern char *generate_xml(NFE *, EVP_PKEY *key, X509 *cert);

/**
 * This function generates the XML for emitting to SEFAZ
 */
extern char *gen_lote_xml(LOTE *, EVP_PKEY *key, X509 *cert);

/**
 * This function generates the events XML used for canceling, correct, etc
 * sent NFe
 */
extern char *gen_lote_evento_xml(LOTE_EVENTO *, EVP_PKEY *key, X509 *cert);

/**
 * XML for consulting service status
 */
extern char *gen_cons_status(int ambiente, int cuf);

/**
 * XML for  consulting NFE status
 */
extern char *gen_cons_nfe(LOTE *, int ambiente);

/**
 * Gerar XML para exportação da NFE
 */
extern char *gen_export_nfe_xml(NFE *nfe);

/**
 * Get XML message version for service
 */
extern char *get_versao(sefaz_servico_t service);

#endif
