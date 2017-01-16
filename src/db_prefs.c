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
#include <sqlite3.h>
#include <string.h>

char *get_ws_url(char *service, int ambiente, char **url_header, 
		char **url_body){
	sqlite3_stmt *stmt;
	char *err, *url, *s;
	err = malloc(sizeof(char) * 200);
	int rc;
	s = ambiente == 1? "url_prod" : "url_cert";

	char *sql = sqlite3_mprintf("SELECT %s, url_header, url_body FROM urls \
		WHERE service = %Q", s, service);
	if(db_select(sql, &err, &stmt)){
	fprintf(stdout, "SQL: %s\n", err);
		return NULL;
	}
	rc = sqlite3_step(stmt);
	if(rc == SQLITE_ROW){
		char *aux = NULL;
		aux = sqlite3_column_text(stmt, 0);
		url = aux == NULL? NULL:strdup(aux);
		aux = sqlite3_column_text(stmt, 1);
		*url_header = aux == NULL? NULL:strdup(aux);
		aux = sqlite3_column_text(stmt, 2);
		*url_body = aux == NULL? NULL:strdup(aux);
	}
	return url;
}
