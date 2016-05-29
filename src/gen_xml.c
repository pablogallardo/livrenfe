/* Copyright (c) 2016 Pablo G. Gallardo <pggllrd@gmail.com>
 *
 * This file is part of LivreNFE.
 *
 * LibreNFE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LibreNFE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LivreNFE.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "gen_xml.h"
#include <stdio.h>
#include <string.h>
#include <libxml/xmlwriter.h>

char *generate_xml(t_nfe *nfe) {
	int rc;
	xmlTextWriterPtr writer;
	xmlBufferPtr buf;

	buf = xmlBufferCreate();
	if (buf == NULL) 
		return NULL;
	rc = xmlTextWriterStartElement(writer, BAD_CAST "nfeProc");
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "versao",
			BAD_CAST NFE_VERSAO);	
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterStartElement(writer, BAD_CAST "NFe");
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterStartElement(writer, BAD_CAST "infNFe");
	if (rc < 0)
		return NULL;
	char *id = malloc(strlen(nfe->idnfe.chave) + strlen(ID_PREFIX));
	strcpy(id, ID_PREFIX);
	strcat(id, nfe->idnfe.chave);
	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "Id",
		BAD_CAST nfe->idnfe.chave);
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "versao",
		BAD_CAST NFE_VERSAO);
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterStartElement(writer, BAD_CAST "ide");
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "cUF",
			"%d", nfe->idnfe.municipio.cod_uf);

	return buf->content;
}
