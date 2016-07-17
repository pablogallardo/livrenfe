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

#ifndef	DB_H
#define	DB_H

#include "livrenfe.h"
#include <sqlite3.h>

#define DB_VERSION	1

/**
 * Execute SQL commands without returning results
 * args:
 * 	1st: IN: sql statement
 * 	2nd: OUT: errmsg
 */
extern int db_exec(const char *, char **);

/**
 * Execute SQL commands returning results
 * args:
 * 	1st: IN: sql statement
 *	2nd: OUT: errmsg
 *	3rd: OUT: db object
 *	4st: OUT: stmt object
 */
extern int db_select(const char *, char **, sqlite3 *db, sqlite3_stmt *stmt);

/**
 * Close DB connection after db_select()
 * args:
 *	1st: IN: db object
 *	2nd: IN: stmt object
 *	3rd: OUT: errmsg
 */
extern int db_close(sqlite3 *, sqlite3_stmt *, char **);

#endif
