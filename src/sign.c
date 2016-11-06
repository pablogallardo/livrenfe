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

#include "sign.h"
#include "crypto_interface.h"
#include <stdlib.h>
#include <string.h>
#include <libxml/tree.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#ifndef XMLSEC_NO_XSLT
#include <libxslt/xslt.h>
#include <libxslt/security.h>
#endif /* XMLSEC_NO_XSLT */
#include <xmlsec/xmlsec.h>
#include <xmlsec/openssl/evp.h>
#include <xmlsec/xmltree.h>
#include <xmlsec/xmldsig.h>
#include <xmlsec/crypto.h>

static xmlSecKeyPtr load_key(const char *pwd) {

	xmlSecKeyPtr key = NULL;
	xmlSecKeyDataPtr data;
	EVP_PKEY* pKey = NULL;
	int ret;

	pKey = get_private_key(pwd);

	data = xmlSecOpenSSLEvpKeyAdopt(pKey);
	if(data == NULL) {
		EVP_PKEY_free(pKey);
		return NULL;    
	}    

	key = xmlSecKeyCreate();
	if(key == NULL) {
		xmlSecKeyDataDestroy(data);
		return NULL;
	}

	ret = xmlSecKeySetValue(key, data);
	if(ret < 0) {
		xmlSecKeyDestroy(key);
		xmlSecKeyDataDestroy(data);
		return NULL;
	}
	return key;
}

int sign_file(const char* tmpl_file, char *password) {
    xmlDocPtr doc = NULL;
    xmlNodePtr node = NULL;
    xmlSecDSigCtxPtr dsigCtx = NULL;
    int res = -1;
    
    /* load template */
    doc = xmlParseFile(tmpl_file);
    if ((doc == NULL) || (xmlDocGetRootElement(doc) == NULL)){
	fprintf(stderr, "Error: unable to parse file \"%s\"\n", tmpl_file);
	goto done;	
    }
    
    /* find start node */
    node = xmlSecFindNode(xmlDocGetRootElement(doc), xmlSecNodeSignature, xmlSecDSigNs);
    if(node == NULL) {
	fprintf(stderr, "Error: start node not found in \"%s\"\n", tmpl_file);
	goto done;	
    }

    /* create signature context, we don't need keys manager in this example */
    dsigCtx = xmlSecDSigCtxCreate(NULL);
    if(dsigCtx == NULL) {
        fprintf(stderr,"Error: failed to create signature context\n");
	goto done;
    }

    /* load private key, assuming that there is not password */
    dsigCtx->signKey = load_key(password);
    if(dsigCtx->signKey == NULL) {
        fprintf(stderr,"Error: failed to load private pem key from smartcard\n");
	goto done;
    }

    /* set key name to the file name 
    if(xmlSecKeySetName(dsigCtx->signKey, key_file) < 0) {
    	fprintf(stderr,"Error: failed to set key name for key from \"%s\"\n", key_file);
	goto done;
    }*/

    /* sign the template */
    if(xmlSecDSigCtxSign(dsigCtx, node) < 0) {
        fprintf(stderr,"Error: signature failed\n");
	goto done;
    }
        
    /* print signed document to stdout */
    xmlDocDump(stdout, doc);
    
    /* success */
    res = 0;

done:    
    /* cleanup */
    if(dsigCtx != NULL) {
	xmlSecDSigCtxDestroy(dsigCtx);
    }
    
    if(doc != NULL) {
	xmlFreeDoc(doc); 
    }
    return(res);
}
