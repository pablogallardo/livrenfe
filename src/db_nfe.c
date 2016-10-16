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
#include "livrenfe.h"
#include <gtk/gtk.h>
#include <sqlite3.h>
#include <stdio.h>

GtkListStore *get_list_nfe(){
	sqlite3 *db;
	sqlite3_stmt *stmt;
	char *err;
	int rc, res;
	GtkListStore *list_store;
	GtkTreePath * path;
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

	char *sql = "SELECT id_nfe, num_nf, serie, dh_emis, \
			cnpj || ' - ' || nome as destinatario \
	       		FROM nfe JOIN destinatarios USING (id_destinatario);";
	if(db_select(sql, &err, &db, &stmt)){
		return NULL;
	}

	do{
		res = sqlite3_step(stmt);
		if(res == SQLITE_ROW){
			int id_nfe = sqlite3_column_int(stmt, 0);
			int n_nfe = sqlite3_column_int(stmt, 1);
			int serie = sqlite3_column_int(stmt, 2);
			const unsigned char *dh_emis = sqlite3_column_text(stmt, 3);
			const unsigned char *destinatario = sqlite3_column_text(stmt, 4);

			gtk_list_store_append(list_store, &iter);
			gtk_list_store_set(list_store, &iter, ID_NFE, id_nfe, 
					N_NFE, n_nfe, SERIE, serie,
					DH_EMIS, dh_emis, DESTINATARIO, destinatario, -1);
		} else if(res == SQLITE_DONE){
			break;
		} else {
			return NULL;
		}
	} while(res == SQLITE_ROW);

	if(db_close(db, stmt, &err)){
		fprintf(stderr, "livrenfe: SQL Error: %s\n", err);
		return NULL;
	}
	
	return list_store;
}

int register_nfe(t_nfe *nfe){
	t_idnfe idnfe = nfe->idnfe;
	char *sql;
	char *err;
       	sprintf(sql, "INSERT INTO nfe (id_municipio, nap_op, ind_pag, mod_nfe,\
		serie, num_nf, dh_emis, dh_saida, tipo, local_destino,\
		tipo_impressao, tipo_ambiente, finalidade, consumidor_final,\
		presencial, versao, div, chave, id_emitente, id_destinatario,\
		q_itens, total, id_transportadora) VALUES \
		(%d, %s, %d, %s, %s, %s, %s, %s, %s, %s , %s, %s, %s, %s, %s, \
		 %s, %s, %s, %s, %s , %s, %s, %s)",
	idnfe.municipio.codigo, idnfe.nat_op, idnfe.ind_pag, idnfe.serie,
	idnfe.num_nf, timetostr(idnfe.dh_emis), timetostr(idnfe.dh_saida), idnfe.tipo,
	idnfe.local_destino, idnfe.tipo_impresao, idnfe.tipo_ambiente, idnfe.finalidade, idnfe.consumidor_final, idnfe.presencial,
	idnfe.versao, idnfe.div, idnfe.chave, nfe->emitente.id, nfe->destinatario.id, nfe->q_itens, nfe->total, "NULL");
	db_exec(sql, &err);
	if(err){
		fprintf(stderr, "livrenfe: Error - %s", err);
		return -1;
	}
	return 0;
}
