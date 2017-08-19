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

/**
 * SECTION:libnfe
 * @short-description: Structs tipos e funções principais usados para emissão de NF
 * @title: Interface da libnfe
 * @include: libnfe/libnfe.h
 *
 * Interface principal da biblioteca
 */


#ifndef	LIBNFE_H
#define	LIBNFE_H

#include <time.h>
#define	VERSION_NAME			"0.1.0"
#define	VERSION_COUNTER			1
#define VERSION_TITLE			"Tartagal"
#define LIBNFE_COPYRIGHT		"© 2016, 2017 Pablo G. Gallardo"
#define LIBNFE_COMMENT			"Emissor Código Aberto de Notas Fiscais Eletrônicas"
#define LIBNFE_WEBSITE			"https://github.com/pablogallardo/livrenfe"
#define LIBNFE_TITLE			"LivreNFE"

#define CANCELAMENTO_TYPE		110111
#define	CARTA_CORRECAO_TYPE		110110

#define	AMBIENTE_PRODUCAO		1
#define	AMBIENTE_HOMOLOGACAO		2

#define WSDL_RECEPCAO_EVENTO		"http://www.portalfiscal.inf.br/nfe/wsdl/RecepcaoEvento"
#define WSDL_NFE_CONSULTA_CADASTRO	"http://www.portalfiscal.inf.br/nfe/wsdl/CadConsultaCadastro2"
#define WSDL_NFE_INUTILIZACAO		"http://www.portalfiscal.inf.br/nfe/wsdl/NfeInutilizacao"
#define WSDL_NFE_CONSULTA_PROTOCOLO	"http://www.portalfiscal.inf.br/nfe/wsdl/NfeConsulta2"
#define WSDL_NFE_STATUS_SERVICO		"http://www.portalfiscal.inf.br/nfe/wsdl/NfeStatusServico2"
#define WSDL_NFE_AUTORIZACAO		"http://www.portalfiscal.inf.br/nfe/wsdl/NfeAutorizacao"
#define WSDL_NFE_RET_AUTORIZACAO	"http://www.portalfiscal.inf.br/nfe/wsdl/NfeRetAutorizacao"

/**
 * Coleção de enums
 *
 * */

/* Este tipo de dado
 * é para ser usado 
 * para dinheiro na
 * notacao de centavos.
 */
typedef long int cents;
/*Este tipo de dado
 * eh para ser usado
 * como porcentagem (%),
 * será em especial,
 * útil para alíquotas
 * de impostos
 */

typedef int aliquota;

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

typedef enum crt_t {
	CRT_SIMPLES_NAC = 1,
	CRT_SIMPLES_NAC_EXC = 2,
	CRT_NORMAL = 3
} Crt;

typedef enum tpIE_t {
	IE_CONTRIBUINTE_PAG_AVISTA = 1,
	IE_CONTRIBUINTE_ISENTO = 2,
	IE_NAO_CONTRIBUINTE = 9
} tpIE;

typedef enum origem_t {
	ORIGEM_NACIONAL = 0, 
	ORIGEM_ESTRANGEIRA_IMP_DIRETA = 1, 
	ORIGEM_ESTRANGEIRA_MERC_INTERNO = 2
} Origem;

typedef enum modfrete_t {
	FRETE_EMITENTE = 0, 
	FRETE_DESTINATARIO = 1, 
	FRETE_TERCEIROS = 2, 
	FRETE_SEM = 9
} modFrete;

/**
 * SEFAZ_WSDL:
 *
 * Array com URL WSDL de cada serviço do SEFAZ
 */
extern const char *SEFAZ_WSDL[7];

typedef int evento_t;

/**
 * PAIS:
 * @cPais: Código do país
 * @xPais: Nome do país
 *
 * País
 */
typedef struct pais_t{
	const char *xPais;
	unsigned int cPais;
} PAIS;

/**
 * UF:
 * @cUF: Código IBGE da UF 
 * @xUF: Nome da UF
 * @pais: Pais
 *
 * País
 */
typedef struct uf_t{
	const char *xUF;
	unsigned int cUF;
	PAIS *pais;
} UF;

/**
 * MUNICIPIO:
 * @xMun: Nome do municício 
 * @cMun: Código IBGE do municício 
 * @uf: UF
 *
 * Informação do Município
 */
typedef struct municipio_t{
	const char *xMun;
	unsigned int cMun;
	UF *uf;
} MUNICIPIO;

/**
 * ENDERECO:
 * @xLgr: Rua do endereço
 * @nro: Número do endereço na rua
 * @Cpl: Complemento do endereço
 * @xBairro: Bairro do endereço
 * @municipio: Município do endereço
 * @CEP: CEP do endereço
 *
 * Endereço
 */
