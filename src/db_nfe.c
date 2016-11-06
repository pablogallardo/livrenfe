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

#include "db_interface.h"
#include "db.h"
#include "utils.h"
#include "nfe.h"
#include "livrenfe.h"
#include <gtk/gtk.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>

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
       	sql = sqlite3_mprintf("INSERT INTO destinatarios (nome, tipo_ie, cnpj, rua, complemento, \
		bairro, id_municipio, cep) VALUES (%Q, '%d', %Q, %Q, %Q, %Q, \
		'%d', '%d');", d->nome, d->tipo_ie, d->cnpj, ed->rua, ed->complemento,
		ed->bairro, ed->municipio->codigo, ed->cep);
	db_exec(sql, &err);
	if(err){
		fprintf(stderr, "livrenfe: Error: %s", err);
		return -1;
	}
	last_id = db_last_insert_id();
       	sql = sqlite3_mprintf("INSERT INTO nfe (id_municipio, nat_op, ind_pag, mod_nfe, \
		serie, num_nf, dh_emis, dh_saida, tipo, local_destino, \
		tipo_impressao, tipo_ambiente, finalidade, consumidor_final, \
		presencial, versao, div, chave, id_emitente, id_destinatario, \
		q_itens, total, id_transportadora) VALUES  \
		(%d, %Q, %d, '%d', '%d', '%d', %lu, %Q, '%d', '%d' , '%d', '%d', \
		 '%d', '%d', '%d', '%s', '%d', %Q, '%d', '%d' , '%d', %f, %Q);",
		idnfe->municipio->codigo, idnfe->nat_op, idnfe->ind_pag, idnfe->mod, idnfe->serie,
		idnfe->num_nf, (unsigned long)idnfe->dh_emis, idnfe->dh_saida == NULL? NULL:itoa(*idnfe->dh_saida), idnfe->tipo,
		idnfe->local_destino, idnfe->tipo_impresao, idnfe->tipo_ambiente, idnfe->finalidade, idnfe->consumidor_final, idnfe->presencial,
		idnfe->versao, idnfe->div, idnfe->chave, nfe->emitente->id, last_id, nfe->q_itens,
		nfe->total, nfe->transp);
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
		sql = sqlite3_mprintf("INSERT INTO  produtos (id_produto, descricao, ncm, cfop, unidade,\
			valor_real) VALUES (%d, %Q, %d, %d, %Q, %f);",
		       p->codigo, p->descricao, p->ncm, p->cfop, p->unidade_comercial,
		       p->valor);
		db_exec(sql, &err);
		if(err){
			fprintf(stderr, "livrenfe: Error - %s", err);
			return -1;
		}
		last_id = db_last_insert_id();
		sql = sqlite3_mprintf("INSERT INTO  nfe_itens (id_nfe, ordem, id_produto, icms_origem,\
		       	icms_tipo, icms_aliquota, icms_valor, pis_aliquota, pis_quantidade,\
			pis_nt, cofins_aliquota, cofins_quantidade, cofins_nt)\
			VALUES (%d, %d, %d, %d, %d, %f, %f, %f, %f, %f, %f, %f, %f);",
			id_nf, item->ordem, last_id, icms->origem, icms->tipo, icms->aliquota, icms->valor,
			pis->aliquota, pis->quantidade, pis->nt, cofins->aliquota,
			cofins->quantidade, cofins->nt);
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

NFE *get_nfe(int id){
	NFE *nfe = new_nfe();
	sqlite3_stmt *stmt;
	char *err;
	int rc;

	char *sql = sqlite3_mprintf("SELECT n.id_nfe, m.id_municipio, m.nome, u.id_uf, u.nome, \
		n.nat_op, n.ind_pag, n.mod_nfe, n.serie, n.num_nf, n.dh_emis, n.dh_saida, \
		n.tipo, n.local_destino, n.tipo_impressao, n.tipo_emissao, n.tipo_ambiente, \
		n.finalidade, n.consumidor_final, n.presencial, n.versao, n.div, n.chave, \
		n.q_itens, n.total, e.id_emitente, e.nome, e.inscricao_estadual, \
		e.crt, e.cnpj, e.rua, e.complemento, e.bairro, e.id_municipio, m_e.nome, \
		u_e.id_uf, u_e.nome, u_e.cep, d.id_destinatario, d.nome, d.tipo_ie, \
		d.cnpj, d.rua, d.complemento, d.bairro, m_d.id_municipio, m_d.nome, \
		u_d.id_uf, u_d.nome, d.cep \
		FROM nfe n INNER JOIN municipios m ON m.id_municipio = n.id_municipio \
		INNER JOIN uf u ON u.id_uf = m.id_uf \
		INNER JOIN emitentes e ON e.id_emitente = n.id_emitente \
		INNER JOIN uf u_e u_e.id_uf = e.id_uf \
		INNER JOIN municipios m_e m_e.id_municipio = e.id_municipio \
		INNER JOIN destinatarios d ON d.id_destinatario = n.id_destinatario \ 
		INNER JOIN uf u_d ON u_d.id_uf = d.id_uf \
		INNER JOIN municipios m_d ON m_d.id_municipio = d.id_municipio \
		WHERE id_nfe = %d", id);
	return NULL;
}
