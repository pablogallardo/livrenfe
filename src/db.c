/* Copyright (c) 2016 Pablo G. Gallardo <pggllrd@gmail.com>
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

#include "db.h"
#include <libnfe/errno.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const char *db_file = NULL;
sqlite3 *db;

int db_exec(const char *sql, char **err){
	sqlite3_stmt *stmt;
	int rc;
	const char *tail_sql;
	char *e = (char*)malloc(sizeof(char) * 200);

	do{
		rc = sqlite3_prepare_v2(db, sql, -1, &stmt, &tail_sql);
		if(rc != SQLITE_OK){
			strcpy(e, sqlite3_errmsg(db));
			err = &e;
			return -ESQL;
		}
		rc = sqlite3_step(stmt);
		if(rc != SQLITE_DONE){
			if(rc == SQLITE_ERROR){
				strcpy(e, sqlite3_errmsg(db));
				err = &e;
				return -ESQL;
			}
			break;
		}
		rc = sqlite3_finalize(stmt);
		if(rc != SQLITE_OK){
			strcpy(e, sqlite3_errmsg(db));
			err = &e;
			return -ESQL;
		}
		sql = tail_sql;
	} while(tail_sql != NULL);
	return 0;
}

int db_select(const char *sql, char **err, sqlite3_stmt **stmt){
	int rc;

	rc = sqlite3_prepare_v2(db, sql, -1, stmt, NULL);
	if(rc != SQLITE_OK){
		strcpy(*err, sqlite3_errmsg(db));
		return -ESQL;
	}
	return 0;
}

int db_close(sqlite3_stmt *stmt, char **err){
	int rc;

	rc = sqlite3_finalize(stmt);
	if(rc != SQLITE_OK){
		strcpy(*err, sqlite3_errmsg(db));
		return -ESQL;
	}
	sqlite3_close_v2(db);
	return 0;
}

int db_last_insert_id(){
	return sqlite3_last_insert_rowid(db);
}
