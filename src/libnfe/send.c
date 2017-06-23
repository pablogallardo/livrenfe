/* Copyright (c) 2016, 2017 Pablo G. Gallardo <pggllrd@gmail.com>
 *
 * This file is part of LivreNFE.
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

#include "send.h"
#include <libnfe/libnfe.h>
#include <libnfe/errno.h>
#include <libnfe/gen_xml.h>
#include <curl/curl.h>
#include <openssl/evp.h>
#include <openssl/ssl.h>
#include <openssl/crypto.h>
#include <openssl/pem.h>
#include <stdio.h>
#include <openssl/x509.h>
#include <openssl/rsa.h>
#include <libxml/xmlwriter.h>
#include <string.h>

static size_t writefunction(void *ptr, size_t size,
		size_t nmemb, void *s){
	char **r = (char**) s;
	*r = strndup(ptr, (size_t)(size*nmemb));
	return (nmemb*size);
}

CURLcode sslctx_function(CURL *curl, void *sslctx, char *pwd){
	X509 *cert = NULL;
	RSA *rsa = NULL;
	EVP_PKEY *pKey = NULL;
	int rc;
	(void)curl; //avoid warnings

	get_private_key(&pKey, &cert, pwd);
	if(pKey == NULL){
		return -1;
	}
	rsa = EVP_PKEY_get1_RSA(pKey);

	// tell SSL to use the X509 certificate 
	rc = SSL_CTX_use_certificate((SSL_CTX*)sslctx, cert);
	if(rc != 1) {
		fprintf(stderr, "Use certificate failed\n");
	}
	//tell SSL to use the RSA key from memory 
	rc = SSL_CTX_use_RSAPrivateKey((SSL_CTX*)sslctx, rsa);
	if(rc != 1) {
		fprintf(stderr, "Use Key failed\n");
	}
	return CURLE_OK;
}

static char *format_soap(sefaz_servico_t service, char *xml, int cuf, 
		const char *wsdl){
	int rc, buffersize;
	xmlTextWriterPtr writer;
	xmlDocPtr doc;
	xmlChar *xmlbuf;
	char *versao = get_versao(service);

	writer = xmlNewTextWriterDoc(&doc, 0);
	if (writer == NULL)
		return NULL;
	xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);
	rc = xmlTextWriterStartElement(writer, BAD_CAST "soap12:Envelope");
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:xsi",
			BAD_CAST "http://www.w3.org/2001/XMLSchema-instance");
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:xsd",
			BAD_CAST "http://www.w3.org/2001/XMLSchema");
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:soap12",
			BAD_CAST "http://www.w3.org/2003/05/soap-envelope");
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterStartElement(writer, BAD_CAST "soap12:Header");
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterStartElement(writer, BAD_CAST "nfeCabecMsg");
	if (rc < 0)
		return NULL;

	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns",
		BAD_CAST wsdl);
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "versaoDados",
			"%s", versao);
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "cUF",
			"%d", cuf);
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterStartElement(writer, BAD_CAST "soap12:Body");
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterStartElement(writer, BAD_CAST "nfeDadosMsg");
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns",
			BAD_CAST wsdl);
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterWriteRaw(writer, BAD_CAST xml);
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return NULL;
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return NULL;

	if (rc < 0)
		return NULL;
	xmlTextWriterEndDocument(writer);
	xmlDocDumpMemory(doc, &xmlbuf, &buffersize);

	return (char*) xmlbuf;
}

char *send_sefaz(sefaz_servico_t service, char *URL, int ambiente, int cuf, 
		char *xml, char *password){
	CURL *ch;
	CURLcode rv;
	rv = curl_global_init(CURL_GLOBAL_ALL);
	ch = curl_easy_init();
	struct curl_slist *header = NULL;
	char *server_response;
	header = curl_slist_append(header, 
		"Content-type: application/soap+xml; charset=UTF-8");
	rv = curl_easy_setopt(ch, CURLOPT_VERBOSE, 0L);
	rv = curl_easy_setopt(ch, CURLOPT_HTTPHEADER, header);
	rv = curl_easy_setopt(ch, CURLOPT_NOPROGRESS, 1L);
	rv = curl_easy_setopt(ch, CURLOPT_NOSIGNAL, 1L);
	rv = curl_easy_setopt(ch, CURLOPT_WRITEFUNCTION, writefunction);
	rv = curl_easy_setopt(ch, CURLOPT_WRITEDATA, &server_response);

	char *h = format_soap(service, xml, cuf, SEFAZ_WSDL[service]);

	rv = curl_easy_setopt(ch, CURLOPT_POSTFIELDS, h);

	/* both VERIFYPEER and VERIFYHOST are set to 0 in this case because there is
	   no CA certificate*/ 

	rv = curl_easy_setopt(ch, CURLOPT_SSL_VERIFYPEER, 0L);
	rv = curl_easy_setopt(ch, CURLOPT_SSL_VERIFYHOST, 0L);
	rv = curl_easy_setopt(ch, CURLOPT_URL, URL);
	rv = curl_easy_setopt(ch, CURLOPT_SSL_CTX_FUNCTION, *sslctx_function);
	rv = curl_easy_setopt(ch, CURLOPT_SSL_CTX_DATA, password);
	rv = curl_easy_perform(ch);

	free(h);
	if(rv!=CURLE_OK) {
		return NULL;
	}

	curl_easy_cleanup(ch);
	curl_global_cleanup();
	return server_response;
}
