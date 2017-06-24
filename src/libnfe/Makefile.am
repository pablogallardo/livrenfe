lib_LTLIBRARIES = libnfe.la

libnfe_la_SOURCES = gen_xml.c nfe.c sefaz.c send.c \
		    sign.c utils.c xml.c libnfe.c

libnfe_la_LDFLAGS = -L/usr/local/lib -lssl -lcrypto -lp11 \
	`xml2-config --libs` `pkg-config --libs xmlsec1-openssl`\
       	`curl-config --libs`

 
libnfe_la_CFLAGS = -I../include -I$(top_srcdir)/include \
    	`xml2-config --cflags` `pkg-config --cflags xmlsec1-openssl`\
       	`curl-config --cflags`
