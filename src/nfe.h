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

#include "livrenfe.h"
#include <time.h>

extern NFE *new_nfe();

extern LOTE *new_lote();

extern ITEM *new_item();

extern EVENTO_CANCELAMENTO *new_evento_cancelamento();

extern LOTE_EVENTO *new_lote_evento();

extern EMITENTE *new_emitente();

extern int add_item(NFE *nfe, ITEM *i);

extern int rm_item(NFE *nfe, ITEM *i);

extern void set_chave(NFE *nfe);

extern int inst_produto(int codigo, char *desc, unsigned int ncm, unsigned int cfop,
		char *unidade_comercial, float valor, PRODUTO *p);

extern int inst_icms(int origem, unsigned int tipo, float aliquota, float valor,
		ICMS *i);

extern int inst_item(float valor, float quantidade, 
		unsigned int ordem, int id_produto, int icms_origem,
		int icms_tipo, int pis_quantidade, int pis_nt,
		int cofins_quantidade, int cofins_nt, int ncm, int cfop,
		float icms_aliquota, float icms_valor, float pis_aliquota,
		float cofins_aliquota, char *descricao, char *unidade,
		ITEM *);

extern int inst_nfe(int id_nfe, int id_mun, int id_uf, int ind_pag, int mod_nfe,
		int serie, int num_nf, int tipo, int local_destino, 
		int tipo_impressao, int tipo_emissao, int tipo_ambiente, 
		int finalidade, int consumidor_final, int presencial, int q_itens,
		int id_emit, char *ie_emit, int crt_emit, int id_mun_emit,
		int id_uf_emit, int cep_emit, int num_e_emit, int id_dest, 
		int t_ie_dest, int id_mun_dest, int id_uf_dest, int num_e_dest,
		int cod_nfe, int cep_dest, int canceled, time_t dh_emis, 
		time_t *dh_saida, float total,
		char *nome_mun, char *uf, char *nat_op, char *versao, 
		char *nome_emit, char *cnpj_emit, char *rua_emit,
		char *comp_emit, char *bairro_emit, char *mun_emit, char *uf_emit,
		char *nome_dest, char *cnpj_dest, char *rua_dest, 
		char *comp_dest, char *bairro_dest, char *mun_dest,
		char *uf_dest, char *chave, char div, char *ie_dest,
		char *tipo_doc_dest, char *protocolo, NFE *nfe);

extern int inst_emitente(int id, char *nome, char *ie, int crt, char *cnpj,
		char *rua, unsigned int num, char *complemento,
		char *bairro, char *uf, char *nome_mun, unsigned int codigo,
		unsigned int cod_uf, unsigned int cep, EMITENTE *e);


extern void free_emitente(EMITENTE *e);

extern int add_nfe(LOTE *lote, NFE *nfe);

extern int add_evento(LOTE_EVENTO *lote, EVENTO *e);
