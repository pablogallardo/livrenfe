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

#include "db_interface.h"
#include "db.h"
#include <stdio.h>

const char *create_sql = "CREATE TABLE paises (id_pais integer, nome varchar(60), CONSTRAINT pais_pk PRIMARY KEY (id_pais)); \
CREATE TABLE uf (id_uf varchar(2), nome varchar (60), cod_ibge integer, \
       	CONSTRAINT uf_pk PRIMARY KEY (id_uf));\
CREATE TABLE municipios (id_municipio varchar(7), id_uf varchar(2), nome varchar(60),\
	id_pais integer,\
	CONSTRAINT municipio_pk PRIMARY KEY (id_municipio),\
	CONSTRAINT municipio_uf_fk FOREIGN KEY (id_uf)\
	REFERENCES uf(id_uf),\
	CONSTRAINT municipio_pais_uf FOREIGN KEY (id_pais)\
	REFERENCES paises(id_pais));\
CREATE TABLE enderecos (id_endereco integer, rua varchar(200), \
	complemento varchar(200), bairro varchar(200), id_municipio varchar(7),\
       	cep varchar(9),\
	CONSTRAINT endereco_pk PRIMARY KEY (id_endereco),\
	CONSTRAINT endereco_municipio_fk FOREIGN KEY (id_municipio)\
	REFERENCES municipios(id_municipio));\
CREATE TABLE emitentes (id_emitente varchar(20), nome varchar(100),\
	inscricao_estadual varchar(20), id_endereco integer,\
	crt char(1),\
	CONSTRAINT emitente_pk PRIMARY KEY (id_emitente),\
	CONSTRAINT emitente_endereco_fk FOREIGN KEY (id_endereco)\
	REFERENCES enderecos(id_endereco));\
CREATE TABLE destinatarios (id_destinatario varchar(20), nome varchar(200),\
	id_endereco integer, tipo_ie char(1),\
	CONSTRAINT destinatario_pk PRIMARY KEY (id_destinatario),\
	CONSTRAINT destinatario_endereco_fk FOREIGN KEY (id_endereco)\
	REFERENCES enderecos(id_endereco));\
CREATE TABLE produtos (id_produto integer, descricao varchar(200), ncm integer,\
	cfop integer, unidade varchar(10), quantidade real, valor real,\
	CONSTRAINT produto_pk PRIMARY KEY (id_produto));\
CREATE TABLE transportadoras (id_transportadora varchar(20), modfrete char(1), \
	nome varchar(200),\
	CONSTRAINT transportadora_pk PRIMARY KEY (id_transportadora));\
CREATE TABLE nfe (id_nfe integer, id_municipio varchar(8),\
	nat_op varchar(20), ind_pag integer, mod_nfe char(2),\
	serie varchar(10), num_nf integer, dh_emis integer, dh_saida integer, tipo char(1),\
	local_destino char(1), tipo_impressao char(1), tipo_emissao char(1),\
	tipo_ambiente char(1), finalidade char(1), consumidor_final char(1),\
	presencial char(1), versao varchar(10), div char(1), chave varchar(20),\
	id_emitente varchar(20), id_destinatario varchar(20), q_itens integer,\
	total real, id_transportadora varchar(20),\
	CONSTRAINT nfe_pk PRIMARY KEY (id_nfe),\
	CONSTRAINT nfe_municipio_fk FOREIGN KEY (id_municipio)\
	REFERENCES municipios(id_municipio),\
	CONSTRAINT nfe_emitente_fk FOREIGN KEY (id_emitente)\
	REFERENCES emitentes(id_emitente),\
	CONSTRAINT nfe_destinatario_fk FOREIGN KEY (id_destinatario)\
	REFERENCES destinatarios(id_destinatario),\
	CONSTRAINT nfe_transportadora_fk FOREIGN KEY (id_transportadora)\
	REFERENCES transportadoras(id_transportadora));\
CREATE TABLE protocolos (id_protocolo integer, numero varchar(20), \
	dh_recib integer, id_nfe integer,\
	CONSTRAINT protocolo_pk PRIMARY KEY (id_protocolo),\
	CONSTRAINT protocolo_nfe_fk FOREIGN KEY (id_nfe)\
	REFERENCES nfe(id_nfe));\
CREATE TABLE nfe_itens (id_nfe integer, ordem integer, id_produto integer, icms_origem char(1),\
	icms_tipo integer, icms_aliquota real, icms_valor real, pis_aliquota real,\
	pis_quantidade integer, pis_nt varchar(40), cofins_aliquota real,\
	cofins_quantidade integer, cofins_nt varchar(40),\
	CONSTRAINT itens_pk PRIMARY KEY (id_nfe, ordem),\
	CONSTRAINT itens_nfe_fk FOREIGN KEY (id_nfe)\
	REFERENCES nfe(id_nfe),\
	CONSTRAINT itens_produto_fk FOREIGN KEY (id_produto)\
	REFERENCES produtos(id_produto));\
";

int create_db(){
}
