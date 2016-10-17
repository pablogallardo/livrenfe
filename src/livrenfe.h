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
#include <gtk/gtk.h>

#define	VERSION_NAME	"0.1"
#define	VERSION_COUNTER	1
#define	LIVRENFE_FOLDER_NAME	"/.livrenfe"
#define	LIVRENFE_DB	"/livrenfe.db"

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
	enum t_mod {MOD_NFE=55, MOD_NFCE=65} mod;
	int serie;
	unsigned int num_nf;
	time_t dh_emis;
	time_t dh_saida;
	enum t_tipo {TIPO_ENTRADA=0,TIPO_SAIDA=1} tipo;
	enum t_local_destino {DEST_INTERNA=1,DEST_INTERESTADUAL=2,DEST_EXTERIOR=3} local_destino;
	enum t_tipo_impresao {IMP_NONE=0,IMP_RET=1,IMP_PAI=2,IMP_SIMP=3,IMP_NFCE=4,IMP_NFCE_MSG=5}
		tipo_impresao;
	enum t_tipo_emissao {TE_NORMAL=1, TE_FS=2, TE_SCAN=3, TE_DPEC=4, TE_FSDA=5, TE_SVCAN=6, 
		TE_SVCRS=7, TE_OFFLINE_NFCE=9} tipo_emissao;
	enum t_tipo_ambiente {PRODUCAO=1, HOMOLOGACAO=2} tipo_ambiente;
	enum t_finalidade {FIN_NORMAL=1, FIN_COMPLEMENTAR=2, FIN_AJUSTE=3, FIN_RETORNO=4}
	finalidade;
	enum t_consumidor_final {FALSE=0, TRUE=1} consumidor_final;
	enum t_presencial {PRE_NA=0, PRE_PRESENCIAL=1, PRE_INTERNET=2, PRE_TELEATENDIMENTO=3,
		PRE_NFCE_ED=4, PRE_OUTRO=5} presencial;
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
	enum t_crt {CRT_SNAC=1, CRT_SNAC_EXC=2, CRT_NORMAL=3} crt;
} t_emitente;

/*
 * Recipient information
 */
typedef struct {
	char *id;
	char *nome;
	t_endereco endereco;
	enum t_tipo_ie {CONT_AV=1, CONT_IS=2, NAO_CONT=3} tipo_ie;
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
	enum t_origem {NACIONAL=0, E_ID=1, E_AI=2} origem;
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
	enum t_modfrete {FRETE_EMITENTE=0, FRETE_DESTINATARIO=1, FRETE_TERCEIROS=2, FRETE_SEM=9}
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

#define LIVRENFE_TYPE	(livrenfe_get_type())
#define	LIVRENFE_APP(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), LIVRENFE_TYPE, Livrenfe))

typedef struct _Livrenfe Livrenfe;
typedef struct _LivrenfeClass LivrenfeClass;

GType livrenfe_get_type(void);
Livrenfe *livrenfe_new(void);

#endif
