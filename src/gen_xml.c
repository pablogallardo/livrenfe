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

#include "gen_xml.h"
#include "sign.h"
#include "errno.h"
#include "db_interface.h"
#include <stdio.h>
#include <string.h>
#include <libxml/xmlwriter.h>

int gen_inf_nfe(xmlTextWriterPtr, NFE *);
int _gen_ide(xmlTextWriterPtr, NFE *);
int _gen_emit(xmlTextWriterPtr, NFE *);
int _gen_dest(xmlTextWriterPtr, NFE *);
int _gen_det(xmlTextWriterPtr, ITEM *);
int _gen_prod(xmlTextWriterPtr, ITEM *);
int _gen_imposto(xmlTextWriterPtr, IMPOSTO *, float);
int _gen_total(xmlTextWriterPtr, float );
static char *generate_evento_xml(EVENTO *e, char *password);

char *gen_cons_status(int ambiente, int cuf){
	int rc;
	xmlTextWriterPtr writer;
	xmlDocPtr doc;
	xmlBufferPtr buf = xmlBufferCreate();
	
	writer = xmlNewTextWriterDoc(&doc, 0);
	if (writer == NULL)
		return NULL;
	xmlTextWriterStartDocument(writer, NULL, "utf-8", NULL);
	rc = xmlTextWriterStartElement(writer, BAD_CAST "consStatServ");
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns",
			BAD_CAST "http://www.portalfiscal.inf.br/nfe");
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "versao",
			BAD_CAST NFE_VERSAO);
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "tpAmb",
			"%d", ambiente);
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "cUF",
			"%d", cuf);
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "xServ",
			"%s", "STATUS");
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return NULL;
	xmlTextWriterEndDocument(writer);
	xmlNodeDump(buf, NULL, xmlDocGetRootElement(doc), 0, 0);
	return buf->content;
}

char *gen_cons_nfe(LOTE *lote, int ambiente){
	int rc;
	xmlTextWriterPtr writer;
	xmlDocPtr doc;
	xmlBufferPtr buf = xmlBufferCreate();
	
	writer = xmlNewTextWriterDoc(&doc, 0);
	if (writer == NULL)
		return NULL;
	xmlTextWriterStartDocument(writer, NULL, "utf-8", NULL);
	rc = xmlTextWriterStartElement(writer, BAD_CAST "consReciNFe");
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns",
			BAD_CAST "http://www.portalfiscal.inf.br/nfe");
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "versao",
			BAD_CAST NFE_VERSAO);
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "tpAmb",
			"%d", ambiente);
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "nRec",
			"%s", lote->recibo);
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return NULL;
	xmlTextWriterEndDocument(writer);
	xmlNodeDump(buf, NULL, xmlDocGetRootElement(doc), 0, 0);
	return buf->content;
}

char *gen_lote_xml(LOTE *lote, char *password){
	int rc;
	xmlTextWriterPtr writer;
	xmlDocPtr doc;
	xmlBufferPtr buf = xmlBufferCreate();

	writer = xmlNewTextWriterDoc(&doc, 0);
	if (writer == NULL)
		return NULL;
	xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);
	rc = xmlTextWriterStartElement(writer, BAD_CAST "enviNFe");
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns",
			BAD_CAST "http://www.portalfiscal.inf.br/nfe");
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "versao",
			BAD_CAST NFE_VERSAO);
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "idLote",
			"%d", lote->id);
	if (rc < 0)
		return NULL;
	int indSinc = lote->qtd == 1? 1 : 0;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "indSinc",
			"%d", indSinc);
	if (rc < 0)
		return NULL;
	int i;
	LOTE_ITEM *it = lote->nfes;
	for (i = 0; i < lote->qtd; i++){
		char *xml;
		xml = generate_xml(it->nfe, password);
		rc = xmlTextWriterWriteRaw(writer, BAD_CAST xml);
		if (rc < 0)
			return NULL;
		it = it->next;
	}
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return NULL;
	xmlTextWriterEndDocument(writer);
	xmlNodeDump(buf, NULL, xmlDocGetRootElement(doc), 0, 0);
	return buf->content;
}

