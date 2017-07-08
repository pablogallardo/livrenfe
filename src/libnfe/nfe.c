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

#include <libnfe/nfe.h>
#include <libnfe/libnfe.h>
#include "utils.h"
#define _XOPEN_SOURCE
#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
 *Funcao para instanciar struct pais_t
 * */
static
PAIS *new_pais(){
	PAIS *p = malloc(sizeof(PAIS));
	PAIS m = {
		.cPais = 1058,
		.xPais = "BRASIL"
	};
	memcpy(p, &m, sizeof(PAIS));
	return p;
}

/*
 *Funcao para instanciar struct uf_t
 * */
static
UF *new_uf(){
	UF *p = malloc(sizeof(UF));
	UF m = {
		.cUF = 0,
		.pais = new_pais()
	};
	memcpy(p, &m, sizeof(UF));
	return p;
}	
/*
 *Funcao para instanciar struct municipio_t
 **/
static 
MUNICIPIO *new_municipio(){
	MUNICIPIO *p = malloc(sizeof(MUNICIPIO));
	MUNICIPIO m = {
		.cMun = 0,
		.uf = new_uf()
	};
	memcpy(p, &m, sizeof(MUNICIPIO));
	return p;
}

/*
 *Funcao para instanciar struct endereco_t
 **/
static 
ENDERECO *new_endereco(){
	ENDERECO *p = malloc(sizeof(ENDERECO));
	ENDERECO m = {
		.CEP = 0,
		.Mun = new_municipio()
	};
	memcpy(p, &m, sizeof(ENDERECO));
	return p;
}




static IDNFE *new_idnfe(){
	IDNFE i = {
		.endereco = new_endereco(),
		.dh_saida = malloc(sizeof(time_t)),
		.cod_nfe = rand() % 99999999,
		.tipo_emissao = EMISSAO_NORMAL,
		.local_destino = OPERACAO_INTERNA,
		.tipo = SAIDA,
		.tipo_impressao = DANFE_NORMAL_RETRATO,
		.finalidade = NFe_NORMAL,
		.consumidor_final = CONSUMIDOR_FINAL
	};
	IDNFE *p = malloc(sizeof(IDNFE));
	memcpy(p, &i, sizeof(IDNFE));
	return p;
}


/*

static ENDERECO *new_endereco(){
	ENDERECO e = {
		.pais = new_pais(),
		.Mun = new_municipio()
	};
	ENDERECO *p = malloc(sizeof(ENDERECO));
	memcpy(p, &e, sizeof(ENDERECO));
	return p;
}
*/


EMITENTE *new_emitente(){
	EMITENTE e = {
		.endereco = new_endereco()
	};
	EMITENTE *p = malloc(sizeof(EMITENTE));
	memcpy(p, &e, sizeof(EMITENTE));
	return p;
}

DESTINATARIO *new_destinatario(){
	DESTINATARIO d = {
		.endereco = new_endereco()
	};
	DESTINATARIO *p = malloc(sizeof(DESTINATARIO));
	memcpy(p, &d, sizeof(DESTINATARIO));
	return p;
}

static PRODUTO *new_produto(){
	PRODUTO m = {
		.id = 0
	};
	PRODUTO *p = malloc(sizeof(PRODUTO));
	memcpy(p, &m, sizeof(PRODUTO));
	return p;
}

static ICMS *new_icms(){
	ICMS m = {
		.valor = 0
	};
	ICMS *p = malloc(sizeof(ICMS));
	memcpy(p, &m, sizeof(ICMS));
	return p;
}

static PIS *new_pis(){
	PIS m = {
		.quantidade = 0
	};
	PIS *p = malloc(sizeof(PIS));
	memcpy(p, &m, sizeof(PIS));
	return p;
}

static COFINS *new_cofins(){
	COFINS m = {
		.quantidade = 0
	};
	COFINS *p = malloc(sizeof(COFINS));
	memcpy(p, &m, sizeof(COFINS));
	return p;
}

static IPI *new_ipi(){
	IPI m = {
		.codigo = 0
	};
	IPI *p = malloc(sizeof(IPI));
	memcpy(p, &m, sizeof(IPI));
	return p;
}

static IMPOSTO *new_imposto(){
	IMPOSTO m = {
		.icms = new_icms(),
		.pis = new_pis(),
		.cofins = new_cofins(),
		.ipi= new_ipi()
	};
	IMPOSTO *p = malloc(sizeof(IMPOSTO));
	memcpy(p, &m, sizeof(IMPOSTO));
	return p;
}

static TRANSP *new_transp(){
	TRANSP m = {
		.modfrete = 0
	};
	TRANSP *p = malloc(sizeof(TRANSP));
	memcpy(p, &m, sizeof(TRANSP));
	return p;
}

static PROTOCOLO *new_protocolo(){
	PROTOCOLO m = {
		.numero = NULL
	};
	PROTOCOLO *p = malloc(sizeof(PROTOCOLO));
	memcpy(p, &m, sizeof(PROTOCOLO));
	return p;
}