typedef struct endereco_t{
	const char *xLgr;
	unsigned int nro;
	const char *Cpl;
	const char *xBairro;
	unsigned int CEP;
	MUNICIPIO *municipio;
} ENDERECO;

/**
 * IDNFE:
 * @municipio: Município
 * @id_nfe: Id da NF
 * @nat_op: Natureza de operação
 * @ind_pag: Índice de pagamento
 * @mod: Modelo de NF
 * @serie: Série da NF
 * @num_nf: Número da NF
 * @dh_emis: Data e hora de emissão
 * @dh_saida: Data e hora de saída 
 * @tipo: Tipo de NF
 * @local_destino: Local de destino da operação
 * @tipo_impressao: Formato de impressão do DANFE
 * @tipo_emissao: Forma de emissão
 * @tipo_ambiente: Ambiente de emissão
 * @finalidade: Finalidade da NF
 * @consumidor_final: Operação com consumidor final
 * @presencial: Indicador de presencia do consumidor
 * @versao: Versão do aplicativo usado para a emissão
 * @div: Dígito verificador da NF
 * @chave: Chave da NF
 * @cod_nfe: Código aleatório da NF (parte da chave)
 *
 * Identificação da Nota Fiscal Eletrônica
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

/**
 * EMITENTE:
 * @id: Id do emitente
 * @nome: Nome do emitente
 * @inscricao_estadual: Inscrição estadual do emitente
 * @endereco: Endereço do emitente
 * @crt: Código de Regime Tributário
 * @cnpj: CNPJ do emitente
 *
 * Emitente da NF
 */
typedef struct {
	int id;
	const char *nome;
	const char *inscricao_estadual;
	ENDERECO *endereco;
	Crt crt;
	const char *cnpj;
} EMITENTE;

/**
 * DESTINATARIO:
 * @id: Id do destinatário
 * @nome: Nome do destinatário
 * @endereco: Endereço do destinatário
 * @tipo_ie: Tipo de inscrição estadual do destinatário
 * @cnpj: CNPJ do destinatário
 * @tipo_doc: tipo de documento (CNPJ ou CPF) do destinatário
 * @inscricao_estadual: Inscrição estadual do destinatário
 *
 * Destinatário da NF
 */
typedef struct {
	int id;
	const char *nome;
	ENDERECO *endereco;
	tpIE tipo_ie;
	const char *cnpj;
	const char *tipo_doc;
	const char *inscricao_estadual;
} DESTINATARIO;

/**
 * PRODUTO:
 * @id: Id do produto 
 * @codigo: Código do produto
 * @descricao: Descrição do produto
 * @ncm: Código NCM
 * @cfop: CFOP do produto
 * @unidade_comercial: Unidade comercial do produto
 * @valor: Valor do produto
 *
 * Produto
 */
typedef struct {
	int id;
	const char *codigo;
	const char *descricao;
	unsigned int ncm;
	unsigned int cfop;
	const char *unidade_comercial;
	cents valor;
} PRODUTO;

/**
 * ICMS:
 * @origem: Origem da mercadoria
 * @tipo: Tipo de ICMS
 * @aliquota: Alíquota do ICMS
 * @valor: Valor do ICMS
 *
 * Imposto ICMS 
 */
typedef struct {
	Origem origem;
	unsigned int tipo;
	aliquota aliquota;
	cents valor;
} ICMS;

/**
 * PIS:
 * @aliquota: Alíquota do PIS 
 * @quantidade: Código de situação tributária do PIS 03
 * @nt: Código de situação tributária do PIS 04
 *
 * Imposto PIS
 */
typedef struct {
	aliquota aliquota;
	unsigned int quantidade;
	const char *nt;
} PIS;

/**
 * COFINS:
 * @aliquota: Alíquota do COFINS 
 * @quantidade: Código de situação tributária do COFINS 01
 * @nt: Código de situação tributária do COFINS 04
 *
 * Imposto COFINS 
 */
typedef struct {
	aliquota aliquota;
	unsigned int quantidade;
	const char *nt;
} COFINS;

/**
 * IPI:
 * @sit_trib: Situação tributária do IPI
 * @classe: Classe do IPI
 * @codigo: Código do IPI
 *
 * Imposto IPI
 */
typedef struct {
	int sit_trib;
	const char *classe;
	const char *codigo;
} IPI;

/**
 * IMPOSTO:
 * @icms: Imposto ICMS
 * @pis: Imposto PIS
 * @cofins: Imposto COFINS 
 * @ipi: Imposto IPI
 *
 * Impostos
 */
typedef struct {
	ICMS *icms;
	PIS *pis;
	COFINS *cofins;
	IPI *ipi;
} IMPOSTO;

