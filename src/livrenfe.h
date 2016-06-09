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

#ifndef	LIVRENFE_H
#define	LIVRENFE_H

#include <time.h>

#define	VERSION_NAME	0.1
#define	VERSION_COUNTER	1

/*
 * City information
 */
typedef struct {
	char *uf;
	char *municipio;
	unsigned int codigo;
	unsigned int cod_uf;
} t_municipio;

/*
 * NFE identification
 */
typedef struct {
	t_municipio municipio;
	unsigned int id_nfe;
	char *nat_op;
	int ind_pag;
	enum t_mod {mod_nfe=55, mod_nfce=65} mod;
	int serie;
	unsigned int num_nf;
	time_t dh_emis;
	time_t dh_saida;
	enum t_tipo {tipo_entrada=0,tipo_saida=1} tipo;
	enum t_local_destino {dest_interna=1,dest_interestadual=2,dest_exterior=3} local_destino;
	enum t_tipo_impressao {imp_none=0,imp_ret=1,imp_pai=2,imp_simp=3,imp_nfce=4,imp_nfce_msg=5}
		tipo_impressao;
	enum t_tipo_emissao {te_normal=1, te_fs=2, te_scan=3, te_dpec=4, te_fsda=5, te_svcan=6, 
		te_svcrs=7, te_offline_nfce=9} tipo_emissao;
	enum t_tipo_ambiente {producao=1, homologacao=2} tipo_ambiente;
	enum t_finalidade {fin_normal=1, fin_complementar=2, fin_ajuste=3, fin_retorno=4}
	finalidade;
	enum t_consumidor_final {false=0, true=1} consumidor_final;
	enum t_presencial {pre_na=0, pre_presencial=1, pre_internet=2, pre_teleatendimento=3,
		pre_nfce_ed=4, pre_outro=5} presencial;
	const char *versao;
	char div;
	char *chave;
} t_idnfe;

/*
 * Country information
 */
typedef struct {
	unsigned int codigo;
	char *nome;
} t_pais;

/*
 * Address information
 */
typedef struct {
	char *rua;
	unsigned int num;
	char **complemento;
	char *bairro;
	t_municipio municipio;
	unsigned int cep;
	t_pais pais;
} t_endereco;

/*
 * Issuer information
 */
typedef struct {
	char *id;
	char *nome;
	char *inscricao_estadual;
	t_endereco endereco;
	enum t_crt {crt_snac=1, crt_snac_exc=2, crt_normal=3} crt;
} t_emitente;

/*
 * Recipient information
 */
typedef struct {
	char *id;
	char *nome;
	t_endereco endereco;
	enum t_tipo_ie {cont_av=1, cont_is=2, nao_cont=3} tipo_ie;
} t_destinatario;

/*
 * Product information
 */
typedef struct {
	unsigned int codigo;
	char *descricao;
	unsigned int ncm;
	unsigned int cfop;
	char *unidade_comercial;
	float quantidade;
	float valor;
} t_produto;

/*
 * ICMS tax
 */
typedef struct {
	enum t_origem {nacional=0, e_id=1, e_ai=2} origem;
	unsigned int tipo;
	float aliquota;
	float valor;
} t_icms;

/*
 * PIS tax
 */
typedef struct {
	float aliquota;
	unsigned int quantidade;
	char *nt;
} t_pis;

/* 
 * COFINS tax
 */
typedef struct {
	float aliquota;
	unsigned int quantidade;
	char *nt;
} t_cofins;

/*
 * Tax information
 */
typedef struct {
	t_icms *icms;
	t_pis *pis;
	t_cofins *cofins;
} t_imposto;

/*
 * Items
 */
typedef struct {
	t_produto produto;
	t_imposto imposto;
	unsigned int ordem;
} t_item;

/*
 * Transportation information
 */
typedef struct {
	enum t_modfrete {frete_emitente=0, frete_destinatario=1, frete_terceiros=2, frete_sem=9}
		modfrete;
} t_transp;

/* 
 * SEFAZ record information
 */
typedef struct {
	char *dh_recib;
	unsigned long long numero;
	int cod_status;
} t_protocolo;

/*
 * Complete NFE information
 */
typedef struct {
	t_idnfe idnfe;
	t_emitente emitente;
	t_destinatario destinatario;
	t_item *itens;
	unsigned int q_itens;
	float total;
	t_transp transp;
	t_protocolo *protocolo;
} t_nfe;

#endif
