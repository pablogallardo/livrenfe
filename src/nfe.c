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
#include "db_interface.h"
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
		.municipio = new_municipio(),
		.dh_saida = malloc(sizeof(time_t))
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

EMITENTE *new_emitente(){
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
		.emitente = get_emitente(1),
		.destinatario = new_destinatario(),
		.protocolo = new_protocolo()
	};
	NFE *p = malloc(sizeof(NFE));
	memcpy(p, &n, sizeof(NFE));
	return p;
}

int inst_produto(int codigo, char *desc, unsigned int ncm, unsigned int cfop,
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
	return 0;
}

int inst_icms(int origem, unsigned int tipo, float aliquota, float valor,
		ICMS *i){
	i->origem = origem;
	i->tipo = tipo;
	i->aliquota = aliquota;
	i->valor = valor;
	return 0;
}

static int inst_imposto(ICMS *i, PIS *p, COFINS *c, IMPOSTO *imp){
	imp->icms = i;
	imp->pis = p;
	imp->cofins = c;
	return 0;
}

int inst_item(float valor, float quantidade, 
		unsigned int ordem, int id_produto, int icms_origem,
		int icms_tipo, int pis_quantidade, int pis_nt,
		int cofins_quantidade, int cofins_nt, int ncm, int cfop,
		float icms_aliquota, float icms_valor, float pis_aliquota,
		float cofins_aliquota, char *descricao, char *unidade,
		ITEM *i){
	inst_produto(id_produto, descricao, ncm, cfop, unidade, valor,
		i->produto);
	inst_icms(ordem, icms_tipo, icms_aliquota, icms_valor,i->imposto->icms);
	i->ordem = ordem;
	i->quantidade = quantidade;
	i->valor = valor;
	return 0;
}

int add_item(NFE *nfe, ITEM *item){
	ITEM *i;
	nfe->q_itens++;
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

static char get_dv(char *base){
	int i, j, div, sum = 0;
	int m[8] = {2,3,4,5,6,7,8,9};
	char *baserev = strrev(base);
	for(i = 0, j = 8; i < strlen(baserev); i++, j++){
		sum += (baserev[i] - '0') * m[j%8];
	}
	div = 11 - (sum % 11);
	div = div == 1? 0:div;
	free(baserev);
	return *itoa(div);
}

void set_chave(NFE *nfe){
	int i, j, div, sum = 0;
	char *base = malloc(sizeof(char) * 60);
	sprintf(base, "%02d%s%s%02d%03d%09d%d%08d",
		nfe->idnfe->municipio->cod_uf,
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

static int inst_municipio(char *uf, char *nome, unsigned int codigo,
		unsigned int cod_uf, MUNICIPIO *m){
	m->uf = uf;		
	m->municipio = nome;
	m->codigo = codigo;
	m->cod_uf = cod_uf;
	return 0;
}

static int inst_endereco(char *rua, unsigned int num, char *complemento,
		char *bairro, unsigned int cep, ENDERECO *e){
	e->rua = rua;
	e->num = num;
	e->complemento = complemento;
	e->bairro = bairro;
	e->cep = cep;
	return 0;
}

int inst_emitente(char *id, char *nome, char *ie, int crt, char *cnpj,
		char *rua, unsigned int num, char *complemento,
		char *bairro, char *uf, char *nome_mun, unsigned int codigo,
		unsigned int cod_uf, unsigned int cep, EMITENTE *e){
	e->id = id;
	e->nome = nome;
	e->inscricao_estadual = ie;
	e->crt = crt;
	e->cnpj = cnpj;
	inst_endereco(rua, num, complemento, bairro, cep, e->endereco);
	inst_municipio(uf, nome_mun, codigo, cod_uf, e->endereco->municipio);
	return 0;
}

static int inst_destinatario(char *id, char *nome, int t_ie, char *tipo_doc, char *ie,
		char *cnpj, char *rua, unsigned int num, char *complemento,
		char *bairro, char *uf, char *nome_mun, unsigned int codigo,
		unsigned int cod_uf, unsigned int cep, DESTINATARIO *d){
	d->id = id;
	d->nome = nome;
	d->tipo_ie = t_ie;
	d->cnpj = cnpj;
	d->tipo_doc = tipo_doc;
	d->inscricao_estadual = ie;
	inst_endereco(rua, num, complemento, bairro, cep, d->endereco);
	inst_municipio(uf, nome_mun, codigo, cod_uf, d->endereco->municipio);
	return 0;
}

int inst_nfe(int id_nfe, int id_mun, int id_uf, int ind_pag, int mod_nfe,
		int serie, int num_nf, int tipo, int local_destino, 
		int tipo_impressao, int tipo_emissao, int tipo_ambiente, 
		int finalidade, int consumidor_final, int presencial, int q_itens,
		int id_emit, char *ie_emit, int crt_emit, int id_mun_emit,
		int id_uf_emit, int cep_emit, int num_e_emit, int id_dest, 
		int t_ie_dest, int id_mun_dest, int id_uf_dest, int num_e_dest,
		int cod_nfe, int cep_dest, float dh_emis, float *dh_saida, float total,
		char *nome_mun, char *uf, char *nat_op, char *versao, 
		char *nome_emit, char *cnpj_emit, char *rua_emit,
		char *comp_emit, char *bairro_emit, char *mun_emit, char *uf_emit,
		char *nome_dest, char *cnpj_dest, char *rua_dest, 
		char *comp_dest, char *bairro_dest, char *mun_dest,
		char *uf_dest, char *chave, char div, char *ie_dest,
		char *tipo_doc_dest, NFE *nfe){
	IDNFE *idnfe = nfe->idnfe;
	idnfe->id_nfe = id_nfe;
	idnfe->nat_op = nat_op;
	idnfe->ind_pag = ind_pag;
	idnfe->mod = mod_nfe;
	idnfe->serie = serie;
	idnfe->num_nf = num_nf;
	idnfe->dh_emis = dh_emis;
	idnfe->dh_saida = dh_saida;
	idnfe->tipo = tipo;
	idnfe->local_destino = local_destino;
	idnfe->tipo_impressao = tipo_impressao;
	idnfe->tipo_emissao = tipo_emissao;
	idnfe->tipo_ambiente = tipo_ambiente;
	idnfe->finalidade = finalidade;
	idnfe->consumidor_final = consumidor_final;
	idnfe->presencial = presencial;
	idnfe->versao = versao;
	idnfe->div = div;
	idnfe->chave = chave;
	idnfe->cod_nfe = cod_nfe;
	inst_municipio(uf, nome_mun, id_mun, id_uf, idnfe->municipio);
	inst_emitente(id_emit, nome_emit, ie_emit, crt_emit, 
		cnpj_emit, rua_emit, num_e_emit, comp_emit, bairro_emit, uf_emit,
		uf_emit, id_mun_emit, id_uf_emit, cep_emit, nfe->emitente);
	inst_destinatario(id_dest, nome_dest, t_ie_dest,
		tipo_doc_dest, ie_dest, cnpj_dest, rua_dest, num_e_dest,
		comp_dest, bairro_dest, 
		uf_dest, mun_dest, id_mun_dest, id_uf_dest, cep_dest, 
		nfe->destinatario);

	return 0;
}
