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
 * */ 

#include <libnfe/libnfe.h>
#include <time.h>

/**
 * new_nfe:
 *
 * Criar um objeto NFE
 */
extern NFE *new_nfe();

/**
 * new_lote:
 *
 * Cria um objeto LOTE 
 */
extern LOTE *new_lote(int id);

/**
 * new_item:
 *
 * Cria um objeto ITEM 
 */
extern ITEM *new_item();

/**
 * new_evento_cancelamento:
 *
 * Cria um objeto EVENTO_CANCELAMENTO 
 */
extern EVENTO_CANCELAMENTO *new_evento_cancelamento();

/**
 * new_lote_evento:
 *
 * Cria um objeto LOTE_EVENTO 
 */
extern LOTE_EVENTO *new_lote_evento(int id);

/**
 * new_uf:
 *
 * Cria um objeto UF 
 */
extern UF *new_uf();

/**
 * new_municipio:
 *
 * Cria um objeto MUNICIPIO 
 */
extern MUNICIPIO *new_municipio();

/**
 * new_endereco:
 *
 * Cria um objeto ENDERECO 
 */
extern ENDERECO *new_endereco();

/**
 * new_emitente:
 *
 * Cria um objeto EMITENTE 
 */
extern EMITENTE *new_emitente();

/**
 * new_destinatario:
 *
 * Cria um objeto DESTINATARIO 
 */
extern DESTINATARIO *new_destinatario();

/**
 * add_item:
 * @nfe: Nota fiscal (Objeto NFE)
 * @i: Item que será adicionado (objeto ITEM)
 *
 * Adiciona um ITEM a um objeto NFE
 */
extern int add_item(NFE *nfe, ITEM *i);

/**
 * rm_item:
 * @nfe: Nota fiscal (Objeto NFE)
 * @i: Item que será removido (objeto ITEM)
 *
 * Remove um ITEM a um objeto NFE
 */
extern int rm_item(NFE *nfe, ITEM *i);

/**
 * set_chave:
 * @nfe: Nota fiscal (Objeto NFE)
 *
 * Calcula a chave da NFE
 */
extern void set_chave(NFE *nfe);

extern int inst_produto(
		int id, 
		const char *codigo, 
		const char *desc, 
		unsigned int ncm, 
		unsigned int cfop, 
		const char *unidade_comercial, 
		cents valor, 
		PRODUTO *p);

extern int inst_icms(
		int origem, 
		unsigned int tipo, 
		aliquota aliquota, 
		cents valor,
		ICMS *i);

extern int inst_ipi(
		int sit_trib, 
		const char *classe, 
		const char *codigo, 
		IPI *i);

extern int inst_item(
		cents valor, 
		double quantidade, 
		unsigned int ordem, 
		int id_produto, 
		char *cod_prod,
		int icms_origem, 
		int icms_tipo, 
		int pis_quantidade, 
		int pis_nt,
		int cofins_quantidade, 
		int cofins_nt, 
		int ncm, 
		int cfop,
		aliquota icms_aliquota, 
		cents icms_valor, 
		aliquota pis_aliquota,
		aliquota cofins_aliquota, 
		int ipi_sit_trib, 
		char *ipi_classe,
		char *ipi_codigo, 
		char *descricao, 
		char *unidade,
		ITEM *);

extern int inst_nfe(
		unsigned int id_nfe, 
		int serie, 
		unsigned int num_nf,
		indPag ind_pag, 
		Mod mod, 
		tpNF tipo, 
		idDest local_destino, 
		tpImp tipo_impressao, 
		tpEmis tipo_emissao, 
		tpAmb tipo_ambiente, 
		finNFe finalidade, 
		indFinal consumidor_final, 
		indPres presencial, 
		int cod_nfe, 
		time_t dh_emis,
		time_t *dh_saida, 
		const char *nat_op,
		char *versao,
		char *inf_ad_fisco,
		char *inf_ad_contrib, 
		PROTOCOLO *protocolo,
		EMITENTE *emitente,
		DESTINATARIO *destinatario,
		MUNICIPIO *municipio, 
		NFE *nfe);

extern int inst_emitente(int id, 
		const char *nome, 
		const char *ie, 
		int crt, 
		const char *cnpj, 
		ENDERECO *end,
		EMITENTE *e);

extern int inst_destinatario(int id, 
		char *nome, 
		int t_ie, 
		char *tipo_doc, 
		char *ie,
		char *cnpj, 
		ENDERECO *end,
		DESTINATARIO *d);

extern int inst_endereco(const char *rua, 
		unsigned int num, 
		const char *complemento, 
		const char *bairro, 
		unsigned int cep, 
		const char *xMun,
		unsigned int cMun,
		const char *xUF,
		unsigned int cUF,
		const char *xPais,
		unsigned int cPais,
		ENDERECO *e);

extern int inst_municipio(const char *xMun, 
		unsigned int cMun, 
		const char *xUF, 
		unsigned int cUF, 
		const char *xPais, 
		unsigned int cPais, 
		MUNICIPIO *m);

extern void free_emitente(EMITENTE *e);

extern void free_destinatario(DESTINATARIO *d);

extern void free_endereco(ENDERECO *e);

extern int add_nfe(LOTE *lote, NFE *nfe);

extern int add_evento(LOTE_EVENTO *lote, EVENTO *e);
