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

#ifndef	LIBNFE_H
#define	LIBNFE_H

#include <time.h>
#define	VERSION_NAME			"0.1.0"
#define	VERSION_COUNTER			1
#define VERSION_TITLE			"Tartagal"
#define LIBNFE_COPYRIGHT		"© 2016, 2017 Pablo G. Gallardo"
#define LIBNFE_COMMENT			"Emissor Código Aberto de Notas Fiscais Eletrónicas"
#define LIBNFE_WEBSITE			"https://github.com/pablogallardo/livrenfe"
#define LIBNFE_TITLE			"LivreNFE"

#define CANCELAMENTO_TYPE		110111
#define	CARTA_CORRECAO_TYPE		110110

#define	AMBIENTE_PRODUCAO		1
#define	AMBIENTE_HOMOLOGACAO		2

#define	CERT_TYPE_A1			1
#define	CERT_TYPE_A3			2

#define	DEFAULT_AMBIENTE		AMBIENTE_HOMOLOGACAO
#define	DEFAULT_CERT_TYPE		CERT_TYPE_A3

#define WSDL_RECEPCAO_EVENTO		"http://www.portalfiscal.inf.br/nfe/wsdl/RecepcaoEvento"
#define WSDL_NFE_CONSULTA_CADASTRO	"http://www.portalfiscal.inf.br/nfe/wsdl/CadConsultaCadastro2"
#define WSDL_NFE_INUTILIZACAO		"http://www.portalfiscal.inf.br/nfe/wsdl/NfeInutilizacao"
#define WSDL_NFE_CONSULTA_PROTOCOLO	"http://www.portalfiscal.inf.br/nfe/wsdl/NfeConsulta2"
#define WSDL_NFE_STATUS_SERVICO		"http://www.portalfiscal.inf.br/nfe/wsdl/NfeStatusServico2"
#define WSDL_NFE_AUTORIZACAO		"http://www.portalfiscal.inf.br/nfe/wsdl/NfeAutorizacao"
#define WSDL_NFE_RET_AUTORIZACAO	"http://www.portalfiscal.inf.br/nfe/wsdl/NfeRetAutorizacao"

/*
 *Colecao de enums
 *
 * */



typedef enum indPag_t {
  A_VISTA = 0,
  A_PRAZO = 1,
   OUTROS = 2
}indPag  ;

 typedef enum  Mod_t {
	 MOD_NFe  = 55,
	 MOD_NFCe = 65
 }Mod;

typedef enum tpNF_t{
	ENTRADA = 0,
	SAIDA   = 1
}tpNF;

typedef enum idDest_t{
	OPERACAO_INTERNA = 1,
	OPERACAO_INTERESTADUAL = 2,
	OPERACAO_EXTERIOR = 3
}idDest;

typedef enum tpImp_t{
	SEM_GERAR_DANFE = 0,
	DANFE_NORMAL_RETRATO = 1,
	DANFE_NORMAL_PAISAGEM = 2,
	DANFE_SIMPLIFICADO = 3,
	DANFE_NFCe = 4,
	DANFE_NFCe_ELETRONICA = 5
}tpImp;

typedef enum tpEmis_t{
	EMISSAO_NORMAL =1,
	EMISSAO_CONTIGENCIA_FSIA = 2,
	EMISSAO_CONTIGENCIA_SCAN = 3,
	EMISSAO_CONTIGENCIA_DPEC = 4,
	EMISSAO_CONTIGENCIA_FSDA = 5,
	EMISSAO_CONTIGENCIA_SVC_AN = 6,
	EMISSAO_CONTIGENCIA_SVC_RS = 7,
	EMISSAO_CONTIGENCIA_OFFLINE = 9
}tpEmis;

typedef enum tpAmb_t{
	PRODUCAO = 1,
	HOMOLOGACAO = 2
}tpAmb;



typedef enum finNFe_t{
	NFe_NORMAL = 1,
	NFe_COMPLEMENTAR = 2,
	NFe_AJUSTE = 3,
	NFe_DEVOLUCAO = 4
}finNFe;

typedef enum indFinal_t{
	NORMAL = 0,
	CONSUMIDOR_FINAL = 1
}indFinal;

typedef enum indPres_t{
	NAO_SE_APLICA = 0,
	OPERACAO_PRESENCIAL = 1,
	OPERACAO_NAO_PRESENCIAL_INTERNET = 2,
	OPERACAO_NAO_PRESENCIAL_TELEATENDIMENTO = 3,
	OPERACAO_NFCe_ENTREGA_DOMICILIO = 4,
	OPERACAO_NAO_PRESENCIAL_OUTROS = 9
}indPres;

typedef enum procEmi_t{
	EMISSAO_NFE_APP_CLIENTE = 0,
	EMISSAO_NFE_AVULSA_FISCO = 1,
	EMISSAO_NFE_AVULSA_CONT_CERT_SITE_FISCO = 2,
	EMISSAO_NFE_FISCO_APP = 3
}procEmi;

typedef enum {
	SEFAZ_RECEPCAO_EVENTO,
	SEFAZ_NFE_CONSULTA_CADASTRO,
	SEFAZ_NFE_INUTILIZACAO,
	SEFAZ_NFE_CONSULTA_PROTOCOLO,
	SEFAZ_NFE_STATUS_SERVICO,
	SEFAZ_NFE_AUTORIZACAO,
	SEFAZ_NFE_RET_AUTORIZACAO
} sefaz_servico_t;

/**
 * Array com URL WSDL de cada serviço do SEFAZ
 */
extern const char *SEFAZ_WSDL[7];

typedef int evento_t;