char *generate_xml(NFE *nfe, char *password) {
	int rc;
	xmlTextWriterPtr writer;
	xmlDocPtr doc;
	xmlBufferPtr buf = xmlBufferCreate();

	writer = xmlNewTextWriterDoc(&doc, 0);
	if (writer == NULL)
		return NULL;
	xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);
	rc = gen_inf_nfe(writer, nfe);
	if (rc < 0)
		return NULL;
	xmlTextWriterEndDocument(writer);
	char *URI = malloc(sizeof(char) * strlen(nfe->idnfe->chave) +
		strlen(ID_PREFIX) + 2);
	strcpy(URI, "#");
	strcat(URI, ID_PREFIX);
	strcat(URI, nfe->idnfe->chave);
	sign_xml(doc, password, URI);
	xmlNodeDump(buf, NULL, xmlDocGetRootElement(doc), 0, 0);
	nfe->xml = strdup(buf->content);
	return buf->content;
}


int gen_inf_nfe(xmlTextWriterPtr writer, NFE *nfe){
	int rc;
	rc = xmlTextWriterStartElement(writer, BAD_CAST "NFe");
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns",
			BAD_CAST "http://www.portalfiscal.inf.br/nfe");	
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterStartElement(writer, BAD_CAST "infNFe");
	if (rc < 0)
		return -EXML;
	char *id = malloc(strlen(nfe->idnfe->chave) + strlen(ID_PREFIX) + 1);
	strcpy(id, ID_PREFIX);
	strcat(id, nfe->idnfe->chave);
	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "Id",
			BAD_CAST id);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "versao",
			BAD_CAST NFE_VERSAO);
	if (rc < 0)
		return -EXML;

	rc = _gen_ide(writer, nfe);
	if (rc < 0)
		return -EXML;

	rc = _gen_emit(writer, nfe);
	if (rc < 0)
		return -EXML;

	rc = _gen_dest(writer, nfe);
	if (rc < 0)
		return -EXML;

	ITEM *item = nfe->itens;
	float valor = 0;
	int i ;
	for (i = 0; i < nfe->q_itens; i++){
		rc = _gen_det(writer, item);
		if (rc < 0)
			return -EXML;
		valor += nfe->itens->produto->valor;
		item = item->pointer;
	}

	rc = _gen_total(writer, valor);
	if (rc < 0)
		return -EXML;

	rc = xmlTextWriterStartElement(writer, BAD_CAST "transp");
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "modFrete",
		//	"%d", nfe->transp->modfrete);
			"%d", 0);
	if (rc < 0)
		return -EXML;

	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return -EXML;

	if(nfe->inf_ad_fisco || nfe->inf_ad_contrib){

		rc = xmlTextWriterStartElement(writer, BAD_CAST "infAdic");
		if (rc < 0)
			return -EXML;
		if(nfe->inf_ad_fisco){
			rc = xmlTextWriterWriteFormatElement(writer, 
					BAD_CAST "infAdFisco",
					"%s", nfe->inf_ad_fisco);
			if (rc < 0)
				return -EXML;
		}

		if(nfe->inf_ad_contrib){
			rc = xmlTextWriterWriteFormatElement(writer, 
					BAD_CAST "infCpl",
					"%s", nfe->inf_ad_contrib);
			if (rc < 0)
				return -EXML;
		}

		rc = xmlTextWriterEndElement(writer);
		if (rc < 0)
			return -EXML;
		
	}

	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return -EXML;

	//rc = xmlTextWriterEndElement(writer);
	//if (rc < 0)
	//	return -EXML;

	return 0;
}