typedef struct ITEM ITEM;

/**
 * ITEM:
 * @produto: Produto do item
 * @imposto: Imposto do item
 * @ordem: Ordem do item
 * @quantidade: Quantidade de produtos do item
 * @valor: Valor do item
 * @pointer: Pointer to next item
 *
 * Item da NF
 */
struct ITEM {
	PRODUTO *produto;
	IMPOSTO *imposto;
	unsigned int ordem;
	unsigned int quantidade;
	cents valor;
	ITEM *pointer; //next item
};

/**
 * TRANSP:
 * @modfrete: Modalidade de frete
 *
 * Transportadora da NF
 */
typedef struct {
	modFrete modfrete;
} TRANSP;

/**
 * PROTOCOLO:
 * @dh_recib: Timestamp do recebimento do protocolo
 * @numero: Número do protocolo
 * @cod_status: Código de status do protocolo
 * @xml: XML do protocolo
 * @xmot: Tag xmot to protocolo
 *
 * Protocolo da NF enviada para o SEFAZ
 */
typedef struct {
	const char *dh_recib;
	const char *numero;
	int cod_status;
	const char *xml;
	const char *xmot;
} PROTOCOLO;

/**
 * NFE:
 * @idnfe: Identificação da NF
 * @emitente: Emitente da NF
 * @destinatario: Destinatário da NF
 * @itens: Itens da NF
 * @q_itens: Quantidade de itens da NF
 * @total: Valor total da NF
 * @transp: Transportadora NF
 * @protocolo: Protocolo de emissão da NF
 * @xml: XML da NF
 * @canceled: Flag de NF cancelada
 * @inf_ad_fisco: Informação adicional para o fisco
 * @inf_ad_contrib: Informação adicional para o contribuinte 
 *
 * Nota fiscal eletrónica
 */
typedef struct {
	IDNFE *idnfe;
	EMITENTE *emitente;
	DESTINATARIO *destinatario;
	ITEM *itens;
	unsigned int q_itens;
	cents total;
	TRANSP *transp;
	PROTOCOLO *protocolo;
	const char *xml;
	int canceled;
	const char *inf_ad_fisco;
	const char *inf_ad_contrib;
} NFE;

typedef struct LOTE_ITEM LOTE_ITEM;

/**
 * LOTE_ITEM:
 * @nfe: NF do lote
 * @next: Seguinte nota fiscal do lote
 *
 * Item do lote
 */
struct LOTE_ITEM{
	NFE *nfe;
	LOTE_ITEM *next;
};

/**
 * LOTE:
 * @id: Id do lote
 * @recibo: Recibo do lote
 * @qtd: Quantidade de itens do lote
 * @nfes: Itens do lote
 * @xml_response: XML de resposta do lote
 *
 * Lote de envio de NFs para o SEFAZ
 */
typedef struct {
	unsigned int id;
	const char *recibo;
	unsigned int qtd;
	LOTE_ITEM *nfes;
	const char *xml_response;
} LOTE;

/**
 * EVENTO:
 * @id: Id do evento 
 * @seq: Sequência do evento
 * @nfe: NF do evento
 * @type: Tipo de evento
 * @cStat: Código de status do evento
 * @xmot: xmot tag na resposta do Evento
 * @protocolo: Protocolo do Evento
 * @xml: XML do evento
 * @xml_response: XML de resposta do evento
 *
 * Evento de NF
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
 * EVENTO_CANCELAMENTO:
 * @evento: Evento
 * @justificativa: Justificativa do cancelamento
 *
 * Evento de cancelamento de NF
 */
typedef struct {
	EVENTO evento;
	const char *justificativa;
} EVENTO_CANCELAMENTO;

/**
 * List evento items
 */
typedef struct LOTE_EVENTO_ITEM LOTE_EVENTO_ITEM;

/**
 * LOTE_EVENTO_ITEM:
 * @evento: Evento
 * @next: Próximo item
 *
 * Item de lote de eventos
 */
struct LOTE_EVENTO_ITEM {
	EVENTO *evento;
	LOTE_EVENTO_ITEM *next;
};

/**
 * LOTE_EVENTO:
 * @id: Id do lote de eventos
 * @recibo: Recibo do lote de eventos
 * @qtd: Quantidade de eventos do lote
 * @eventos: Itens de lote de eventos
 * @xml_response: Resposta XML do lote de eventos
 *
 * Item de lote de eventos
 */
typedef struct {
	unsigned int id;
	const char *recibo;
	unsigned int qtd;
	LOTE_EVENTO_ITEM *eventos;
	const char *xml_response;
} LOTE_EVENTO;


#endif