/*
 * City information
 */
typedef struct {
	const char *uf;
	const char *municipio;
	unsigned int codigo;
	unsigned int cod_uf;
} MUNICIPIO;

/*
 * NFE identification
 *
 * @Pablo
 * Manteve-se o nome dos fields
 * para compatibilidade, porem
 * deveriamos muda-los para o nome
 * do elemento.
 * EX's:ind_pag --> indPag
 * 	tipo --> tpNF	
 */
typedef struct {
	MUNICIPIO *municipio;
	unsigned int id_nfe;
	const char *nat_op;
	indPag  ind_pag;
	Mod mod;
	int serie;
	unsigned int num_nf;
	time_t dh_emis;
	time_t *dh_saida;
	tpNF tipo;
	idDest local_destino;
	tpImp tipo_impressao;
	tpEmis tipo_emissao;
	tpAmb tipo_ambiente;
	finNFe finalidade;
	indFinal consumidor_final;
	indPres presencial;
	const char *versao;
	char div;
	char *chave;
	int cod_nfe;
} IDNFE;

/*
 * Country information
 */
typedef struct {
	unsigned int codigo;
	const char *nome;
} PAIS;

/*
 * Address information
 */
typedef struct {
	const char *rua;
	unsigned int num;
	const char *complemento;
	const char *bairro;
	MUNICIPIO *municipio;
	unsigned int cep;
	PAIS *pais;
} ENDERECO;

/*
 * Issuer information
 */
typedef struct {
	int id;
	const char *nome;
	const char *inscricao_estadual;
	ENDERECO *endereco;
	enum t_crt {CRT_SNAC=1, CRT_SNAC_EXC=2, CRT_NORMAL=3} crt;
	const char *cnpj;
} EMITENTE;

/*
 * Recipient information
 */
typedef struct {
	int id;
	const char *nome;
	ENDERECO *endereco;
	enum t_tipo_ie {CONT_AV=1, CONT_IS=2, NAO_CONT=9} tipo_ie;
	const char *cnpj;
	const char *tipo_doc;
	const char *inscricao_estadual;
} DESTINATARIO;

/*
 * Product information
 */
typedef struct {
	int id;
	const char *codigo;
	const char *descricao;
	unsigned int ncm;
	unsigned int cfop;
	const char *unidade_comercial;
	float valor;
} PRODUTO;

/*
 * ICMS tax
 */
typedef struct {
	enum t_origem {NACIONAL=0, E_ID=1, E_AI=2} origem;
	unsigned int tipo;
	float aliquota;
	float valor;
} ICMS;

/*
 * PIS tax
 */
typedef struct {
	float aliquota;
	unsigned int quantidade;
	const char *nt;
} PIS;

/* 
 * COFINS tax
 */
typedef struct {
	float aliquota;
	unsigned int quantidade;
	const char *nt;
} COFINS;

/* 
 * IPI tax
 */
typedef struct {
	int sit_trib;
	const char *classe;
	const char *codigo;
} IPI;

/*
 * Tax information
 */
typedef struct {
	ICMS *icms;
	PIS *pis;
	COFINS *cofins;
	IPI *ipi;
} IMPOSTO;

typedef struct ITEM ITEM;

/*
 * Items
 */
struct ITEM {
	PRODUTO *produto;
	IMPOSTO *imposto;
	unsigned int ordem;
	unsigned int quantidade;
	float valor;
	ITEM *pointer; //next item
};

/*
 * Transportation information
 */
typedef struct {
	enum t_modfrete {FRETE_EMITENTE=0, FRETE_DESTINATARIO=1, 
		FRETE_TERCEIROS=2, FRETE_SEM=9}
		modfrete;
} TRANSP;

/* 
 * SEFAZ record information
 */
typedef struct {
	const char *dh_recib;
	const char *numero;
	int cod_status;
	const char *xml;
	const char *xmot;
} PROTOCOLO;

/*
 * Complete NFE information
 */
typedef struct {
	IDNFE *idnfe;
	EMITENTE *emitente;
	DESTINATARIO *destinatario;
	ITEM *itens;
	unsigned int q_itens;
	float total;
	TRANSP *transp;
	PROTOCOLO *protocolo;
	const char *xml;
	int canceled;
	const char *inf_ad_fisco;
	const char *inf_ad_contrib;
} NFE;

typedef struct LOTE_ITEM LOTE_ITEM;

/**
 * List items
 */
struct LOTE_ITEM{
	NFE *nfe;
	LOTE_ITEM *next;
};

/**
 * NFE Lote
 */
typedef struct {
	unsigned int id;
	const char *recibo;
	unsigned int qtd;
	LOTE_ITEM *nfes;
	const char *xml_response;
} LOTE;

/**
 * Evento
 */
typedef struct {
	const char *id;
	int seq;
	NFE *nfe;
	evento_t type;
	int cStat;
	const char *xmot;
	const char *protocolo;
	const char *xml;
	const char *xml_response;
} EVENTO;

/**
 * Evento cancelamento
 */
typedef struct {
	EVENTO evento;
	const char *justificativa;
} EVENTO_CANCELAMENTO;

/**
 * List evento items
 */
typedef struct LOTE_EVENTO_ITEM LOTE_EVENTO_ITEM;
struct LOTE_EVENTO_ITEM {
	EVENTO *evento;
	LOTE_EVENTO_ITEM *next;
};

/**
 * Eventos Lote
 */
typedef struct {
	unsigned int id;
	const char *recibo;
	unsigned int qtd;
	LOTE_EVENTO_ITEM *eventos;
	const char *xml_response;
} LOTE_EVENTO;


#endif
