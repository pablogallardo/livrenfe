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
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <libxml/tree.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libxml/valid.h>
#ifndef XMLSEC_NO_XSLT
#include <libxslt/xslt.h>
#include <libxslt/security.h>
#endif /* XMLSEC_NO_XSLT */
#include <xmlsec/xmlsec.h>
#include <xmlsec/openssl/evp.h>
#include <xmlsec/xmltree.h>
#include <xmlsec/xmldsig.h>
#include <xmlsec/crypto.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>

xmlSecKeyPtr load_key(const char *pwd) {

	xmlSecKeyPtr key = NULL;
	xmlSecKeyDataPtr data;
	xmlSecKeyDataPtr dataX509;
	EVP_PKEY *pKey;
	X509 *x509 = NULL;
	int ret;

	get_private_key(&pKey, &x509, pwd);
	if(pKey == NULL || x509 == NULL)
		return NULL;

	data = xmlSecOpenSSLEvpKeyAdopt(pKey);
	if(data == NULL) {
		EVP_PKEY_free(pKey);
		return NULL;    
	}    
	dataX509 = xmlSecKeyDataCreate(xmlSecOpenSSLKeyDataX509Id);
	xmlSecOpenSSLKeyDataX509AdoptCert(dataX509, x509);

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
	xmlSecKeyAdoptData(key, dataX509);
	return key;
}