static LOTE_ITEM *new_lote_item(){
	LOTE_ITEM m = {
		.nfe = NULL
	};
	LOTE_ITEM *l = malloc(sizeof(LOTE_ITEM));
	memcpy(l, &m, sizeof(LOTE_ITEM));
	return l;
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
		.protocolo = new_protocolo()
	};
	NFE *p = malloc(sizeof(NFE));
	memcpy(p, &n, sizeof(NFE));
	return p;
}

LOTE *new_lote(int id){
	LOTE m = {
		.id = id 
	};
	LOTE *l = malloc(sizeof(LOTE));
	memcpy(l, &m, sizeof(LOTE));
	return l;
}

EVENTO_CANCELAMENTO *new_evento_cancelamento(){
	EVENTO e = {
		.type = CANCELAMENTO_TYPE
	};

	EVENTO_CANCELAMENTO c = {
		.evento = e
	};

	EVENTO_CANCELAMENTO *ec = malloc(sizeof(EVENTO_CANCELAMENTO));
	memcpy(ec, &c, sizeof(EVENTO_CANCELAMENTO));
	return ec;
}

static LOTE_EVENTO_ITEM *new_lote_evento_item(){
	LOTE_EVENTO_ITEM m = {
		.evento = NULL
	};
	LOTE_EVENTO_ITEM *l = malloc(sizeof(LOTE_EVENTO_ITEM));
	memcpy(l, &m, sizeof(LOTE_EVENTO_ITEM));
	return l;
}

LOTE_EVENTO *new_lote_evento(int id){
	LOTE_EVENTO m = {
		.id = id
	};
	LOTE_EVENTO *l = malloc(sizeof(LOTE_EVENTO));
	memcpy(l, &m, sizeof(LOTE_EVENTO));
	return l;
}

int inst_produto(int id, const char *codigo, const char *desc, unsigned int ncm, 
		unsigned int cfop, const char *unidade_comercial, double valor, 
		PRODUTO *p){
	p->id = id;
	if(desc != NULL){
		p->codigo = strdup(codigo);
	}
	if(desc != NULL){
		p->descricao = strdup(desc);
	}
	p->ncm = ncm;
	p->cfop = cfop;
	p->unidade_comercial = unidade_comercial;
	if(unidade_comercial != NULL){
		p->unidade_comercial = strdup(unidade_comercial);
	}
	p->valor = valor;
	return 0;
}

int inst_icms(int origem, unsigned int tipo, double aliquota, double valor,
		ICMS *i){
	i->origem = origem;
	i->tipo = tipo;
	i->aliquota = aliquota;
	i->valor = valor;
	return 0;
}

int inst_ipi(int sit_trib, const char *classe, const char *codigo, IPI *i){
	i->sit_trib = sit_trib;
	i->classe = strdup(classe);
	i->codigo = strdup(codigo);
	return 0;
}

static int inst_imposto(ICMS *i, PIS *p, COFINS *c, IMPOSTO *imp){
	imp->icms = i;
	imp->pis = p;
	imp->cofins = c;
	return 0;
}

int inst_item(double valor, double quantidade, 
		unsigned int ordem, int id_produto, char *cod_produto, 
		int icms_origem, int icms_tipo, int pis_quantidade, int pis_nt,
		int cofins_quantidade, int cofins_nt, int ncm, int cfop,
		double icms_aliquota, double icms_valor, double pis_aliquota,
		double cofins_aliquota, int ipi_sit_trib, char *ipi_classe,
		char *ipi_codigo, char *descricao, char *unidade, ITEM *i){
	inst_produto(id_produto, cod_produto, descricao, ncm, cfop, unidade, 
		valor, i->produto);
	inst_icms(ordem, icms_tipo, icms_aliquota, icms_valor,i->imposto->icms);
	inst_ipi(ipi_sit_trib, ipi_classe, ipi_codigo, i->imposto->ipi);
	i->ordem = ordem;
	i->quantidade = quantidade;
	i->valor = valor;
	return 0;
}