int _gen_ide(xmlTextWriterPtr writer, NFE *nfe){

	int rc;

	rc = xmlTextWriterStartElement(writer, BAD_CAST "ide");
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "cUF",
			"%d", nfe->idnfe->municipio->cod_uf);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "cNF",
			"%08d", nfe->idnfe->cod_nfe);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "natOp",
			"%s", nfe->idnfe->nat_op);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "indPag",
			"%d", nfe->idnfe->ind_pag);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "mod",
			"%d", nfe->idnfe->mod);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "serie",
			"%d", nfe->idnfe->serie);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "nNF",
			"%d", nfe->idnfe->num_nf);
	if (rc < 0)
		return -EXML;

	char buffer[26];
	struct tm *tm_info;
	tm_info = localtime(&(nfe->idnfe->dh_emis));
	strftime(buffer, 26, "%Y-%m-%dT%H:%M:%S-03:00", tm_info);

	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "dhEmi",
			"%s", buffer);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "tpNF",
			"%d", nfe->idnfe->tipo);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "idDest",
			"%d", nfe->idnfe->local_destino);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "cMunFG",	
			"%d", nfe->idnfe->municipio->codigo);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "tpImp",	
			"%d", nfe->idnfe->tipo_impressao);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "tpEmis",
			"%d", nfe->idnfe->tipo_emissao);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "cDV",
			"%s", &(nfe->idnfe->div));
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "tpAmb",
			"%d", nfe->idnfe->tipo_ambiente);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "finNFe",
			"%d", nfe->idnfe->finalidade);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "indFinal",
			"%d", nfe->idnfe->consumidor_final);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "indPres",
			"%d", nfe->idnfe->presencial);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "procEmi",
			"%d", 0);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "verProc",
			"%s", nfe->idnfe->versao);
	if (rc < 0)
		return -EXML;

	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return -EXML;
	return 0;
}

int _gen_emit(xmlTextWriterPtr writer, NFE *nfe){

	int rc;

	rc = xmlTextWriterStartElement(writer, BAD_CAST "emit");
	if (rc < 0)
		return -EXML;

	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "CNPJ",
			"%s", nfe->emitente->cnpj);
	if (rc < 0)
		return -EXML;

	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "xNome",
			"%s", nfe->emitente->nome);
	if (rc < 0)
		return -EXML;

	rc = xmlTextWriterStartElement(writer, BAD_CAST "enderEmit");
	if (rc < 0)
		return -EXML;

	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "xLgr",
			"%s", nfe->emitente->endereco->rua);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "nro",
			"%d", nfe->emitente->endereco->num);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "xBairro",
			"%s", nfe->emitente->endereco->bairro);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "cMun",
			"%d", nfe->emitente->endereco->municipio->codigo);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "xMun",
			"%s", nfe->emitente->endereco->municipio->municipio);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "UF",
			"%s", nfe->emitente->endereco->municipio->uf);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "CEP",
			"%d", nfe->emitente->endereco->cep);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "cPais",
			"%d", nfe->emitente->endereco->pais->codigo);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "xPais",
			"%s", nfe->emitente->endereco->pais->nome);
	if (rc < 0)
		return -EXML;

	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return -EXML;

	if (nfe->emitente->inscricao_estadual) {
		rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "IE",
				"%s", nfe->emitente->inscricao_estadual);
		if (rc < 0)
			return -EXML;
	}

	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "CRT",
			"%d", nfe->emitente->crt);
	if (rc < 0)
		return -EXML;

	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return -EXML;
	return 0;
}

