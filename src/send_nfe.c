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

#include "send_nfe.h"
#include "livrenfe.h"
#include "errno.h"
#include "gen_xml.h"
#include "crypto_interface.h"
#include <curl/curl.h>
#include <openssl/evp.h>
#include <openssl/ssl.h>
#include <openssl/crypto.h>
#include <openssl/pem.h>
#include <stdio.h>
#include <openssl/x509.h>
#include <openssl/rsa.h>
#include <libxml/xmlwriter.h>

static size_t writefunction(void *ptr, size_t size,
		size_t nmemb, void *stream){
	fwrite(ptr, size, nmemb, stream);
	return (nmemb*size);
}

CURLcode sslctx_function(CURL *curl, void *sslctx, void *parm){
	X509 *cert = NULL;
	RSA *rsa = NULL;
	EVP_PKEY *pKey = NULL;
	int rc;
	char *pwd = "";
	(void)curl; //avoid warnings
	(void)parm; //avoid warnings

	get_private_key(&pKey, &cert, pwd);
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

static char *format_soap(NFE *nfe){
	int rc, buffersize;
	xmlTextWriterPtr writer;
	xmlDocPtr doc;
	xmlChar *xmlbuf;

	writer = xmlNewTextWriterDoc(&doc, 0);
	if (writer == NULL)
		return NULL;
	xmlTextWriterStartDocument(writer, NULL, "UTF-8", NULL);
	rc = xmlTextWriterStartElement(writer, BAD_CAST "soap12:Envelope");
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:xsi",
			BAD_CAST "http://www.w3.org/2001/XMLSchema-instance");
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:xsd",
			BAD_CAST "http://www.w3.org/2001/XMLSchema");
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns:soap12",
			BAD_CAST "http://www.w3.org/2003/05/soap-envelope");
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterStartElement(writer, BAD_CAST "soap12:Header");
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterStartElement(writer, BAD_CAST "nfeCabecMsg");
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns",
			BAD_CAST "http://www.portalfiscal.inf.br/sce/wsdl/NfeAutorizacao");
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "versaoDados",
			"%s", NFE_VERSAO);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteFormatElement(writer, BAD_CAST "cUF",
			"%d", nfe->idnfe->municipio->cod_uf);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterStartElement(writer, BAD_CAST "soap12:Body");
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterStartElement(writer, BAD_CAST "nfeDadosMsg");
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteAttribute(writer, BAD_CAST "xmlns",
			BAD_CAST "http://www.portalfiscal.inf.br/nfe/wsdl/NfeAutorizacao");
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterWriteRaw(writer, BAD_CAST generate_xml(nfe));
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return -EXML;
	rc = xmlTextWriterEndElement(writer);
	if (rc < 0)
		return -EXML;

	if (rc < 0)
		return NULL;
	xmlTextWriterEndDocument(writer);
	xmlDocDumpMemory(doc, &xmlbuf, &buffersize);
	char *xml = str_replace(">\n<","><", xmlbuf);
	return xml;
}

int send_nfe(NFE *nfe){
	CURL *ch;
	CURLcode rv;
	rv = curl_global_init(CURL_GLOBAL_ALL);
	ch = curl_easy_init();
	struct curl_slist *header = NULL;
	header = curl_slist_append(header, "Content-type: application/soap+xml; charset=UTF-8");
	//header = curl_slist_append(header, "charset=utf-8");
	rv = curl_easy_setopt(ch, CURLOPT_VERBOSE, 1L);
	rv = curl_easy_setopt(ch, CURLOPT_HTTPHEADER, header);
	rv = curl_easy_setopt(ch, CURLOPT_NOPROGRESS, 1L);
	rv = curl_easy_setopt(ch, CURLOPT_NOSIGNAL, 1L);
	rv = curl_easy_setopt(ch, CURLOPT_WRITEFUNCTION, writefunction);
	rv = curl_easy_setopt(ch, CURLOPT_WRITEDATA, stdout);
	//rv = curl_easy_setopt(ch, CURLOPT_HEADERFUNCTION, writefunction);
	rv = curl_easy_setopt(ch, CURLOPT_HEADERDATA, stderr);
	char *test = format_soap(nfe);
	fprintf(stdout, test);
	rv = curl_easy_setopt(ch, CURLOPT_POSTFIELDS, test);

	/* both VERIFYPEER and VERIFYHOST are set to 0 in this case because there is
	   no CA certificate*/ 

	rv = curl_easy_setopt(ch, CURLOPT_SSL_VERIFYPEER, 0L);
	rv = curl_easy_setopt(ch, CURLOPT_SSL_VERIFYHOST, 0L);
	rv = curl_easy_setopt(ch, CURLOPT_URL, 
		"https://homologacao.nfe.fazenda.sp.gov.br/ws/nfeautorizacao.asmx");
	rv = curl_easy_setopt(ch, CURLOPT_SSL_CTX_FUNCTION, *sslctx_function);
	rv = curl_easy_perform(ch);
	if(rv==CURLE_OK) {
		printf("*** transfer succeeded ***\n");
	}
	else {
		printf("*** transfer failed ***\n");
	}

	curl_easy_cleanup(ch);
	curl_global_cleanup();
	return rv;
}
