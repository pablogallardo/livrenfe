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

#include "db.h"
#include "errno.h"
#include <sqlite3.h>
#include <stdio.h>
#include <string.h>

const char *db_file = NULL;

int db_exec(const char *sql, char **err){
	sqlite3 *db;
	sqlite3_stmt *stmt;
	int rc;
	const char *tail_sql;

	rc = sqlite3_open(db_file, &db);
	if(rc)
		return -ESQL;

	do{
		rc = sqlite3_prepare_v2(db, sql, -1, &stmt, &tail_sql);
		if(rc != SQLITE_OK){
			strcpy(*err, sqlite3_errmsg(db));
			return -ESQL;
		}
		rc = sqlite3_step(stmt);
		if(rc != SQLITE_DONE){
			if(rc == SQLITE_ERROR){
				strcpy(*err, sqlite3_errmsg(db));
				return -ESQL;
			}
			break;
		}
		rc = sqlite3_finalize(stmt);
		if(rc != SQLITE_OK){
			strcpy(*err, sqlite3_errmsg(db));
			return -ESQL;
		}
		sql = tail_sql;
	} while(tail_sql != NULL);

	sqlite3_close(db);
	return 0;
}

int db_select(const char *sql, char **err, sqlite3 **db, sqlite3_stmt **stmt){
	int rc;

	rc = sqlite3_open(db_file, db);
	if(rc)
		return -ESQL;
	rc = sqlite3_prepare_v2(*db, sql, -1, stmt, NULL);
	if(rc != SQLITE_OK){
		strcpy(*err, sqlite3_errmsg(*db));
		return -ESQL;
	}
	return 0;
}

int db_close(sqlite3 *db, sqlite3_stmt *stmt, char **err){
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
	sqlite3 *db;
	int rc;
	rc = sqlite3_open(db_file, &db);
	if(rc)
		return -ESQL;
	rc = sqlite3_last_insert_rowid(db);
	sqlite3_close_v2(db);
	return rc;
}