int _gen_dest(xmlTextWriterPtr writer, NFE *nfe){

	int rc;

	rc = xmlTextWriterStartElement(writer, BAD_CAST "dest");
	if (rc < 0)
		return -EXML;

	if (strlen(nfe->destinatario->tipo_doc) == 3)
		rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "CPF",
				"%s", nfe->destinatario->id);
	else
		rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "CNPJ",
				"%s", nfe->destinatario->cnpj);
	if (rc < 0)
		return -EXML;

	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "xNome",
			"%s", nfe->destinatario->nome);
	if (rc < 0)
		return -EXML;

	rc = xmlTextWriterStartElement(writer, BAD_CAST "enderDest");
	if (rc < 0)
		return -EXML;

	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "xLgr",
			"%s", nfe->destinatario->endereco->rua);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "nro",
			"%d", nfe->destinatario->endereco->num);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "xBairro",
			"%s", nfe->destinatario->endereco->bairro);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "cMun",
			"%d", nfe->destinatario->endereco->municipio->codigo);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "xMun",
			"%s", nfe->destinatario->endereco->municipio->municipio);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "UF",
			"%s", nfe->destinatario->endereco->municipio->uf);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "cPais",
			"%d", nfe->destinatario->endereco->pais->codigo);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "xPais",
			"%s", nfe->destinatario->endereco->pais->nome);
	if (rc < 0)
		return -EXML;

	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return -EXML;

	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "indIEDest",
			"%d", 9);
	if (rc < 0)
		return -EXML;
/*TODO	if (nfe->destinatario->inscricao_estadual) {
		rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "IE",
				"%s", nfe->emitente->inscricao_estadual);
		if (rc < 0)
			return -EXML;
	}*/

	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return -EXML;
	return 0;
}

int _gen_det(xmlTextWriterPtr writer, ITEM *item){
	int rc;

	rc = xmlTextWriterStartElement(writer, BAD_CAST "det");
	if (rc < 0)
		return -EXML;
	char *ordem = malloc(sizeof(char) * 2);
	sprintf(ordem, "%d", item->ordem);
	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "nItem",
			BAD_CAST ordem);
	if (rc < 0)
		return -EXML;

	rc = _gen_prod(writer, item);
	if (rc < 0)
		return -EXML;
	rc = _gen_imposto(writer, item->imposto, item->produto->valor);
	if (rc < 0)
		return -EXML;

	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return -EXML;

	return 0;
}

int _gen_prod(xmlTextWriterPtr writer, ITEM *i){
	PRODUTO *p = i->produto;
	int rc;
	rc = xmlTextWriterStartElement(writer, BAD_CAST "prod");
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "cProd",
			"%d", p->codigo);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterStartElement(writer, BAD_CAST "cEAN");
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "xProd",
			"%s", p->descricao);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "NCM",
			"%d", p->ncm);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "CFOP",
			"%d", p->cfop);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "uCom",
			"%s", p->unidade_comercial);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "qCom",
			"%.4f", (float) i->quantidade);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "vUnCom",
			"%.10f", p->valor);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "vProd",
			"%.2f", p->valor);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterStartElement(writer, BAD_CAST "cEANTrib");
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "uTrib",
			"%s", p->unidade_comercial);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "qTrib",
			"%.4f", (float)i->quantidade);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "vUnTrib",
			"%.10f", p->valor);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteElement(writer, BAD_CAST "indTot",
			"1");
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return -EXML;
	return 0;
}

