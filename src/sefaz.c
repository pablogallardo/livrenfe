/* Copyright (c) 2016 Pablo G. Gallardo <pggllrd@gmail.com>
 *
 * This file is part of LivreNFE.
 * * LivreNFE is free software: you can redistribute it and/or modify
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
#include "db_interface.h"
#include <libxml/parser.h>
#include <string.h>

int get_status_servico(int ambiente, int cuf, char *passwd, char **msg){ char *response, *status;
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

static int sefaz_response_protocolos(LOTE *lote, xmlDocPtr doc){
	int i;
	LOTE_ITEM *it = lote->nfes;
	for (i = 0; i < lote->qtd; i++){
		char *status, *motivo;
		int cStat;
		NFE *n = it->nfe;
		char *xp = malloc(sizeof(char) * 100);
		sprintf(xp, "//nfe:protNFe/nfe:infProt[nfe:chNFe='%s']/nfe:cStat", 
			n->idnfe->chave);
		status = get_xml_element(doc, xp);
		if(status == NULL){
			xmlFree(status);
			free(xp);
			return -ESEFAZ;	
		}
		cStat = atoi(status);
		xmlFree(status);
		n->protocolo->cod_status = cStat;

		sprintf(xp, "//nfe:protNFe/nfe:infProt[nfe:chNFe='%s']/nfe:xMotivo", 
			n->idnfe->chave);
		motivo = get_xml_element(doc, xp);
		n->protocolo->xmot = strdup(motivo);
		xmlFree(motivo);
		if(cStat == 100){
			char *nProt;
			sprintf(xp, "//nfe:protNFe/nfe:infProt[nfe:chNFe='%s']/nfe:nProt", 
				n->idnfe->chave);
			nProt = get_xml_element(doc, xp);
			n->protocolo->numero = strdup(nProt);
			sprintf(xp, "//nfe:protNFe/nfe:infProt[nfe:chNFe='%s']/..", 
				n->idnfe->chave);
			char *xml_prot = get_xml_subtree(doc, xp);
			n->protocolo->xml = strdup(xml_prot);
			xmlFree(nProt);
			xmlFree(xml_prot);
		}
		it = it->next;
		free(xp);
	}
}

int send_lote(LOTE *lote, int ambiente, char *passwd, char **msg){
	char *response, *status;
	int cStat, rc;
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
	fprintf(stdout, "Lote: %s\n", nRec);
	lote->recibo = strdup(nRec);
	lote->xml_response = strdup(response);
	xmlFree(motivo);
	xmlFree(status);
	xmlFree(nRec);

	rc = db_save_lote(lote);
	if(rc){
		*msg = malloc(sizeof(char) * 200);
		sprintf(*msg, "Erro ao salvar lote\nNúmero de recibo: %d",
			nRec);
		return -ESQL;
	}
	return cStat;
}

int cons_lote(LOTE *lote, int ambiente, char *passwd, char **msg){
	char *response, *status;
	int cStat, rc;
	xmlDocPtr doc;
	char *xml = gen_cons_nfe(lote, ambiente);
	response = send_sefaz("NFeRetAutorizacao", 2, 35, 
		xml, passwd);
	if(response == NULL){
		*msg = strdup("Sem resposta do SEFAZ, tente novamente");
		return -ESEFAZ;
	}
	fprintf(stdout, "%s\n", response);
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
	if(cStat == 104)
		sefaz_response_protocolos(lote, doc);
	db_save_lote(lote);
	*msg = strdup(motivo);
	xmlFree(motivo);
	xmlFree(status);

	return cStat;
}
