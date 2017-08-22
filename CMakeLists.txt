cmake_minimum_required(VERSION 3.0)
project(livrenfe)

set(CMAKE_CXX_STANDARD 11)


find_package(LibXml2 REQUIRED)
if (LIBXML2_FOUND)
    include_directories(${LIBXML2_INCLUDE_DIR})
    set(LIBS ${LIBS} ${LIBXML2_LIBRARIES})
else ()
    message(FATAL_ERROR "LibXml2's libraries not found")
endif ()

find_package(OpenSSL REQUIRED)
if (OPENSSL_FOUND)
    include_directories(${OPENSSL_INCLUDE_DIR})
    set(LIBS ${LIBS} ${SSL_LIBRARIES})
else ()
    message(FATAL_ERROR "Não foi possível localizar biblioteca OpenSSL e seus arquivos de desenvolvimento")
endif ()

find_package(CURL REQUIRED)
if (CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIR})
    set(LIBS ${LIBS} ${CURL_LIBRARIES})
else ()
    message(FATAL_ERROR "Não foi possível localizar biblioteca cURL e seus arquivos de desenvolvimento")
endif ()

find_package(PkgConfig REQUIRED)
pkg_check_modules(GTK3 REQUIRED gtk+-3.0)
if (GTK3_FOUND)
    include_directories(${GTK3_INCLUDE_DIRS})
    link_directories($(GTK3_LIBRARY_DIRS))
    set(LIBS ${LIBS} ${GTK3_LIBRARIES})
else ()
    message(FATAL_ERROR "Não foi possível localizar biblioteca GTK3 e seus arquivos de desenvolvimento")
endif ()

add_definitions(-I/usr/include/libxml2)
add_definitions(-I/usr/local/include/xmlsec1)
add_definitions(-I/usr/local/include/openssl)
add_definitions(-DXMLSEC_OPENSSL_100=1)
add_definitions(-DXMLSEC_CRYPTO_OPENSSL=1)
add_definitions(-DXMLSEC_CRYPTO=openssl)

include_directories(.)
include_directories(include/libnfe)

set(SOURCE_FILES
        include/libnfe/errno.h
        include/libnfe/gen_xml.h
        include/libnfe/libnfe.h
        include/libnfe/nfe.h
        include/libnfe/sefaz.h
        src/libnfe/gen_xml.c
        src/libnfe/libnfe.c
        src/libnfe/nfe.c
        src/libnfe/sefaz.c
        src/libnfe/send.c
        src/libnfe/send.h
        src/libnfe/sign.c
        src/libnfe/sign.h
        src/libnfe/utils.c
        src/libnfe/utils.h
        src/libnfe/xml.c
        src/libnfe/xml.h
        src/crypto_interface.c
        src/crypto_interface.h
        src/db.c
        src/db.h
        src/db_create.c
        src/db_init.c
        src/db_interface.h
        src/db_nfe.c
        src/db_prefs.c
        src/emitente_manager.c
        src/emitente_manager.h
        src/gtk_common.c
        src/gtk_common.h
        src/item_manager.c
        src/item_manager.h
        src/livrenfe.c
        src/livrenfe.h
        src/lnfe_window.c
        src/lnfe_window.h
        src/main.c
        src/nfe_manager.c
        src/nfe_manager.h
        src/prefs.c
        src/prefs.h
        src/prefs_dialog.c
        src/prefs_dialog.h
        src/sefaz_response.c
        src/sefaz_response.h
        config.h)

add_executable(livrenfe ${SOURCE_FILES})

target_link_libraries(livrenfe ${GTK3_LIBRARIES} -lpthread ${LIBXML2_LIBRARIES} -lxmlsec1 -lp11 -lsqlite3)