int add_item(NFE *nfe, ITEM *item){
	ITEM *i;
	nfe->q_itens++;
	nfe->total += item->valor * item->quantidade;
	item->ordem = nfe->q_itens;
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

int add_nfe(LOTE *lote, NFE *nfe){
	LOTE_ITEM *i = new_lote_item();
	LOTE_ITEM *aux;
	i->nfe = nfe;
	lote->qtd++;
	if((aux = lote->nfes) == NULL){
		lote->nfes= i;
		return 0;
	}
	while(aux->next != NULL){
		aux = aux->next;
	}
	aux->next = i;
	return 0;
}

int add_evento(LOTE_EVENTO *lote, EVENTO *e){
	LOTE_EVENTO_ITEM *i = new_lote_evento_item();
	LOTE_EVENTO_ITEM *aux;
	i->evento = e;
	lote->qtd++;
	i->evento->seq = lote->qtd;
	if((aux = lote->eventos) == NULL){
		lote->eventos = i;
		return 0;
	}
	while(aux->next != NULL){
		aux = aux->next;
	}
	aux->next = i;
	return 0;
}

static char get_dv(char *base){
	int i, j, div, sum = 0;
	int m[8] = {2,3,4,5,6,7,8,9};
	char *baserev = strrev(base);
	for(i = 0, j = 8; i < strlen(baserev); i++, j++){
		sum += (baserev[i] - '0') * m[j%8];
	}
	div = 11 - (sum % 11);
	div = (div == 1)? 0 : div;
	free(baserev);
	return *itoa(div);
}

void set_chave(NFE *nfe){
	char *base = malloc(sizeof(char) * 60);
	sprintf(base, "%02d%s%s%02d%03d%09d%d%08d",
		nfe->idnfe->endereco->Mun->uf->cUF,
		timef(nfe->idnfe->dh_emis, "%y%m", 4),
		nfe->emitente->cnpj,
		nfe->idnfe->mod,
		nfe->idnfe->serie,
		nfe->idnfe->num_nf,
		nfe->idnfe->tipo_emissao,
		nfe->idnfe->cod_nfe);
	nfe->idnfe->div = get_dv(base);
	nfe->idnfe->chave = malloc(sizeof(char) * (strlen(base) + 2));
	sprintf(nfe->idnfe->chave, "%s%c", base , nfe->idnfe->div);
}
/*
static int inst_municipio(const char *uf, const char *nome, unsigned int codigo,
		unsigned int cod_uf, MUNICIPIO *m){
	m->uf = uf;		
	m->municipio = nome;
	m->codigo = codigo;
	m->cod_uf = cod_uf;
	return 0;
}
*/

static 
int inst_endereco(
		const char *xPais,
		unsigned int cPais,
		
		const char *xUF,
		unsigned int cUF,
		
		const char *xMun,
		unsigned int cMun,
		
		const char *rua, 
		const char *num, 
		const char *complemento, 
		const char *bairro, 
		unsigned int cep, 
		
		ENDERECO *e){
	
	e->xLgr = rua;
	e->nro = num;
	e->Cpl = complemento;
	e->xBairro = bairro;
	e->CEP = cep;
	
	e->Mun->xMun = xMun;
	e->Mun->cMun = cMun;
	
	e->Mun->uf->xUF = xUF;
	e->Mun->uf->cUF = cUF;
	
	if(e->Mun->uf->pais->xPais != "\0")
		e->Mun->uf->pais->xPais = xPais;
	if(e->Mun->uf->pais->cPais != 1058)
		e->Mun->uf->pais->cPais = cPais;



	return 0 ;
}

int inst_emitente(
		int id, 
		const char *nome, 
		const char *ie, 
		int crt, 
		const char *cnpj, 
	/*	const char *rua, 
		unsigned int num, 
		const char *complemento, 
		const char *bairro, 
		const char *uf, 
		const char *nome_mun, 
		unsigned int codigo, 
		unsigned int cod_uf, 
		unsigned int cep, */
		EMITENTE *e,
		ENDERECO *end){
	e->id = id;
	e->nome = nome;
	e->inscricao_estadual = ie;
	e->crt = crt;
	e->cnpj = cnpj;
	e->endereco = end;
//	inst_endereco(rua, num, complemento, bairro, cep, e->endereco);
//	inst_municipio(uf, nome_mun, codigo, cod_uf, e->endereco->municipio);
	return 0;
}

int inst_destinatario(int id, 
		char *nome, 
		int t_ie, 
		char *tipo_doc, 
		char *ie,
		char *cnpj, 
	/*	
		char *rua, 
		unsigned int num, 
		char *complemento,
		char *bairro, 
		char *uf, 
		char *nome_mun, 
		unsigned int codigo,
		unsigned int cod_uf,
	       	unsigned int cep, 
	*/	
		DESTINATARIO *d,
		ENDERECO *end){
	d->id = id;
	d->nome = nome;
	d->tipo_ie = t_ie;
	d->cnpj = cnpj;
	d->tipo_doc = tipo_doc;
	d->inscricao_estadual = ie;
	d->endereco = end;
//	inst_endereco(rua, num, complemento, bairro, cep, d->endereco);
//	inst_municipio(uf, nome_mun, codigo, cod_uf, d->endereco->municipio);
	return 0;
}

int inst_nfe(	){

	return 0;
}

static void free_pais(PAIS *e){
	free(e);
}

static void free_uf(UF *e){
	free_pais(e->pais);
	free(e);
}
static void free_municipio(MUNICIPIO *e){
	free_uf(e->uf);
	free(e);
}
static void free_endereco(ENDERECO *e){
	free(e->Mun);
	free(e);
}

void free_emitente(EMITENTE *e){
	free_endereco(e->endereco);
	free(e);
}

void free_destinatario(DESTINATARIO *d){
	free_endereco(d->endereco);
	free(d);
}