int _gen_imposto(xmlTextWriterPtr writer, IMPOSTO *i, float v){

	int rc;
	rc = xmlTextWriterStartElement(writer, BAD_CAST "imposto");
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterStartElement(writer, BAD_CAST "ICMS");
	if (rc < 0)
		return -EXML;
	switch(i->icms->tipo){
		case 101:
			rc = xmlTextWriterStartElement(writer, 
				BAD_CAST "ICMSSN101");
			if (rc < 0)
				return -EXML;
			rc = xmlTextWriterWriteFormatElement(writer, 
				BAD_CAST "orig",
					"%d", i->icms->origem);
			if (rc < 0)
				return -EXML;
			rc = xmlTextWriterWriteFormatElement(writer, 
				BAD_CAST "CSOSN",
					"%d", i->icms->tipo);
			if (rc < 0)
				return -EXML;
			rc = xmlTextWriterWriteFormatElement(writer, 
				BAD_CAST "pCredSN",
					"%.4f", i->icms->aliquota);
			if (rc < 0)
				return -EXML;
			float valor_icms = (v) * (i->icms->aliquota);
			rc = xmlTextWriterWriteFormatElement(writer, 
				BAD_CAST "vCredICMSSN",
					"%.2f", i->icms->aliquota);
			if (rc < 0)
				return -EXML;
			break;
		case 102:
		default:
			rc = xmlTextWriterStartElement(writer, 
				BAD_CAST "ICMSSN102");
			if (rc < 0)
				return -EXML;
			rc = xmlTextWriterWriteFormatElement(writer, 
				BAD_CAST "orig",
					"%d", i->icms->origem);
			if (rc < 0)
				return -EXML;
			rc = xmlTextWriterWriteFormatElement(writer, 
				BAD_CAST "CSOSN",
					//"%d", i->icms->tipo);
					"%d", 102);
			if (rc < 0)
				return -EXML;
	
			break;
	}
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return -EXML;

	rc = xmlTextWriterStartElement(writer, BAD_CAST "IPI");
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, 
		BAD_CAST "clEnq",
			"%s", i->ipi->classe);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, 
		BAD_CAST "cEnq",
			"%s", i->ipi->codigo);
	if (rc < 0)
		return -EXML;

	rc = xmlTextWriterStartElement(writer, BAD_CAST "IPINT");
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, 
		BAD_CAST "CST",
			"%d", i->ipi->sit_trib);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return -EXML;

	rc = xmlTextWriterStartElement(writer, BAD_CAST "PIS");
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterStartElement(writer, BAD_CAST "PISNT");
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "CST",
			"%02d", 8);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return -EXML;

	rc = xmlTextWriterStartElement(writer, BAD_CAST "COFINS");
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterStartElement(writer, BAD_CAST "COFINSNT");
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "CST",
			"%02d", 8);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return -EXML;
	
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return -EXML;

	return 0;
}

int _gen_total(xmlTextWriterPtr writer, float v){

	int rc;

	rc = xmlTextWriterStartElement(writer, BAD_CAST "total");
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterStartElement(writer, BAD_CAST "ICMSTot");
	if (rc < 0)
		return -EXML;

	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "vBC",
			"%.2f", 0.0);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "vICMS",
			"%.2f", 0.0);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "vICMSDeson",
			"%.2f", 0.0);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "vFCPUFDest",
			"%.2f", 0.0);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "vICMSUFDest",
			"%.2f", 0.0);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "vICMSUFRemet",
			"%.2f", 0.0);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "vBCST",
			"%.2f", 0.0);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "vST",
			"%.2f", 0.0);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "vProd",
			"%.2f", v);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "vFrete",
			"%.2f", 0.0);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "vSeg",
			"%.2f", 0.0);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "vDesc",
			"%.2f", 0.0);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "vII",
			"%.2f", 0.0);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "vIPI",
			"%.2f", 0.0);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "vPIS",
			"%.2f", 0.0);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "vCOFINS",
			"%.2f", 0.0);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "vOutro",
			"%.2f", 0.0);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "vNF",
			"%.2f", v);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "vTotTrib",
			"%.2f", 0.0);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return -EXML;
	return 0;
}

char *gen_lote_evento_xml(LOTE_EVENTO *lote, char *password){
	int rc;
	xmlTextWriterPtr writer;
	xmlDocPtr doc;
	xmlBufferPtr buf = xmlBufferCreate();

	writer = xmlNewTextWriterDoc(&doc, 0);
	if (writer == NULL)
		return NULL;
	xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);
	rc = xmlTextWriterStartElement(writer, BAD_CAST "envEvento");
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns",
			BAD_CAST "http://www.portalfiscal.inf.br/nfe");
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "versao",
			BAD_CAST "1.00");
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "idLote",
			"%d", lote->id);
	if (rc < 0)
		return NULL;

	int i;
	LOTE_EVENTO_ITEM *it = lote->eventos;
	for (i = 0; i < lote->qtd; i++){
		char *xml;
		xml = generate_evento_xml(it->evento, password);
		rc = xmlTextWriterWriteRaw(writer, BAD_CAST xml);
		if (rc < 0)
			return NULL;
		rc = xmlTextWriterEndElement(writer);
		if (rc < 0)
			return NULL;

		it = it->next;
	}

	xmlTextWriterEndDocument(writer);
	xmlNodeDump(buf, NULL, xmlDocGetRootElement(doc), 0, 0);
	return buf->content;

}

