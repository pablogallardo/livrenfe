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

#include "sefaz.h"
#include "livrenfe.h"
#include "send.h"
#include "xml.h"
#include "gen_xml.h"
#include "errno.h"
#include <libxml/parser.h>
#include <string.h>

int get_status_servico(int ambiente, int cuf, char *passwd, char **msg){
	char *response, *status;
	int cStat;
	xmlDocPtr doc;
	char *xml = gen_cons_status(2, 35);
	response = send_sefaz("NfeStatusServico", 2, 35, 
		xml, passwd);
	if(response == NULL){
		*msg = strdup("Sem resposta do SEFAZ, tente novamente");
		return -ESEFAZ;
	}
	doc = xmlReadMemory(response, strlen(response), "noname.xml", NULL, 0);
	status = get_xml_element(doc, "nfe:cStat");
	if(status == NULL){
		return -ESEFAZ;	
	}
	cStat = atoi(status);
	char *motivo = get_xml_element(doc, "nfe:xMotivo");
	if(motivo == NULL){
		return -ESEFAZ;	
	}
	*msg = strdup(motivo);
	xmlFree(motivo);
	xmlFree(status);
	return cStat;
}

int send_lote(LOTE *lote, int ambiente, char *passwd, char **msg){
	char *response, *status;
	int cStat;
	xmlDocPtr doc;
	char *xml = gen_lote_xml(lote, passwd);
	response = send_sefaz("NFeAutorizacao", 2, 35, 
		xml, passwd);
	if(response == NULL){
		*msg = strdup("Sem resposta do SEFAZ, tente novamente");
		return -ESEFAZ;
	}
	doc = xmlReadMemory(response, strlen(response), "noname.xml", NULL, 0);
	status = get_xml_element(doc, "nfe:cStat");
	if(status == NULL){
		return -ESEFAZ;	
	}
	cStat = atoi(status);
	char *motivo = get_xml_element(doc, "nfe:xMotivo");
	if(motivo == NULL){
		return -ESEFAZ;	
	}
	*msg = strdup(motivo);
	char *nRec = get_xml_element(doc, "nfe:nRec");
	xmlFree(motivo);
	xmlFree(status);
	xmlFree(nRec);
	return cStat;
}
