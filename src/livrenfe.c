/* Copyright (c) 2016, 2017 Pablo G. Gallardo <pggllrd@gmail.com>
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

#include "livrenfe.h"
#include "lnfe_window.h"
#include "nfe_manager.h"
#include "db_interface.h"
#include <libnfe/errno.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <gtk/gtk.h>

static int init(char *);
static int init_db(char *);
static char *get_livrenfepath(void);
static int connect(void);

struct _Livrenfe{
	GtkApplication parent;
};

struct _LivrenfeClass{
	GtkApplicationClass parent_class;
};

G_DEFINE_TYPE(Livrenfe, livrenfe, GTK_TYPE_APPLICATION)

static void livrenfe_init(Livrenfe *app){
}

static void livrenfe_activate(GApplication *app){
	LivrenfeWindow *win;
	win = livrenfe_window_new(LIVRENFE_APP(app));
	gtk_window_present(GTK_WINDOW(win));
}

static void livrenfe_class_init(LivrenfeClass *class){
	G_APPLICATION_CLASS(class)->activate = livrenfe_activate;
}

Livrenfe *livrenfe_new(void){
	connect();
	return g_object_new(LIVRENFE_TYPE, "application-id", "br.com.lapagina.livrenfe",
			"flags", G_APPLICATION_HANDLES_OPEN, NULL);
}

int connect(){
	char *path = get_livrenfepath();
	if(access(path, F_OK) != -1){
		char *p = malloc(sizeof(char) * (strlen(path) + strlen(LIVRENFE_DB) + 2));
		strcpy(p, path);
		strcat(p, LIVRENFE_DB);
		if(access(p, F_OK) == -1){
			if(init_db(path)){
				fprintf(stderr, "livrenfe: couldn't create database %s\n", path);
				return -EFOL;
			}
		} else {
			path = p;
			set_db(path);
		}
	} else {
		if(init(path))
			return -EFOL;
	}
	return 0;
}

int init(char *path){
	if(mkdir(path, S_IRWXU)){
		fprintf(stderr, "livrenfe: couldn't create application folder %s\n", path);
		return -EFOL;
	}
	if(init_db(path))
		return -EFOP;
	return 0;
}

int init_db(char *path){
	strcat(path, LIVRENFE_DB);
	fprintf(stdout, "livrenfe: creating database...\n\t%s\n", path);
	FILE *fp = fopen(path, "wb");
	if(fp == NULL){
		fprintf(stderr, "livrenfe: couldn't create database %s\n", path);
		return -EFOP;
	}
	if(fclose(fp)){
		fprintf(stderr, "livrenfe: couldn't create database %s\n", path);
		return -EFOP;
	}
	set_db(path);
	if(create_db())
		return -EFOP;
	return 0;
}

char *get_livrenfepath(){
	char *home_path = getenv("HOME");
	char *livrenfe_path = malloc(sizeof(char) * (strlen(home_path) + strlen(LIVRENFE_FOLDER_NAME) + strlen(LIVRENFE_DB) + 3));
	strcpy(livrenfe_path, home_path);
	strcat(livrenfe_path, LIVRENFE_FOLDER_NAME);
	return livrenfe_path;
}
