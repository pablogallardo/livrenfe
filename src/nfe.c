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
#include <time.h>
#include <stdlib.h>

NFE *new_nfe(unsigned int num, int serie, time_t dh_emis, time_t dh_saida,
		int ind_pag, char *nat_op, MUNICIPIO *munic, DESTINATARIO *dest,
	       	int orig, ITEM *i){
	IDNFE *id = malloc(sizeof(IDNFE));
	id->num_nf = num;
	id->serie = serie;
	id->dh_emis = dh_emis;
	id->dh_saida = dh_saida;
	id->ind_pag = ind_pag;
	id->nat_op = nat_op;
	id->municipio = munic;
	
	NFE *n = malloc(sizeof(NFE));
	n->idnfe = id;
	n->q_itens = 0;
	n->destinatario = dest;
}

PRODUTO *new_produto(int codigo, char *desc, unsigned int ncm, unsigned int cfop,
		char *unidade_comercial, float valor){
	PRODUTO *p = malloc(sizeof(PRODUTO));
	p->codigo = codigo;
	p->descricao = desc;
	p->ncm = ncm;
	p->cfop = cfop;
	p->unidade_comercial = unidade_comercial;
	p->valor = valor;
	return p;
}

ITEM *new_item(PRODUTO *p, IMPOSTO *imp, float valor, float quantidade, unsigned int ordem){
	ITEM *i = malloc(sizeof(ITEM));
	i->produto = p;
	i->imposto = imp;
	i->valor = valor;
	i->ordem = ordem;
	i->quantidade = quantidade;
	i->pointer = NULL;
	return i;
}

ICMS *new_icms(int origem, unsigned int tipo, float aliquota, float valor){
	ICMS *i = malloc(sizeof(ICMS));	
	i->origem = origem;
	i->tipo = tipo;
	i->aliquota = aliquota;
	i->valor = valor;
	return i;
}

IMPOSTO *new_imposto(ICMS *i, PIS *p, COFINS *c){
	IMPOSTO *imp = malloc(sizeof(IMPOSTO));
	imp->icms = i;
	imp->pis = p;
	imp->cofins = c;
	return imp;
}

DESTINATARIO *new_destinatario(char *rua, int num, char *complemento, 
		char *bairro, MUNICIPIO *m, unsigned int cep, PAIS *p){
	ENDERECO *e = malloc(sizeof(ENDERECO));
	e->rua = rua;
	e->num = num;
	e->complemento = complemento;
	e->bairro = bairro;
	e->municipio = m;
	e->cep = cep;
	e->pais = p;
	DESTINATARIO *d = malloc(sizeof(DESTINATARIO));
	return d;
}

int add_item(NFE *nfe, ITEM *item){
	ITEM *i;
	nfe->q_itens++;
	if((i = nfe->itens) == NULL){
		nfe->itens = item;
		return 0;
	}
	while(i != NULL){
		i = i->pointer;
	}
	i->pointer = item;
	return 0;
}

