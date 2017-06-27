/* Copyright (c) 2017 Pablo G. Gallardo <pggllrd@gmail.com>
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

#include "prefs.h"
#include "livrenfe.h"
#include <stdlib.h>
#include <string.h>

void free_urls(URLS *u){
	free(u->recepcaoevento);
	free(u->nfeconsultaprotocolo);
	free(u->nfeconsultacadastro);
	free(u->nfestatusservico);
	free(u->nfeautorizacao);
	free(u->nferetautorizacao);
	free(u->nfeinutilizacao);
	free(u);
}

void free_prefs(PREFS *p){
	free_urls(p->urls);
	if(strcmp(p->cert_file, ""))
		free(p->cert_file);
	if(strcmp(p->card_reader_lib, ""))
		free(p->card_reader_lib);
}

void empty_prefs_urls(PREFS_URLS *u){
	free(u->recepcaoevento_prod);
	free(u->recepcaoevento_cert);
	free(u->nfeconsultacadastro_prod);
	free(u->nfeconsultacadastro_cert);
	free(u->nfeinutilizacao_prod);
	free(u->nfeinutilizacao_cert);
	free(u->nfeconsultaprotocolo_prod);
	free(u->nfeconsultaprotocolo_cert);
	free(u->nfeautorizacao_prod);
	free(u->nfeautorizacao_cert);
	free(u->nferetautorizacao_prod);
	free(u->nferetautorizacao_cert);
	free(u->nfestatusservico_prod);
	free(u->nfestatusservico_cert);
}

void free_prefs_urls(PREFS_URLS *u){
	empty_prefs_urls(u);
	free(u);
}
