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
 * */

#ifndef	LIVRENFE_H
#define	LIVRENFE_H

#include <time.h>
#include <gtk/gtk.h>

#define	VERSION_NAME		"0.1.0"
#define	VERSION_COUNTER		1
#define VERSION_TITLE		"Tartagal"
#define LIVRENFE_COPYRIGHT	"© 2016, 2017 Pablo G. Gallardo"
#define	LIVRENFE_FOLDER_NAME	"/.livrenfe"
#define	LIVRENFE_DB		"/livrenfe.db"
#define LIVRENFE_COMMENT	"Emissor Código Aberto de Notas Fiscais Eletrónicas"
#define LIVRENFE_WEBSITE	"livrenfe.org"
#define LIVRENFE_TITLE		"LivreNFE"

#define CANCELAMENTO_TYPE	110111
#define	CARTA_CORRECAO_TYPE	110110

#define	AMBIENTE_PRODUCAO	1
#define	AMBIENTE_HOMOLOGACAO	2

#define	CERT_TYPE_A1		1
#define	CERT_TYPE_A3		2

#define	DEFAULT_AMBIENTE	AMBIENTE_HOMOLOGACAO
#define	DEFAULT_CERT_TYPE	CERT_TYPE_A1


/**
 * URLs for preferences window
 */
typedef struct {
	char *recepcaoevento_prod;
	char *recepcaoevento_cert;
	char *nfeconsultacadastro_prod;
	char *nfeconsultacadastro_cert;
	char *nfeinutilizacao_prod;
	char *nfeinutilizacao_cert;
	char *nfeconsultaprotocolo_prod;
	char *nfeconsultaprotocolo_cert;
	char *nfestatusservico_prod;
	char *nfestatusservico_cert;
	char *nferetautorizacao_prod;
	char *nferetautorizacao_cert;
	char *nfeautorizacao_prod;
	char *nfeautorizacao_cert;
} PREFS_URLS;

/**
 * Urls
 */
typedef struct {
	char *recepcaoevento;
	char *nfeconsultacadastro;
	char *nfeinutilizacao;
	char *nfeconsultaprotocolo;
	char *nfestatusservico;
	char *nferetautorizacao;
	char *nfeautorizacao;
} URLS;

/**
 * User preferences
 */
typedef struct {
	URLS *urls;
	char *cert_file;
	char *card_reader_lib;
	int ambiente;
	int cert_type;
} PREFS;


#define LIVRENFE_TYPE	(livrenfe_get_type())
#define	LIVRENFE_APP(obj)	(G_TYPE_CHECK_INSTANCE_CAST ((obj), LIVRENFE_TYPE, Livrenfe))

typedef struct _Livrenfe Livrenfe;
typedef struct _LivrenfeClass LivrenfeClass;

GType livrenfe_get_type(void);
Livrenfe *livrenfe_new(void);

#endif
