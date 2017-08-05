/* Copyright (c) 2016, 2017 Pablo G. Gallardo <pggllrd@gmail.com>
 *
 * This file is part of LivreNFE.
 *
 * LivreNFE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * LivreNFE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with LivreNFE.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "db_interface.h"
#include "db.h"
#include <libnfe/nfe.h>
#include <libnfe/libnfe.h>
#include <libnfe/errno.h>
#include "libnfe/utils.h"
#include <gtk/gtk.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

GtkListStore *get_list_nfe(){
	sqlite3_stmt *stmt;
	char *err;
	int rc;
	GtkListStore *list_store;
	GtkTreeIter iter;

	enum{
		ID_NFE,
		N_NFE,
		SERIE,
		DH_EMIS,
		DESTINATARIO,
		CANCELADA,
		EMITIDA,
		STATUS,
		N_COLS
	};

	list_store = gtk_list_store_new(N_COLS, G_TYPE_INT, G_TYPE_INT,
		G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_INT,
		G_TYPE_INT, G_TYPE_STRING);

	char *sql = "SELECT id_nfe, num_nf, serie,\
		strftime('%d/%m/%Y %H:%M:%S', dh_emis, 'unixepoch', 'localtime'), \
		cnpj || ' - ' || nome as destinatario, canceled, protocolo\
		FROM nfe JOIN destinatarios USING (id_destinatario);";
	if(db_select(sql, &err, &stmt)){
		return NULL;
	}

	do{
		rc = sqlite3_step(stmt);
		if(rc == SQLITE_ROW){
			int id_nfe = sqlite3_column_int(stmt, 0);
			int n_nfe = sqlite3_column_int(stmt, 1);
			int serie = sqlite3_column_int(stmt, 2);
			const unsigned char *dh_emis = sqlite3_column_text(stmt, 3);
			const unsigned char *destinatario = sqlite3_column_text(stmt, 4);
			int cancelada = sqlite3_column_int(stmt, 5);
			int emitida = sqlite3_column_type(stmt, 6) == SQLITE_NULL? 0:1;
			char *status;

			status = emitida? "Autorizada" : "Não enviada";
			status = cancelada? "Cancelada" : status;

			gtk_list_store_append(list_store, &iter);
			gtk_list_store_set(list_store, &iter, ID_NFE, id_nfe, 
				N_NFE, n_nfe, SERIE, serie,
				DH_EMIS, dh_emis, DESTINATARIO, destinatario, 
				CANCELADA, cancelada, EMITIDA, emitida,
				STATUS, status, -1);
		} else if(rc == SQLITE_DONE){
			break;
		} else {
			return NULL;
		}
	} while(rc == SQLITE_ROW);

	
	return list_store;
}

int register_nfe(NFE *nfe){
	set_chave(nfe);
	IDNFE *idnfe = nfe->idnfe;
	DESTINATARIO *d = nfe->destinatario;
	ENDERECO *ed = d->endereco;
	char *sql, *err;
	sql = malloc(400);
	err = NULL;
	int last_id, id_nf; 
	sql = sqlite3_mprintf("REPLACE INTO destinatarios (id_destinatario,\
		nome, tipo_ie, cnpj, rua, complemento, \
		bairro, id_municipio, cep, numero, inscricao_estadual, \
		tipo_doc) VALUES (%Q, %Q, '%d', %Q, %Q, %Q, %Q, \
		'%d', '%d', %d, %Q, %Q);", 
		d->id == 0? NULL : itoa(d->id), d->nome, d->tipo_ie, d->cnpj, 
		ed->xLgr, ed->Cpl, ed->xBairro, ed->municipio->cMun, 
		ed->CEP, ed->nro, d->inscricao_estadual, d->tipo_doc);
	db_exec(sql, &err);
	if(err){
		fprintf(stderr, "livrenfe: Error: %s", err);
		return -1;
	}
	last_id = db_last_insert_id();
       	sql = sqlite3_mprintf("REPLACE INTO nfe (id_municipio, nat_op, ind_pag, mod_nfe, \
		serie, num_nf, dh_emis, dh_saida, tipo, local_destino, \
		tipo_impressao, tipo_ambiente, finalidade, consumidor_final, \
		presencial, versao, div, chave, id_emitente, id_destinatario, \
		q_itens, total, id_transportadora, cod_nfe, tipo_emissao, id_nfe,\
		xml, protocolo, sefaz_cstat, sefaz_xmot, xml_protocolo, canceled,\
		inf_ad_fisco, inf_ad_contrib) VALUES  \
		(%d, %Q, %d, '%d', '%d', '%d', %lu, %Q, '%d', '%d' , '%d', '%d', \
		 '%d', '%d', '%d', '%s', '%d', %Q, %d, '%d' , '%d', %f, %Q,\
		 %d, %d, %Q, %Q, %Q, %d, %Q, %Q, %d, %Q, %Q);",
		idnfe->municipio->cMun, idnfe->nat_op, idnfe->ind_pag, idnfe->mod, 
		idnfe->serie, idnfe->num_nf, (unsigned long)idnfe->dh_emis,
		idnfe->dh_saida == NULL? NULL:itoa(*idnfe->dh_saida), idnfe->tipo,
		idnfe->local_destino, idnfe->tipo_impressao, idnfe->tipo_ambiente, 
		idnfe->finalidade, idnfe->consumidor_final, idnfe->presencial,
		VERSION_NAME, idnfe->div, idnfe->chave, nfe->emitente->id, 
		last_id, nfe->q_itens, nfe->total, nfe->transp, idnfe->cod_nfe, 
		idnfe->tipo_emissao, 
		nfe->idnfe->id_nfe == 0? NULL:itoa(nfe->idnfe->id_nfe),
		nfe->xml, nfe->protocolo->numero, nfe->protocolo->cod_status,
		nfe->protocolo->xmot, nfe->protocolo->xml, nfe->canceled,
		nfe->inf_ad_fisco, nfe->inf_ad_contrib);
	db_exec(sql, &err);
	if(err){
		fprintf(stderr, "livrenfe: Error - %s", err);
		return -1;
	}
	last_id = db_last_insert_id();
	id_nf = last_id;
	ITEM *item = nfe->itens;
	sql = sqlite3_mprintf("DELETE FROM nfe_itens WHERE id_nfe = %d;",
		last_id);
	db_exec(sql, &err);
	while(item != NULL){
		PRODUTO *p = item->produto;
		IMPOSTO *imp = item->imposto;
		ICMS *icms = imp->icms;
		PIS *pis = imp->pis;
		COFINS *cofins = imp->cofins;
		IPI *ipi = imp->ipi;
		sql = sqlite3_mprintf("REPLACE INTO  produtos (id_produto, \
			codigo, descricao, ncm, cfop, unidade,\
			valor) VALUES (%Q, %Q, %Q, %d, %d, %Q, %f);",
		       p->id == 0? NULL : itoa(p->id), p->codigo, 
		       p->descricao, p->ncm, p->cfop, p->unidade_comercial,
		       p->valor);
		db_exec(sql, &err);
		if(err){
			fprintf(stderr, "livrenfe: Error - %s", err);
			return -1;
		}
		last_id = db_last_insert_id();
		sql = sqlite3_mprintf("REPLACE INTO  nfe_itens (id_nfe, ordem,\
			id_produto, icms_origem, icms_tipo, icms_aliquota,\
			icms_valor, pis_aliquota, pis_quantidade,\
			pis_nt, cofins_aliquota, cofins_quantidade, cofins_nt,\
			ipi_sit_trib, ipi_classe, ipi_codigo, qtd, valor)\
			VALUES (%d, %d, %d, %d, %d, %f, %f, %f, %d, %Q, %f, \
			%d, %Q, %d, %Q, %Q, %d, %f);",
			id_nf, item->ordem, last_id, icms->origem, icms->tipo, 
			icms->aliquota, icms->valor, pis->aliquota, 
			pis->quantidade, pis->nt, cofins->aliquota,
			cofins->quantidade, cofins->nt, ipi->sit_trib,
			ipi->classe, ipi->codigo, item->quantidade,
			item->valor);
		db_exec(sql, &err);
		if(err){
			fprintf(stderr, "livrenfe: Error - %s", err);
			return -1;
		}
		item = item->pointer;
	}
	return 0;
}

GtkListStore *db_list_uf(){
	sqlite3_stmt *stmt;
	char *err;
	int rc;
	GtkListStore *list_store;
	GtkTreeIter iter;

	enum{
		ID_UF,
		UF,
		N_COLS
	};

	list_store = gtk_list_store_new(N_COLS, G_TYPE_STRING, G_TYPE_STRING);

	char *sql = "SELECT cod_ibge, nome FROM uf;";
	if(db_select(sql, &err, &stmt)){
		return NULL;
	}

	do{
		rc = sqlite3_step(stmt);
		if(rc == SQLITE_ROW){
			const unsigned char *id_uf = sqlite3_column_text(stmt, ID_UF);
			const unsigned char *uf = sqlite3_column_text(stmt, UF);

			gtk_list_store_append(list_store, &iter);
			gtk_list_store_set(list_store, &iter, ID_UF, id_uf, 
					UF, uf, -1);
		} else if(rc == SQLITE_DONE){
			break;
		} else {
			return NULL;
		}
	} while(rc == SQLITE_ROW);

	
	return list_store;
}

GtkListStore *db_list_municipios(char *uf){
	sqlite3_stmt *stmt;
	char *err;
	int rc;
	GtkListStore *list_store;
	GtkTreeIter iter;

	enum{
		ID_MUNICIPIO,
		MUNICIPIO,
		N_COLS
	};

	list_store = gtk_list_store_new(N_COLS, G_TYPE_STRING, G_TYPE_STRING);

	char *sql = malloc(100);
       	sprintf(sql, "SELECT m.id_municipio, m.nome FROM municipios m\
		JOIN uf u using(id_uf) WHERE u.cod_ibge = '%s';",
			uf);
	if(db_select(sql, &err, &stmt)){
		return NULL;
	}

	do{
		rc = sqlite3_step(stmt);
		if(rc == SQLITE_ROW){
			const unsigned char *id_municipio = sqlite3_column_text(stmt, ID_MUNICIPIO);
			const unsigned char *municipio = sqlite3_column_text(stmt, MUNICIPIO);

			gtk_list_store_append(list_store, &iter);
			gtk_list_store_set(list_store, &iter, ID_MUNICIPIO, id_municipio, 
					MUNICIPIO, municipio, -1);
		} else if(rc == SQLITE_DONE){
			break;
		} else {
			return NULL;
		}
	} while(rc == SQLITE_ROW);

	
	return list_store;
}

static int get_itens(NFE *n){
	sqlite3_stmt *stmt;
	char *err;
	int rc;

	enum{
		ID_NFE, ORDEM, ID_PRODUTO, ICMS_ORIGEM, ICMS_TIPO, ICMS_ALIQ,
		ICMS_VALOR, PIS_ALIQ, PIS_QTD, PIS_NT, COFINS_ALIQ,
		COFINS_QTD, COFINS_NT, IPI_SIT_TRIB, IPI_CLASSE, IPI_CODIGO,
		QTD, DESC, NCM, CFOP, UNIDADE, VALOR, COD_PROD, N_COLS
	};

	char *sql = sqlite3_mprintf("SELECT ni.id_nfe, ni.ordem, ni.id_produto,\
		ni.icms_origem, ni.icms_tipo, ni.icms_aliquota, ni.icms_valor,\
		ni.pis_aliquota, ni.pis_quantidade, ni.pis_nt, ni.cofins_aliquota,\
		ni.cofins_quantidade, ni.cofins_nt, ni.ipi_sit_trib,\
		ni.ipi_classe, ni.ipi_codigo, ni.qtd, p.descricao, p.ncm,\
		p.cfop, p.unidade, p.valor, p.codigo\
		FROM produtos p LEFT JOIN nfe_itens ni\
			ON ni.id_produto = p.id_produto\
		WHERE ni.id_nfe = %d", n->idnfe->id_nfe);
	if(db_select(sql, &err, &stmt)){
		return -ESQL;
	}

	do{
		ITEM *i = new_item();
		rc = sqlite3_step(stmt);
		if(rc == SQLITE_ROW){
			int ordem, id_produto, icms_origem, icms_tipo,
				pis_quantidade, pis_nt, cofins_quantidade,
				cofins_nt, ipi_sit_trib, ncm, cfop;
			double icms_aliquota, icms_valor, pis_aliquota,
				cofins_aliquota, valor, quantidade;
			char *descricao, *unidade, *ipi_classe, *ipi_codigo,
				*cod_prod; 

			ordem = sqlite3_column_int(stmt, ORDEM);
			id_produto = sqlite3_column_int(stmt, ID_PRODUTO);
			icms_origem = sqlite3_column_int(stmt, ICMS_ORIGEM);
			icms_tipo = sqlite3_column_int(stmt, ICMS_TIPO);
			pis_quantidade = sqlite3_column_int(stmt, PIS_QTD);
			pis_nt = sqlite3_column_int(stmt, PIS_NT);
			cofins_quantidade = sqlite3_column_int(stmt, COFINS_QTD);
			cofins_nt = sqlite3_column_int(stmt, COFINS_NT);
			ipi_sit_trib = sqlite3_column_int(stmt, IPI_SIT_TRIB);
			ncm = sqlite3_column_int(stmt, NCM);
			cfop = sqlite3_column_int(stmt, CFOP);

			icms_aliquota = sqlite3_column_double(stmt, ICMS_ALIQ);
			icms_valor = sqlite3_column_double(stmt, ICMS_VALOR);
			pis_aliquota = sqlite3_column_double(stmt, PIS_ALIQ);
			pis_quantidade= sqlite3_column_double(stmt, PIS_QTD);
			cofins_aliquota = sqlite3_column_double(stmt, COFINS_ALIQ);
			valor = sqlite3_column_double(stmt, VALOR);
			quantidade = sqlite3_column_double(stmt, QTD);

			descricao = strdup((char*)sqlite3_column_text(stmt, DESC));
			unidade = strdup((char*)sqlite3_column_text(stmt, UNIDADE));
			if(sqlite3_column_type(stmt, IPI_CLASSE) != SQLITE_NULL){
				ipi_classe = strdup((char*)sqlite3_column_text(stmt, 
					IPI_CLASSE));
			}
			if(sqlite3_column_type(stmt, IPI_CODIGO) != SQLITE_NULL){
				ipi_codigo = strdup((char*)sqlite3_column_text(stmt, 
					IPI_CODIGO));
			}
			if(sqlite3_column_type(stmt, COD_PROD) != SQLITE_NULL){
				cod_prod = strdup((char*)sqlite3_column_text(stmt, 
					COD_PROD));
			}

			inst_item(valor, quantidade, 
				ordem, id_produto, cod_prod, icms_origem,
				icms_tipo, pis_quantidade, pis_nt,
				cofins_quantidade, cofins_nt, ncm, cfop,
				icms_aliquota, icms_valor, pis_aliquota,
				cofins_aliquota, ipi_sit_trib, ipi_classe,
				ipi_codigo, descricao, unidade, i);
			add_item(n, i);
		} else if(rc == SQLITE_DONE){
			break;
		} else {
			return -ESQL;
		}
	} while(rc == SQLITE_ROW);
	return 0;
}

NFE *get_nfe(int id){
	sqlite3_stmt *stmt;
	char *err;
	int rc;
	NFE *nfe = NULL;
	int id_nfe, id_mun, id_uf, ind_pag, mod_nfe, serie,
		num_nf, tipo, local_destino, tipo_impressao,
		tipo_emissao, tipo_ambiente, finalidade, 
		consumidor_final, presencial, q_itens,
		id_emit, crt_emit, id_mun_emit,
		id_uf_emit, cep_emit, id_dest, t_ie_dest,
		id_mun_dest, id_uf_dest, cod_nfe,
		num_e_emit, num_e_dest, cep_dest, canceled;
	id_nfe = id_mun = id_uf = ind_pag = mod_nfe = serie =
		num_nf = tipo = local_destino = tipo_impressao =
		tipo_emissao = tipo_ambiente = finalidade = 
		consumidor_final = presencial = q_itens =
		id_emit = crt_emit = id_mun_emit =
		id_uf_emit = cep_emit = id_dest = t_ie_dest =
		id_mun_dest = id_uf_dest = cod_nfe =
		num_e_emit = num_e_dest = cep_dest = canceled = 0;
	time_t dh_emis = 0, *dh_saida = NULL;
	double total = 0;
	char *nome_mun, *uf, *nat_op, *versao,  *nome_emit, 
		*cnpj_emit, *rua_emit, *comp_emit, *bairro_emit,
		*mun_emit, *uf_emit, *nome_dest, *cnpj_dest,
		*rua_dest, *comp_dest,*bairro_dest, *mun_dest,
		*uf_dest, *chave, div, *ie_emit, *ie_dest,
		*inf_ad_fisco, *inf_ad_contrib, *tipo_doc_dest, *protocolo,
		*xml, *xml_protocolo;
	nome_mun = uf = nat_op = versao =  nome_emit = 
		cnpj_emit = rua_emit = comp_emit = bairro_emit =
		mun_emit = uf_emit = nome_dest = cnpj_dest =
		rua_dest = comp_dest =bairro_dest = mun_dest =
		uf_dest = chave = ie_emit = ie_dest = inf_ad_fisco = 
		inf_ad_contrib = tipo_doc_dest = protocolo = xml = 
		xml_protocolo = NULL;
	div = 0;

	enum{
		ID_NFE, ID_MUN, MUN, ID_UF, UF, NAT_OP, IND_PAG, MOD_NFE,
		SERIE, NUM_NF, DH_EMIS, DH_SAIDA, TIPO, LOCAL_DESTINO,
		TIPO_IMPRESSAO, TIPO_EMISSAO, TIPO_AMBIENTE, FINALIDADE, 
		CONSUMIDOR_FINAL, PRESENCIAL, VERSAO, DIV, CHAVE, Q_ITENS, 
		TOTAL, ID_EMIT, NOME_EMIT, IE_EMIT, CRT_EMIT, CNPJ_EMIT, 
		RUA_EMIT, COMP_EMIT, BAIRRO_EMIT, ID_MUN_EMIT, MUN_EMIT, 
		ID_UF_EMIT, UF_EMIT, CEP_EMIT, ID_DEST, NOME_DEST, T_IE_DEST, 
		CNPJ_DEST, RUA_DEST, COMP_DEST, BAIRRO_DEST, ID_MUN_DEST, 
		MUN_DEST, ID_UF_DEST, UF_DEST, COD_NFE, NUM_E_EMIT, NUM_E_DEST,
		IE_DEST, TIPO_DOC_DEST, CEP_DEST, CANCELED, PROTOCOLO, 
		INF_AD_FISCO, INF_AD_CONTRIB, NFE_XML, PROT_XML, N_COLS
	};

	char *sql = sqlite3_mprintf("SELECT n.id_nfe, m.id_municipio, m.nome, u.cod_ibge, u.nome, \
		n.nat_op, n.ind_pag, n.mod_nfe, n.serie, n.num_nf, n.dh_emis, n.dh_saida, \
		n.tipo, n.local_destino, n.tipo_impressao, n.tipo_emissao, n.tipo_ambiente, \
		n.finalidade, n.consumidor_final, n.presencial, n.versao, n.div, n.chave, \
		n.q_itens, n.total, e.id_emitente, e.nome, e.inscricao_estadual, \
		e.crt, e.cnpj, e.rua, e.complemento, e.bairro, e.id_municipio, m_e.nome, \
		u_e.cod_ibge, u_e.nome, e.cep, d.id_destinatario, d.nome, d.tipo_ie, \
		d.cnpj, d.rua, d.complemento, d.bairro, m_d.id_municipio, m_d.nome, \
		u_d.cod_ibge, u_d.nome, n.cod_nfe, e.numero, d.numero, \
		d.inscricao_estadual, d.tipo_doc, d.cep, n.canceled, n.protocolo,\
		n.inf_ad_fisco, n.inf_ad_contrib, n.xml, n.xml_protocolo\
		FROM nfe n LEFT JOIN municipios m ON m.id_municipio = n.id_municipio \
		LEFT JOIN uf u ON u.id_uf = m.id_uf \
		LEFT JOIN emitentes e ON e.id_emitente = n.id_emitente \
		LEFT JOIN municipios m_e ON m_e.id_municipio = e.id_municipio \
		LEFT JOIN uf u_e ON u_e.id_uf = m_e.id_uf \
		LEFT JOIN destinatarios d ON d.id_destinatario = n.id_destinatario \
		LEFT JOIN municipios m_d ON m_d.id_municipio = d.id_municipio \
		LEFT JOIN uf u_d ON u_d.id_uf = m_d.id_uf \
		WHERE n.id_nfe = %d", id);
	if(db_select(sql, &err, &stmt)){
		return NULL;
	}

	do{
		rc = sqlite3_step(stmt);
		if(rc == SQLITE_ROW){
			nfe = new_nfe();

			id_nfe = sqlite3_column_int(stmt, ID_NFE);
			id_mun = sqlite3_column_int(stmt, ID_MUN);
			id_uf = sqlite3_column_int(stmt, ID_UF);
			ind_pag = sqlite3_column_int(stmt, IND_PAG);
			mod_nfe = sqlite3_column_int(stmt, MOD_NFE);
			serie = sqlite3_column_int(stmt, SERIE);
			num_nf = sqlite3_column_int(stmt, NUM_NF);
			tipo = sqlite3_column_int(stmt, TIPO);
			local_destino = sqlite3_column_int(stmt, LOCAL_DESTINO);
			tipo_impressao = sqlite3_column_int(stmt, 
				TIPO_IMPRESSAO);
			tipo_emissao = sqlite3_column_int(stmt, TIPO_EMISSAO);
			tipo_ambiente = sqlite3_column_int(stmt, TIPO_AMBIENTE);
			finalidade = sqlite3_column_int(stmt, FINALIDADE);
			consumidor_final = sqlite3_column_int(stmt, 
				CONSUMIDOR_FINAL);
			presencial = sqlite3_column_int(stmt, PRESENCIAL);
			q_itens = sqlite3_column_int(stmt, Q_ITENS);
			id_emit = sqlite3_column_int(stmt, ID_EMIT);
			crt_emit = sqlite3_column_int(stmt, CRT_EMIT);
			id_mun_emit = sqlite3_column_int(stmt, ID_MUN_EMIT);
			id_uf_emit = sqlite3_column_int(stmt, ID_UF_EMIT);
			cep_emit = sqlite3_column_int(stmt, CEP_EMIT);
			cep_dest = sqlite3_column_int(stmt, CEP_DEST);
			id_dest = sqlite3_column_int(stmt, ID_DEST);
			t_ie_dest = sqlite3_column_int(stmt, T_IE_DEST);
			id_mun_dest = sqlite3_column_int(stmt, ID_MUN_DEST);
			id_uf_dest = sqlite3_column_int(stmt, ID_UF_DEST);
			cod_nfe = sqlite3_column_int(stmt, COD_NFE);
			num_e_emit = sqlite3_column_int(stmt, NUM_E_EMIT);
			num_e_dest = sqlite3_column_int(stmt, NUM_E_DEST);
			div = sqlite3_column_int(stmt, DIV); 
			canceled = sqlite3_column_int(stmt, CANCELED); 

			dh_emis = sqlite3_column_double(stmt, DH_EMIS);
			dh_saida = malloc(sizeof(time_t));
			if(sqlite3_column_type(stmt, DH_SAIDA) == SQLITE_NULL){
				free(dh_saida);
				dh_saida = NULL;
			} else {
				*dh_saida = sqlite3_column_double(stmt, DH_SAIDA);
			}
			total = sqlite3_column_double(stmt, TOTAL);

			nome_mun = strdup((char*)sqlite3_column_text(stmt, MUN)); 
			uf = strdup((char*)sqlite3_column_text(stmt, UF)); 
			nat_op = strdup((char*)sqlite3_column_text(stmt, NAT_OP)); 
			versao = strdup((char*)sqlite3_column_text(stmt, VERSAO)); 
			nome_emit = strdup((char*)sqlite3_column_text(stmt, NOME_EMIT)); 
			cnpj_emit = strdup((char*)sqlite3_column_text(stmt, CNPJ_EMIT)); 
			rua_emit = strdup((char*)sqlite3_column_text(stmt, RUA_EMIT)); 
			//comp_emit = strdup(sqlite3_column_text(stmt, COMP_EMIT)); 
			bairro_emit = strdup((char*)sqlite3_column_text(stmt, BAIRRO_EMIT)); 
			mun_emit = strdup((char*)sqlite3_column_text(stmt, MUN_EMIT)); 
			uf_emit = strdup((char*)sqlite3_column_text(stmt, UF_EMIT)); 
			ie_emit = strdup((char*)sqlite3_column_text(stmt, IE_EMIT));
			//ie_dest = strdup(sqlite3_column_text(stmt, IE_DEST));
			nome_dest = strdup((char*)sqlite3_column_text(stmt, NOME_DEST)); 
			cnpj_dest = strdup((char*)sqlite3_column_text(stmt, CNPJ_DEST)); 
			rua_dest = strdup((char*)sqlite3_column_text(stmt, RUA_DEST)); 
			comp_dest = strdup((char*)sqlite3_column_text(stmt, COMP_DEST)); 
			bairro_dest = strdup((char*)sqlite3_column_text(stmt, BAIRRO_DEST)); 
			mun_dest = strdup((char*)sqlite3_column_text(stmt, MUN_DEST)); 
			uf_dest = strdup((char*)sqlite3_column_text(stmt, UF_DEST)); 
			tipo_doc_dest = strdup((char*)sqlite3_column_text(stmt, TIPO_DOC_DEST));
			chave = strdup((char*)sqlite3_column_text(stmt, CHAVE)); 
			if(sqlite3_column_type(stmt, PROTOCOLO) == SQLITE_NULL){
				protocolo = NULL;
			} else {
				protocolo = strdup((char*)sqlite3_column_text(stmt,
					PROTOCOLO)); 
			}
			if(sqlite3_column_type(stmt, INF_AD_FISCO) == SQLITE_NULL){
				inf_ad_fisco = NULL;
			} else {
				inf_ad_fisco = strdup((char*)sqlite3_column_text(stmt,
					INF_AD_FISCO)); 
			}
			if(sqlite3_column_type(stmt, INF_AD_CONTRIB) == SQLITE_NULL){
				inf_ad_contrib = NULL;
			} else {
				inf_ad_contrib = strdup((char*)sqlite3_column_text(stmt,
					INF_AD_CONTRIB)); 
			}
			if(sqlite3_column_type(stmt, NFE_XML) != SQLITE_NULL){
				xml = strdup((char*)sqlite3_column_text(stmt,
					NFE_XML)); 
			}
			if(sqlite3_column_type(stmt, PROT_XML) != SQLITE_NULL){
				xml_protocolo = strdup((char*)sqlite3_column_text(stmt,
					PROT_XML)); 
			}
		} else if(rc == SQLITE_DONE){
			break;
		} else {
			return NULL;
		}
	} while(rc == SQLITE_ROW);
	nfe = new_nfe();

	//EMITENTE
	inst_endereco(rua_emit, num_e_emit, comp_emit, bairro_emit, cep_emit,
		mun_emit, id_mun_emit, uf_emit, id_uf_emit, "Brasil", 1058,
		nfe->emitente->endereco);
	inst_emitente(id_emit, nome_emit, ie_emit, crt_emit, cnpj_emit,
		nfe->emitente->endereco, nfe->emitente);


	//DESTINATARIO
	inst_endereco(rua_dest, num_e_dest, comp_dest, bairro_dest, cep_dest,
		mun_dest, id_mun_dest, uf_dest, id_uf_dest, "Brasil", 1058,
		nfe->destinatario->endereco);
	inst_destinatario(id_dest, nome_dest, t_ie_dest, tipo_doc_dest, 
		ie_dest, cnpj_dest, nfe->destinatario->endereco, nfe->destinatario);

	//MUNICIPIO DA NFE
	inst_municipio(nome_mun, id_mun, uf, id_uf, "Brasil", 1058,
		nfe->idnfe->municipio);

	inst_nfe(id_nfe, serie, num_nf, ind_pag, mod_nfe, tipo, local_destino,
		tipo_impressao, tipo_emissao, tipo_ambiente, finalidade,
		consumidor_final, presencial, cod_nfe, dh_emis, dh_saida,
		nat_op, versao, inf_ad_fisco, inf_ad_contrib, NULL,
		nfe->emitente, nfe->destinatario, nfe->idnfe->municipio,
		nfe);

	nfe->xml = xml;
	nfe->protocolo->numero = protocolo;
	nfe->protocolo->xml = xml_protocolo;

	get_itens(nfe);
	return nfe; 
}

EMITENTE *get_emitente(int id){
	EMITENTE *e = new_emitente();

	char *err;
	sqlite3_stmt *stmt;
	enum{
		ID_EMIT, NOME, IE, CRT, CNPJ, RUA, BAIRRO, ID_MUN, CEP,
		NUM, COMP, MUN, ID_UF, UF
	};
	int crt, num, id_uf, id_mun, cep, rc;
	char *nome, *ie, *cnpj, *rua, *bairro, *mun, *uf, *comp;
	char *sql = sqlite3_mprintf("SELECT e.id_emitente, e.nome,\
		e.inscricao_estadual, e.crt, e.cnpj, e.rua, e.bairro,\
		e.id_municipio, e.cep, e.numero, e.complemento, m.nome,\
		u.cod_ibge, u.nome\
		FROM emitentes e LEFT JOIN municipios m \
			ON m.id_municipio = e.id_municipio\
		LEFT JOIN uf u ON u.id_uf = m.id_uf\
		WHERE e.id_emitente = %d", id);
	if(db_select(sql, &err, &stmt)){
		free_emitente(e);
		return NULL;
	}

	rc = sqlite3_step(stmt);
	if(rc != SQLITE_ROW){
		free_emitente(e);
		return NULL;
	}
	crt = sqlite3_column_int(stmt, CRT);
	
	nome = strdup((char*)sqlite3_column_text(stmt, NOME));
	ie = strdup((char*)sqlite3_column_text(stmt, IE));
	cnpj = strdup((char*)sqlite3_column_text(stmt, CNPJ));

	rua = strdup((char*)sqlite3_column_text(stmt, RUA));
	bairro = strdup((char*)sqlite3_column_text(stmt, BAIRRO));
	mun = strdup((char*)sqlite3_column_text(stmt, MUN));
	
	id_uf = sqlite3_column_int(stmt, ID_UF);
	uf = strdup((char*)sqlite3_column_text(stmt, UF));
	
	comp = sqlite3_column_text(stmt, COMP)? 
		strdup((char*)sqlite3_column_text(stmt, COMP)) : NULL;
	num = sqlite3_column_int(stmt, NUM);
	id_mun = sqlite3_column_int(stmt, ID_MUN);
	cep = sqlite3_column_int(stmt, CEP);

	inst_endereco(rua, num, comp, bairro, cep, mun, id_mun, uf, id_uf, 
		"Brasil", 1058,  e->endereco);
	inst_emitente(id, nome, ie, crt, cnpj, e->endereco, e);
	return e;

}

DESTINATARIO *get_destinatario_by_doc(const char *doc){
	DESTINATARIO *d = new_destinatario();
	char *err;
	sqlite3_stmt *stmt;
	enum{
		ID_DEST, NOME, TIPO_IE, IE, TIPO_DOC, CNPJ, RUA, BAIRRO, 
		ID_MUN, CEP, NUM, COMP, MUN, ID_UF, UF
	};
	int id, num, id_uf, id_mun, cep, rc, t_ie;
	char *nome, *ie, *cnpj, *rua, *bairro, *mun, *uf, *comp, *tipo_doc;
	char *sql = sqlite3_mprintf("SELECT d.id_destinatario, d.nome,\
		d.tipo_ie, d.inscricao_estadual, d.tipo_doc, d.cnpj, d.rua, \
		d.bairro, d.id_municipio, d.cep, d.numero, d.complemento, \
		m.nome, u.cod_ibge, u.nome\
		FROM destinatarios d LEFT JOIN municipios m \
			ON m.id_municipio = d.id_municipio\
		LEFT JOIN uf u ON u.id_uf = m.id_uf\
		WHERE d.cnpj = %Q ORDER BY d.id_destinatario desc", doc);
	if(db_select(sql, &err, &stmt)){
		return NULL;
	}

	rc = sqlite3_step(stmt);
	if(rc != SQLITE_ROW)
		return NULL;

	id = sqlite3_column_int(stmt, ID_DEST);
	num = sqlite3_column_int(stmt, NUM);
	id_uf = sqlite3_column_int(stmt, ID_UF);
	id_mun = sqlite3_column_int(stmt, ID_MUN);
	cep = sqlite3_column_int(stmt, CEP);
	t_ie = sqlite3_column_int(stmt, TIPO_IE);

	nome = strdup((char*)sqlite3_column_text(stmt, NOME));
	ie = sqlite3_column_text(stmt, IE)? 
		strdup((char*)sqlite3_column_text(stmt, IE)) : NULL;
	cnpj = strdup((char*)sqlite3_column_text(stmt, CNPJ));
	rua = strdup((char*)sqlite3_column_text(stmt, RUA));
	bairro = strdup((char*)sqlite3_column_text(stmt, BAIRRO));
	mun = strdup((char*)sqlite3_column_text(stmt, MUN));
	uf = strdup((char*)sqlite3_column_text(stmt, UF));
	tipo_doc = strdup((char*)sqlite3_column_text(stmt, TIPO_DOC));
	comp = sqlite3_column_text(stmt, COMP)? 
		strdup((char*)sqlite3_column_text(stmt, COMP)) : NULL;
	inst_endereco(rua, num, comp, bairro, cep, mun, id_mun, uf, id_uf, 
		"Brasil", 1058, d->endereco);
	inst_destinatario(id, nome, t_ie, tipo_doc, ie, cnpj, d->endereco, d);
	return d;
}

int set_emitente(EMITENTE *e){
	char *sql, *err;
	err = NULL;
       	sql = sqlite3_mprintf("REPLACE INTO emitentes\
  		(id_emitente, nome, inscricao_estadual, crt, cnpj, rua,\
		complemento, bairro, id_municipio, cep, numero)\
 		VALUES (1, %Q, %Q, %d, %Q, %Q, %Q, %Q, %d, %d, %d);", 
		e->nome, e->inscricao_estadual, e->crt, e->cnpj,
		e->endereco->xLgr, e->endereco->Cpl,
		e->endereco->xBairro, e->endereco->municipio->cMun,
		e->endereco->CEP, e->endereco->nro);
	db_exec(sql, &err);
	if(err)
		return -ESQL;
	return 0;
}

int get_lote_id(){
	char *err;
	sqlite3_stmt *stmt;
	int id, rc;
	char *sql = sqlite3_mprintf("SELECT max(l.id_lote)\
		FROM lotes l");
	if(db_select(sql, &err, &stmt)){
		return -ESQL;
	}

	rc = sqlite3_step(stmt);
	if(rc != SQLITE_ROW)
		return 1;
	id = sqlite3_column_int(stmt, 0);
	return ++id;
}

int get_lote_evento_id(){
	char *err;
	sqlite3_stmt *stmt;
	int id, rc;
	char *sql = sqlite3_mprintf("SELECT max(l.id_lote_evento)\
		FROM lotes_evento l");
	if(db_select(sql, &err, &stmt)){
		return -ESQL;
	}

	rc = sqlite3_step(stmt);
	if(rc != SQLITE_ROW)
		return 1;
	id = sqlite3_column_int(stmt, 0);
	return ++id;
}

int get_next_nfe_number(int *number, int *serie){
	char *err;
	sqlite3_stmt *stmt;
	int rc;
	char *sql = sqlite3_mprintf("SELECT max(num_nf) + 1, serie\
		FROM nfe");
	if(db_select(sql, &err, &stmt)){
		return -ESQL;	
	}

	rc = sqlite3_step(stmt);
	if(rc != SQLITE_ROW){
		return -1;
	}
	*number = sqlite3_column_int(stmt, 0);
	*serie = sqlite3_column_int(stmt, 1);
	return 0;
}

int db_save_lote(LOTE *lote){
	char *sql, *err;
	err = NULL;
	if(lote->qtd > 0){
		sql = sqlite3_mprintf("INSERT INTO lotes (id_lote, recibo, \
			xml_response)\
			VALUES (%d, %Q, %Q)", lote->id, lote->recibo, 
				lote->xml_response);
		db_exec(sql, &err);
		if(err)
			return -ESQL;
		LOTE_ITEM *i = lote->nfes;
		sql = malloc(sizeof(char) * 500);
		strcpy(sql, "INSERT INTO lotes_nfes (id_lote, id_nfe) VALUES ");
		while(i != NULL){
			NFE *n = i->nfe;
			char *aux = sqlite3_mprintf("(%d, %d)", lote->id,
				n->idnfe);
			if(i != lote->nfes)
				strcat(sql, ", ");
			strcat(sql, aux);
			register_nfe(n);
			i = i->next;
		}
		db_exec(sql, &err);
		if(err)
			return -ESQL;
	}
	return 0;
}

int db_save_lote_evento(LOTE_EVENTO *lote){
	char *sql, *err;
	err = NULL;
	if(lote->qtd > 0){
		sql = sqlite3_mprintf("INSERT INTO lotes_evento \
			(id_lote_evento, recibo, xml_response)\
			VALUES (%d, %Q, %Q)", lote->id, lote->recibo, 
				lote->xml_response);
		db_exec(sql, &err);
		if(err)
			return -ESQL;
		LOTE_EVENTO_ITEM *i = lote->eventos;
		sql = malloc(sizeof(char) * 500);
		strcpy(sql, "INSERT INTO lotes_evento_items(id_lote_evento,\
			id_evento) VALUES ");
		while(i != NULL){
			EVENTO *e = i->evento;
			register_evento(e);
			char *aux = sqlite3_mprintf("(%d, %Q)", lote->id,
				e->id);
			if(i != lote->eventos)
				strcat(sql, ", ");
			strcat(sql, aux);
			i = i->next;
		}
		db_exec(sql, &err);
		if(err)
			return -ESQL;
	}
	return 0;
}

int register_evento(EVENTO *e){
	char *sql, *err;
	sql = malloc(400);
	err = NULL;
	int last_id; 
	sql = sqlite3_mprintf("REPLACE INTO eventos (id_evento, id_nfe, type,\
		xml, xml_response, xmot)\
		VALUES (%Q, %d, %d, %Q, %Q, %Q);",
		e->id == 0? NULL : e->id, e->nfe->idnfe->id_nfe, e->type, 
		e->xml, e->xml_response, e->xmot);
	db_exec(sql, &err);
	last_id = db_last_insert_id();
	e->id = itoa(last_id);
	if(err){
		fprintf(stderr, "livrenfe: Error: %s", err);
		return -ESQL;
	}
	register_nfe(e->nfe);
	switch(e->type){
		case CANCELAMENTO_TYPE:{
			EVENTO_CANCELAMENTO *ec = (EVENTO_CANCELAMENTO*) e;
			sql = sqlite3_mprintf("UPDATE evento SET \
				justificativa = %Q WHERE id_evento = %d;", 
				ec->justificativa, last_id);
			db_exec(sql, &err);
			if(err){
				fprintf(stderr, "livrenfe: Error: %s", err);
				return -ESQL;
			}
			break;
		}
	}
	return 0;
}
