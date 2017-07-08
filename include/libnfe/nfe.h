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
 * TODO: Remove um ITEM a um objeto NFE
 */
extern int rm_item(NFE *nfe, ITEM *i);

/**
 * set_chave:
 * @nfe: Nota fiscal (Objeto NFE)
 *
 * Calcula a chave da NFE
 */
extern void set_chave(NFE *nfe);

extern int inst_produto(int id, const char *codigo, const char *desc, 
		unsigned int ncm, unsigned int cfop, 
		const char *unidade_comercial, double valor, PRODUTO *p);

extern int inst_icms(int origem, unsigned int tipo, double aliquota, double valor,
		ICMS *i);

extern int inst_ipi(int sit_trib, const char *classe, 
		const char *codigo, IPI *i);

extern int inst_item(double valor, double quantidade, 
		unsigned int ordem, int id_produto, char *cod_prod,
		int icms_origem, int icms_tipo, int pis_quantidade, int pis_nt,
		int cofins_quantidade, int cofins_nt, int ncm, int cfop,
		double icms_aliquota, double icms_valor, double pis_aliquota,
		double cofins_aliquota, int ipi_sit_trib, char *ipi_classe,
		char *ipi_codigo, char *descricao, char *unidade,
		ITEM *);

extern int inst_nfe(int id_nfe, int id_mun, int id_uf, int ind_pag, int mod_nfe,
		int serie, int num_nf, int tipo, int local_destino, 
		int tipo_impressao, int tipo_emissao, int tipo_ambiente, 
		int finalidade, int consumidor_final, int presencial, int q_itens,
		int id_emit, char *ie_emit, int crt_emit, int id_mun_emit,
		int id_uf_emit, int cep_emit, int num_e_emit, int id_dest, 
		int t_ie_dest, int id_mun_dest, int id_uf_dest, int num_e_dest,
		int cod_nfe, int cep_dest, int canceled, time_t dh_emis, 
		time_t *dh_saida, double total,
		char *nome_mun, char *uf, char *nat_op, char *versao, 
		char *nome_emit, char *cnpj_emit, char *rua_emit,
		char *comp_emit, char *bairro_emit, char *mun_emit, char *uf_emit,
		char *nome_dest, char *cnpj_dest, char *rua_dest, 
		char *comp_dest, char *bairro_dest, char *mun_dest,
		char *uf_dest, char *chave, char div, char *ie_dest,
		char *tipo_doc_dest, char *inf_ad_fisco, char *inf_ad_contrib,
		char *protocolo, NFE *nfe);

extern int inst_emitente(int id, 
		const char *nome, 
		const char *ie, 
		int crt, 
		const char *cnpj, 
	/*
		const char *rua, 
		unsigned int num, 
		const char *complemento, 
		const char *bairro, 
		const char *uf, 
		const char *nome_mun,
		unsigned int codigo,
		unsigned int cod_uf, 
		unsigned int cep, 
	*/
		EMITENTE *e,
		ENDERECO *end);

extern int inst_destinatario(int id, 
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
		ENDERECO *end);

extern void free_emitente(EMITENTE *e);

extern void free_destinatario(DESTINATARIO *d);

extern int add_nfe(LOTE *lote, NFE *nfe);

extern int add_evento(LOTE_EVENTO *lote, EVENTO *e);
