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

#include "nfe.h"
#include "livrenfe.h"
#include "utils.h"
#define _XOPEN_SOURCE
#include <time.h>
#include <stdlib.h>
#include <string.h>


static MUNICIPIO *new_municipio(){
	MUNICIPIO m = {};
	MUNICIPIO *p = malloc(sizeof(MUNICIPIO));
	memcpy(p, &m, sizeof(MUNICIPIO));
	return p;
}

static IDNFE *new_idnfe(){
	IDNFE i = {
		.municipio = new_municipio()
	};
	IDNFE *p = malloc(sizeof(IDNFE));
	memcpy(p, &i, sizeof(IDNFE));
	return p;
}

static PAIS *new_pais(){
	PAIS m = {};
	PAIS *p = malloc(sizeof(PAIS));
	memcpy(p, &m, sizeof(PAIS));
	return p;
}


static ENDERECO *new_endereco(){
	ENDERECO e = {
		.pais = new_pais(),
		.municipio = new_municipio()
	};
	ENDERECO *p = malloc(sizeof(ENDERECO));
	memcpy(p, &e, sizeof(ENDERECO));
	return p;
}

static EMITENTE *new_emitente(){
	EMITENTE e = {
		.endereco = new_endereco()
	};
	EMITENTE *p = malloc(sizeof(EMITENTE));
	memcpy(p, &e, sizeof(EMITENTE));
	return p;
}

static DESTINATARIO *new_destinatario(){
	DESTINATARIO d = {
		.endereco = new_endereco()
	};
	DESTINATARIO *p = malloc(sizeof(DESTINATARIO));
	memcpy(p, &d, sizeof(DESTINATARIO));
	return p;
}

static PRODUTO *new_produto(){
	PRODUTO m = {};
	PRODUTO *p = malloc(sizeof(PRODUTO));
	memcpy(p, &m, sizeof(PRODUTO));
	return p;
}

static ICMS *new_icms(){
	ICMS m = {};
	ICMS *p = malloc(sizeof(ICMS));
	memcpy(p, &m, sizeof(ICMS));
	return p;
}

static PIS *new_pis(){
	PIS m = {};
	PIS *p = malloc(sizeof(PIS));
	memcpy(p, &m, sizeof(PIS));
	return p;
}

static COFINS *new_cofins(){
	COFINS m = {};
	COFINS *p = malloc(sizeof(COFINS));
	memcpy(p, &m, sizeof(COFINS));
	return p;
}

static IMPOSTO *new_imposto(){
	IMPOSTO m = {
		.icms = new_icms(),
		.pis = new_pis(),
		.cofins = new_cofins()
	};
	IMPOSTO *p = malloc(sizeof(IMPOSTO));
	memcpy(p, &m, sizeof(IMPOSTO));
	return p;
}

static TRANSP *new_transp(){
	TRANSP m = {};
	TRANSP *p = malloc(sizeof(TRANSP));
	memcpy(p, &m, sizeof(TRANSP));
	return p;
}

static PROTOCOLO *new_protocolo(){
	PROTOCOLO m = {};
	PROTOCOLO *p = malloc(sizeof(PROTOCOLO));
	memcpy(p, &m, sizeof(PROTOCOLO));
	return p;
}

ITEM *new_item(){
	ITEM m = {
		.produto = new_produto(),
		.imposto = new_imposto()
	};
	ITEM *p = malloc(sizeof(ITEM));
	memcpy(p, &m, sizeof(ITEM));
	return p;
}
		
NFE *new_nfe(){
	NFE n = {
		.idnfe = new_idnfe(),
		.emitente = new_emitente(),
		.destinatario = new_destinatario(),
		//.itens = new_item(),
		//.transp = new_transp(),
		.protocolo = new_protocolo()
	};
	NFE *p = malloc(sizeof(NFE));
	memcpy(p, &n, sizeof(NFE));
	return p;
}

PRODUTO *inst_produto(int codigo, char *desc, unsigned int ncm, unsigned int cfop,
		char *unidade_comercial, float valor, PRODUTO *p){
	p->codigo = codigo;
	if(desc != NULL){
		p->descricao =  malloc(sizeof(char) * strlen(desc));
		strcpy(p->descricao, desc);
	}
	p->ncm = ncm;
	p->cfop = cfop;
	p->unidade_comercial = unidade_comercial;
	if(unidade_comercial != NULL){
		p->unidade_comercial =  malloc(sizeof(char) * strlen(unidade_comercial));
		strcpy(p->unidade_comercial, unidade_comercial);
	}
	p->valor = valor;
	return p;
}

ITEM *inst_item(float valor, float quantidade,
	       	unsigned int ordem, ITEM *i){
	i->valor = valor;
	i->ordem = ordem;
	i->quantidade = quantidade;
	i->pointer = NULL;
	return i;
}

ICMS *inst_icms(int origem, unsigned int tipo, float aliquota, float valor,
		ICMS *i){
	i->origem = origem;
	i->tipo = tipo;
	i->aliquota = aliquota;
	i->valor = valor;
	return i;
}

IMPOSTO *inst_imposto(ICMS *i, PIS *p, COFINS *c, IMPOSTO *imp){
	imp->icms = i;
	imp->pis = p;
	imp->cofins = c;
	return imp;
}

DESTINATARIO *inst_destinatario(char *rua, int num, char *complemento, 
		char *bairro, MUNICIPIO *m, unsigned int cep, PAIS *p,
		DESTINATARIO *d){
	ENDERECO *e = d->endereco;
	e->rua = rua;
	e->num = num;
	e->complemento = complemento;
	e->bairro = bairro;
	e->municipio = m;
	e->cep = cep;
	e->pais = p;
	return d;
}

int add_item(NFE *nfe, ITEM *item){
	ITEM *i;
	nfe->q_itens++;
	if((i = nfe->itens) == NULL){
		nfe->itens = item;
		return 0;
	}
	while(i->pointer != NULL){
		i = i->pointer;
	}
	i->pointer = item;
	return 0;
}

void set_chave(NFE *nfe){
	int i, j, div, sum = 0;
	char *base;
	sprintf(base, "%02d%s%14d%02d%03d%09d%s%08d",
		nfe->idnfe->municipio->cod_uf,
		timef(nfe->idnfe->dh_emis, "%Y%m"),
		nfe->emitente->cnpj,
		nfe->idnfe->mod,
		nfe->idnfe->serie,
		nfe->idnfe->num_nf,
		nfe->idnfe->tipo_emissao,
		nfe->idnfe->id_nfe);
	int m[7] = {2,3,4,5,6,7,8,9};
	char *baserev = strrev(base);
	for(i = 0, j = 7; i < strlen(baserev); i++, j++){
		sum += (baserev[i] - '0') * m[j%7];
	}
	div = 11 - (sum % 11);
	div = div == 1? 0:div;
	nfe->idnfe->div = itoa(div);
	nfe->idnfe->chave = malloc(sizeof(char) * (strlen(base) + 1));
	strcat(nfe->idnfe->chave, base);
	strcat(nfe->idnfe->chave, &(nfe->idnfe->div));
}

