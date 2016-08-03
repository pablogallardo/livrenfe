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
#include <gtk/gtk.h>
#include <sqlite3.h>

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
		DH_EMIS,
		N_COLS
	};

	list_store = gtk_list_store_new(N_COLS, G_TYPE_INT, G_TYPE_INT);

	char *sql = "SELECT id_nfe, dh_emis FROM nfe";
	if(db_select(sql, &err, db, stmt)){
		return NULL;
	}

	do{
		res = sqlite3_step(stmt);
		if(res == SQLITE_ROW || res == SQLITE_DONE){
			int id_nfe = sqlite3_column_int(stmt, 0);
			int dh_emis = sqlite3_column_int(stmt, 1);

			gtk_list_store_append(list_store, &iter);
			gtk_list_store_set(list_store, &iter, ID_NFE, id_nfe, 
					DH_EMIS, dh_emis, -1);
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
