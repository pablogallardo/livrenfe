/* Copyright (c)  2017 Gabriel Lampa  <gabrielampa@gmail.com>
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

#ifndef ENUM_H
#define ENUM_H


typedef enum t_indPag {
  A_VISTA = 0,
  A_PRAZO = 1,
   OUTROS = 2
}TE_INDPAG  ;

 typedef enum  t_mod {
	 NFE  =55,
	 NFCe = 65
 }TE_MOD;

typedef enum t_tpNF{
	ENTRADA = 0,
	SAIDA   = 1
}TE_TPNF;

typedef enum t_idDest{
	OPERACAO_INTERNA = 1,
	OPERACAO_INTERESTADUAL = 2,
	OPERACAO_EXTERIOR = 3
}TE_IDDEST;

typedef enum t_tpImp{
	SEM_GERAR_DANFE = 0,
	DANFE_NORMAL_RETRATO = 1,
	DANFE_NORMAL_PAISAGEM = 2,
	DANFE_SIMPLIFICADO = 3,
	DANFE_NFCe = 4,
	DANFE_NFCe_ELETRONICA = 5
}TE_TPIMP;

typedef enum t_tpEmis{
	EMISSAO_NORMAL =1,
	EMISSAO_CONTIGENCIA_FSIA = 2,
	EMISSAO_CONTIGENCIA_SCAN = 3,
	EMISSAO_CONTIGENCIA_DPEC = 4,
	EMISSAO_CONTIGENCIA_FSDA = 5,
	EMISSAO_CONTIGENCIA_SVC_AN = 6,
	EMISSAO_CONTIGENCIA_SVC_RS = 7,
	EMISSAO_CONTIGENCIA_OFFLINE = 9
}TE_TPEMIS;

typedef enum t_tpAmb{
	PRODUCAO = 1,
	HOMOLOGACAO = 2
}TE_TPAMB;

typedef enum t_finNFe{
	NFe_NORMAL = 1,
	NFe_COMPLEMENTAR = 2,
	NFe_AJUSTE = 3,
	NFe_DEVOLUCAO = 4
}TE_FINNFE;

typedef enum t_indFinal{
	NORMAL = 0,
	CONSUMIDOR_FINAL = 1
}TE_INDFINAL;

typedef enum t_indPres{
	NAO_SE_APLICA = 0,
	OPERACAO_PRESENCIAL = 1,
	OPERACAO_NAO_PRESENCIAL_INTERNET = 2,
	OPERACAO_NAO_PRESENCIAL_TELEATENDIMENTO = 3,
	OPERACAO_NFCe_ENTREGA_DOMICILIO = 4,
	OPERACAO_NAO_PRESENCIAL_OUTROS = 9
}TE_INDPRES;

typedef enum t_procEmi{
	EMISSAO_NFE_APP_CLIENTE = 0,
	EMISSAO_NFE_AVULSA_FISCO = 1,
	EMISSAO_NFE_AVULSA_CONT_CERT_SITE_FISCO = 2,
	EMISSAO_NFE_FISCO_APP = 3
}TE_PROCEMI;





#endif
