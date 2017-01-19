/* Copyright (c) 2016 Pablo G. Gallardo <pggllrd@gmail.com>
 *
 * This file is part of LivreNFE.
 *
 * LivreNFE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * * LivreNFE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LivreNFE.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "db_interface.h"
#include "db.h"
#include "utils.h"
#include "nfe.h"
#include "livrenfe.h"
#include "errno.h"
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
		N_COLS
	};

	list_store = gtk_list_store_new(N_COLS, G_TYPE_INT, G_TYPE_INT,
			G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING);

	char *sql = "SELECT id_nfe, num_nf, serie, strftime('%d/%m/%Y %H:%M:%S', dh_emis, 'unixepoch', 'localtime'), \
			cnpj || ' - ' || nome as destinatario \
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

			gtk_list_store_append(list_store, &iter);
			gtk_list_store_set(list_store, &iter, ID_NFE, id_nfe, 
					N_NFE, n_nfe, SERIE, serie,
					DH_EMIS, dh_emis, DESTINATARIO, destinatario, -1);
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
       	sql = sqlite3_mprintf("REPLACE INTO destinatarios (nome, tipo_ie, cnpj, rua, complemento, \
		bairro, id_municipio, cep, numero, inscricao_estadual, \
		tipo_doc) VALUES (%Q, '%d', %Q, %Q, %Q, %Q, \
		'%d', '%d', %d, %Q, %Q);", d->nome, d->tipo_ie, d->cnpj, 
		ed->rua, ed->complemento, ed->bairro, ed->municipio->codigo, 
		ed->cep, ed->num, d->inscricao_estadual, d->tipo_doc);
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
		q_itens, total, id_transportadora, cod_nfe, tipo_emissao) VALUES  \
		(%d, %Q, %d, '%d', '%d', '%d', %lu, %Q, '%d', '%d' , '%d', '%d', \
		 '%d', '%d', '%d', '%s', '%d', %Q, %d, '%d' , '%d', %f, %Q,\
		 %d, %d);",
		idnfe->municipio->codigo, idnfe->nat_op, idnfe->ind_pag, idnfe->mod, idnfe->serie,
		idnfe->num_nf, (unsigned long)idnfe->dh_emis, idnfe->dh_saida == NULL? NULL:itoa(*idnfe->dh_saida), idnfe->tipo,
		idnfe->local_destino, idnfe->tipo_impressao, idnfe->tipo_ambiente, idnfe->finalidade, idnfe->consumidor_final, idnfe->presencial,
		VERSION_NAME, idnfe->div, idnfe->chave, nfe->emitente->id, 
		last_id, nfe->q_itens, nfe->total, nfe->transp, idnfe->cod_nfe, 
		idnfe->tipo_emissao);
	db_exec(sql, &err);
	if(err){
		fprintf(stderr, "livrenfe: Error - %s", err);
		return -1;
	}
	last_id = db_last_insert_id();
	id_nf = last_id;
	ITEM *item = nfe->itens;
	while(item != NULL){
		PRODUTO *p = item->produto;
		IMPOSTO *imp = item->imposto;
		ICMS *icms = imp->icms;
		PIS *pis = imp->pis;
		COFINS *cofins = imp->cofins;
		sql = sqlite3_mprintf("REPLACE INTO  produtos (id_produto, descricao, ncm, cfop, unidade,\
			valor) VALUES (%d, %Q, %d, %d, %Q, %f);",
		       p->codigo, p->descricao, p->ncm, p->cfop, p->unidade_comercial,
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
			qtd, valor)\
			VALUES (%d, %d, %d, %d, %d, %f, %f, %f, %d, %Q, %f, \
			%d, %Q, %d, %f);",
			id_nf, item->ordem, last_id, icms->origem, icms->tipo, 
			icms->aliquota, icms->valor, pis->aliquota, 
			pis->quantidade, pis->nt, cofins->aliquota,
			cofins->quantidade, cofins->nt, item->quantidade,
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
		COFINS_QTD, COFINS_NT, QTD, DESC, NCM, CFOP, UNIDADE, VALOR,
		N_COLS
	};

	char *sql = sqlite3_mprintf("SELECT ni.id_nfe, ni.ordem, ni.id_produto,\
		ni.icms_origem, ni.icms_tipo, ni.icms_aliquota, ni.icms_valor,\
		ni.pis_aliquota, ni.pis_quantidade, ni.pis_nt, ni.cofins_aliquota,\
		ni.cofins_quantidade, ni.cofins_nt, ni.qtd, p.descricao, p.ncm, p.cfop,\
		p.unidade, p.valor\
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
			int id_nfe, ordem, id_produto, icms_origem, icms_tipo,
				pis_quantidade, pis_nt, cofins_quantidade,
				cofins_nt, ncm, cfop;
			float icms_aliquota, icms_valor, pis_aliquota,
				cofins_aliquota, valor, quantidade;
			char *descricao, *unidade; 

			id_nfe = sqlite3_column_int(stmt, ID_NFE);
			ordem = sqlite3_column_int(stmt, ORDEM);
			id_produto = sqlite3_column_int(stmt, ID_PRODUTO);
			icms_origem = sqlite3_column_int(stmt, ICMS_ORIGEM);
			icms_tipo = sqlite3_column_int(stmt, ICMS_TIPO);
			pis_quantidade = sqlite3_column_int(stmt, PIS_QTD);
			pis_nt = sqlite3_column_int(stmt, PIS_NT);
			cofins_quantidade = sqlite3_column_int(stmt, COFINS_QTD);
			cofins_nt = sqlite3_column_int(stmt, COFINS_NT);
			ncm = sqlite3_column_int(stmt, NCM);
			cfop = sqlite3_column_int(stmt, CFOP);

			icms_aliquota = sqlite3_column_double(stmt, ICMS_ALIQ);
			icms_valor = sqlite3_column_double(stmt, ICMS_VALOR);
			pis_aliquota = sqlite3_column_double(stmt, PIS_ALIQ);
			pis_quantidade= sqlite3_column_double(stmt, PIS_QTD);
			cofins_aliquota = sqlite3_column_double(stmt, COFINS_ALIQ);
			valor = sqlite3_column_double(stmt, VALOR);
			quantidade = sqlite3_column_double(stmt, QTD);

			descricao = strdup(sqlite3_column_text(stmt, DESC));
			unidade = strdup(sqlite3_column_text(stmt, UNIDADE));

			inst_item(valor, quantidade, 
				ordem, id_produto, icms_origem,
				icms_tipo, pis_quantidade, pis_nt,
				cofins_quantidade, cofins_nt, ncm, cfop,
				icms_aliquota, icms_valor, pis_aliquota,
				cofins_aliquota, descricao, unidade, i);
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
	NFE *nfe;
	int id_nfe, id_mun, id_uf, ind_pag, mod_nfe, serie,
		num_nf, tipo, local_destino, tipo_impressao,
		tipo_emissao, tipo_ambiente, finalidade, 
		consumidor_final, presencial, q_itens,
		id_emit, crt_emit, id_mun_emit,
		id_uf_emit, cep_emit, id_dest, t_ie_dest,
		id_mun_dest, id_uf_dest, cod_nfe,
		num_e_emit, num_e_dest, cep_dest;
	time_t dh_emis, *dh_saida;
	float total;
	char *nome_mun, *uf, *nat_op, *versao,  *nome_emit, 
		*cnpj_emit, *rua_emit, *comp_emit, *bairro_emit,
		*mun_emit, *uf_emit, *nome_dest, *cnpj_dest,
		*rua_dest, *comp_dest,*bairro_dest, *mun_dest,
		*uf_dest, *chave, div, *ie_emit, *ie_dest,
		*tipo_doc_dest;

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
		IE_DEST, TIPO_DOC_DEST, CEP_DEST, N_COLS
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
		d.inscricao_estadual, d.tipo_doc, d.cep\
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

			dh_emis = sqlite3_column_double(stmt, DH_EMIS);
			dh_saida = malloc(sizeof(float));
			if(sqlite3_column_type(stmt, DH_SAIDA) == SQLITE_NULL){
				free(dh_saida);
				dh_saida = NULL;
			} else {
				*dh_saida = sqlite3_column_double(stmt, DH_SAIDA);
			}
			total = sqlite3_column_double(stmt, TOTAL);

			nome_mun = strdup(sqlite3_column_text(stmt, MUN)); 
			uf = strdup(sqlite3_column_text(stmt, UF)); 
			nat_op = strdup(sqlite3_column_text(stmt, NAT_OP)); 
			versao = strdup(sqlite3_column_text(stmt, VERSAO)); 
			nome_emit = strdup(sqlite3_column_text(stmt, NOME_EMIT)); 
			cnpj_emit = strdup(sqlite3_column_text(stmt, CNPJ_EMIT)); 
			rua_emit = strdup(sqlite3_column_text(stmt, RUA_EMIT)); 
			//comp_emit = strdup(sqlite3_column_text(stmt, COMP_EMIT)); 
			bairro_emit = strdup(sqlite3_column_text(stmt, BAIRRO_EMIT)); 
			mun_emit = strdup(sqlite3_column_text(stmt, MUN_EMIT)); 
			uf_emit = strdup(sqlite3_column_text(stmt, UF_EMIT)); 
			//ie_emit = strdup(sqlite3_column_text(stmt, IE_EMIT));
			//ie_dest = strdup(sqlite3_column_text(stmt, IE_DEST));
			nome_dest = strdup(sqlite3_column_text(stmt, NOME_DEST)); 
			cnpj_dest = strdup(sqlite3_column_text(stmt, CNPJ_DEST)); 
			rua_dest = strdup(sqlite3_column_text(stmt, RUA_DEST)); 
			comp_dest = strdup(sqlite3_column_text(stmt, COMP_DEST)); 
			bairro_dest = strdup(sqlite3_column_text(stmt, BAIRRO_DEST)); 
			mun_dest = strdup(sqlite3_column_text(stmt, MUN_DEST)); 
			uf_dest = strdup(sqlite3_column_text(stmt, UF_DEST)); 
			tipo_doc_dest = strdup(sqlite3_column_text(stmt, TIPO_DOC_DEST));
			chave = strdup(sqlite3_column_text(stmt, CHAVE)); 
		} else if(rc == SQLITE_DONE){
			break;
		} else {
			return NULL;
		}
	} while(rc == SQLITE_ROW);
	inst_nfe(id_nfe, id_mun, id_uf, ind_pag, mod_nfe,
		serie, num_nf, tipo, local_destino, 
		tipo_impressao, tipo_emissao, tipo_ambiente, 
		finalidade, consumidor_final, presencial, q_itens,
		id_emit, ie_emit, crt_emit, id_mun_emit,
		id_uf_emit, cep_emit, num_e_emit, id_dest, 
		t_ie_dest, id_mun_dest, id_uf_dest, num_e_dest,
		cod_nfe, cep_dest, dh_emis, dh_saida, total,
		nome_mun, uf, nat_op, versao, 
		nome_emit, cnpj_emit, rua_emit,
		comp_emit, bairro_emit, mun_emit, uf_emit,
		nome_dest, cnpj_dest, rua_dest, 
		comp_dest, bairro_dest, mun_dest,
		uf_dest, chave, div, ie_dest,
		tipo_doc_dest, nfe);
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
		return NULL;
	}

	rc = sqlite3_step(stmt);
	if(rc != SQLITE_ROW)
		return NULL;
	crt = sqlite3_column_int(stmt, CRT);
	num = sqlite3_column_int(stmt, NUM);
	id_uf = sqlite3_column_int(stmt, ID_UF);
	id_mun = sqlite3_column_int(stmt, ID_MUN);
	cep = sqlite3_column_int(stmt, CEP);

	nome = strdup(sqlite3_column_text(stmt, NOME));
	ie = strdup(sqlite3_column_text(stmt, IE));
	cnpj = strdup(sqlite3_column_text(stmt, CNPJ));
	rua = strdup(sqlite3_column_text(stmt, RUA));
	bairro = strdup(sqlite3_column_text(stmt, BAIRRO));
	mun = strdup(sqlite3_column_text(stmt, MUN));
	uf = strdup(sqlite3_column_text(stmt, UF));
	comp = sqlite3_column_text(stmt, COMP)? 
		strdup(sqlite3_column_text(stmt, COMP)) : NULL;
	inst_emitente(id, nome, ie, crt, cnpj, rua, num, comp,
		bairro, uf, mun, id_mun, id_uf, cep, e);
	return e;

}

int set_emitente(EMITENTE *e){
	char *sql, *err;
	err = NULL;
       	sql = sqlite3_mprintf("REPLACE INTO emitentes\
  		(id_emitente, nome, inscricao_estadual, crt, cnpj, rua,\
		complemento, bairro, id_municipio, cep, numero)\
 		VALUES (1, %Q, %Q, %d, %Q, %Q, %Q, %Q, %d, %d, %d);", 
		e->nome, e->inscricao_estadual, e->crt, e->cnpj,
		e->endereco->rua, e->endereco->complemento,
		e->endereco->bairro, e->endereco->municipio->codigo,
		e->endereco->cep, e->endereco->num);
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
		FROM lote l");
	if(db_select(sql, &err, &stmt)){
		return -ESQL;
	}

	rc = sqlite3_step(stmt);
	if(rc != SQLITE_ROW)
		return 1;
	id = sqlite3_column_int(stmt, 0);
	return ++id;
}

int db_save_lote(LOTE *lote){
	char *sql, *err;
	err = NULL;
	if(lote->qtd > 0){
		LOTE_ITEM *i = lote->nfes;
		while(i != NULL){
			//TODO
			i = i->next;
		}
		db_exec(sql, &err);
		if(err)
			return -ESQL;
	}
	return 0;
}