char *generate_evento_xml(EVENTO *e, char *password) {
	int rc;
	xmlTextWriterPtr writer;
	xmlDocPtr doc;
	xmlBufferPtr buf = xmlBufferCreate();

	NFE *nfe = e->nfe;

	writer = xmlNewTextWriterDoc(&doc, 0);
	if (writer == NULL)
		return NULL;
	xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);
	rc = xmlTextWriterStartElement(writer, BAD_CAST "evento");
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns",
			BAD_CAST "http://www.portalfiscal.inf.br/nfe");
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "versao",
			BAD_CAST "1.00");
	if (rc < 0)
		return NULL;

	rc = xmlTextWriterStartElement(writer, BAD_CAST "infEvento");
	if (rc < 0)
		return NULL;

	char id[70];
	sprintf(id, "ID%d%s%02d", e->type, nfe->idnfe->chave, e->seq);

	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "Id",
			BAD_CAST id);
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "cOrgao",
			"%d", nfe->idnfe->municipio->cod_uf);
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "tpAmb",
			"%d", nfe->idnfe->tipo_ambiente);
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "CNPJ",
			"%s", nfe->emitente->cnpj);
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "chNFe",
			"%s", nfe->idnfe->chave);
	if (rc < 0)
		return NULL;
	time_t now;
	char buffer[26];
	struct tm *tm_info;
	time(&now);
	tm_info = localtime(&(now));
	strftime(buffer, 26, "%Y-%m-%dT%H:%M:%S-02:00", tm_info);

	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "dhEvento",
			"%s", buffer);
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "tpEvento",
			"%d", e->type);
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "nSeqEvento",
			"%d", e->seq);
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "verEvento",
			"%s", "1.00");
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterStartElement(writer, BAD_CAST "detEvento");
	if (rc < 0)
		return NULL;

	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "versao",
			BAD_CAST "1.00");
	if (rc < 0)
		return NULL;

	switch(e->type){
		case CANCELAMENTO_TYPE: {
			EVENTO_CANCELAMENTO *ec;
			ec = (EVENTO_CANCELAMENTO *) e;
			rc = xmlTextWriterWriteFormatElement(writer, 
				BAD_CAST "descEvento",
				"%s", "Cancelamento");
			if (rc < 0)
				return NULL;
			rc = xmlTextWriterWriteFormatElement(writer, 
				BAD_CAST "nProt",
				"%s", nfe->protocolo->numero);
			if (rc < 0)
				return NULL;
			rc = xmlTextWriterWriteFormatElement(writer, 
				BAD_CAST "xJust",
				"%s", ec->justificativa);
			if (rc < 0)
				return NULL;
			break;	
		}
	}

	
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return NULL;
	xmlTextWriterEndDocument(writer);
	char *URI = malloc(70);
	strcpy(URI, "#");
	strcat(URI, id);
	sign_xml(doc, password, URI);
	xmlNodeDump(buf, NULL, xmlDocGetRootElement(doc), 0, 0);
	return buf->content;
}

char *get_versao(char *service){
	char *versao = malloc(sizeof(char) * 5);
	int id_versao = get_url_id(service);
	switch(id_versao){
		case 1:
			versao = "1.00";
			break;
		case 5:
		case 6:
		case 7:
		default:
			versao = "3.10";
			break;
	}
	return versao;
}