int sign_xml(xmlDocPtr doc, char *password, char *id) {
#ifndef XMLSEC_NO_XSLT
    xsltSecurityPrefsPtr xsltSecPrefs = NULL;
#endif /* XMLSEC_NO_XSLT */


    /* Init libxml and libxslt libraries */
    xmlInitParser();
    LIBXML_TEST_VERSION
    xmlLoadExtDtdDefaultValue = XML_DETECT_IDS | XML_COMPLETE_ATTRS;
    xmlSubstituteEntitiesDefault(1);
    xmlKeepBlanksDefault(0);
#ifndef XMLSEC_NO_XSLT
    xmlIndentTreeOutput = 0; 
#endif /* XMLSEC_NO_XSLT */

    /* Init libxslt */
#ifndef XMLSEC_NO_XSLT
    /* disable everything */
    xsltSecPrefs = xsltNewSecurityPrefs(); 
    xsltSetSecurityPrefs(xsltSecPrefs,  XSLT_SECPREF_READ_FILE,        xsltSecurityForbid);
    xsltSetSecurityPrefs(xsltSecPrefs,  XSLT_SECPREF_WRITE_FILE,       xsltSecurityForbid);
    xsltSetSecurityPrefs(xsltSecPrefs,  XSLT_SECPREF_CREATE_DIRECTORY, xsltSecurityForbid);
    xsltSetSecurityPrefs(xsltSecPrefs,  XSLT_SECPREF_READ_NETWORK,     xsltSecurityForbid);
    xsltSetSecurityPrefs(xsltSecPrefs,  XSLT_SECPREF_WRITE_NETWORK,    xsltSecurityForbid);
    xsltSetDefaultSecurityPrefs(xsltSecPrefs); 
#endif /* XMLSEC_NO_XSLT */
                
    /* Init xmlsec library */
    if(xmlSecInit() < 0) {
        fprintf(stderr, "Error: xmlsec initialization failed.\n");
        return(-1);
    }

    /* Check loaded library version */
    if(xmlSecCheckVersion() != 1) {
        fprintf(stderr, "Error: loaded xmlsec library version is not compatible.\n");
        return(-1);
    }

    /* Load default crypto engine if we are supporting dynamic
     * loading for xmlsec-crypto libraries. Use the crypto library
     * name ("openssl", "nss", etc.) to load corresponding 
     * xmlsec-crypto library.
     */
#ifdef XMLSEC_CRYPTO_DYNAMIC_LOADING
    if(xmlSecCryptoDLLoadLibrary(NULL) < 0) {
        fprintf(stderr, "Error: unable to load default xmlsec-crypto library. Make sure\n"
                        "that you have it installed and check shared libraries path\n"
                        "(LD_LIBRARY_PATH) envornment variable.\n");
        return(-1);     
    }
#endif /* XMLSEC_CRYPTO_DYNAMIC_LOADING */

    /* Init crypto library */
    if(xmlSecCryptoAppInit(NULL) < 0) {
        fprintf(stderr, "Error: crypto initialization failed.\n");
        return(-1);
    }

    /* Init xmlsec-crypto library */
    if(xmlSecCryptoInit() < 0) {
        fprintf(stderr, "Error: xmlsec-crypto initialization failed.\n");
        return(-1);
    }
    xmlNodePtr signNode = NULL;
    xmlNodePtr refNode = NULL;
    xmlNodePtr keyInfoNode = NULL;
    xmlNodePtr x509DataNode = NULL;
    xmlSecDSigCtxPtr dsigCtx = NULL;
    int res = -1;
    
    /* create signature template for RSA-SHA1 enveloped signature */
    signNode = xmlSecTmplSignatureCreate(doc, xmlSecTransformInclC14NId,
                                         xmlSecTransformRsaSha1Id, NULL);
    if(signNode == NULL) {
        fprintf(stderr, "Error: failed to create signature template\n");
        goto done;              
    }

    /* add <dsig:Signature/> node to the doc */
    xmlAddChild(xmlDocGetRootElement(doc), signNode);
    xmlAttrPtr attr = xmlHasProp(doc->children->children, "Id");
    if(attr){
	xmlAddID(NULL, doc, str_replace("#", "" ,id), attr);
    }
    
    /* add reference */
    refNode = xmlSecTmplSignatureAddReference(signNode, xmlSecTransformSha1Id,
                                        NULL, id, NULL);
    if(refNode == NULL) {
        fprintf(stderr, "Error: failed to add reference to signature template\n");
        goto done;              
    }

    /* add enveloped transform */
    if(xmlSecTmplReferenceAddTransform(refNode, xmlSecTransformEnvelopedId) == NULL) {
        fprintf(stderr, "Error: failed to add enveloped transform to reference\n");
        goto done;              
    }
    if(xmlSecTmplReferenceAddTransform(refNode, xmlSecTransformInclC14NId) == NULL) {
        fprintf(stderr, "Error: failed to add enveloped transform to reference\n");
        goto done;              
    }
    
    /* add <dsig:KeyInfo/> and <dsig:X509Data/> */
    keyInfoNode = xmlSecTmplSignatureEnsureKeyInfo(signNode, NULL);
    if(keyInfoNode == NULL) {
        fprintf(stderr, "Error: failed to add key info\n");
        goto done;              
    }
    
    x509DataNode = xmlSecTmplKeyInfoAddX509Data(keyInfoNode);
    if(x509DataNode == NULL) {
        fprintf(stderr, "Error: failed to add X509Data node\n");
        goto done;              
    }

    if(xmlSecTmplX509DataAddCertificate(x509DataNode) == NULL) {
        fprintf(stderr, "Error: failed to add X509Certificate node\n");
        goto done;
    }

    /* create signature context */
    dsigCtx = xmlSecDSigCtxCreate(NULL);
    if(dsigCtx == NULL) {
        fprintf(stderr,"Error: failed to create signature context\n");
        goto done;
    }

    /* load private key */
    dsigCtx->signKey = load_key(password);
    if(dsigCtx->signKey == NULL) {
        fprintf(stderr,"Error: failed to load private key from smartcard\n");
	goto done;
    }
    
    /* sign the template */
    if(xmlSecDSigCtxSign(dsigCtx, signNode) < 0) {
        fprintf(stderr,"Error: signature failed\n");
        goto done;
    }
        
    /* success */
    res = 0;

done:    
    /* cleanup */
    if(dsigCtx != NULL) {
	xmlSecDSigCtxDestroy(dsigCtx);
    }
    
    return(res);
}
