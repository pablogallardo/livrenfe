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
#include "errno.h"
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
CREATE TABLE emitentes (id_emitente integer, nome varchar(100),\
	inscricao_estadual varchar(20), \
	crt char(1), cnpj varchar(14), rua varchar(200), complemento varchar(200),\
	bairro varchar(200), id_municipio varchar(7), cep varchar(9),\
	CONSTRAINT emitente_pk PRIMARY KEY (id_emitente),\
	CONSTRAINT emitente_munic_fk FOREIGN KEY (id_municipio)\
		REFERENCES municipios(id_municipio));\
CREATE TABLE destinatarios (id_destinatario integer, nome varchar(200),\
	tipo_ie char(1), cnpj varchar(14), rua varchar(200),\
	complemento varchar(200), bairro varchar(200), id_municipio varchar(7),\
	cep varchar(9),\
	CONSTRAINT destinatario_pk PRIMARY KEY (id_destinatario),\
	CONSTRAINT destinatario_munic_fk FOREIGN KEY (id_municipio)\
		REFERENCES municipios(id_municipio));\
CREATE TABLE produtos (id_produto integer, descricao varchar(200), ncm integer,\
	cfop integer, unidade varchar(10), quantidade real, valor real,\
	CONSTRAINT produto_pk PRIMARY KEY (id_produto));\
CREATE TABLE transportadoras (id_transportadora varchar(20), modfrete char(1), \
	nome varchar(200),\
	CONSTRAINT transportadora_pk PRIMARY KEY (id_transportadora));\
CREATE TABLE nfe (id_nfe integer, id_municipio varchar(8),\
	nat_op varchar(20), ind_pag integer, mod_nfe char(2),\
	serie varchar(10), num_nf integer, dh_emis datetime, dh_saida datetime, tipo char(1),\
	local_destino char(1), tipo_impressao char(1), tipo_emissao char(1),\
	tipo_ambiente char(1), finalidade char(1), consumidor_final char(1),\
	presencial char(1), versao varchar(10), div char(1), chave varchar(20),\
	id_emitente integer, id_destinatario integer, q_itens integer,\
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
	dh_recib datetime, id_nfe integer,\
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
	REFERENCES produtos(id_produto));";

const char *insert_sql = "INSERT INTO paises (id_pais, nome) VALUES (1, 'Brasil');\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100015', 'RO', 'Alta Floresta D''Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100379', 'RO', 'Alto Alegre dos Parecis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100403', 'RO', 'Alto Paraíso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100346', 'RO', 'Alvorada D''Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100023', 'RO', 'Ariquemes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100452', 'RO', 'Buritis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100031', 'RO', 'Cabixi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100601', 'RO', 'Cacaulândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100049', 'RO', 'Cacoal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100700', 'RO', 'Campo Novo de Rondônia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100809', 'RO', 'Candeias do Jamari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100908', 'RO', 'Castanheiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100056', 'RO', 'Cerejeiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100924', 'RO', 'Chupinguaia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100064', 'RO', 'Colorado do Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100072', 'RO', 'Corumbiara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100080', 'RO', 'Costa Marques', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100940', 'RO', 'Cujubim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100098', 'RO', 'Espigão D''Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1101005', 'RO', 'Governador Jorge Teixeira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100106', 'RO', 'Guajará-Mirim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1101104', 'RO', 'Itapuã do Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100114', 'RO', 'Jaru', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100122', 'RO', 'Ji-Paraná', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100130', 'RO', 'Machadinho D''Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1101203', 'RO', 'Ministro Andreazza', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1101302', 'RO', 'Mirante da Serra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1101401', 'RO', 'Monte Negro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100148', 'RO', 'Nova Brasilândia D''Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100338', 'RO', 'Nova Mamoré', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1101435', 'RO', 'Nova União', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100502', 'RO', 'Novo Horizonte do Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100155', 'RO', 'Ouro Preto do Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1101450', 'RO', 'Parecis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100189', 'RO', 'Pimenta Bueno', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1101468', 'RO', 'Pimenteiras do Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100205', 'RO', 'Porto Velho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100254', 'RO', 'Presidente Médici', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1101476', 'RO', 'Primavera de Rondônia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100262', 'RO', 'Rio Crespo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100288', 'RO', 'Rolim de Moura', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100296', 'RO', 'Santa Luzia D''Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1101484', 'RO', 'São Felipe D''Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1101492', 'RO', 'São Francisco do Guaporé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100320', 'RO', 'São Miguel do Guaporé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1101500', 'RO', 'Seringueiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1101559', 'RO', 'Teixeirópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1101609', 'RO', 'Theobroma', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1101708', 'RO', 'Urupá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1101757', 'RO', 'Vale do Anari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1101807', 'RO', 'Vale do Paraíso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1100304', 'RO', 'Vilhena', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1200013', 'AC', 'Acrelândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1200054', 'AC', 'Assis Brasil', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1200104', 'AC', 'Brasiléia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1200138', 'AC', 'Bujari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1200179', 'AC', 'Capixaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1200203', 'AC', 'Cruzeiro do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1200252', 'AC', 'Epitaciolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1200302', 'AC', 'Feijó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1200328', 'AC', 'Jordão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1200336', 'AC', 'Mâncio Lima', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1200344', 'AC', 'Manoel Urbano', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1200351', 'AC', 'Marechal Thaumaturgo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1200385', 'AC', 'Plácido de Castro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1200807', 'AC', 'Porto Acre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1200393', 'AC', 'Porto Walter', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1200401', 'AC', 'Rio Branco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1200427', 'AC', 'Rodrigues Alves', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1200435', 'AC', 'Santa Rosa do Purus', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1200500', 'AC', 'Sena Madureira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1200450', 'AC', 'Senador Guiomard', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1200609', 'AC', 'Tarauacá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1200708', 'AC', 'Xapuri', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1300029', 'AM', 'Alvarães', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1300060', 'AM', 'Amaturá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1300086', 'AM', 'Anamã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1300102', 'AM', 'Anori', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1300144', 'AM', 'Apuí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1300201', 'AM', 'Atalaia do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1300300', 'AM', 'Autazes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1300409', 'AM', 'Barcelos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1300508', 'AM', 'Barreirinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1300607', 'AM', 'Benjamin Constant', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1300631', 'AM', 'Beruri', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1300680', 'AM', 'Boa Vista do Ramos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1300706', 'AM', 'Boca do Acre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1300805', 'AM', 'Borba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1300839', 'AM', 'Caapiranga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1300904', 'AM', 'Canutama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1301001', 'AM', 'Carauari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1301100', 'AM', 'Careiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1301159', 'AM', 'Careiro da Várzea', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1301209', 'AM', 'Coari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1301308', 'AM', 'Codajás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1301407', 'AM', 'Eirunepé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1301506', 'AM', 'Envira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1301605', 'AM', 'Fonte Boa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1301654', 'AM', 'Guajará', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1301704', 'AM', 'Humaitá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1301803', 'AM', 'Ipixuna', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1301852', 'AM', 'Iranduba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1301902', 'AM', 'Itacoatiara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1301951', 'AM', 'Itamarati', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1302009', 'AM', 'Itapiranga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1302108', 'AM', 'Japurá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1302207', 'AM', 'Juruá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1302306', 'AM', 'Jutaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1302405', 'AM', 'Lábrea', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1302504', 'AM', 'Manacapuru', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1302553', 'AM', 'Manaquiri', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1302603', 'AM', 'Manaus', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1302702', 'AM', 'Manicoré', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1302801', 'AM', 'Maraã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1302900', 'AM', 'Maués', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1303007', 'AM', 'Nhamundá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1303106', 'AM', 'Nova Olinda do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1303205', 'AM', 'Novo Airão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1303304', 'AM', 'Novo Aripuanã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1303403', 'AM', 'Parintins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1303502', 'AM', 'Pauini', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1303536', 'AM', 'Presidente Figueiredo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1303569', 'AM', 'Rio Preto da Eva', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1303601', 'AM', 'Santa Isabel do Rio Negro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1303700', 'AM', 'Santo Antônio do Içá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1303809', 'AM', 'São Gabriel da Cachoeira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1303908', 'AM', 'São Paulo de Olivença', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1303957', 'AM', 'São Sebastião do Uatumã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1304005', 'AM', 'Silves', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1304062', 'AM', 'Tabatinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1304104', 'AM', 'Tapauá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1304203', 'AM', 'Tefé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1304237', 'AM', 'Tonantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1304260', 'AM', 'Uarini', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1304302', 'AM', 'Urucará', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1304401', 'AM', 'Urucurituba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1400050', 'RR', 'Alto Alegre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1400027', 'RR', 'Amajari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1400100', 'RR', 'Boa Vista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1400159', 'RR', 'Bonfim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1400175', 'RR', 'Cantá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1400209', 'RR', 'Caracaraí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1400233', 'RR', 'Caroebe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1400282', 'RR', 'Iracema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1400308', 'RR', 'Mucajaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1400407', 'RR', 'Normandia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1400456', 'RR', 'Pacaraima', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1400472', 'RR', 'Rorainópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1400506', 'RR', 'São João da Baliza', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1400605', 'RR', 'São Luiz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1400704', 'RR', 'Uiramutã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1500107', 'PA', 'Abaetetuba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1500131', 'PA', 'Abel Figueiredo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1500206', 'PA', 'Acará', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1500305', 'PA', 'Afuá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1500347', 'PA', 'Água Azul do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1500404', 'PA', 'Alenquer', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1500503', 'PA', 'Almeirim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1500602', 'PA', 'Altamira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1500701', 'PA', 'Anajás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1500800', 'PA', 'Ananindeua', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1500859', 'PA', 'Anapu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1500909', 'PA', 'Augusto Corrêa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1500958', 'PA', 'Aurora do Pará', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1501006', 'PA', 'Aveiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1501105', 'PA', 'Bagre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1501204', 'PA', 'Baião', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1501253', 'PA', 'Bannach', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1501303', 'PA', 'Barcarena', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1501402', 'PA', 'Belém', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1501451', 'PA', 'Belterra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1501501', 'PA', 'Benevides', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1501576', 'PA', 'Bom Jesus do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1501600', 'PA', 'Bonito', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1501709', 'PA', 'Bragança', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1501725', 'PA', 'Brasil Novo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1501758', 'PA', 'Brejo Grande do Araguaia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1501782', 'PA', 'Breu Branco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1501808', 'PA', 'Breves', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1501907', 'PA', 'Bujaru', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1502004', 'PA', 'Cachoeira do Arari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1501956', 'PA', 'Cachoeira do Piriá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1502103', 'PA', 'Cametá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1502152', 'PA', 'Canaã dos Carajás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1502202', 'PA', 'Capanema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1502301', 'PA', 'Capitão Poço', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1502400', 'PA', 'Castanhal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1502509', 'PA', 'Chaves', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1502608', 'PA', 'Colares', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1502707', 'PA', 'Conceição do Araguaia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1502756', 'PA', 'Concórdia do Pará', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1502764', 'PA', 'Cumaru do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1502772', 'PA', 'Curionópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1502806', 'PA', 'Curralinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1502855', 'PA', 'Curuá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1502905', 'PA', 'Curuçá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1502939', 'PA', 'Dom Eliseu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1502954', 'PA', 'Eldorado dos Carajás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1503002', 'PA', 'Faro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1503044', 'PA', 'Floresta do Araguaia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1503077', 'PA', 'Garrafão do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1503093', 'PA', 'Goianésia do Pará', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1503101', 'PA', 'Gurupá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1503200', 'PA', 'Igarapé-Açu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1503309', 'PA', 'Igarapé-Miri', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1503408', 'PA', 'Inhangapi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1503457', 'PA', 'Ipixuna do Pará', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1503507', 'PA', 'Irituia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1503606', 'PA', 'Itaituba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1503705', 'PA', 'Itupiranga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1503754', 'PA', 'Jacareacanga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1503804', 'PA', 'Jacundá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1503903', 'PA', 'Juruti', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1504000', 'PA', 'Limoeiro do Ajuru', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1504059', 'PA', 'Mãe do Rio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1504109', 'PA', 'Magalhães Barata', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1504208', 'PA', 'Marabá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1504307', 'PA', 'Maracanã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1504406', 'PA', 'Marapanim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1504422', 'PA', 'Marituba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1504455', 'PA', 'Medicilândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1504505', 'PA', 'Melgaço', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1504604', 'PA', 'Mocajuba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1504703', 'PA', 'Moju', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1504802', 'PA', 'Monte Alegre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1504901', 'PA', 'Muaná', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1504950', 'PA', 'Nova Esperança do Piriá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1504976', 'PA', 'Nova Ipixuna', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1505007', 'PA', 'Nova Timboteua', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1505031', 'PA', 'Novo Progresso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1505064', 'PA', 'Novo Repartimento', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1505106', 'PA', 'Óbidos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1505205', 'PA', 'Oeiras do Pará', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1505304', 'PA', 'Oriximiná', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1505403', 'PA', 'Ourém', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1505437', 'PA', 'Ourilândia do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1505486', 'PA', 'Pacajá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1505494', 'PA', 'Palestina do Pará', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1505502', 'PA', 'Paragominas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1505536', 'PA', 'Parauapebas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1505551', 'PA', 'Pau D''Arco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1505601', 'PA', 'Peixe-Boi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1505635', 'PA', 'Piçarra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1505650', 'PA', 'Placas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1505700', 'PA', 'Ponta de Pedras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1505809', 'PA', 'Portel', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1505908', 'PA', 'Porto de Moz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1506005', 'PA', 'Prainha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1506104', 'PA', 'Primavera', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1506112', 'PA', 'Quatipuru', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1506138', 'PA', 'Redenção', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1506161', 'PA', 'Rio Maria', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1506187', 'PA', 'Rondon do Pará', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1506195', 'PA', 'Rurópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1506203', 'PA', 'Salinópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1506302', 'PA', 'Salvaterra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1506351', 'PA', 'Santa Bárbara do Pará', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1506401', 'PA', 'Santa Cruz do Arari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1506500', 'PA', 'Santa Isabel do Pará', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1506559', 'PA', 'Santa Luzia do Pará', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1506583', 'PA', 'Santa Maria das Barreiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1506609', 'PA', 'Santa Maria do Pará', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1506708', 'PA', 'Santana do Araguaia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1506807', 'PA', 'Santarém', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1506906', 'PA', 'Santarém Novo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1507003', 'PA', 'Santo Antônio do Tauá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1507102', 'PA', 'São Caetano de Odivelas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1507151', 'PA', 'São Domingos do Araguaia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1507201', 'PA', 'São Domingos do Capim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1507300', 'PA', 'São Félix do Xingu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1507409', 'PA', 'São Francisco do Pará', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1507458', 'PA', 'São Geraldo do Araguaia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1507466', 'PA', 'São João da Ponta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1507474', 'PA', 'São João de Pirabas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1507508', 'PA', 'São João do Araguaia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1507607', 'PA', 'São Miguel do Guamá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1507706', 'PA', 'São Sebastião da Boa Vista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1507755', 'PA', 'Sapucaia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1507805', 'PA', 'Senador José Porfírio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1507904', 'PA', 'Soure', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1507953', 'PA', 'Tailândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1507961', 'PA', 'Terra Alta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1507979', 'PA', 'Terra Santa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1508001', 'PA', 'Tomé-Açu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1508035', 'PA', 'Tracuateua', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1508050', 'PA', 'Trairão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1508084', 'PA', 'Tucumã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1508100', 'PA', 'Tucuruí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1508126', 'PA', 'Ulianópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1508159', 'PA', 'Uruará', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1508209', 'PA', 'Vigia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1508308', 'PA', 'Viseu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1508357', 'PA', 'Vitória do Xingu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1508407', 'PA', 'Xinguara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1600105', 'AP', 'Amapá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1600204', 'AP', 'Calçoene', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1600212', 'AP', 'Cutias', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1600238', 'AP', 'Ferreira Gomes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1600253', 'AP', 'Itaubal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1600279', 'AP', 'Laranjal do Jari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1600303', 'AP', 'Macapá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1600402', 'AP', 'Mazagão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1600501', 'AP', 'Oiapoque', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1600154', 'AP', 'Pedra Branca do Amapari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1600535', 'AP', 'Porto Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1600550', 'AP', 'Pracuúba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1600600', 'AP', 'Santana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1600055', 'AP', 'Serra do Navio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1600709', 'AP', 'Tartarugalzinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1600808', 'AP', 'Vitória do Jari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1700251', 'TO', 'Abreulândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1700301', 'TO', 'Aguiarnópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1700350', 'TO', 'Aliança do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1700400', 'TO', 'Almas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1700707', 'TO', 'Alvorada', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1701002', 'TO', 'Ananás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1701051', 'TO', 'Angico', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1701101', 'TO', 'Aparecida do Rio Negro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1701309', 'TO', 'Aragominas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1701903', 'TO', 'Araguacema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1702000', 'TO', 'Araguaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1702109', 'TO', 'Araguaína', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1702158', 'TO', 'Araguanã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1702208', 'TO', 'Araguatins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1702307', 'TO', 'Arapoema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1702406', 'TO', 'Arraias', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1702554', 'TO', 'Augustinópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1702703', 'TO', 'Aurora do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1702901', 'TO', 'Axixá do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1703008', 'TO', 'Babaçulândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1703057', 'TO', 'Bandeirantes do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1703073', 'TO', 'Barra do Ouro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1703107', 'TO', 'Barrolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1703206', 'TO', 'Bernardo Sayão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1703305', 'TO', 'Bom Jesus do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1703602', 'TO', 'Brasilândia do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1703701', 'TO', 'Brejinho de Nazaré', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1703800', 'TO', 'Buriti do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1703826', 'TO', 'Cachoeirinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1703842', 'TO', 'Campos Lindos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1703867', 'TO', 'Cariri do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1703883', 'TO', 'Carmolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1703891', 'TO', 'Carrasco Bonito', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1703909', 'TO', 'Caseara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1704105', 'TO', 'Centenário', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1705102', 'TO', 'Chapada da Natividade', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1704600', 'TO', 'Chapada de Areia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1705508', 'TO', 'Colinas do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1716703', 'TO', 'Colméia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1705557', 'TO', 'Combinado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1705607', 'TO', 'Conceição do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1706001', 'TO', 'Couto Magalhães', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1706100', 'TO', 'Cristalândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1706258', 'TO', 'Crixás do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1706506', 'TO', 'Darcinópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1707009', 'TO', 'Dianópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1707108', 'TO', 'Divinópolis do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1707207', 'TO', 'Dois Irmãos do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1707306', 'TO', 'Dueré', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1707405', 'TO', 'Esperantina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1707553', 'TO', 'Fátima', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1707652', 'TO', 'Figueirópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1707702', 'TO', 'Filadélfia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1708205', 'TO', 'Formoso do Araguaia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1708254', 'TO', 'Fortaleza do Tabocão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1708304', 'TO', 'Goianorte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1709005', 'TO', 'Goiatins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1709302', 'TO', 'Guaraí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1709500', 'TO', 'Gurupi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1709807', 'TO', 'Ipueiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1710508', 'TO', 'Itacajá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1710706', 'TO', 'Itaguatins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1710904', 'TO', 'Itapiratins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1711100', 'TO', 'Itaporã do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1711506', 'TO', 'Jaú do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1711803', 'TO', 'Juarina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1711902', 'TO', 'Lagoa da Confusão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1711951', 'TO', 'Lagoa do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1712009', 'TO', 'Lajeado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1712157', 'TO', 'Lavandeira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1712405', 'TO', 'Lizarda', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1712454', 'TO', 'Luzinópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1712504', 'TO', 'Marianópolis do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1712702', 'TO', 'Mateiros', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1712801', 'TO', 'Maurilândia do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1713205', 'TO', 'Miracema do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1713304', 'TO', 'Miranorte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1713601', 'TO', 'Monte do Carmo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1713700', 'TO', 'Monte Santo do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1713957', 'TO', 'Muricilândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1714203', 'TO', 'Natividade', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1714302', 'TO', 'Nazaré', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1714880', 'TO', 'Nova Olinda', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1715002', 'TO', 'Nova Rosalândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1715101', 'TO', 'Novo Acordo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1715150', 'TO', 'Novo Alegre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1715259', 'TO', 'Novo Jardim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1715507', 'TO', 'Oliveira de Fátima', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1721000', 'TO', 'Palmas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1715705', 'TO', 'Palmeirante', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1713809', 'TO', 'Palmeiras do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1715754', 'TO', 'Palmeirópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1716109', 'TO', 'Paraíso do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1716208', 'TO', 'Paranã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1716307', 'TO', 'Pau D''Arco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1716505', 'TO', 'Pedro Afonso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1716604', 'TO', 'Peixe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1716653', 'TO', 'Pequizeiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1717008', 'TO', 'Pindorama do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1717206', 'TO', 'Piraquê', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1717503', 'TO', 'Pium', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1717800', 'TO', 'Ponte Alta do Bom Jesus', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1717909', 'TO', 'Ponte Alta do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1718006', 'TO', 'Porto Alegre do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1718204', 'TO', 'Porto Nacional', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1718303', 'TO', 'Praia Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1718402', 'TO', 'Presidente Kennedy', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1718451', 'TO', 'Pugmil', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1718501', 'TO', 'Recursolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1718550', 'TO', 'Riachinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1718659', 'TO', 'Rio da Conceição', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1718709', 'TO', 'Rio dos Bois', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1718758', 'TO', 'Rio Sono', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1718808', 'TO', 'Sampaio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1718840', 'TO', 'Sandolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1718865', 'TO', 'Santa Fé do Araguaia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1718881', 'TO', 'Santa Maria do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1718899', 'TO', 'Santa Rita do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1718907', 'TO', 'Santa Rosa do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1719004', 'TO', 'Santa Tereza do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1720002', 'TO', 'Santa Terezinha do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1720101', 'TO', 'São Bento do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1720150', 'TO', 'São Félix do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1720200', 'TO', 'São Miguel do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1720259', 'TO', 'São Salvador do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1720309', 'TO', 'São Sebastião do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1720499', 'TO', 'São Valério', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1720655', 'TO', 'Silvanópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1720804', 'TO', 'Sítio Novo do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1720853', 'TO', 'Sucupira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1720903', 'TO', 'Taguatinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1720937', 'TO', 'Taipas do Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1720978', 'TO', 'Talismã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1721109', 'TO', 'Tocantínia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1721208', 'TO', 'Tocantinópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1721257', 'TO', 'Tupirama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1721307', 'TO', 'Tupiratins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1722081', 'TO', 'Wanderlândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('1722107', 'TO', 'Xambioá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2100055', 'MA', 'Açailândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2100105', 'MA', 'Afonso Cunha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2100154', 'MA', 'Água Doce do Maranhão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2100204', 'MA', 'Alcântara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2100303', 'MA', 'Aldeias Altas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2100402', 'MA', 'Altamira do Maranhão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2100436', 'MA', 'Alto Alegre do Maranhão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2100477', 'MA', 'Alto Alegre do Pindaré', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2100501', 'MA', 'Alto Parnaíba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2100550', 'MA', 'Amapá do Maranhão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2100600', 'MA', 'Amarante do Maranhão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2100709', 'MA', 'Anajatuba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2100808', 'MA', 'Anapurus', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2100832', 'MA', 'Apicum-Açu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2100873', 'MA', 'Araguanã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2100907', 'MA', 'Araioses', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2100956', 'MA', 'Arame', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2101004', 'MA', 'Arari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2101103', 'MA', 'Axixá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2101202', 'MA', 'Bacabal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2101251', 'MA', 'Bacabeira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2101301', 'MA', 'Bacuri', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2101350', 'MA', 'Bacurituba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2101400', 'MA', 'Balsas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2101509', 'MA', 'Barão de Grajaú', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2101608', 'MA', 'Barra do Corda', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2101707', 'MA', 'Barreirinhas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2101772', 'MA', 'Bela Vista do Maranhão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2101731', 'MA', 'Belágua', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2101806', 'MA', 'Benedito Leite', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2101905', 'MA', 'Bequimão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2101939', 'MA', 'Bernardo do Mearim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2101970', 'MA', 'Boa Vista do Gurupi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2102002', 'MA', 'Bom Jardim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2102036', 'MA', 'Bom Jesus das Selvas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2102077', 'MA', 'Bom Lugar', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2102101', 'MA', 'Brejo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2102150', 'MA', 'Brejo de Areia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2102200', 'MA', 'Buriti', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2102309', 'MA', 'Buriti Bravo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2102325', 'MA', 'Buriticupu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2102358', 'MA', 'Buritirana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2102374', 'MA', 'Cachoeira Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2102408', 'MA', 'Cajapió', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2102507', 'MA', 'Cajari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2102556', 'MA', 'Campestre do Maranhão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2102606', 'MA', 'Cândido Mendes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2102705', 'MA', 'Cantanhede', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2102754', 'MA', 'Capinzal do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2102804', 'MA', 'Carolina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2102903', 'MA', 'Carutapera', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2103000', 'MA', 'Caxias', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2103109', 'MA', 'Cedral', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2103125', 'MA', 'Central do Maranhão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2103158', 'MA', 'Centro do Guilherme', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2103174', 'MA', 'Centro Novo do Maranhão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2103208', 'MA', 'Chapadinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2103257', 'MA', 'Cidelândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2103307', 'MA', 'Codó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2103406', 'MA', 'Coelho Neto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2103505', 'MA', 'Colinas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2103554', 'MA', 'Conceição do Lago-Açu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2103604', 'MA', 'Coroatá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2103703', 'MA', 'Cururupu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2103752', 'MA', 'Davinópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2103802', 'MA', 'Dom Pedro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2103901', 'MA', 'Duque Bacelar', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2104008', 'MA', 'Esperantinópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2104057', 'MA', 'Estreito', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2104073', 'MA', 'Feira Nova do Maranhão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2104081', 'MA', 'Fernando Falcão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2104099', 'MA', 'Formosa da Serra Negra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2104107', 'MA', 'Fortaleza dos Nogueiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2104206', 'MA', 'Fortuna', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2104305', 'MA', 'Godofredo Viana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2104404', 'MA', 'Gonçalves Dias', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2104503', 'MA', 'Governador Archer', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2104552', 'MA', 'Governador Edison Lobão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2104602', 'MA', 'Governador Eugênio Barros', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2104628', 'MA', 'Governador Luiz Rocha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2104651', 'MA', 'Governador Newton Bello', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2104677', 'MA', 'Governador Nunes Freire', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2104701', 'MA', 'Graça Aranha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2104800', 'MA', 'Grajaú', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2104909', 'MA', 'Guimarães', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2105005', 'MA', 'Humberto de Campos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2105104', 'MA', 'Icatu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2105153', 'MA', 'Igarapé do Meio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2105203', 'MA', 'Igarapé Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2105302', 'MA', 'Imperatriz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2105351', 'MA', 'Itaipava do Grajaú', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2105401', 'MA', 'Itapecuru Mirim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2105427', 'MA', 'Itinga do Maranhão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2105450', 'MA', 'Jatobá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2105476', 'MA', 'Jenipapo dos Vieiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2105500', 'MA', 'João Lisboa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2105609', 'MA', 'Joselândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2105658', 'MA', 'Junco do Maranhão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2105708', 'MA', 'Lago da Pedra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2105807', 'MA', 'Lago do Junco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2105948', 'MA', 'Lago dos Rodrigues', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2105906', 'MA', 'Lago Verde', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2105922', 'MA', 'Lagoa do Mato', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2105963', 'MA', 'Lagoa Grande do Maranhão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2105989', 'MA', 'Lajeado Novo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2106003', 'MA', 'Lima Campos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2106102', 'MA', 'Loreto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2106201', 'MA', 'Luís Domingues', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2106300', 'MA', 'Magalhães de Almeida', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2106326', 'MA', 'Maracaçumé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2106359', 'MA', 'Marajá do Sena', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2106375', 'MA', 'Maranhãozinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2106409', 'MA', 'Mata Roma', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2106508', 'MA', 'Matinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2106607', 'MA', 'Matões', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2106631', 'MA', 'Matões do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2106672', 'MA', 'Milagres do Maranhão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2106706', 'MA', 'Mirador', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2106755', 'MA', 'Miranda do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2106805', 'MA', 'Mirinzal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2106904', 'MA', 'Monção', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2107001', 'MA', 'Montes Altos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2107100', 'MA', 'Morros', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2107209', 'MA', 'Nina Rodrigues', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2107258', 'MA', 'Nova Colinas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2107308', 'MA', 'Nova Iorque', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2107357', 'MA', 'Nova Olinda do Maranhão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2107407', 'MA', 'Olho d''Água das Cunhãs', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2107456', 'MA', 'Olinda Nova do Maranhão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2107506', 'MA', 'Paço do Lumiar', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2107605', 'MA', 'Palmeirândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2107704', 'MA', 'Paraibano', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2107803', 'MA', 'Parnarama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2107902', 'MA', 'Passagem Franca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2108009', 'MA', 'Pastos Bons', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2108058', 'MA', 'Paulino Neves', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2108108', 'MA', 'Paulo Ramos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2108207', 'MA', 'Pedreiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2108256', 'MA', 'Pedro do Rosário', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2108306', 'MA', 'Penalva', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2108405', 'MA', 'Peri Mirim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2108454', 'MA', 'Peritoró', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2108504', 'MA', 'Pindaré-Mirim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2108603', 'MA', 'Pinheiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2108702', 'MA', 'Pio XII', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2108801', 'MA', 'Pirapemas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2108900', 'MA', 'Poção de Pedras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2109007', 'MA', 'Porto Franco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2109056', 'MA', 'Porto Rico do Maranhão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2109106', 'MA', 'Presidente Dutra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2109205', 'MA', 'Presidente Juscelino', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2109239', 'MA', 'Presidente Médici', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2109270', 'MA', 'Presidente Sarney', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2109304', 'MA', 'Presidente Vargas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2109403', 'MA', 'Primeira Cruz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2109452', 'MA', 'Raposa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2109502', 'MA', 'Riachão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2109551', 'MA', 'Ribamar Fiquene', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2109601', 'MA', 'Rosário', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2109700', 'MA', 'Sambaíba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2109759', 'MA', 'Santa Filomena do Maranhão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2109809', 'MA', 'Santa Helena', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2109908', 'MA', 'Santa Inês', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2110005', 'MA', 'Santa Luzia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2110039', 'MA', 'Santa Luzia do Paruá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2110104', 'MA', 'Santa Quitéria do Maranhão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2110203', 'MA', 'Santa Rita', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2110237', 'MA', 'Santana do Maranhão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2110278', 'MA', 'Santo Amaro do Maranhão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2110302', 'MA', 'Santo Antônio dos Lopes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2110401', 'MA', 'São Benedito do Rio Preto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2110500', 'MA', 'São Bento', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2110609', 'MA', 'São Bernardo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2110658', 'MA', 'São Domingos do Azeitão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2110708', 'MA', 'São Domingos do Maranhão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2110807', 'MA', 'São Félix de Balsas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2110856', 'MA', 'São Francisco do Brejão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2110906', 'MA', 'São Francisco do Maranhão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2111003', 'MA', 'São João Batista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2111029', 'MA', 'São João do Carú', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2111052', 'MA', 'São João do Paraíso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2111078', 'MA', 'São João do Soter', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2111102', 'MA', 'São João dos Patos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2111201', 'MA', 'São José de Ribamar', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2111250', 'MA', 'São José dos Basílios', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2111300', 'MA', 'São Luís', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2111409', 'MA', 'São Luís Gonzaga do Maranhão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2111508', 'MA', 'São Mateus do Maranhão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2111532', 'MA', 'São Pedro da Água Branca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2111573', 'MA', 'São Pedro dos Crentes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2111607', 'MA', 'São Raimundo das Mangabeiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2111631', 'MA', 'São Raimundo do Doca Bezerra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2111672', 'MA', 'São Roberto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2111706', 'MA', 'São Vicente Ferrer', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2111722', 'MA', 'Satubinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2111748', 'MA', 'Senador Alexandre Costa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2111763', 'MA', 'Senador La Rocque', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2111789', 'MA', 'Serrano do Maranhão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2111805', 'MA', 'Sítio Novo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2111904', 'MA', 'Sucupira do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2111953', 'MA', 'Sucupira do Riachão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2112001', 'MA', 'Tasso Fragoso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2112100', 'MA', 'Timbiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2112209', 'MA', 'Timon', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2112233', 'MA', 'Trizidela do Vale', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2112274', 'MA', 'Tufilândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2112308', 'MA', 'Tuntum', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2112407', 'MA', 'Turiaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2112456', 'MA', 'Turilândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2112506', 'MA', 'Tutóia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2112605', 'MA', 'Urbano Santos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2112704', 'MA', 'Vargem Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2112803', 'MA', 'Viana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2112852', 'MA', 'Vila Nova dos Martírios', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2112902', 'MA', 'Vitória do Mearim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2113009', 'MA', 'Vitorino Freire', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2114007', 'MA', 'Zé Doca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2200053', 'PI', 'Acauã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2200103', 'PI', 'Agricolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2200202', 'PI', 'Água Branca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2200251', 'PI', 'Alagoinha do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2200277', 'PI', 'Alegrete do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2200301', 'PI', 'Alto Longá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2200400', 'PI', 'Altos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2200459', 'PI', 'Alvorada do Gurguéia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2200509', 'PI', 'Amarante', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2200608', 'PI', 'Angical do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2200707', 'PI', 'Anísio de Abreu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2200806', 'PI', 'Antônio Almeida', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2200905', 'PI', 'Aroazes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2200954', 'PI', 'Aroeiras do Itaim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2201002', 'PI', 'Arraial', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2201051', 'PI', 'Assunção do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2201101', 'PI', 'Avelino Lopes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2201150', 'PI', 'Baixa Grande do Ribeiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2201176', 'PI', 'Barra D''Alcântara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2201200', 'PI', 'Barras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2201309', 'PI', 'Barreiras do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2201408', 'PI', 'Barro Duro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2201507', 'PI', 'Batalha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2201556', 'PI', 'Bela Vista do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2201572', 'PI', 'Belém do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2201606', 'PI', 'Beneditinos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2201705', 'PI', 'Bertolínia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2201739', 'PI', 'Betânia do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2201770', 'PI', 'Boa Hora', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2201804', 'PI', 'Bocaina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2201903', 'PI', 'Bom Jesus', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2201919', 'PI', 'Bom Princípio do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2201929', 'PI', 'Bonfim do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2201945', 'PI', 'Boqueirão do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2201960', 'PI', 'Brasileira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2201988', 'PI', 'Brejo do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2202000', 'PI', 'Buriti dos Lopes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2202026', 'PI', 'Buriti dos Montes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2202059', 'PI', 'Cabeceiras do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2202075', 'PI', 'Cajazeiras do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2202083', 'PI', 'Cajueiro da Praia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2202091', 'PI', 'Caldeirão Grande do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2202109', 'PI', 'Campinas do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2202117', 'PI', 'Campo Alegre do Fidalgo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2202133', 'PI', 'Campo Grande do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2202174', 'PI', 'Campo Largo do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2202208', 'PI', 'Campo Maior', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2202251', 'PI', 'Canavieira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2202307', 'PI', 'Canto do Buriti', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2202406', 'PI', 'Capitão de Campos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2202455', 'PI', 'Capitão Gervásio Oliveira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2202505', 'PI', 'Caracol', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2202539', 'PI', 'Caraúbas do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2202554', 'PI', 'Caridade do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2202604', 'PI', 'Castelo do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2202653', 'PI', 'Caxingó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2202703', 'PI', 'Cocal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2202711', 'PI', 'Cocal de Telha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2202729', 'PI', 'Cocal dos Alves', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2202737', 'PI', 'Coivaras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2202752', 'PI', 'Colônia do Gurguéia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2202778', 'PI', 'Colônia do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2202802', 'PI', 'Conceição do Canindé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2202851', 'PI', 'Coronel José Dias', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2202901', 'PI', 'Corrente', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2203008', 'PI', 'Cristalândia do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2203107', 'PI', 'Cristino Castro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2203206', 'PI', 'Curimatá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2203230', 'PI', 'Currais', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2203271', 'PI', 'Curral Novo do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2203255', 'PI', 'Curralinhos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2203305', 'PI', 'Demerval Lobão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2203354', 'PI', 'Dirceu Arcoverde', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2203404', 'PI', 'Dom Expedito Lopes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2203453', 'PI', 'Dom Inocêncio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2203420', 'PI', 'Domingos Mourão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2203503', 'PI', 'Elesbão Veloso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2203602', 'PI', 'Eliseu Martins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2203701', 'PI', 'Esperantina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2203750', 'PI', 'Fartura do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2203800', 'PI', 'Flores do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2203859', 'PI', 'Floresta do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2203909', 'PI', 'Floriano', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2204006', 'PI', 'Francinópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2204105', 'PI', 'Francisco Ayres', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2204154', 'PI', 'Francisco Macedo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2204204', 'PI', 'Francisco Santos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2204303', 'PI', 'Fronteiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2204352', 'PI', 'Geminiano', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2204402', 'PI', 'Gilbués', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2204501', 'PI', 'Guadalupe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2204550', 'PI', 'Guaribas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2204600', 'PI', 'Hugo Napoleão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2204659', 'PI', 'Ilha Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2204709', 'PI', 'Inhuma', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2204808', 'PI', 'Ipiranga do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2204907', 'PI', 'Isaías Coelho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2205003', 'PI', 'Itainópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2205102', 'PI', 'Itaueira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2205151', 'PI', 'Jacobina do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2205201', 'PI', 'Jaicós', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2205250', 'PI', 'Jardim do Mulato', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2205276', 'PI', 'Jatobá do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2205300', 'PI', 'Jerumenha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2205359', 'PI', 'João Costa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2205409', 'PI', 'Joaquim Pires', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2205458', 'PI', 'Joca Marques', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2205508', 'PI', 'José de Freitas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2205516', 'PI', 'Juazeiro do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2205524', 'PI', 'Júlio Borges', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2205532', 'PI', 'Jurema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2205557', 'PI', 'Lagoa Alegre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2205573', 'PI', 'Lagoa de São Francisco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2205565', 'PI', 'Lagoa do Barro do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2205581', 'PI', 'Lagoa do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2205599', 'PI', 'Lagoa do Sítio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2205540', 'PI', 'Lagoinha do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2205607', 'PI', 'Landri Sales', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2205706', 'PI', 'Luís Correia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2205805', 'PI', 'Luzilândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2205854', 'PI', 'Madeiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2205904', 'PI', 'Manoel Emídio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2205953', 'PI', 'Marcolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2206001', 'PI', 'Marcos Parente', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2206050', 'PI', 'Massapê do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2206100', 'PI', 'Matias Olímpio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2206209', 'PI', 'Miguel Alves', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2206308', 'PI', 'Miguel Leão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2206357', 'PI', 'Milton Brandão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2206407', 'PI', 'Monsenhor Gil', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2206506', 'PI', 'Monsenhor Hipólito', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2206605', 'PI', 'Monte Alegre do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2206654', 'PI', 'Morro Cabeça no Tempo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2206670', 'PI', 'Morro do Chapéu do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2206696', 'PI', 'Murici dos Portelas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2206704', 'PI', 'Nazaré do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2206720', 'PI', 'Nazária ', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2206753', 'PI', 'Nossa Senhora de Nazaré', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2206803', 'PI', 'Nossa Senhora dos Remédios', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2207959', 'PI', 'Nova Santa Rita', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2206902', 'PI', 'Novo Oriente do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2206951', 'PI', 'Novo Santo Antônio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2207009', 'PI', 'Oeiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2207108', 'PI', 'Olho D''Água do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2207207', 'PI', 'Padre Marcos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2207306', 'PI', 'Paes Landim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2207355', 'PI', 'Pajeú do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2207405', 'PI', 'Palmeira do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2207504', 'PI', 'Palmeirais', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2207553', 'PI', 'Paquetá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2207603', 'PI', 'Parnaguá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2207702', 'PI', 'Parnaíba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2207751', 'PI', 'Passagem Franca do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2207777', 'PI', 'Patos do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2207793', 'PI', 'Pau D''Arco do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2207801', 'PI', 'Paulistana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2207850', 'PI', 'Pavussu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2207900', 'PI', 'Pedro II', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2207934', 'PI', 'Pedro Laurentino', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2208007', 'PI', 'Picos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2208106', 'PI', 'Pimenteiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2208205', 'PI', 'Pio IX', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2208304', 'PI', 'Piracuruca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2208403', 'PI', 'Piripiri', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2208502', 'PI', 'Porto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2208551', 'PI', 'Porto Alegre do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2208601', 'PI', 'Prata do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2208650', 'PI', 'Queimada Nova', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2208700', 'PI', 'Redenção do Gurguéia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2208809', 'PI', 'Regeneração', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2208858', 'PI', 'Riacho Frio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2208874', 'PI', 'Ribeira do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2208908', 'PI', 'Ribeiro Gonçalves', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2209005', 'PI', 'Rio Grande do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2209104', 'PI', 'Santa Cruz do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2209153', 'PI', 'Santa Cruz dos Milagres', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2209203', 'PI', 'Santa Filomena', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2209302', 'PI', 'Santa Luz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2209377', 'PI', 'Santa Rosa do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2209351', 'PI', 'Santana do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2209401', 'PI', 'Santo Antônio de Lisboa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2209450', 'PI', 'Santo Antônio dos Milagres', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2209500', 'PI', 'Santo Inácio do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2209559', 'PI', 'São Braz do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2209609', 'PI', 'São Félix do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2209658', 'PI', 'São Francisco de Assis do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2209708', 'PI', 'São Francisco do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2209757', 'PI', 'São Gonçalo do Gurguéia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2209807', 'PI', 'São Gonçalo do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2209856', 'PI', 'São João da Canabrava', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2209872', 'PI', 'São João da Fronteira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2209906', 'PI', 'São João da Serra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2209955', 'PI', 'São João da Varjota', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2209971', 'PI', 'São João do Arraial', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2210003', 'PI', 'São João do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2210052', 'PI', 'São José do Divino', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2210102', 'PI', 'São José do Peixe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2210201', 'PI', 'São José do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2210300', 'PI', 'São Julião', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2210359', 'PI', 'São Lourenço do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2210375', 'PI', 'São Luis do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2210383', 'PI', 'São Miguel da Baixa Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2210391', 'PI', 'São Miguel do Fidalgo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2210409', 'PI', 'São Miguel do Tapuio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2210508', 'PI', 'São Pedro do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2210607', 'PI', 'São Raimundo Nonato', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2210623', 'PI', 'Sebastião Barros', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2210631', 'PI', 'Sebastião Leal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2210656', 'PI', 'Sigefredo Pacheco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2210706', 'PI', 'Simões', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2210805', 'PI', 'Simplício Mendes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2210904', 'PI', 'Socorro do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2210938', 'PI', 'Sussuapara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2210953', 'PI', 'Tamboril do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2210979', 'PI', 'Tanque do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2211001', 'PI', 'Teresina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2211100', 'PI', 'União', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2211209', 'PI', 'Uruçuí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2211308', 'PI', 'Valença do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2211357', 'PI', 'Várzea Branca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2211407', 'PI', 'Várzea Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2211506', 'PI', 'Vera Mendes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2211605', 'PI', 'Vila Nova do Piauí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2211704', 'PI', 'Wall Ferraz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2300101', 'CE', 'Abaiara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2300150', 'CE', 'Acarape', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2300200', 'CE', 'Acaraú', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2300309', 'CE', 'Acopiara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2300408', 'CE', 'Aiuaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2300507', 'CE', 'Alcântaras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2300606', 'CE', 'Altaneira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2300705', 'CE', 'Alto Santo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2300754', 'CE', 'Amontada', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2300804', 'CE', 'Antonina do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2300903', 'CE', 'Apuiarés', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2301000', 'CE', 'Aquiraz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2301109', 'CE', 'Aracati', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2301208', 'CE', 'Aracoiaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2301257', 'CE', 'Ararendá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2301307', 'CE', 'Araripe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2301406', 'CE', 'Aratuba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2301505', 'CE', 'Arneiroz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2301604', 'CE', 'Assaré', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2301703', 'CE', 'Aurora', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2301802', 'CE', 'Baixio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2301851', 'CE', 'Banabuiú', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2301901', 'CE', 'Barbalha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2301950', 'CE', 'Barreira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2302008', 'CE', 'Barro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2302057', 'CE', 'Barroquinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2302107', 'CE', 'Baturité', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2302206', 'CE', 'Beberibe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2302305', 'CE', 'Bela Cruz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2302404', 'CE', 'Boa Viagem', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2302503', 'CE', 'Brejo Santo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2302602', 'CE', 'Camocim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2302701', 'CE', 'Campos Sales', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2302800', 'CE', 'Canindé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2302909', 'CE', 'Capistrano', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2303006', 'CE', 'Caridade', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2303105', 'CE', 'Cariré', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2303204', 'CE', 'Caririaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2303303', 'CE', 'Cariús', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2303402', 'CE', 'Carnaubal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2303501', 'CE', 'Cascavel', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2303600', 'CE', 'Catarina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2303659', 'CE', 'Catunda', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2303709', 'CE', 'Caucaia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2303808', 'CE', 'Cedro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2303907', 'CE', 'Chaval', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2303931', 'CE', 'Choró', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2303956', 'CE', 'Chorozinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2304004', 'CE', 'Coreaú', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2304103', 'CE', 'Crateús', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2304202', 'CE', 'Crato', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2304236', 'CE', 'Croatá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2304251', 'CE', 'Cruz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2304269', 'CE', 'Deputado Irapuan Pinheiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2304277', 'CE', 'Ererê', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2304285', 'CE', 'Eusébio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2304301', 'CE', 'Farias Brito', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2304350', 'CE', 'Forquilha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2304400', 'CE', 'Fortaleza', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2304459', 'CE', 'Fortim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2304509', 'CE', 'Frecheirinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2304608', 'CE', 'General Sampaio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2304657', 'CE', 'Graça', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2304707', 'CE', 'Granja', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2304806', 'CE', 'Granjeiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2304905', 'CE', 'Groaíras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2304954', 'CE', 'Guaiúba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2305001', 'CE', 'Guaraciaba do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2305100', 'CE', 'Guaramiranga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2305209', 'CE', 'Hidrolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2305233', 'CE', 'Horizonte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2305266', 'CE', 'Ibaretama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2305308', 'CE', 'Ibiapina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2305332', 'CE', 'Ibicuitinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2305357', 'CE', 'Icapuí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2305407', 'CE', 'Icó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2305506', 'CE', 'Iguatu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2305605', 'CE', 'Independência', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2305654', 'CE', 'Ipaporanga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2305704', 'CE', 'Ipaumirim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2305803', 'CE', 'Ipu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2305902', 'CE', 'Ipueiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2306009', 'CE', 'Iracema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2306108', 'CE', 'Irauçuba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2306207', 'CE', 'Itaiçaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2306256', 'CE', 'Itaitinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2306306', 'CE', 'Itapagé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2306405', 'CE', 'Itapipoca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2306504', 'CE', 'Itapiúna', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2306553', 'CE', 'Itarema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2306603', 'CE', 'Itatira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2306702', 'CE', 'Jaguaretama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2306801', 'CE', 'Jaguaribara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2306900', 'CE', 'Jaguaribe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2307007', 'CE', 'Jaguaruana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2307106', 'CE', 'Jardim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2307205', 'CE', 'Jati', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2307254', 'CE', 'Jijoca de Jericoacoara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2307304', 'CE', 'Juazeiro do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2307403', 'CE', 'Jucás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2307502', 'CE', 'Lavras da Mangabeira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2307601', 'CE', 'Limoeiro do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2307635', 'CE', 'Madalena', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2307650', 'CE', 'Maracanaú', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2307700', 'CE', 'Maranguape', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2307809', 'CE', 'Marco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2307908', 'CE', 'Martinópole', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2308005', 'CE', 'Massapê', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2308104', 'CE', 'Mauriti', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2308203', 'CE', 'Meruoca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2308302', 'CE', 'Milagres', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2308351', 'CE', 'Milhã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2308377', 'CE', 'Miraíma', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2308401', 'CE', 'Missão Velha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2308500', 'CE', 'Mombaça', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2308609', 'CE', 'Monsenhor Tabosa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2308708', 'CE', 'Morada Nova', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2308807', 'CE', 'Moraújo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2308906', 'CE', 'Morrinhos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2309003', 'CE', 'Mucambo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2309102', 'CE', 'Mulungu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2309201', 'CE', 'Nova Olinda', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2309300', 'CE', 'Nova Russas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2309409', 'CE', 'Novo Oriente', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2309458', 'CE', 'Ocara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2309508', 'CE', 'Orós', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2309607', 'CE', 'Pacajus', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2309706', 'CE', 'Pacatuba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2309805', 'CE', 'Pacoti', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2309904', 'CE', 'Pacujá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2310001', 'CE', 'Palhano', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2310100', 'CE', 'Palmácia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2310209', 'CE', 'Paracuru', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2310258', 'CE', 'Paraipaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2310308', 'CE', 'Parambu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2310407', 'CE', 'Paramoti', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2310506', 'CE', 'Pedra Branca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2310605', 'CE', 'Penaforte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2310704', 'CE', 'Pentecoste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2310803', 'CE', 'Pereiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2310852', 'CE', 'Pindoretama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2310902', 'CE', 'Piquet Carneiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2310951', 'CE', 'Pires Ferreira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2311009', 'CE', 'Poranga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2311108', 'CE', 'Porteiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2311207', 'CE', 'Potengi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2311231', 'CE', 'Potiretama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2311264', 'CE', 'Quiterianópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2311306', 'CE', 'Quixadá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2311355', 'CE', 'Quixelô', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2311405', 'CE', 'Quixeramobim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2311504', 'CE', 'Quixeré', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2311603', 'CE', 'Redenção', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2311702', 'CE', 'Reriutaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2311801', 'CE', 'Russas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2311900', 'CE', 'Saboeiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2311959', 'CE', 'Salitre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2312205', 'CE', 'Santa Quitéria', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2312007', 'CE', 'Santana do Acaraú', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2312106', 'CE', 'Santana do Cariri', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2312304', 'CE', 'São Benedito', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2312403', 'CE', 'São Gonçalo do Amarante', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2312502', 'CE', 'São João do Jaguaribe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2312601', 'CE', 'São Luís do Curu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2312700', 'CE', 'Senador Pompeu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2312809', 'CE', 'Senador Sá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2312908', 'CE', 'Sobral', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2313005', 'CE', 'Solonópole', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2313104', 'CE', 'Tabuleiro do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2313203', 'CE', 'Tamboril', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2313252', 'CE', 'Tarrafas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2313302', 'CE', 'Tauá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2313351', 'CE', 'Tejuçuoca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2313401', 'CE', 'Tianguá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2313500', 'CE', 'Trairi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2313559', 'CE', 'Tururu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2313609', 'CE', 'Ubajara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2313708', 'CE', 'Umari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2313757', 'CE', 'Umirim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2313807', 'CE', 'Uruburetama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2313906', 'CE', 'Uruoca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2313955', 'CE', 'Varjota', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2314003', 'CE', 'Várzea Alegre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2314102', 'CE', 'Viçosa do Ceará', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2400109', 'RN', 'Acari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2400208', 'RN', 'Açu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2400307', 'RN', 'Afonso Bezerra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2400406', 'RN', 'Água Nova', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2400505', 'RN', 'Alexandria', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2400604', 'RN', 'Almino Afonso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2400703', 'RN', 'Alto do Rodrigues', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2400802', 'RN', 'Angicos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2400901', 'RN', 'Antônio Martins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2401008', 'RN', 'Apodi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2401107', 'RN', 'Areia Branca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2401206', 'RN', 'Arês', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2401305', 'RN', 'Augusto Severo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2401404', 'RN', 'Baía Formosa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2401453', 'RN', 'Baraúna', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2401503', 'RN', 'Barcelona', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2401602', 'RN', 'Bento Fernandes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2401651', 'RN', 'Bodó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2401701', 'RN', 'Bom Jesus', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2401800', 'RN', 'Brejinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2401859', 'RN', 'Caiçara do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2401909', 'RN', 'Caiçara do Rio do Vento', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2402006', 'RN', 'Caicó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2402105', 'RN', 'Campo Redondo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2402204', 'RN', 'Canguaretama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2402303', 'RN', 'Caraúbas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2402402', 'RN', 'Carnaúba dos Dantas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2402501', 'RN', 'Carnaubais', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2402600', 'RN', 'Ceará-Mirim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2402709', 'RN', 'Cerro Corá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2402808', 'RN', 'Coronel Ezequiel', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2402907', 'RN', 'Coronel João Pessoa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2403004', 'RN', 'Cruzeta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2403103', 'RN', 'Currais Novos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2403202', 'RN', 'Doutor Severiano', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2403301', 'RN', 'Encanto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2403400', 'RN', 'Equador', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2403509', 'RN', 'Espírito Santo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2403608', 'RN', 'Extremoz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2403707', 'RN', 'Felipe Guerra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2403756', 'RN', 'Fernando Pedroza', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2403806', 'RN', 'Florânia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2403905', 'RN', 'Francisco Dantas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2404002', 'RN', 'Frutuoso Gomes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2404101', 'RN', 'Galinhos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2404200', 'RN', 'Goianinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2404309', 'RN', 'Governador Dix-Sept Rosado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2404408', 'RN', 'Grossos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2404507', 'RN', 'Guamaré', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2404606', 'RN', 'Ielmo Marinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2404705', 'RN', 'Ipanguaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2404804', 'RN', 'Ipueira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2404853', 'RN', 'Itajá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2404903', 'RN', 'Itaú', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2405009', 'RN', 'Jaçanã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2405108', 'RN', 'Jandaíra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2405207', 'RN', 'Janduís', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2405306', 'RN', 'Januário Cicco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2405405', 'RN', 'Japi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2405504', 'RN', 'Jardim de Angicos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2405603', 'RN', 'Jardim de Piranhas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2405702', 'RN', 'Jardim do Seridó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2405801', 'RN', 'João Câmara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2405900', 'RN', 'João Dias', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2406007', 'RN', 'José da Penha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2406106', 'RN', 'Jucurutu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2406155', 'RN', 'Jundiá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2406205', 'RN', 'Lagoa d''Anta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2406304', 'RN', 'Lagoa de Pedras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2406403', 'RN', 'Lagoa de Velhos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2406502', 'RN', 'Lagoa Nova', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2406601', 'RN', 'Lagoa Salgada', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2406700', 'RN', 'Lajes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2406809', 'RN', 'Lajes Pintadas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2406908', 'RN', 'Lucrécia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2407005', 'RN', 'Luís Gomes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2407104', 'RN', 'Macaíba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2407203', 'RN', 'Macau', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2407252', 'RN', 'Major Sales', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2407302', 'RN', 'Marcelino Vieira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2407401', 'RN', 'Martins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2407500', 'RN', 'Maxaranguape', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2407609', 'RN', 'Messias Targino', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2407708', 'RN', 'Montanhas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2407807', 'RN', 'Monte Alegre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2407906', 'RN', 'Monte das Gameleiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2408003', 'RN', 'Mossoró', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2408102', 'RN', 'Natal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2408201', 'RN', 'Nísia Floresta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2408300', 'RN', 'Nova Cruz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2408409', 'RN', 'Olho-d''Água do Borges', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2408508', 'RN', 'Ouro Branco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2408607', 'RN', 'Paraná', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2408706', 'RN', 'Paraú', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2408805', 'RN', 'Parazinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2408904', 'RN', 'Parelhas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2403251', 'RN', 'Parnamirim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2409100', 'RN', 'Passa e Fica', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2409209', 'RN', 'Passagem', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2409308', 'RN', 'Patu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2409407', 'RN', 'Pau dos Ferros', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2409506', 'RN', 'Pedra Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2409605', 'RN', 'Pedra Preta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2409704', 'RN', 'Pedro Avelino', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2409803', 'RN', 'Pedro Velho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2409902', 'RN', 'Pendências', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2410009', 'RN', 'Pilões', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2410108', 'RN', 'Poço Branco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2410207', 'RN', 'Portalegre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2410256', 'RN', 'Porto do Mangue', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2410306', 'RN', 'Presidente Juscelino', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2410405', 'RN', 'Pureza', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2410504', 'RN', 'Rafael Fernandes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2410603', 'RN', 'Rafael Godeiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2410702', 'RN', 'Riacho da Cruz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2410801', 'RN', 'Riacho de Santana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2410900', 'RN', 'Riachuelo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2408953', 'RN', 'Rio do Fogo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2411007', 'RN', 'Rodolfo Fernandes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2411106', 'RN', 'Ruy Barbosa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2411205', 'RN', 'Santa Cruz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2409332', 'RN', 'Santa Maria', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2411403', 'RN', 'Santana do Matos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2411429', 'RN', 'Santana do Seridó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2411502', 'RN', 'Santo Antônio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2411601', 'RN', 'São Bento do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2411700', 'RN', 'São Bento do Trairí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2411809', 'RN', 'São Fernando', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2411908', 'RN', 'São Francisco do Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2412005', 'RN', 'São Gonçalo do Amarante', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2412104', 'RN', 'São João do Sabugi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2412203', 'RN', 'São José de Mipibu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2412302', 'RN', 'São José do Campestre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2412401', 'RN', 'São José do Seridó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2412500', 'RN', 'São Miguel', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2412559', 'RN', 'São Miguel do Gostoso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2412609', 'RN', 'São Paulo do Potengi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2412708', 'RN', 'São Pedro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2412807', 'RN', 'São Rafael', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2412906', 'RN', 'São Tomé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2413003', 'RN', 'São Vicente', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2413102', 'RN', 'Senador Elói de Souza', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2413201', 'RN', 'Senador Georgino Avelino', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2413300', 'RN', 'Serra de São Bento', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2413359', 'RN', 'Serra do Mel', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2413409', 'RN', 'Serra Negra do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2413508', 'RN', 'Serrinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2413557', 'RN', 'Serrinha dos Pintos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2413607', 'RN', 'Severiano Melo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2413706', 'RN', 'Sítio Novo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2413805', 'RN', 'Taboleiro Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2413904', 'RN', 'Taipu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2414001', 'RN', 'Tangará', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2414100', 'RN', 'Tenente Ananias', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2414159', 'RN', 'Tenente Laurentino Cruz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2411056', 'RN', 'Tibau', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2414209', 'RN', 'Tibau do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2414308', 'RN', 'Timbaúba dos Batistas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2414407', 'RN', 'Touros', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2414456', 'RN', 'Triunfo Potiguar', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2414506', 'RN', 'Umarizal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2414605', 'RN', 'Upanema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2414704', 'RN', 'Várzea', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2414753', 'RN', 'Venha-Ver', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2414803', 'RN', 'Vera Cruz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2414902', 'RN', 'Viçosa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2415008', 'RN', 'Vila Flor', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2500106', 'PB', 'Água Branca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2500205', 'PB', 'Aguiar', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2500304', 'PB', 'Alagoa Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2500403', 'PB', 'Alagoa Nova', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2500502', 'PB', 'Alagoinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2500536', 'PB', 'Alcantil', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2500577', 'PB', 'Algodão de Jandaíra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2500601', 'PB', 'Alhandra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2500734', 'PB', 'Amparo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2500775', 'PB', 'Aparecida', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2500809', 'PB', 'Araçagi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2500908', 'PB', 'Arara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2501005', 'PB', 'Araruna', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2501104', 'PB', 'Areia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2501153', 'PB', 'Areia de Baraúnas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2501203', 'PB', 'Areial', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2501302', 'PB', 'Aroeiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2501351', 'PB', 'Assunção', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2501401', 'PB', 'Baía da Traição', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2501500', 'PB', 'Bananeiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2501534', 'PB', 'Baraúna', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2501609', 'PB', 'Barra de Santa Rosa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2501575', 'PB', 'Barra de Santana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2501708', 'PB', 'Barra de São Miguel', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2501807', 'PB', 'Bayeux', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2501906', 'PB', 'Belém', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2502003', 'PB', 'Belém do Brejo do Cruz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2502052', 'PB', 'Bernardino Batista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2502102', 'PB', 'Boa Ventura', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2502151', 'PB', 'Boa Vista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2502201', 'PB', 'Bom Jesus', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2502300', 'PB', 'Bom Sucesso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2502409', 'PB', 'Bonito de Santa Fé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2502508', 'PB', 'Boqueirão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2502706', 'PB', 'Borborema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2502805', 'PB', 'Brejo do Cruz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2502904', 'PB', 'Brejo dos Santos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2503001', 'PB', 'Caaporã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2503100', 'PB', 'Cabaceiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2503209', 'PB', 'Cabedelo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2503308', 'PB', 'Cachoeira dos Índios', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2503407', 'PB', 'Cacimba de Areia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2503506', 'PB', 'Cacimba de Dentro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2503555', 'PB', 'Cacimbas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2503605', 'PB', 'Caiçara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2503704', 'PB', 'Cajazeiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2503753', 'PB', 'Cajazeirinhas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2503803', 'PB', 'Caldas Brandão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2503902', 'PB', 'Camalaú', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2504009', 'PB', 'Campina Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2516409', 'PB', 'Tacima', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2504033', 'PB', 'Capim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2504074', 'PB', 'Caraúbas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2504108', 'PB', 'Carrapateira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2504157', 'PB', 'Casserengue', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2504207', 'PB', 'Catingueira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2504306', 'PB', 'Catolé do Rocha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2504355', 'PB', 'Caturité', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2504405', 'PB', 'Conceição', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2504504', 'PB', 'Condado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2504603', 'PB', 'Conde', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2504702', 'PB', 'Congo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2504801', 'PB', 'Coremas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2504850', 'PB', 'Coxixola', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2504900', 'PB', 'Cruz do Espírito Santo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2505006', 'PB', 'Cubati', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2505105', 'PB', 'Cuité', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2505238', 'PB', 'Cuité de Mamanguape', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2505204', 'PB', 'Cuitegi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2505279', 'PB', 'Curral de Cima', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2505303', 'PB', 'Curral Velho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2505352', 'PB', 'Damião', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2505402', 'PB', 'Desterro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2505600', 'PB', 'Diamante', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2505709', 'PB', 'Dona Inês', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2505808', 'PB', 'Duas Estradas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2505907', 'PB', 'Emas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2506004', 'PB', 'Esperança', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2506103', 'PB', 'Fagundes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2506202', 'PB', 'Frei Martinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2506251', 'PB', 'Gado Bravo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2506301', 'PB', 'Guarabira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2506400', 'PB', 'Gurinhém', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2506509', 'PB', 'Gurjão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2506608', 'PB', 'Ibiara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2502607', 'PB', 'Igaracy', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2506707', 'PB', 'Imaculada', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2506806', 'PB', 'Ingá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2506905', 'PB', 'Itabaiana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2507002', 'PB', 'Itaporanga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2507101', 'PB', 'Itapororoca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2507200', 'PB', 'Itatuba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2507309', 'PB', 'Jacaraú', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2507408', 'PB', 'Jericó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2507507', 'PB', 'João Pessoa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2507606', 'PB', 'Juarez Távora', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2507705', 'PB', 'Juazeirinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2507804', 'PB', 'Junco do Seridó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2507903', 'PB', 'Juripiranga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2508000', 'PB', 'Juru', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2508109', 'PB', 'Lagoa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2508208', 'PB', 'Lagoa de Dentro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2508307', 'PB', 'Lagoa Seca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2508406', 'PB', 'Lastro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2508505', 'PB', 'Livramento', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2508554', 'PB', 'Logradouro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2508604', 'PB', 'Lucena', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2508703', 'PB', 'Mãe d''Água', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2508802', 'PB', 'Malta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2508901', 'PB', 'Mamanguape', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2509008', 'PB', 'Manaíra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2509057', 'PB', 'Marcação', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2509107', 'PB', 'Mari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2509156', 'PB', 'Marizópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2509206', 'PB', 'Massaranduba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2509305', 'PB', 'Mataraca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2509339', 'PB', 'Matinhas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2509370', 'PB', 'Mato Grosso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2509396', 'PB', 'Maturéia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2509404', 'PB', 'Mogeiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2509503', 'PB', 'Montadas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2509602', 'PB', 'Monte Horebe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2509701', 'PB', 'Monteiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2509800', 'PB', 'Mulungu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2509909', 'PB', 'Natuba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2510006', 'PB', 'Nazarezinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2510105', 'PB', 'Nova Floresta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2510204', 'PB', 'Nova Olinda', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2510303', 'PB', 'Nova Palmeira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2510402', 'PB', 'Olho d''Água', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2510501', 'PB', 'Olivedos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2510600', 'PB', 'Ouro Velho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2510659', 'PB', 'Parari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2510709', 'PB', 'Passagem', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2510808', 'PB', 'Patos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2510907', 'PB', 'Paulista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2511004', 'PB', 'Pedra Branca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2511103', 'PB', 'Pedra Lavrada', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2511202', 'PB', 'Pedras de Fogo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2512721', 'PB', 'Pedro Régis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2511301', 'PB', 'Piancó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2511400', 'PB', 'Picuí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2511509', 'PB', 'Pilar', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2511608', 'PB', 'Pilões', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2511707', 'PB', 'Pilõezinhos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2511806', 'PB', 'Pirpirituba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2511905', 'PB', 'Pitimbu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2512002', 'PB', 'Pocinhos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2512036', 'PB', 'Poço Dantas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2512077', 'PB', 'Poço de José de Moura', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2512101', 'PB', 'Pombal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2512200', 'PB', 'Prata', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2512309', 'PB', 'Princesa Isabel', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2512408', 'PB', 'Puxinanã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2512507', 'PB', 'Queimadas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2512606', 'PB', 'Quixabá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2512705', 'PB', 'Remígio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2512747', 'PB', 'Riachão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2512754', 'PB', 'Riachão do Bacamarte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2512762', 'PB', 'Riachão do Poço', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2512788', 'PB', 'Riacho de Santo Antônio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2512804', 'PB', 'Riacho dos Cavalos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2512903', 'PB', 'Rio Tinto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2513000', 'PB', 'Salgadinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2513109', 'PB', 'Salgado de São Félix', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2513158', 'PB', 'Santa Cecília', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2513208', 'PB', 'Santa Cruz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2513307', 'PB', 'Santa Helena', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2513356', 'PB', 'Santa Inês', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2513406', 'PB', 'Santa Luzia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2513703', 'PB', 'Santa Rita', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2513802', 'PB', 'Santa Teresinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2513505', 'PB', 'Santana de Mangueira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2513604', 'PB', 'Santana dos Garrotes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2513653', 'PB', 'Joca Claudino', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2513851', 'PB', 'Santo André', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2513927', 'PB', 'São Bentinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2513901', 'PB', 'São Bento', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2513968', 'PB', 'São Domingos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2513943', 'PB', 'São Domingos do Cariri', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2513984', 'PB', 'São Francisco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2514008', 'PB', 'São João do Cariri', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2500700', 'PB', 'São João do Rio do Peixe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2514107', 'PB', 'São João do Tigre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2514206', 'PB', 'São José da Lagoa Tapada', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2514305', 'PB', 'São José de Caiana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2514404', 'PB', 'São José de Espinharas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2514503', 'PB', 'São José de Piranhas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2514552', 'PB', 'São José de Princesa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2514602', 'PB', 'São José do Bonfim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2514651', 'PB', 'São José do Brejo do Cruz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2514701', 'PB', 'São José do Sabugi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2514800', 'PB', 'São José dos Cordeiros', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2514453', 'PB', 'São José dos Ramos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2514909', 'PB', 'São Mamede', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2515005', 'PB', 'São Miguel de Taipu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2515104', 'PB', 'São Sebastião de Lagoa de Roça', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2515203', 'PB', 'São Sebastião do Umbuzeiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2515302', 'PB', 'Sapé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2515401', 'PB', 'Seridó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2515500', 'PB', 'Serra Branca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2515609', 'PB', 'Serra da Raiz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2515708', 'PB', 'Serra Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2515807', 'PB', 'Serra Redonda', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2515906', 'PB', 'Serraria', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2515930', 'PB', 'Sertãozinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2515971', 'PB', 'Sobrado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2516003', 'PB', 'Solânea', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2516102', 'PB', 'Soledade', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2516151', 'PB', 'Sossêgo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2516201', 'PB', 'Sousa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2516300', 'PB', 'Sumé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2516508', 'PB', 'Taperoá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2516607', 'PB', 'Tavares', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2516706', 'PB', 'Teixeira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2516755', 'PB', 'Tenório', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2516805', 'PB', 'Triunfo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2516904', 'PB', 'Uiraúna', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2517001', 'PB', 'Umbuzeiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2517100', 'PB', 'Várzea', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2517209', 'PB', 'Vieirópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2505501', 'PB', 'Vista Serrana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2517407', 'PB', 'Zabelê', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2600054', 'PE', 'Abreu e Lima', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2600104', 'PE', 'Afogados da Ingazeira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2600203', 'PE', 'Afrânio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2600302', 'PE', 'Agrestina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2600401', 'PE', 'Água Preta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2600500', 'PE', 'Águas Belas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2600609', 'PE', 'Alagoinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2600708', 'PE', 'Aliança', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2600807', 'PE', 'Altinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2600906', 'PE', 'Amaraji', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2601003', 'PE', 'Angelim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2601052', 'PE', 'Araçoiaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2601102', 'PE', 'Araripina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2601201', 'PE', 'Arcoverde', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2601300', 'PE', 'Barra de Guabiraba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2601409', 'PE', 'Barreiros', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2601508', 'PE', 'Belém de Maria', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2601607', 'PE', 'Belém do São Francisco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2601706', 'PE', 'Belo Jardim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2601805', 'PE', 'Betânia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2601904', 'PE', 'Bezerros', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2602001', 'PE', 'Bodocó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2602100', 'PE', 'Bom Conselho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2602209', 'PE', 'Bom Jardim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2602308', 'PE', 'Bonito', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2602407', 'PE', 'Brejão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2602506', 'PE', 'Brejinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2602605', 'PE', 'Brejo da Madre de Deus', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2602704', 'PE', 'Buenos Aires', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2602803', 'PE', 'Buíque', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2602902', 'PE', 'Cabo de Santo Agostinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2603009', 'PE', 'Cabrobó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2603108', 'PE', 'Cachoeirinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2603207', 'PE', 'Caetés', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2603306', 'PE', 'Calçado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2603405', 'PE', 'Calumbi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2603454', 'PE', 'Camaragibe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2603504', 'PE', 'Camocim de São Félix', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2603603', 'PE', 'Camutanga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2603702', 'PE', 'Canhotinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2603801', 'PE', 'Capoeiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2603900', 'PE', 'Carnaíba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2603926', 'PE', 'Carnaubeira da Penha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2604007', 'PE', 'Carpina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2604106', 'PE', 'Caruaru', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2604155', 'PE', 'Casinhas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2604205', 'PE', 'Catende', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2604304', 'PE', 'Cedro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2604403', 'PE', 'Chã de Alegria', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2604502', 'PE', 'Chã Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2604601', 'PE', 'Condado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2604700', 'PE', 'Correntes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2604809', 'PE', 'Cortês', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2604908', 'PE', 'Cumaru', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2605004', 'PE', 'Cupira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2605103', 'PE', 'Custódia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2605152', 'PE', 'Dormentes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2605202', 'PE', 'Escada', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2605301', 'PE', 'Exu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2605400', 'PE', 'Feira Nova', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2605459', 'PE', 'Fernando de Noronha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2605509', 'PE', 'Ferreiros', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2605608', 'PE', 'Flores', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2605707', 'PE', 'Floresta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2605806', 'PE', 'Frei Miguelinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2605905', 'PE', 'Gameleira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2606002', 'PE', 'Garanhuns', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2606101', 'PE', 'Glória do Goitá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2606200', 'PE', 'Goiana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2606309', 'PE', 'Granito', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2606408', 'PE', 'Gravatá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2606507', 'PE', 'Iati', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2606606', 'PE', 'Ibimirim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2606705', 'PE', 'Ibirajuba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2606804', 'PE', 'Igarassu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2606903', 'PE', 'Iguaraci', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2607604', 'PE', 'Ilha de Itamaracá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2607000', 'PE', 'Inajá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2607109', 'PE', 'Ingazeira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2607208', 'PE', 'Ipojuca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2607307', 'PE', 'Ipubi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2607406', 'PE', 'Itacuruba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2607505', 'PE', 'Itaíba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2607653', 'PE', 'Itambé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2607703', 'PE', 'Itapetim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2607752', 'PE', 'Itapissuma', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2607802', 'PE', 'Itaquitinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2607901', 'PE', 'Jaboatão dos Guararapes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2607950', 'PE', 'Jaqueira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2608008', 'PE', 'Jataúba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2608057', 'PE', 'Jatobá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2608107', 'PE', 'João Alfredo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2608206', 'PE', 'Joaquim Nabuco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2608255', 'PE', 'Jucati', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2608305', 'PE', 'Jupi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2608404', 'PE', 'Jurema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2608453', 'PE', 'Lagoa do Carro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2608503', 'PE', 'Lagoa de Itaenga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2608602', 'PE', 'Lagoa do Ouro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2608701', 'PE', 'Lagoa dos Gatos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2608750', 'PE', 'Lagoa Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2608800', 'PE', 'Lajedo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2608909', 'PE', 'Limoeiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2609006', 'PE', 'Macaparana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2609105', 'PE', 'Machados', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2609154', 'PE', 'Manari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2609204', 'PE', 'Maraial', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2609303', 'PE', 'Mirandiba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2614303', 'PE', 'Moreilândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2609402', 'PE', 'Moreno', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2609501', 'PE', 'Nazaré da Mata', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2609600', 'PE', 'Olinda', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2609709', 'PE', 'Orobó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2609808', 'PE', 'Orocó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2609907', 'PE', 'Ouricuri', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2610004', 'PE', 'Palmares', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2610103', 'PE', 'Palmeirina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2610202', 'PE', 'Panelas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2610301', 'PE', 'Paranatama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2610400', 'PE', 'Parnamirim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2610509', 'PE', 'Passira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2610608', 'PE', 'Paudalho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2610707', 'PE', 'Paulista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2610806', 'PE', 'Pedra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2610905', 'PE', 'Pesqueira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2611002', 'PE', 'Petrolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2611101', 'PE', 'Petrolina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2611200', 'PE', 'Poção', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2611309', 'PE', 'Pombos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2611408', 'PE', 'Primavera', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2611507', 'PE', 'Quipapá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2611533', 'PE', 'Quixaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2611606', 'PE', 'Recife', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2611705', 'PE', 'Riacho das Almas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2611804', 'PE', 'Ribeirão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2611903', 'PE', 'Rio Formoso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2612000', 'PE', 'Sairé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2612109', 'PE', 'Salgadinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2612208', 'PE', 'Salgueiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2612307', 'PE', 'Saloá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2612406', 'PE', 'Sanharó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2612455', 'PE', 'Santa Cruz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2612471', 'PE', 'Santa Cruz da Baixa Verde', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2612505', 'PE', 'Santa Cruz do Capibaribe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2612554', 'PE', 'Santa Filomena', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2612604', 'PE', 'Santa Maria da Boa Vista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2612703', 'PE', 'Santa Maria do Cambucá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2612802', 'PE', 'Santa Terezinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2612901', 'PE', 'São Benedito do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2613008', 'PE', 'São Bento do Una', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2613107', 'PE', 'São Caitano', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2613206', 'PE', 'São João', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2613305', 'PE', 'São Joaquim do Monte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2613404', 'PE', 'São José da Coroa Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2613503', 'PE', 'São José do Belmonte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2613602', 'PE', 'São José do Egito', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2613701', 'PE', 'São Lourenço da Mata', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2613800', 'PE', 'São Vicente Ferrer', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2613909', 'PE', 'Serra Talhada', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2614006', 'PE', 'Serrita', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2614105', 'PE', 'Sertânia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2614204', 'PE', 'Sirinhaém', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2614402', 'PE', 'Solidão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2614501', 'PE', 'Surubim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2614600', 'PE', 'Tabira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2614709', 'PE', 'Tacaimbó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2614808', 'PE', 'Tacaratu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2614857', 'PE', 'Tamandaré', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2615003', 'PE', 'Taquaritinga do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2615102', 'PE', 'Terezinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2615201', 'PE', 'Terra Nova', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2615300', 'PE', 'Timbaúba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2615409', 'PE', 'Toritama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2615508', 'PE', 'Tracunhaém', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2615607', 'PE', 'Trindade', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2615706', 'PE', 'Triunfo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2615805', 'PE', 'Tupanatinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2615904', 'PE', 'Tuparetama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2616001', 'PE', 'Venturosa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2616100', 'PE', 'Verdejante', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2616183', 'PE', 'Vertente do Lério', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2616209', 'PE', 'Vertentes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2616308', 'PE', 'Vicência', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2616407', 'PE', 'Vitória de Santo Antão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2616506', 'PE', 'Xexéu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2700102', 'AL', 'Água Branca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2700201', 'AL', 'Anadia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2700300', 'AL', 'Arapiraca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2700409', 'AL', 'Atalaia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2700508', 'AL', 'Barra de Santo Antônio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2700607', 'AL', 'Barra de São Miguel', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2700706', 'AL', 'Batalha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2700805', 'AL', 'Belém', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2700904', 'AL', 'Belo Monte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2701001', 'AL', 'Boca da Mata', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2701100', 'AL', 'Branquinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2701209', 'AL', 'Cacimbinhas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2701308', 'AL', 'Cajueiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2701357', 'AL', 'Campestre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2701407', 'AL', 'Campo Alegre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2701506', 'AL', 'Campo Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2701605', 'AL', 'Canapi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2701704', 'AL', 'Capela', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2701803', 'AL', 'Carneiros', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2701902', 'AL', 'Chã Preta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2702009', 'AL', 'Coité do Nóia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2702108', 'AL', 'Colônia Leopoldina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2702207', 'AL', 'Coqueiro Seco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2702306', 'AL', 'Coruripe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2702355', 'AL', 'Craíbas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2702405', 'AL', 'Delmiro Gouveia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2702504', 'AL', 'Dois Riachos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2702553', 'AL', 'Estrela de Alagoas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2702603', 'AL', 'Feira Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2702702', 'AL', 'Feliz Deserto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2702801', 'AL', 'Flexeiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2702900', 'AL', 'Girau do Ponciano', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2703007', 'AL', 'Ibateguara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2703106', 'AL', 'Igaci', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2703205', 'AL', 'Igreja Nova', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2703304', 'AL', 'Inhapi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2703403', 'AL', 'Jacaré dos Homens', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2703502', 'AL', 'Jacuípe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2703601', 'AL', 'Japaratinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2703700', 'AL', 'Jaramataia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2703759', 'AL', 'Jequiá da Praia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2703809', 'AL', 'Joaquim Gomes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2703908', 'AL', 'Jundiá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2704005', 'AL', 'Junqueiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2704104', 'AL', 'Lagoa da Canoa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2704203', 'AL', 'Limoeiro de Anadia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2704302', 'AL', 'Maceió', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2704401', 'AL', 'Major Isidoro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2704906', 'AL', 'Mar Vermelho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2704500', 'AL', 'Maragogi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2704609', 'AL', 'Maravilha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2704708', 'AL', 'Marechal Deodoro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2704807', 'AL', 'Maribondo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2705002', 'AL', 'Mata Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2705101', 'AL', 'Matriz de Camaragibe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2705200', 'AL', 'Messias', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2705309', 'AL', 'Minador do Negrão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2705408', 'AL', 'Monteirópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2705507', 'AL', 'Murici', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2705606', 'AL', 'Novo Lino', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2705705', 'AL', 'Olho d''Água das Flores', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2705804', 'AL', 'Olho d''Água do Casado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2705903', 'AL', 'Olho d''Água Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2706000', 'AL', 'Olivença', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2706109', 'AL', 'Ouro Branco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2706208', 'AL', 'Palestina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2706307', 'AL', 'Palmeira dos Índios', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2706406', 'AL', 'Pão de Açúcar', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2706422', 'AL', 'Pariconha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2706448', 'AL', 'Paripueira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2706505', 'AL', 'Passo de Camaragibe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2706604', 'AL', 'Paulo Jacinto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2706703', 'AL', 'Penedo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2706802', 'AL', 'Piaçabuçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2706901', 'AL', 'Pilar', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2707008', 'AL', 'Pindoba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2707107', 'AL', 'Piranhas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2707206', 'AL', 'Poço das Trincheiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2707305', 'AL', 'Porto Calvo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2707404', 'AL', 'Porto de Pedras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2707503', 'AL', 'Porto Real do Colégio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2707602', 'AL', 'Quebrangulo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2707701', 'AL', 'Rio Largo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2707800', 'AL', 'Roteiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2707909', 'AL', 'Santa Luzia do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2708006', 'AL', 'Santana do Ipanema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2708105', 'AL', 'Santana do Mundaú', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2708204', 'AL', 'São Brás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2708303', 'AL', 'São José da Laje', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2708402', 'AL', 'São José da Tapera', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2708501', 'AL', 'São Luís do Quitunde', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2708600', 'AL', 'São Miguel dos Campos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2708709', 'AL', 'São Miguel dos Milagres', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2708808', 'AL', 'São Sebastião', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2708907', 'AL', 'Satuba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2708956', 'AL', 'Senador Rui Palmeira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2709004', 'AL', 'Tanque d''Arca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2709103', 'AL', 'Taquarana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2709152', 'AL', 'Teotônio Vilela', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2709202', 'AL', 'Traipu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2709301', 'AL', 'União dos Palmares', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2709400', 'AL', 'Viçosa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2800100', 'SE', 'Amparo de São Francisco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2800209', 'SE', 'Aquidabã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2800308', 'SE', 'Aracaju', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2800407', 'SE', 'Arauá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2800506', 'SE', 'Areia Branca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2800605', 'SE', 'Barra dos Coqueiros', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2800670', 'SE', 'Boquim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2800704', 'SE', 'Brejo Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2801009', 'SE', 'Campo do Brito', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2801108', 'SE', 'Canhoba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2801207', 'SE', 'Canindé de São Francisco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2801306', 'SE', 'Capela', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2801405', 'SE', 'Carira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2801504', 'SE', 'Carmópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2801603', 'SE', 'Cedro de São João', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2801702', 'SE', 'Cristinápolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2801900', 'SE', 'Cumbe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2802007', 'SE', 'Divina Pastora', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2802106', 'SE', 'Estância', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2802205', 'SE', 'Feira Nova', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2802304', 'SE', 'Frei Paulo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2802403', 'SE', 'Gararu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2802502', 'SE', 'General Maynard', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2802601', 'SE', 'Gracho Cardoso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2802700', 'SE', 'Ilha das Flores', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2802809', 'SE', 'Indiaroba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2802908', 'SE', 'Itabaiana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2803005', 'SE', 'Itabaianinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2803104', 'SE', 'Itabi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2803203', 'SE', 'Itaporanga d''Ajuda', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2803302', 'SE', 'Japaratuba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2803401', 'SE', 'Japoatã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2803500', 'SE', 'Lagarto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2803609', 'SE', 'Laranjeiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2803708', 'SE', 'Macambira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2803807', 'SE', 'Malhada dos Bois', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2803906', 'SE', 'Malhador', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2804003', 'SE', 'Maruim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2804102', 'SE', 'Moita Bonita', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2804201', 'SE', 'Monte Alegre de Sergipe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2804300', 'SE', 'Muribeca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2804409', 'SE', 'Neópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2804458', 'SE', 'Nossa Senhora Aparecida', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2804508', 'SE', 'Nossa Senhora da Glória', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2804607', 'SE', 'Nossa Senhora das Dores', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2804706', 'SE', 'Nossa Senhora de Lourdes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2804805', 'SE', 'Nossa Senhora do Socorro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2804904', 'SE', 'Pacatuba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2805000', 'SE', 'Pedra Mole', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2805109', 'SE', 'Pedrinhas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2805208', 'SE', 'Pinhão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2805307', 'SE', 'Pirambu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2805406', 'SE', 'Poço Redondo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2805505', 'SE', 'Poço Verde', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2805604', 'SE', 'Porto da Folha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2805703', 'SE', 'Propriá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2805802', 'SE', 'Riachão do Dantas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2805901', 'SE', 'Riachuelo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2806008', 'SE', 'Ribeirópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2806107', 'SE', 'Rosário do Catete', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2806206', 'SE', 'Salgado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2806305', 'SE', 'Santa Luzia do Itanhy', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2806503', 'SE', 'Santa Rosa de Lima', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2806404', 'SE', 'Santana do São Francisco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2806602', 'SE', 'Santo Amaro das Brotas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2806701', 'SE', 'São Cristóvão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2806800', 'SE', 'São Domingos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2806909', 'SE', 'São Francisco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2807006', 'SE', 'São Miguel do Aleixo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2807105', 'SE', 'Simão Dias', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2807204', 'SE', 'Siriri', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2807303', 'SE', 'Telha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2807402', 'SE', 'Tobias Barreto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2807501', 'SE', 'Tomar do Geru', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2807600', 'SE', 'Umbaúba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2900108', 'BA', 'Abaíra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2900207', 'BA', 'Abaré', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2900306', 'BA', 'Acajutiba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2900355', 'BA', 'Adustina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2900405', 'BA', 'Água Fria', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2900603', 'BA', 'Aiquara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2900702', 'BA', 'Alagoinhas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2900801', 'BA', 'Alcobaça', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2900900', 'BA', 'Almadina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2901007', 'BA', 'Amargosa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2901106', 'BA', 'Amélia Rodrigues', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2901155', 'BA', 'América Dourada', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2901205', 'BA', 'Anagé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2901304', 'BA', 'Andaraí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2901353', 'BA', 'Andorinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2901403', 'BA', 'Angical', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2901502', 'BA', 'Anguera', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2901601', 'BA', 'Antas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2901700', 'BA', 'Antônio Cardoso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2901809', 'BA', 'Antônio Gonçalves', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2901908', 'BA', 'Aporá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2901957', 'BA', 'Apuarema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2902054', 'BA', 'Araças', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2902005', 'BA', 'Aracatu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2902104', 'BA', 'Araci', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2902203', 'BA', 'Aramari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2902252', 'BA', 'Arataca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2902302', 'BA', 'Aratuípe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2902401', 'BA', 'Aurelino Leal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2902500', 'BA', 'Baianópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2902609', 'BA', 'Baixa Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2902658', 'BA', 'Banzaê', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2902708', 'BA', 'Barra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2902807', 'BA', 'Barra da Estiva', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2902906', 'BA', 'Barra do Choça', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2903003', 'BA', 'Barra do Mendes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2903102', 'BA', 'Barra do Rocha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2903201', 'BA', 'Barreiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2903235', 'BA', 'Barro Alto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2903300', 'BA', 'Barro Preto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2903276', 'BA', 'Barrocas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2903409', 'BA', 'Belmonte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2903508', 'BA', 'Belo Campo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2903607', 'BA', 'Biritinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2903706', 'BA', 'Boa Nova', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2903805', 'BA', 'Boa Vista do Tupim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2903904', 'BA', 'Bom Jesus da Lapa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2903953', 'BA', 'Bom Jesus da Serra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2904001', 'BA', 'Boninal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2904050', 'BA', 'Bonito', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2904100', 'BA', 'Boquira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2904209', 'BA', 'Botuporã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2904308', 'BA', 'Brejões', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2904407', 'BA', 'Brejolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2904506', 'BA', 'Brotas de Macaúbas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2904605', 'BA', 'Brumado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2904704', 'BA', 'Buerarema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2904753', 'BA', 'Buritirama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2904803', 'BA', 'Caatiba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2904852', 'BA', 'Cabaceiras do Paraguaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2904902', 'BA', 'Cachoeira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2905008', 'BA', 'Caculé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2905107', 'BA', 'Caém', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2905156', 'BA', 'Caetanos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2905206', 'BA', 'Caetité', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2905305', 'BA', 'Cafarnaum', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2905404', 'BA', 'Cairu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2905503', 'BA', 'Caldeirão Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2905602', 'BA', 'Camacan', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2905701', 'BA', 'Camaçari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2905800', 'BA', 'Camamu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2905909', 'BA', 'Campo Alegre de Lourdes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2906006', 'BA', 'Campo Formoso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2906105', 'BA', 'Canápolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2906204', 'BA', 'Canarana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2906303', 'BA', 'Canavieiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2906402', 'BA', 'Candeal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2906501', 'BA', 'Candeias', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2906600', 'BA', 'Candiba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2906709', 'BA', 'Cândido Sales', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2906808', 'BA', 'Cansanção', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2906824', 'BA', 'Canudos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2906857', 'BA', 'Capela do Alto Alegre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2906873', 'BA', 'Capim Grosso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2906899', 'BA', 'Caraíbas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2906907', 'BA', 'Caravelas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2907004', 'BA', 'Cardeal da Silva', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2907103', 'BA', 'Carinhanha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2907202', 'BA', 'Casa Nova', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2907301', 'BA', 'Castro Alves', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2907400', 'BA', 'Catolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2907509', 'BA', 'Catu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2907558', 'BA', 'Caturama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2907608', 'BA', 'Central', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2907707', 'BA', 'Chorrochó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2907806', 'BA', 'Cícero Dantas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2907905', 'BA', 'Cipó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2908002', 'BA', 'Coaraci', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2908101', 'BA', 'Cocos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2908200', 'BA', 'Conceição da Feira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2908309', 'BA', 'Conceição do Almeida', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2908408', 'BA', 'Conceição do Coité', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2908507', 'BA', 'Conceição do Jacuípe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2908606', 'BA', 'Conde', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2908705', 'BA', 'Condeúba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2908804', 'BA', 'Contendas do Sincorá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2908903', 'BA', 'Coração de Maria', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2909000', 'BA', 'Cordeiros', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2909109', 'BA', 'Coribe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2909208', 'BA', 'Coronel João Sá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2909307', 'BA', 'Correntina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2909406', 'BA', 'Cotegipe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2909505', 'BA', 'Cravolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2909604', 'BA', 'Crisópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2909703', 'BA', 'Cristópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2909802', 'BA', 'Cruz das Almas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2909901', 'BA', 'Curaçá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2910008', 'BA', 'Dário Meira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2910057', 'BA', 'Dias d''Ávila', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2910107', 'BA', 'Dom Basílio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2910206', 'BA', 'Dom Macedo Costa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2910305', 'BA', 'Elísio Medrado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2910404', 'BA', 'Encruzilhada', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2910503', 'BA', 'Entre Rios', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2900504', 'BA', 'Érico Cardoso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2910602', 'BA', 'Esplanada', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2910701', 'BA', 'Euclides da Cunha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2910727', 'BA', 'Eunápolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2910750', 'BA', 'Fátima', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2910776', 'BA', 'Feira da Mata', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2910800', 'BA', 'Feira de Santana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2910859', 'BA', 'Filadélfia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2910909', 'BA', 'Firmino Alves', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2911006', 'BA', 'Floresta Azul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2911105', 'BA', 'Formosa do Rio Preto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2911204', 'BA', 'Gandu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2911253', 'BA', 'Gavião', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2911303', 'BA', 'Gentio do Ouro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2911402', 'BA', 'Glória', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2911501', 'BA', 'Gongogi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2911600', 'BA', 'Governador Mangabeira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2911659', 'BA', 'Guajeru', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2911709', 'BA', 'Guanambi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2911808', 'BA', 'Guaratinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2911857', 'BA', 'Heliópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2911907', 'BA', 'Iaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2912004', 'BA', 'Ibiassucê', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2912103', 'BA', 'Ibicaraí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2912202', 'BA', 'Ibicoara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2912301', 'BA', 'Ibicuí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2912400', 'BA', 'Ibipeba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2912509', 'BA', 'Ibipitanga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2912608', 'BA', 'Ibiquera', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2912707', 'BA', 'Ibirapitanga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2912806', 'BA', 'Ibirapuã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2912905', 'BA', 'Ibirataia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2913002', 'BA', 'Ibitiara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2913101', 'BA', 'Ibititá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2913200', 'BA', 'Ibotirama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2913309', 'BA', 'Ichu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2913408', 'BA', 'Igaporã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2913457', 'BA', 'Igrapiúna', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2913507', 'BA', 'Iguaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2913606', 'BA', 'Ilhéus', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2913705', 'BA', 'Inhambupe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2913804', 'BA', 'Ipecaetá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2913903', 'BA', 'Ipiaú', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2914000', 'BA', 'Ipirá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2914109', 'BA', 'Ipupiara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2914208', 'BA', 'Irajuba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2914307', 'BA', 'Iramaia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2914406', 'BA', 'Iraquara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2914505', 'BA', 'Irará', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2914604', 'BA', 'Irecê', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2914653', 'BA', 'Itabela', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2914703', 'BA', 'Itaberaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2914802', 'BA', 'Itabuna', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2914901', 'BA', 'Itacaré', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2915007', 'BA', 'Itaeté', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2915106', 'BA', 'Itagi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2915205', 'BA', 'Itagibá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2915304', 'BA', 'Itagimirim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2915353', 'BA', 'Itaguaçu da Bahia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2915403', 'BA', 'Itaju do Colônia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2915502', 'BA', 'Itajuípe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2915601', 'BA', 'Itamaraju', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2915700', 'BA', 'Itamari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2915809', 'BA', 'Itambé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2915908', 'BA', 'Itanagra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2916005', 'BA', 'Itanhém', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2916104', 'BA', 'Itaparica', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2916203', 'BA', 'Itapé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2916302', 'BA', 'Itapebi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2916401', 'BA', 'Itapetinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2916500', 'BA', 'Itapicuru', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2916609', 'BA', 'Itapitanga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2916708', 'BA', 'Itaquara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2916807', 'BA', 'Itarantim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2916856', 'BA', 'Itatim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2916906', 'BA', 'Itiruçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2917003', 'BA', 'Itiúba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2917102', 'BA', 'Itororó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2917201', 'BA', 'Ituaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2917300', 'BA', 'Ituberá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2917334', 'BA', 'Iuiú', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2917359', 'BA', 'Jaborandi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2917409', 'BA', 'Jacaraci', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2917508', 'BA', 'Jacobina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2917607', 'BA', 'Jaguaquara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2917706', 'BA', 'Jaguarari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2917805', 'BA', 'Jaguaripe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2917904', 'BA', 'Jandaíra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2918001', 'BA', 'Jequié', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2918100', 'BA', 'Jeremoabo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2918209', 'BA', 'Jiquiriçá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2918308', 'BA', 'Jitaúna', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2918357', 'BA', 'João Dourado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2918407', 'BA', 'Juazeiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2918456', 'BA', 'Jucuruçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2918506', 'BA', 'Jussara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2918555', 'BA', 'Jussari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2918605', 'BA', 'Jussiape', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2918704', 'BA', 'Lafaiete Coutinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2918753', 'BA', 'Lagoa Real', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2918803', 'BA', 'Laje', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2918902', 'BA', 'Lajedão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2919009', 'BA', 'Lajedinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2919058', 'BA', 'Lajedo do Tabocal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2919108', 'BA', 'Lamarão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2919157', 'BA', 'Lapão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2919207', 'BA', 'Lauro de Freitas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2919306', 'BA', 'Lençóis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2919405', 'BA', 'Licínio de Almeida', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2919504', 'BA', 'Livramento de Nossa Senhora', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2919553', 'BA', 'Luís Eduardo Magalhães', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2919603', 'BA', 'Macajuba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2919702', 'BA', 'Macarani', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2919801', 'BA', 'Macaúbas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2919900', 'BA', 'Macururé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2919926', 'BA', 'Madre de Deus', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2919959', 'BA', 'Maetinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2920007', 'BA', 'Maiquinique', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2920106', 'BA', 'Mairi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2920205', 'BA', 'Malhada', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2920304', 'BA', 'Malhada de Pedras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2920403', 'BA', 'Manoel Vitorino', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2920452', 'BA', 'Mansidão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2920502', 'BA', 'Maracás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2920601', 'BA', 'Maragogipe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2920700', 'BA', 'Maraú', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2920809', 'BA', 'Marcionílio Souza', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2920908', 'BA', 'Mascote', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2921005', 'BA', 'Mata de São João', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2921054', 'BA', 'Matina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2921104', 'BA', 'Medeiros Neto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2921203', 'BA', 'Miguel Calmon', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2921302', 'BA', 'Milagres', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2921401', 'BA', 'Mirangaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2921450', 'BA', 'Mirante', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2921500', 'BA', 'Monte Santo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2921609', 'BA', 'Morpará', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2921708', 'BA', 'Morro do Chapéu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2921807', 'BA', 'Mortugaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2921906', 'BA', 'Mucugê', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2922003', 'BA', 'Mucuri', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2922052', 'BA', 'Mulungu do Morro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2922102', 'BA', 'Mundo Novo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2922201', 'BA', 'Muniz Ferreira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2922250', 'BA', 'Muquém de São Francisco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2922300', 'BA', 'Muritiba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2922409', 'BA', 'Mutuípe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2922508', 'BA', 'Nazaré', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2922607', 'BA', 'Nilo Peçanha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2922656', 'BA', 'Nordestina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2922706', 'BA', 'Nova Canaã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2922730', 'BA', 'Nova Fátima', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2922755', 'BA', 'Nova Ibiá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2922805', 'BA', 'Nova Itarana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2922854', 'BA', 'Nova Redenção', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2922904', 'BA', 'Nova Soure', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2923001', 'BA', 'Nova Viçosa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2923035', 'BA', 'Novo Horizonte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2923050', 'BA', 'Novo Triunfo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2923100', 'BA', 'Olindina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2923209', 'BA', 'Oliveira dos Brejinhos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2923308', 'BA', 'Ouriçangas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2923357', 'BA', 'Ourolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2923407', 'BA', 'Palmas de Monte Alto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2923506', 'BA', 'Palmeiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2923605', 'BA', 'Paramirim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2923704', 'BA', 'Paratinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2923803', 'BA', 'Paripiranga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2923902', 'BA', 'Pau Brasil', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2924009', 'BA', 'Paulo Afonso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2924058', 'BA', 'Pé de Serra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2924108', 'BA', 'Pedrão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2924207', 'BA', 'Pedro Alexandre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2924306', 'BA', 'Piatã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2924405', 'BA', 'Pilão Arcado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2924504', 'BA', 'Pindaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2924603', 'BA', 'Pindobaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2924652', 'BA', 'Pintadas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2924678', 'BA', 'Piraí do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2924702', 'BA', 'Piripá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2924801', 'BA', 'Piritiba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2924900', 'BA', 'Planaltino', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2925006', 'BA', 'Planalto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2925105', 'BA', 'Poções', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2925204', 'BA', 'Pojuca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2925253', 'BA', 'Ponto Novo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2925303', 'BA', 'Porto Seguro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2925402', 'BA', 'Potiraguá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2925501', 'BA', 'Prado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2925600', 'BA', 'Presidente Dutra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2925709', 'BA', 'Presidente Jânio Quadros', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2925758', 'BA', 'Presidente Tancredo Neves', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2925808', 'BA', 'Queimadas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2925907', 'BA', 'Quijingue', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2925931', 'BA', 'Quixabeira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2925956', 'BA', 'Rafael Jambeiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2926004', 'BA', 'Remanso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2926103', 'BA', 'Retirolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2926202', 'BA', 'Riachão das Neves', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2926301', 'BA', 'Riachão do Jacuípe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2926400', 'BA', 'Riacho de Santana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2926509', 'BA', 'Ribeira do Amparo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2926608', 'BA', 'Ribeira do Pombal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2926657', 'BA', 'Ribeirão do Largo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2926707', 'BA', 'Rio de Contas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2926806', 'BA', 'Rio do Antônio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2926905', 'BA', 'Rio do Pires', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2927002', 'BA', 'Rio Real', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2927101', 'BA', 'Rodelas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2927200', 'BA', 'Ruy Barbosa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2927309', 'BA', 'Salinas da Margarida', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2927408', 'BA', 'Salvador', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2927507', 'BA', 'Santa Bárbara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2927606', 'BA', 'Santa Brígida', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2927705', 'BA', 'Santa Cruz Cabrália', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2927804', 'BA', 'Santa Cruz da Vitória', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2927903', 'BA', 'Santa Inês', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2928059', 'BA', 'Santa Luzia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2928109', 'BA', 'Santa Maria da Vitória', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2928406', 'BA', 'Santa Rita de Cássia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2928505', 'BA', 'Santa Teresinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2928000', 'BA', 'Santaluz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2928208', 'BA', 'Santana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2928307', 'BA', 'Santanópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2928604', 'BA', 'Santo Amaro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2928703', 'BA', 'Santo Antônio de Jesus', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2928802', 'BA', 'Santo Estêvão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2928901', 'BA', 'São Desidério', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2928950', 'BA', 'São Domingos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2929107', 'BA', 'São Felipe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2929008', 'BA', 'São Félix', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2929057', 'BA', 'São Félix do Coribe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2929206', 'BA', 'São Francisco do Conde', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2929255', 'BA', 'São Gabriel', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2929305', 'BA', 'São Gonçalo dos Campos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2929354', 'BA', 'São José da Vitória', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2929370', 'BA', 'São José do Jacuípe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2929404', 'BA', 'São Miguel das Matas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2929503', 'BA', 'São Sebastião do Passé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2929602', 'BA', 'Sapeaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2929701', 'BA', 'Sátiro Dias', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2929750', 'BA', 'Saubara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2929800', 'BA', 'Saúde', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2929909', 'BA', 'Seabra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2930006', 'BA', 'Sebastião Laranjeiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2930105', 'BA', 'Senhor do Bonfim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2930204', 'BA', 'Sento Sé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2930154', 'BA', 'Serra do Ramalho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2930303', 'BA', 'Serra Dourada', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2930402', 'BA', 'Serra Preta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2930501', 'BA', 'Serrinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2930600', 'BA', 'Serrolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2930709', 'BA', 'Simões Filho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2930758', 'BA', 'Sítio do Mato', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2930766', 'BA', 'Sítio do Quinto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2930774', 'BA', 'Sobradinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2930808', 'BA', 'Souto Soares', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2930907', 'BA', 'Tabocas do Brejo Velho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2931004', 'BA', 'Tanhaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2931053', 'BA', 'Tanque Novo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2931103', 'BA', 'Tanquinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2931202', 'BA', 'Taperoá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2931301', 'BA', 'Tapiramutá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2931350', 'BA', 'Teixeira de Freitas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2931400', 'BA', 'Teodoro Sampaio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2931509', 'BA', 'Teofilândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2931608', 'BA', 'Teolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2931707', 'BA', 'Terra Nova', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2931806', 'BA', 'Tremedal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2931905', 'BA', 'Tucano', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2932002', 'BA', 'Uauá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2932101', 'BA', 'Ubaíra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2932200', 'BA', 'Ubaitaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2932309', 'BA', 'Ubatã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2932408', 'BA', 'Uibaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2932457', 'BA', 'Umburanas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2932507', 'BA', 'Una', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2932606', 'BA', 'Urandi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2932705', 'BA', 'Uruçuca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2932804', 'BA', 'Utinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2932903', 'BA', 'Valença', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2933000', 'BA', 'Valente', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2933059', 'BA', 'Várzea da Roça', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2933109', 'BA', 'Várzea do Poço', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2933158', 'BA', 'Várzea Nova', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2933174', 'BA', 'Varzedo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2933208', 'BA', 'Vera Cruz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2933257', 'BA', 'Vereda', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2933307', 'BA', 'Vitória da Conquista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2933406', 'BA', 'Wagner', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2933455', 'BA', 'Wanderley', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2933505', 'BA', 'Wenceslau Guimarães', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('2933604', 'BA', 'Xique-Xique', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3100104', 'MG', 'Abadia dos Dourados', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3100203', 'MG', 'Abaeté', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3100302', 'MG', 'Abre Campo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3100401', 'MG', 'Acaiaca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3100500', 'MG', 'Açucena', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3100609', 'MG', 'Água Boa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3100708', 'MG', 'Água Comprida', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3100807', 'MG', 'Aguanil', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3100906', 'MG', 'Águas Formosas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3101003', 'MG', 'Águas Vermelhas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3101102', 'MG', 'Aimorés', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3101201', 'MG', 'Aiuruoca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3101300', 'MG', 'Alagoa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3101409', 'MG', 'Albertina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3101508', 'MG', 'Além Paraíba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3101607', 'MG', 'Alfenas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3101631', 'MG', 'Alfredo Vasconcelos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3101706', 'MG', 'Almenara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3101805', 'MG', 'Alpercata', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3101904', 'MG', 'Alpinópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3102001', 'MG', 'Alterosa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3102050', 'MG', 'Alto Caparaó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3153509', 'MG', 'Alto Jequitibá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3102100', 'MG', 'Alto Rio Doce', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3102209', 'MG', 'Alvarenga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3102308', 'MG', 'Alvinópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3102407', 'MG', 'Alvorada de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3102506', 'MG', 'Amparo do Serra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3102605', 'MG', 'Andradas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3102803', 'MG', 'Andrelândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3102852', 'MG', 'Angelândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3102902', 'MG', 'Antônio Carlos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3103009', 'MG', 'Antônio Dias', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3103108', 'MG', 'Antônio Prado de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3103207', 'MG', 'Araçaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3103306', 'MG', 'Aracitaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3103405', 'MG', 'Araçuaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3103504', 'MG', 'Araguari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3103603', 'MG', 'Arantina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3103702', 'MG', 'Araponga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3103751', 'MG', 'Araporã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3103801', 'MG', 'Arapuá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3103900', 'MG', 'Araújos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3104007', 'MG', 'Araxá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3104106', 'MG', 'Arceburgo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3104205', 'MG', 'Arcos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3104304', 'MG', 'Areado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3104403', 'MG', 'Argirita', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3104452', 'MG', 'Aricanduva', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3104502', 'MG', 'Arinos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3104601', 'MG', 'Astolfo Dutra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3104700', 'MG', 'Ataléia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3104809', 'MG', 'Augusto de Lima', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3104908', 'MG', 'Baependi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3105004', 'MG', 'Baldim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3105103', 'MG', 'Bambuí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3105202', 'MG', 'Bandeira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3105301', 'MG', 'Bandeira do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3105400', 'MG', 'Barão de Cocais', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3105509', 'MG', 'Barão de Monte Alto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3105608', 'MG', 'Barbacena', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3105707', 'MG', 'Barra Longa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3105905', 'MG', 'Barroso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3106002', 'MG', 'Bela Vista de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3106101', 'MG', 'Belmiro Braga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3106200', 'MG', 'Belo Horizonte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3106309', 'MG', 'Belo Oriente', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3106408', 'MG', 'Belo Vale', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3106507', 'MG', 'Berilo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3106655', 'MG', 'Berizal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3106606', 'MG', 'Bertópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3106705', 'MG', 'Betim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3106804', 'MG', 'Bias Fortes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3106903', 'MG', 'Bicas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3107000', 'MG', 'Biquinhas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3107109', 'MG', 'Boa Esperança', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3107208', 'MG', 'Bocaina de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3107307', 'MG', 'Bocaiúva', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3107406', 'MG', 'Bom Despacho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3107505', 'MG', 'Bom Jardim de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3107604', 'MG', 'Bom Jesus da Penha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3107703', 'MG', 'Bom Jesus do Amparo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3107802', 'MG', 'Bom Jesus do Galho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3107901', 'MG', 'Bom Repouso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3108008', 'MG', 'Bom Sucesso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3108107', 'MG', 'Bonfim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3108206', 'MG', 'Bonfinópolis de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3108255', 'MG', 'Bonito de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3108305', 'MG', 'Borda da Mata', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3108404', 'MG', 'Botelhos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3108503', 'MG', 'Botumirim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3108701', 'MG', 'Brás Pires', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3108552', 'MG', 'Brasilândia de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3108602', 'MG', 'Brasília de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3108909', 'MG', 'Brasópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3108800', 'MG', 'Braúnas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3109006', 'MG', 'Brumadinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3109105', 'MG', 'Bueno Brandão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3109204', 'MG', 'Buenópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3109253', 'MG', 'Bugre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3109303', 'MG', 'Buritis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3109402', 'MG', 'Buritizeiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3109451', 'MG', 'Cabeceira Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3109501', 'MG', 'Cabo Verde', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3109600', 'MG', 'Cachoeira da Prata', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3109709', 'MG', 'Cachoeira de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3102704', 'MG', 'Cachoeira de Pajeú', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3109808', 'MG', 'Cachoeira Dourada', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3109907', 'MG', 'Caetanópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3110004', 'MG', 'Caeté', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3110103', 'MG', 'Caiana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3110202', 'MG', 'Cajuri', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3110301', 'MG', 'Caldas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3110400', 'MG', 'Camacho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3110509', 'MG', 'Camanducaia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3110608', 'MG', 'Cambuí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3110707', 'MG', 'Cambuquira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3110806', 'MG', 'Campanário', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3110905', 'MG', 'Campanha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3111002', 'MG', 'Campestre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3111101', 'MG', 'Campina Verde', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3111150', 'MG', 'Campo Azul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3111200', 'MG', 'Campo Belo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3111309', 'MG', 'Campo do Meio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3111408', 'MG', 'Campo Florido', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3111507', 'MG', 'Campos Altos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3111606', 'MG', 'Campos Gerais', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3111903', 'MG', 'Cana Verde', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3111705', 'MG', 'Canaã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3111804', 'MG', 'Canápolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3112000', 'MG', 'Candeias', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3112059', 'MG', 'Cantagalo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3112109', 'MG', 'Caparaó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3112208', 'MG', 'Capela Nova', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3112307', 'MG', 'Capelinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3112406', 'MG', 'Capetinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3112505', 'MG', 'Capim Branco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3112604', 'MG', 'Capinópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3112653', 'MG', 'Capitão Andrade', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3112703', 'MG', 'Capitão Enéas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3112802', 'MG', 'Capitólio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3112901', 'MG', 'Caputira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3113008', 'MG', 'Caraí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3113107', 'MG', 'Caranaíba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3113206', 'MG', 'Carandaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3113305', 'MG', 'Carangola', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3113404', 'MG', 'Caratinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3113503', 'MG', 'Carbonita', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3113602', 'MG', 'Careaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3113701', 'MG', 'Carlos Chagas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3113800', 'MG', 'Carmésia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3113909', 'MG', 'Carmo da Cachoeira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3114006', 'MG', 'Carmo da Mata', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3114105', 'MG', 'Carmo de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3114204', 'MG', 'Carmo do Cajuru', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3114303', 'MG', 'Carmo do Paranaíba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3114402', 'MG', 'Carmo do Rio Claro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3114501', 'MG', 'Carmópolis de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3114550', 'MG', 'Carneirinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3114600', 'MG', 'Carrancas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3114709', 'MG', 'Carvalhópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3114808', 'MG', 'Carvalhos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3114907', 'MG', 'Casa Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3115003', 'MG', 'Cascalho Rico', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3115102', 'MG', 'Cássia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3115300', 'MG', 'Cataguases', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3115359', 'MG', 'Catas Altas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3115409', 'MG', 'Catas Altas da Noruega', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3115458', 'MG', 'Catuji', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3115474', 'MG', 'Catuti', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3115508', 'MG', 'Caxambu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3115607', 'MG', 'Cedro do Abaeté', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3115706', 'MG', 'Central de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3115805', 'MG', 'Centralina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3115904', 'MG', 'Chácara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3116001', 'MG', 'Chalé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3116100', 'MG', 'Chapada do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3116159', 'MG', 'Chapada Gaúcha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3116209', 'MG', 'Chiador', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3116308', 'MG', 'Cipotânea', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3116407', 'MG', 'Claraval', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3116506', 'MG', 'Claro dos Poções', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3116605', 'MG', 'Cláudio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3116704', 'MG', 'Coimbra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3116803', 'MG', 'Coluna', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3116902', 'MG', 'Comendador Gomes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3117009', 'MG', 'Comercinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3117108', 'MG', 'Conceição da Aparecida', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3115201', 'MG', 'Conceição da Barra de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3117306', 'MG', 'Conceição das Alagoas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3117207', 'MG', 'Conceição das Pedras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3117405', 'MG', 'Conceição de Ipanema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3117504', 'MG', 'Conceição do Mato Dentro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3117603', 'MG', 'Conceição do Pará', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3117702', 'MG', 'Conceição do Rio Verde', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3117801', 'MG', 'Conceição dos Ouros', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3117836', 'MG', 'Cônego Marinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3117876', 'MG', 'Confins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3117900', 'MG', 'Congonhal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3118007', 'MG', 'Congonhas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3118106', 'MG', 'Congonhas do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3118205', 'MG', 'Conquista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3118304', 'MG', 'Conselheiro Lafaiete', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3118403', 'MG', 'Conselheiro Pena', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3118502', 'MG', 'Consolação', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3118601', 'MG', 'Contagem', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3118700', 'MG', 'Coqueiral', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3118809', 'MG', 'Coração de Jesus', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3118908', 'MG', 'Cordisburgo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3119005', 'MG', 'Cordislândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3119104', 'MG', 'Corinto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3119203', 'MG', 'Coroaci', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3119302', 'MG', 'Coromandel', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3119401', 'MG', 'Coronel Fabriciano', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3119500', 'MG', 'Coronel Murta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3119609', 'MG', 'Coronel Pacheco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3119708', 'MG', 'Coronel Xavier Chaves', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3119807', 'MG', 'Córrego Danta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3119906', 'MG', 'Córrego do Bom Jesus', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3119955', 'MG', 'Córrego Fundo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3120003', 'MG', 'Córrego Novo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3120102', 'MG', 'Couto de Magalhães de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3120151', 'MG', 'Crisólita', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3120201', 'MG', 'Cristais', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3120300', 'MG', 'Cristália', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3120409', 'MG', 'Cristiano Otoni', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3120508', 'MG', 'Cristina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3120607', 'MG', 'Crucilândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3120706', 'MG', 'Cruzeiro da Fortaleza', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3120805', 'MG', 'Cruzília', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3120839', 'MG', 'Cuparaque', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3120870', 'MG', 'Curral de Dentro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3120904', 'MG', 'Curvelo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3121001', 'MG', 'Datas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3121100', 'MG', 'Delfim Moreira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3121209', 'MG', 'Delfinópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3121258', 'MG', 'Delta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3121308', 'MG', 'Descoberto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3121407', 'MG', 'Desterro de Entre Rios', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3121506', 'MG', 'Desterro do Melo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3121605', 'MG', 'Diamantina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3121704', 'MG', 'Diogo de Vasconcelos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3121803', 'MG', 'Dionísio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3121902', 'MG', 'Divinésia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3122009', 'MG', 'Divino', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3122108', 'MG', 'Divino das Laranjeiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3122207', 'MG', 'Divinolândia de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3122306', 'MG', 'Divinópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3122355', 'MG', 'Divisa Alegre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3122405', 'MG', 'Divisa Nova', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3122454', 'MG', 'Divisópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3122470', 'MG', 'Dom Bosco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3122504', 'MG', 'Dom Cavati', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3122603', 'MG', 'Dom Joaquim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3122702', 'MG', 'Dom Silvério', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3122801', 'MG', 'Dom Viçoso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3122900', 'MG', 'Dona Eusébia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3123007', 'MG', 'Dores de Campos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3123106', 'MG', 'Dores de Guanhães', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3123205', 'MG', 'Dores do Indaiá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3123304', 'MG', 'Dores do Turvo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3123403', 'MG', 'Doresópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3123502', 'MG', 'Douradoquara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3123528', 'MG', 'Durandé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3123601', 'MG', 'Elói Mendes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3123700', 'MG', 'Engenheiro Caldas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3123809', 'MG', 'Engenheiro Navarro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3123858', 'MG', 'Entre Folhas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3123908', 'MG', 'Entre Rios de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3124005', 'MG', 'Ervália', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3124104', 'MG', 'Esmeraldas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3124203', 'MG', 'Espera Feliz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3124302', 'MG', 'Espinosa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3124401', 'MG', 'Espírito Santo do Dourado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3124500', 'MG', 'Estiva', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3124609', 'MG', 'Estrela Dalva', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3124708', 'MG', 'Estrela do Indaiá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3124807', 'MG', 'Estrela do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3124906', 'MG', 'Eugenópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3125002', 'MG', 'Ewbank da Câmara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3125101', 'MG', 'Extrema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3125200', 'MG', 'Fama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3125309', 'MG', 'Faria Lemos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3125408', 'MG', 'Felício dos Santos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3125606', 'MG', 'Felisburgo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3125705', 'MG', 'Felixlândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3125804', 'MG', 'Fernandes Tourinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3125903', 'MG', 'Ferros', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3125952', 'MG', 'Fervedouro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3126000', 'MG', 'Florestal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3126109', 'MG', 'Formiga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3126208', 'MG', 'Formoso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3126307', 'MG', 'Fortaleza de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3126406', 'MG', 'Fortuna de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3126505', 'MG', 'Francisco Badaró', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3126604', 'MG', 'Francisco Dumont', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3126703', 'MG', 'Francisco Sá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3126752', 'MG', 'Franciscópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3126802', 'MG', 'Frei Gaspar', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3126901', 'MG', 'Frei Inocêncio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3126950', 'MG', 'Frei Lagonegro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3127008', 'MG', 'Fronteira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3127057', 'MG', 'Fronteira dos Vales', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3127073', 'MG', 'Fruta de Leite', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3127107', 'MG', 'Frutal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3127206', 'MG', 'Funilândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3127305', 'MG', 'Galiléia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3127339', 'MG', 'Gameleiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3127354', 'MG', 'Glaucilândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3127370', 'MG', 'Goiabeira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3127388', 'MG', 'Goianá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3127404', 'MG', 'Gonçalves', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3127503', 'MG', 'Gonzaga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3127602', 'MG', 'Gouveia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3127701', 'MG', 'Governador Valadares', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3127800', 'MG', 'Grão Mogol', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3127909', 'MG', 'Grupiara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3128006', 'MG', 'Guanhães', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3128105', 'MG', 'Guapé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3128204', 'MG', 'Guaraciaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3128253', 'MG', 'Guaraciama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3128303', 'MG', 'Guaranésia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3128402', 'MG', 'Guarani', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3128501', 'MG', 'Guarará', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3128600', 'MG', 'Guarda-Mor', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3128709', 'MG', 'Guaxupé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3128808', 'MG', 'Guidoval', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3128907', 'MG', 'Guimarânia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3129004', 'MG', 'Guiricema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3129103', 'MG', 'Gurinhatã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3129202', 'MG', 'Heliodora', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3129301', 'MG', 'Iapu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3129400', 'MG', 'Ibertioga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3129509', 'MG', 'Ibiá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3129608', 'MG', 'Ibiaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3129657', 'MG', 'Ibiracatu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3129707', 'MG', 'Ibiraci', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3129806', 'MG', 'Ibirité', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3129905', 'MG', 'Ibitiúra de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3130002', 'MG', 'Ibituruna', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3130051', 'MG', 'Icaraí de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3130101', 'MG', 'Igarapé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3130200', 'MG', 'Igaratinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3130309', 'MG', 'Iguatama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3130408', 'MG', 'Ijaci', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3130507', 'MG', 'Ilicínea', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3130556', 'MG', 'Imbé de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3130606', 'MG', 'Inconfidentes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3130655', 'MG', 'Indaiabira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3130705', 'MG', 'Indianópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3130804', 'MG', 'Ingaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3130903', 'MG', 'Inhapim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3131000', 'MG', 'Inhaúma', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3131109', 'MG', 'Inimutaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3131158', 'MG', 'Ipaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3131208', 'MG', 'Ipanema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3131307', 'MG', 'Ipatinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3131406', 'MG', 'Ipiaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3131505', 'MG', 'Ipuiúna', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3131604', 'MG', 'Iraí de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3131703', 'MG', 'Itabira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3131802', 'MG', 'Itabirinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3131901', 'MG', 'Itabirito', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3132008', 'MG', 'Itacambira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3132107', 'MG', 'Itacarambi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3132206', 'MG', 'Itaguara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3132305', 'MG', 'Itaipé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3132404', 'MG', 'Itajubá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3132503', 'MG', 'Itamarandiba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3132602', 'MG', 'Itamarati de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3132701', 'MG', 'Itambacuri', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3132800', 'MG', 'Itambé do Mato Dentro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3132909', 'MG', 'Itamogi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3133006', 'MG', 'Itamonte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3133105', 'MG', 'Itanhandu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3133204', 'MG', 'Itanhomi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3133303', 'MG', 'Itaobim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3133402', 'MG', 'Itapagipe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3133501', 'MG', 'Itapecerica', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3133600', 'MG', 'Itapeva', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3133709', 'MG', 'Itatiaiuçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3133758', 'MG', 'Itaú de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3133808', 'MG', 'Itaúna', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3133907', 'MG', 'Itaverava', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3134004', 'MG', 'Itinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3134103', 'MG', 'Itueta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3134202', 'MG', 'Ituiutaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3134301', 'MG', 'Itumirim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3134400', 'MG', 'Iturama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3134509', 'MG', 'Itutinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3134608', 'MG', 'Jaboticatubas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3134707', 'MG', 'Jacinto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3134806', 'MG', 'Jacuí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3134905', 'MG', 'Jacutinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3135001', 'MG', 'Jaguaraçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3135050', 'MG', 'Jaíba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3135076', 'MG', 'Jampruca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3135100', 'MG', 'Janaúba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3135209', 'MG', 'Januária', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3135308', 'MG', 'Japaraíba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3135357', 'MG', 'Japonvar', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3135407', 'MG', 'Jeceaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3135456', 'MG', 'Jenipapo de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3135506', 'MG', 'Jequeri', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3135605', 'MG', 'Jequitaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3135704', 'MG', 'Jequitibá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3135803', 'MG', 'Jequitinhonha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3135902', 'MG', 'Jesuânia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3136009', 'MG', 'Joaíma', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3136108', 'MG', 'Joanésia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3136207', 'MG', 'João Monlevade', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3136306', 'MG', 'João Pinheiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3136405', 'MG', 'Joaquim Felício', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3136504', 'MG', 'Jordânia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3136520', 'MG', 'José Gonçalves de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3136553', 'MG', 'José Raydan', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3136579', 'MG', 'Josenópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3136652', 'MG', 'Juatuba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3136702', 'MG', 'Juiz de Fora', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3136801', 'MG', 'Juramento', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3136900', 'MG', 'Juruaia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3136959', 'MG', 'Juvenília', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3137007', 'MG', 'Ladainha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3137106', 'MG', 'Lagamar', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3137205', 'MG', 'Lagoa da Prata', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3137304', 'MG', 'Lagoa dos Patos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3137403', 'MG', 'Lagoa Dourada', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3137502', 'MG', 'Lagoa Formosa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3137536', 'MG', 'Lagoa Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3137601', 'MG', 'Lagoa Santa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3137700', 'MG', 'Lajinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3137809', 'MG', 'Lambari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3137908', 'MG', 'Lamim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3138005', 'MG', 'Laranjal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3138104', 'MG', 'Lassance', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3138203', 'MG', 'Lavras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3138302', 'MG', 'Leandro Ferreira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3138351', 'MG', 'Leme do Prado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3138401', 'MG', 'Leopoldina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3138500', 'MG', 'Liberdade', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3138609', 'MG', 'Lima Duarte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3138625', 'MG', 'Limeira do Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3138658', 'MG', 'Lontra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3138674', 'MG', 'Luisburgo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3138682', 'MG', 'Luislândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3138708', 'MG', 'Luminárias', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3138807', 'MG', 'Luz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3138906', 'MG', 'Machacalis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3139003', 'MG', 'Machado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3139102', 'MG', 'Madre de Deus de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3139201', 'MG', 'Malacacheta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3139250', 'MG', 'Mamonas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3139300', 'MG', 'Manga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3139409', 'MG', 'Manhuaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3139508', 'MG', 'Manhumirim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3139607', 'MG', 'Mantena', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3139805', 'MG', 'Mar de Espanha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3139706', 'MG', 'Maravilhas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3139904', 'MG', 'Maria da Fé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3140001', 'MG', 'Mariana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3140100', 'MG', 'Marilac', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3140159', 'MG', 'Mário Campos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3140209', 'MG', 'Maripá de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3140308', 'MG', 'Marliéria', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3140407', 'MG', 'Marmelópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3140506', 'MG', 'Martinho Campos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3140530', 'MG', 'Martins Soares', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3140555', 'MG', 'Mata Verde', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3140605', 'MG', 'Materlândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3140704', 'MG', 'Mateus Leme', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3171501', 'MG', 'Mathias Lobato', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3140803', 'MG', 'Matias Barbosa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3140852', 'MG', 'Matias Cardoso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3140902', 'MG', 'Matipó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3141009', 'MG', 'Mato Verde', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3141108', 'MG', 'Matozinhos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3141207', 'MG', 'Matutina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3141306', 'MG', 'Medeiros', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3141405', 'MG', 'Medina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3141504', 'MG', 'Mendes Pimentel', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3141603', 'MG', 'Mercês', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3141702', 'MG', 'Mesquita', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3141801', 'MG', 'Minas Novas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3141900', 'MG', 'Minduri', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3142007', 'MG', 'Mirabela', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3142106', 'MG', 'Miradouro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3142205', 'MG', 'Miraí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3142254', 'MG', 'Miravânia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3142304', 'MG', 'Moeda', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3142403', 'MG', 'Moema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3142502', 'MG', 'Monjolos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3142601', 'MG', 'Monsenhor Paulo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3142700', 'MG', 'Montalvânia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3142809', 'MG', 'Monte Alegre de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3142908', 'MG', 'Monte Azul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3143005', 'MG', 'Monte Belo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3143104', 'MG', 'Monte Carmelo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3143153', 'MG', 'Monte Formoso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3143203', 'MG', 'Monte Santo de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3143401', 'MG', 'Monte Sião', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3143302', 'MG', 'Montes Claros', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3143450', 'MG', 'Montezuma', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3143500', 'MG', 'Morada Nova de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3143609', 'MG', 'Morro da Garça', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3143708', 'MG', 'Morro do Pilar', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3143807', 'MG', 'Munhoz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3143906', 'MG', 'Muriaé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3144003', 'MG', 'Mutum', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3144102', 'MG', 'Muzambinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3144201', 'MG', 'Nacip Raydan', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3144300', 'MG', 'Nanuque', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3144359', 'MG', 'Naque', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3144375', 'MG', 'Natalândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3144409', 'MG', 'Natércia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3144508', 'MG', 'Nazareno', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3144607', 'MG', 'Nepomuceno', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3144656', 'MG', 'Ninheira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3144672', 'MG', 'Nova Belém', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3144706', 'MG', 'Nova Era', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3144805', 'MG', 'Nova Lima', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3144904', 'MG', 'Nova Módica', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3145000', 'MG', 'Nova Ponte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3145059', 'MG', 'Nova Porteirinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3145109', 'MG', 'Nova Resende', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3145208', 'MG', 'Nova Serrana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3136603', 'MG', 'Nova União', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3145307', 'MG', 'Novo Cruzeiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3145356', 'MG', 'Novo Oriente de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3145372', 'MG', 'Novorizonte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3145406', 'MG', 'Olaria', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3145455', 'MG', 'Olhos-d''Água', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3145505', 'MG', 'Olímpio Noronha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3145604', 'MG', 'Oliveira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3145703', 'MG', 'Oliveira Fortes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3145802', 'MG', 'Onça de Pitangui', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3145851', 'MG', 'Oratórios', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3145877', 'MG', 'Orizânia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3145901', 'MG', 'Ouro Branco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3146008', 'MG', 'Ouro Fino', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3146107', 'MG', 'Ouro Preto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3146206', 'MG', 'Ouro Verde de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3146255', 'MG', 'Padre Carvalho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3146305', 'MG', 'Padre Paraíso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3146552', 'MG', 'Pai Pedro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3146404', 'MG', 'Paineiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3146503', 'MG', 'Pains', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3146602', 'MG', 'Paiva', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3146701', 'MG', 'Palma', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3146750', 'MG', 'Palmópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3146909', 'MG', 'Papagaios', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3147105', 'MG', 'Pará de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3147006', 'MG', 'Paracatu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3147204', 'MG', 'Paraguaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3147303', 'MG', 'Paraisópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3147402', 'MG', 'Paraopeba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3147600', 'MG', 'Passa Quatro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3147709', 'MG', 'Passa Tempo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3147501', 'MG', 'Passabém', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3147808', 'MG', 'Passa-Vinte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3147907', 'MG', 'Passos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3147956', 'MG', 'Patis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3148004', 'MG', 'Patos de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3148103', 'MG', 'Patrocínio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3148202', 'MG', 'Patrocínio do Muriaé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3148301', 'MG', 'Paula Cândido', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3148400', 'MG', 'Paulistas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3148509', 'MG', 'Pavão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3148608', 'MG', 'Peçanha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3148707', 'MG', 'Pedra Azul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3148756', 'MG', 'Pedra Bonita', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3148806', 'MG', 'Pedra do Anta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3148905', 'MG', 'Pedra do Indaiá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3149002', 'MG', 'Pedra Dourada', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3149101', 'MG', 'Pedralva', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3149150', 'MG', 'Pedras de Maria da Cruz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3149200', 'MG', 'Pedrinópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3149309', 'MG', 'Pedro Leopoldo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3149408', 'MG', 'Pedro Teixeira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3149507', 'MG', 'Pequeri', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3149606', 'MG', 'Pequi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3149705', 'MG', 'Perdigão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3149804', 'MG', 'Perdizes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3149903', 'MG', 'Perdões', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3149952', 'MG', 'Periquito', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3150000', 'MG', 'Pescador', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3150109', 'MG', 'Piau', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3150158', 'MG', 'Piedade de Caratinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3150208', 'MG', 'Piedade de Ponte Nova', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3150307', 'MG', 'Piedade do Rio Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3150406', 'MG', 'Piedade dos Gerais', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3150505', 'MG', 'Pimenta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3150539', 'MG', 'Pingo-d''Água', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3150570', 'MG', 'Pintópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3150604', 'MG', 'Piracema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3150703', 'MG', 'Pirajuba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3150802', 'MG', 'Piranga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3150901', 'MG', 'Piranguçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3151008', 'MG', 'Piranguinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3151107', 'MG', 'Pirapetinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3151206', 'MG', 'Pirapora', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3151305', 'MG', 'Piraúba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3151404', 'MG', 'Pitangui', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3151503', 'MG', 'Piumhi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3151602', 'MG', 'Planura', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3151701', 'MG', 'Poço Fundo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3151800', 'MG', 'Poços de Caldas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3151909', 'MG', 'Pocrane', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3152006', 'MG', 'Pompéu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3152105', 'MG', 'Ponte Nova', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3152131', 'MG', 'Ponto Chique', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3152170', 'MG', 'Ponto dos Volantes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3152204', 'MG', 'Porteirinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3152303', 'MG', 'Porto Firme', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3152402', 'MG', 'Poté', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3152501', 'MG', 'Pouso Alegre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3152600', 'MG', 'Pouso Alto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3152709', 'MG', 'Prados', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3152808', 'MG', 'Prata', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3152907', 'MG', 'Pratápolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3153004', 'MG', 'Pratinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3153103', 'MG', 'Presidente Bernardes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3153202', 'MG', 'Presidente Juscelino', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3153301', 'MG', 'Presidente Kubitschek', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3153400', 'MG', 'Presidente Olegário', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3153608', 'MG', 'Prudente de Morais', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3153707', 'MG', 'Quartel Geral', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3153806', 'MG', 'Queluzito', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3153905', 'MG', 'Raposos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3154002', 'MG', 'Raul Soares', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3154101', 'MG', 'Recreio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3154150', 'MG', 'Reduto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3154200', 'MG', 'Resende Costa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3154309', 'MG', 'Resplendor', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3154408', 'MG', 'Ressaquinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3154457', 'MG', 'Riachinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3154507', 'MG', 'Riacho dos Machados', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3154606', 'MG', 'Ribeirão das Neves', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3154705', 'MG', 'Ribeirão Vermelho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3154804', 'MG', 'Rio Acima', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3154903', 'MG', 'Rio Casca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3155108', 'MG', 'Rio do Prado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3155009', 'MG', 'Rio Doce', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3155207', 'MG', 'Rio Espera', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3155306', 'MG', 'Rio Manso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3155405', 'MG', 'Rio Novo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3155504', 'MG', 'Rio Paranaíba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3155603', 'MG', 'Rio Pardo de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3155702', 'MG', 'Rio Piracicaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3155801', 'MG', 'Rio Pomba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3155900', 'MG', 'Rio Preto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3156007', 'MG', 'Rio Vermelho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3156106', 'MG', 'Ritápolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3156205', 'MG', 'Rochedo de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3156304', 'MG', 'Rodeiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3156403', 'MG', 'Romaria', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3156452', 'MG', 'Rosário da Limeira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3156502', 'MG', 'Rubelita', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3156601', 'MG', 'Rubim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3156700', 'MG', 'Sabará', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3156809', 'MG', 'Sabinópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3156908', 'MG', 'Sacramento', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3157005', 'MG', 'Salinas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3157104', 'MG', 'Salto da Divisa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3157203', 'MG', 'Santa Bárbara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3157252', 'MG', 'Santa Bárbara do Leste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3157278', 'MG', 'Santa Bárbara do Monte Verde', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3157302', 'MG', 'Santa Bárbara do Tugúrio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3157336', 'MG', 'Santa Cruz de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3157377', 'MG', 'Santa Cruz de Salinas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3157401', 'MG', 'Santa Cruz do Escalvado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3157500', 'MG', 'Santa Efigênia de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3157609', 'MG', 'Santa Fé de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3157658', 'MG', 'Santa Helena de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3157708', 'MG', 'Santa Juliana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3157807', 'MG', 'Santa Luzia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3157906', 'MG', 'Santa Margarida', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3158003', 'MG', 'Santa Maria de Itabira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3158102', 'MG', 'Santa Maria do Salto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3158201', 'MG', 'Santa Maria do Suaçuí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3159209', 'MG', 'Santa Rita de Caldas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3159407', 'MG', 'Santa Rita de Ibitipoca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3159308', 'MG', 'Santa Rita de Jacutinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3159357', 'MG', 'Santa Rita de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3159506', 'MG', 'Santa Rita do Itueto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3159605', 'MG', 'Santa Rita do Sapucaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3159704', 'MG', 'Santa Rosa da Serra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3159803', 'MG', 'Santa Vitória', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3158300', 'MG', 'Santana da Vargem', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3158409', 'MG', 'Santana de Cataguases', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3158508', 'MG', 'Santana de Pirapama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3158607', 'MG', 'Santana do Deserto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3158706', 'MG', 'Santana do Garambéu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3158805', 'MG', 'Santana do Jacaré', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3158904', 'MG', 'Santana do Manhuaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3158953', 'MG', 'Santana do Paraíso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3159001', 'MG', 'Santana do Riacho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3159100', 'MG', 'Santana dos Montes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3159902', 'MG', 'Santo Antônio do Amparo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3160009', 'MG', 'Santo Antônio do Aventureiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3160108', 'MG', 'Santo Antônio do Grama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3160207', 'MG', 'Santo Antônio do Itambé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3160306', 'MG', 'Santo Antônio do Jacinto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3160405', 'MG', 'Santo Antônio do Monte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3160454', 'MG', 'Santo Antônio do Retiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3160504', 'MG', 'Santo Antônio do Rio Abaixo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3160603', 'MG', 'Santo Hipólito', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3160702', 'MG', 'Santos Dumont', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3160801', 'MG', 'São Bento Abade', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3160900', 'MG', 'São Brás do Suaçuí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3160959', 'MG', 'São Domingos das Dores', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3161007', 'MG', 'São Domingos do Prata', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3161056', 'MG', 'São Félix de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3161106', 'MG', 'São Francisco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3161205', 'MG', 'São Francisco de Paula', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3161304', 'MG', 'São Francisco de Sales', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3161403', 'MG', 'São Francisco do Glória', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3161502', 'MG', 'São Geraldo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3161601', 'MG', 'São Geraldo da Piedade', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3161650', 'MG', 'São Geraldo do Baixio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3161700', 'MG', 'São Gonçalo do Abaeté', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3161809', 'MG', 'São Gonçalo do Pará', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3161908', 'MG', 'São Gonçalo do Rio Abaixo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3125507', 'MG', 'São Gonçalo do Rio Preto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3162005', 'MG', 'São Gonçalo do Sapucaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3162104', 'MG', 'São Gotardo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3162203', 'MG', 'São João Batista do Glória', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3162252', 'MG', 'São João da Lagoa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3162302', 'MG', 'São João da Mata', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3162401', 'MG', 'São João da Ponte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3162450', 'MG', 'São João das Missões', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3162500', 'MG', 'São João del Rei', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3162559', 'MG', 'São João do Manhuaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3162575', 'MG', 'São João do Manteninha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3162609', 'MG', 'São João do Oriente', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3162658', 'MG', 'São João do Pacuí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3162708', 'MG', 'São João do Paraíso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3162807', 'MG', 'São João Evangelista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3162906', 'MG', 'São João Nepomuceno', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3162922', 'MG', 'São Joaquim de Bicas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3162948', 'MG', 'São José da Barra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3162955', 'MG', 'São José da Lapa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3163003', 'MG', 'São José da Safira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3163102', 'MG', 'São José da Varginha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3163201', 'MG', 'São José do Alegre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3163300', 'MG', 'São José do Divino', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3163409', 'MG', 'São José do Goiabal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3163508', 'MG', 'São José do Jacuri', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3163607', 'MG', 'São José do Mantimento', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3163706', 'MG', 'São Lourenço', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3163805', 'MG', 'São Miguel do Anta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3163904', 'MG', 'São Pedro da União', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3164100', 'MG', 'São Pedro do Suaçuí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3164001', 'MG', 'São Pedro dos Ferros', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3164209', 'MG', 'São Romão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3164308', 'MG', 'São Roque de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3164407', 'MG', 'São Sebastião da Bela Vista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3164431', 'MG', 'São Sebastião da Vargem Alegre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3164472', 'MG', 'São Sebastião do Anta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3164506', 'MG', 'São Sebastião do Maranhão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3164605', 'MG', 'São Sebastião do Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3164704', 'MG', 'São Sebastião do Paraíso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3164803', 'MG', 'São Sebastião do Rio Preto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3164902', 'MG', 'São Sebastião do Rio Verde', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3165206', 'MG', 'São Thomé das Letras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3165008', 'MG', 'São Tiago', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3165107', 'MG', 'São Tomás de Aquino', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3165305', 'MG', 'São Vicente de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3165404', 'MG', 'Sapucaí-Mirim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3165503', 'MG', 'Sardoá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3165537', 'MG', 'Sarzedo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3165560', 'MG', 'Sem-Peixe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3165578', 'MG', 'Senador Amaral', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3165602', 'MG', 'Senador Cortes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3165701', 'MG', 'Senador Firmino', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3165800', 'MG', 'Senador José Bento', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3165909', 'MG', 'Senador Modestino Gonçalves', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3166006', 'MG', 'Senhora de Oliveira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3166105', 'MG', 'Senhora do Porto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3166204', 'MG', 'Senhora dos Remédios', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3166303', 'MG', 'Sericita', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3166402', 'MG', 'Seritinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3166501', 'MG', 'Serra Azul de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3166600', 'MG', 'Serra da Saudade', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3166808', 'MG', 'Serra do Salitre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3166709', 'MG', 'Serra dos Aimorés', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3166907', 'MG', 'Serrania', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3166956', 'MG', 'Serranópolis de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3167004', 'MG', 'Serranos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3167103', 'MG', 'Serro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3167202', 'MG', 'Sete Lagoas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3165552', 'MG', 'Setubinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3167301', 'MG', 'Silveirânia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3167400', 'MG', 'Silvianópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3167509', 'MG', 'Simão Pereira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3167608', 'MG', 'Simonésia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3167707', 'MG', 'Sobrália', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3167806', 'MG', 'Soledade de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3167905', 'MG', 'Tabuleiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3168002', 'MG', 'Taiobeiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3168051', 'MG', 'Taparuba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3168101', 'MG', 'Tapira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3168200', 'MG', 'Tapiraí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3168309', 'MG', 'Taquaraçu de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3168408', 'MG', 'Tarumirim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3168507', 'MG', 'Teixeiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3168606', 'MG', 'Teófilo Otoni', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3168705', 'MG', 'Timóteo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3168804', 'MG', 'Tiradentes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3168903', 'MG', 'Tiros', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3169000', 'MG', 'Tocantins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3169059', 'MG', 'Tocos do Moji', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3169109', 'MG', 'Toledo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3169208', 'MG', 'Tombos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3169307', 'MG', 'Três Corações', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3169356', 'MG', 'Três Marias', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3169406', 'MG', 'Três Pontas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3169505', 'MG', 'Tumiritinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3169604', 'MG', 'Tupaciguara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3169703', 'MG', 'Turmalina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3169802', 'MG', 'Turvolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3169901', 'MG', 'Ubá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3170008', 'MG', 'Ubaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3170057', 'MG', 'Ubaporanga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3170107', 'MG', 'Uberaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3170206', 'MG', 'Uberlândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3170305', 'MG', 'Umburatiba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3170404', 'MG', 'Unaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3170438', 'MG', 'União de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3170479', 'MG', 'Uruana de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3170503', 'MG', 'Urucânia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3170529', 'MG', 'Urucuia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3170578', 'MG', 'Vargem Alegre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3170602', 'MG', 'Vargem Bonita', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3170651', 'MG', 'Vargem Grande do Rio Pardo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3170701', 'MG', 'Varginha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3170750', 'MG', 'Varjão de Minas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3170800', 'MG', 'Várzea da Palma', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3170909', 'MG', 'Varzelândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3171006', 'MG', 'Vazante', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3171030', 'MG', 'Verdelândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3171071', 'MG', 'Veredinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3171105', 'MG', 'Veríssimo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3171154', 'MG', 'Vermelho Novo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3171204', 'MG', 'Vespasiano', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3171303', 'MG', 'Viçosa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3171402', 'MG', 'Vieiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3171600', 'MG', 'Virgem da Lapa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3171709', 'MG', 'Virgínia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3171808', 'MG', 'Virginópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3171907', 'MG', 'Virgolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3172004', 'MG', 'Visconde do Rio Branco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3172103', 'MG', 'Volta Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3172202', 'MG', 'Wenceslau Braz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3200102', 'ES', 'Afonso Cláudio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3200169', 'ES', 'Água Doce do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3200136', 'ES', 'Águia Branca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3200201', 'ES', 'Alegre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3200300', 'ES', 'Alfredo Chaves', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3200359', 'ES', 'Alto Rio Novo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3200409', 'ES', 'Anchieta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3200508', 'ES', 'Apiacá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3200607', 'ES', 'Aracruz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3200706', 'ES', 'Atilio Vivacqua', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3200805', 'ES', 'Baixo Guandu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3200904', 'ES', 'Barra de São Francisco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3201001', 'ES', 'Boa Esperança', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3201100', 'ES', 'Bom Jesus do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3201159', 'ES', 'Brejetuba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3201209', 'ES', 'Cachoeiro de Itapemirim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3201308', 'ES', 'Cariacica', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3201407', 'ES', 'Castelo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3201506', 'ES', 'Colatina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3201605', 'ES', 'Conceição da Barra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3201704', 'ES', 'Conceição do Castelo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3201803', 'ES', 'Divino de São Lourenço', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3201902', 'ES', 'Domingos Martins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3202009', 'ES', 'Dores do Rio Preto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3202108', 'ES', 'Ecoporanga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3202207', 'ES', 'Fundão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3202256', 'ES', 'Governador Lindenberg', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3202306', 'ES', 'Guaçuí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3202405', 'ES', 'Guarapari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3202454', 'ES', 'Ibatiba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3202504', 'ES', 'Ibiraçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3202553', 'ES', 'Ibitirama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3202603', 'ES', 'Iconha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3202652', 'ES', 'Irupi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3202702', 'ES', 'Itaguaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3202801', 'ES', 'Itapemirim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3202900', 'ES', 'Itarana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3203007', 'ES', 'Iúna', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3203056', 'ES', 'Jaguaré', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3203106', 'ES', 'Jerônimo Monteiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3203130', 'ES', 'João Neiva', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3203163', 'ES', 'Laranja da Terra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3203205', 'ES', 'Linhares', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3203304', 'ES', 'Mantenópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3203320', 'ES', 'Marataízes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3203346', 'ES', 'Marechal Floriano', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3203353', 'ES', 'Marilândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3203403', 'ES', 'Mimoso do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3203502', 'ES', 'Montanha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3203601', 'ES', 'Mucurici', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3203700', 'ES', 'Muniz Freire', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3203809', 'ES', 'Muqui', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3203908', 'ES', 'Nova Venécia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3204005', 'ES', 'Pancas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3204054', 'ES', 'Pedro Canário', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3204104', 'ES', 'Pinheiros', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3204203', 'ES', 'Piúma', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3204252', 'ES', 'Ponto Belo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3204302', 'ES', 'Presidente Kennedy', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3204351', 'ES', 'Rio Bananal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3204401', 'ES', 'Rio Novo do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3204500', 'ES', 'Santa Leopoldina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3204559', 'ES', 'Santa Maria de Jetibá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3204609', 'ES', 'Santa Teresa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3204658', 'ES', 'São Domingos do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3204708', 'ES', 'São Gabriel da Palha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3204807', 'ES', 'São José do Calçado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3204906', 'ES', 'São Mateus', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3204955', 'ES', 'São Roque do Canaã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3205002', 'ES', 'Serra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3205010', 'ES', 'Sooretama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3205036', 'ES', 'Vargem Alta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3205069', 'ES', 'Venda Nova do Imigrante', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3205101', 'ES', 'Viana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3205150', 'ES', 'Vila Pavão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3205176', 'ES', 'Vila Valério', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3205200', 'ES', 'Vila Velha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3205309', 'ES', 'Vitória', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3300100', 'RJ', 'Angra dos Reis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3300159', 'RJ', 'Aperibé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3300209', 'RJ', 'Araruama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3300225', 'RJ', 'Areal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3300233', 'RJ', 'Armação dos Búzios', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3300258', 'RJ', 'Arraial do Cabo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3300308', 'RJ', 'Barra do Piraí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3300407', 'RJ', 'Barra Mansa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3300456', 'RJ', 'Belford Roxo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3300506', 'RJ', 'Bom Jardim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3300605', 'RJ', 'Bom Jesus do Itabapoana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3300704', 'RJ', 'Cabo Frio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3300803', 'RJ', 'Cachoeiras de Macacu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3300902', 'RJ', 'Cambuci', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3301009', 'RJ', 'Campos dos Goytacazes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3301108', 'RJ', 'Cantagalo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3300936', 'RJ', 'Carapebus', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3301157', 'RJ', 'Cardoso Moreira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3301207', 'RJ', 'Carmo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3301306', 'RJ', 'Casimiro de Abreu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3300951', 'RJ', 'Comendador Levy Gasparian', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3301405', 'RJ', 'Conceição de Macabu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3301504', 'RJ', 'Cordeiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3301603', 'RJ', 'Duas Barras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3301702', 'RJ', 'Duque de Caxias', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3301801', 'RJ', 'Engenheiro Paulo de Frontin', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3301850', 'RJ', 'Guapimirim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3301876', 'RJ', 'Iguaba Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3301900', 'RJ', 'Itaboraí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3302007', 'RJ', 'Itaguaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3302056', 'RJ', 'Italva', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3302106', 'RJ', 'Itaocara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3302205', 'RJ', 'Itaperuna', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3302254', 'RJ', 'Itatiaia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3302270', 'RJ', 'Japeri', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3302304', 'RJ', 'Laje do Muriaé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3302403', 'RJ', 'Macaé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3302452', 'RJ', 'Macuco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3302502', 'RJ', 'Magé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3302601', 'RJ', 'Mangaratiba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3302700', 'RJ', 'Maricá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3302809', 'RJ', 'Mendes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3302858', 'RJ', 'Mesquita', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3302908', 'RJ', 'Miguel Pereira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3303005', 'RJ', 'Miracema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3303104', 'RJ', 'Natividade', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3303203', 'RJ', 'Nilópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3303302', 'RJ', 'Niterói', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3303401', 'RJ', 'Nova Friburgo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3303500', 'RJ', 'Nova Iguaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3303609', 'RJ', 'Paracambi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3303708', 'RJ', 'Paraíba do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3303807', 'RJ', 'Paraty', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3303856', 'RJ', 'Paty do Alferes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3303906', 'RJ', 'Petrópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3303955', 'RJ', 'Pinheiral', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3304003', 'RJ', 'Piraí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3304102', 'RJ', 'Porciúncula', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3304110', 'RJ', 'Porto Real', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3304128', 'RJ', 'Quatis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3304144', 'RJ', 'Queimados', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3304151', 'RJ', 'Quissamã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3304201', 'RJ', 'Resende', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3304300', 'RJ', 'Rio Bonito', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3304409', 'RJ', 'Rio Claro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3304508', 'RJ', 'Rio das Flores', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3304524', 'RJ', 'Rio das Ostras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3304557', 'RJ', 'Rio de Janeiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3304607', 'RJ', 'Santa Maria Madalena', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3304706', 'RJ', 'Santo Antônio de Pádua', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3304805', 'RJ', 'São Fidélis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3304755', 'RJ', 'São Francisco de Itabapoana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3304904', 'RJ', 'São Gonçalo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3305000', 'RJ', 'São João da Barra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3305109', 'RJ', 'São João de Meriti', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3305133', 'RJ', 'São José de Ubá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3305158', 'RJ', 'São José do Vale do Rio Preto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3305208', 'RJ', 'São Pedro da Aldeia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3305307', 'RJ', 'São Sebastião do Alto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3305406', 'RJ', 'Sapucaia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3305505', 'RJ', 'Saquarema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3305554', 'RJ', 'Seropédica', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3305604', 'RJ', 'Silva Jardim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3305703', 'RJ', 'Sumidouro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3305752', 'RJ', 'Tanguá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3305802', 'RJ', 'Teresópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3305901', 'RJ', 'Trajano de Moraes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3306008', 'RJ', 'Três Rios', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3306107', 'RJ', 'Valença', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3306156', 'RJ', 'Varre-Sai', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3306206', 'RJ', 'Vassouras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3306305', 'RJ', 'Volta Redonda', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3500105', 'SP', 'Adamantina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3500204', 'SP', 'Adolfo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3500303', 'SP', 'Aguaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3500402', 'SP', 'Águas da Prata', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3500501', 'SP', 'Águas de Lindóia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3500550', 'SP', 'Águas de Santa Bárbara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3500600', 'SP', 'Águas de São Pedro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3500709', 'SP', 'Agudos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3500758', 'SP', 'Alambari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3500808', 'SP', 'Alfredo Marcondes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3500907', 'SP', 'Altair', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3501004', 'SP', 'Altinópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3501103', 'SP', 'Alto Alegre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3501152', 'SP', 'Alumínio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3501202', 'SP', 'Álvares Florence', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3501301', 'SP', 'Álvares Machado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3501400', 'SP', 'Álvaro de Carvalho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3501509', 'SP', 'Alvinlândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3501608', 'SP', 'Americana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3501707', 'SP', 'Américo Brasiliense', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3501806', 'SP', 'Américo de Campos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3501905', 'SP', 'Amparo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3502002', 'SP', 'Analândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3502101', 'SP', 'Andradina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3502200', 'SP', 'Angatuba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3502309', 'SP', 'Anhembi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3502408', 'SP', 'Anhumas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3502507', 'SP', 'Aparecida', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3502606', 'SP', 'Aparecida d''Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3502705', 'SP', 'Apiaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3502754', 'SP', 'Araçariguama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3502804', 'SP', 'Araçatuba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3502903', 'SP', 'Araçoiaba da Serra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3503000', 'SP', 'Aramina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3503109', 'SP', 'Arandu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3503158', 'SP', 'Arapeí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3503208', 'SP', 'Araraquara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3503307', 'SP', 'Araras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3503356', 'SP', 'Arco-Íris', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3503406', 'SP', 'Arealva', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3503505', 'SP', 'Areias', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3503604', 'SP', 'Areiópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3503703', 'SP', 'Ariranha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3503802', 'SP', 'Artur Nogueira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3503901', 'SP', 'Arujá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3503950', 'SP', 'Aspásia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3504008', 'SP', 'Assis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3504107', 'SP', 'Atibaia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3504206', 'SP', 'Auriflama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3504305', 'SP', 'Avaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3504404', 'SP', 'Avanhandava', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3504503', 'SP', 'Avaré', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3504602', 'SP', 'Bady Bassitt', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3504701', 'SP', 'Balbinos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3504800', 'SP', 'Bálsamo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3504909', 'SP', 'Bananal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3505005', 'SP', 'Barão de Antonina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3505104', 'SP', 'Barbosa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3505203', 'SP', 'Bariri', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3505302', 'SP', 'Barra Bonita', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3505351', 'SP', 'Barra do Chapéu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3505401', 'SP', 'Barra do Turvo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3505500', 'SP', 'Barretos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3505609', 'SP', 'Barrinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3505708', 'SP', 'Barueri', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3505807', 'SP', 'Bastos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3505906', 'SP', 'Batatais', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3506003', 'SP', 'Bauru', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3506102', 'SP', 'Bebedouro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3506201', 'SP', 'Bento de Abreu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3506300', 'SP', 'Bernardino de Campos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3506359', 'SP', 'Bertioga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3506409', 'SP', 'Bilac', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3506508', 'SP', 'Birigui', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3506607', 'SP', 'Biritiba-Mirim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3506706', 'SP', 'Boa Esperança do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3506805', 'SP', 'Bocaina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3506904', 'SP', 'Bofete', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3507001', 'SP', 'Boituva', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3507100', 'SP', 'Bom Jesus dos Perdões', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3507159', 'SP', 'Bom Sucesso de Itararé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3507209', 'SP', 'Borá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3507308', 'SP', 'Boracéia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3507407', 'SP', 'Borborema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3507456', 'SP', 'Borebi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3507506', 'SP', 'Botucatu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3507605', 'SP', 'Bragança Paulista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3507704', 'SP', 'Braúna', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3507753', 'SP', 'Brejo Alegre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3507803', 'SP', 'Brodowski', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3507902', 'SP', 'Brotas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3508009', 'SP', 'Buri', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3508108', 'SP', 'Buritama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3508207', 'SP', 'Buritizal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3508306', 'SP', 'Cabrália Paulista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3508405', 'SP', 'Cabreúva', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3508504', 'SP', 'Caçapava', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3508603', 'SP', 'Cachoeira Paulista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3508702', 'SP', 'Caconde', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3508801', 'SP', 'Cafelândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3508900', 'SP', 'Caiabu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3509007', 'SP', 'Caieiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3509106', 'SP', 'Caiuá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3509205', 'SP', 'Cajamar', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3509254', 'SP', 'Cajati', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3509304', 'SP', 'Cajobi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3509403', 'SP', 'Cajuru', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3509452', 'SP', 'Campina do Monte Alegre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3509502', 'SP', 'Campinas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3509601', 'SP', 'Campo Limpo Paulista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3509700', 'SP', 'Campos do Jordão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3509809', 'SP', 'Campos Novos Paulista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3509908', 'SP', 'Cananéia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3509957', 'SP', 'Canas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3510005', 'SP', 'Cândido Mota', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3510104', 'SP', 'Cândido Rodrigues', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3510153', 'SP', 'Canitar', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3510203', 'SP', 'Capão Bonito', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3510302', 'SP', 'Capela do Alto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3510401', 'SP', 'Capivari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3510500', 'SP', 'Caraguatatuba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3510609', 'SP', 'Carapicuíba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3510708', 'SP', 'Cardoso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3510807', 'SP', 'Casa Branca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3510906', 'SP', 'Cássia dos Coqueiros', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3511003', 'SP', 'Castilho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3511102', 'SP', 'Catanduva', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3511201', 'SP', 'Catiguá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3511300', 'SP', 'Cedral', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3511409', 'SP', 'Cerqueira César', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3511508', 'SP', 'Cerquilho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3511607', 'SP', 'Cesário Lange', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3511706', 'SP', 'Charqueada', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3557204', 'SP', 'Chavantes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3511904', 'SP', 'Clementina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3512001', 'SP', 'Colina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3512100', 'SP', 'Colômbia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3512209', 'SP', 'Conchal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3512308', 'SP', 'Conchas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3512407', 'SP', 'Cordeirópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3512506', 'SP', 'Coroados', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3512605', 'SP', 'Coronel Macedo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3512704', 'SP', 'Corumbataí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3512803', 'SP', 'Cosmópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3512902', 'SP', 'Cosmorama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3513009', 'SP', 'Cotia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3513108', 'SP', 'Cravinhos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3513207', 'SP', 'Cristais Paulista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3513306', 'SP', 'Cruzália', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3513405', 'SP', 'Cruzeiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3513504', 'SP', 'Cubatão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3513603', 'SP', 'Cunha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3513702', 'SP', 'Descalvado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3513801', 'SP', 'Diadema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3513850', 'SP', 'Dirce Reis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3513900', 'SP', 'Divinolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3514007', 'SP', 'Dobrada', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3514106', 'SP', 'Dois Córregos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3514205', 'SP', 'Dolcinópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3514304', 'SP', 'Dourado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3514403', 'SP', 'Dracena', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3514502', 'SP', 'Duartina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3514601', 'SP', 'Dumont', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3514700', 'SP', 'Echaporã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3514809', 'SP', 'Eldorado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3514908', 'SP', 'Elias Fausto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3514924', 'SP', 'Elisiário', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3514957', 'SP', 'Embaúba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3515004', 'SP', 'Embu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3515103', 'SP', 'Embu-Guaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3515129', 'SP', 'Emilianópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3515152', 'SP', 'Engenheiro Coelho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3515186', 'SP', 'Espírito Santo do Pinhal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3515194', 'SP', 'Espírito Santo do Turvo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3557303', 'SP', 'Estiva Gerbi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3515301', 'SP', 'Estrela do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3515202', 'SP', 'Estrela d''Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3515350', 'SP', 'Euclides da Cunha Paulista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3515400', 'SP', 'Fartura', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3515608', 'SP', 'Fernando Prestes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3515509', 'SP', 'Fernandópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3515657', 'SP', 'Fernão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3515707', 'SP', 'Ferraz de Vasconcelos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3515806', 'SP', 'Flora Rica', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3515905', 'SP', 'Floreal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3516002', 'SP', 'Flórida Paulista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3516101', 'SP', 'Florínia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3516200', 'SP', 'Franca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3516309', 'SP', 'Francisco Morato', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3516408', 'SP', 'Franco da Rocha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3516507', 'SP', 'Gabriel Monteiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3516606', 'SP', 'Gália', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3516705', 'SP', 'Garça', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3516804', 'SP', 'Gastão Vidigal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3516853', 'SP', 'Gavião Peixoto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3516903', 'SP', 'General Salgado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3517000', 'SP', 'Getulina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3517109', 'SP', 'Glicério', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3517208', 'SP', 'Guaiçara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3517307', 'SP', 'Guaimbê', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3517406', 'SP', 'Guaíra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3517505', 'SP', 'Guapiaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3517604', 'SP', 'Guapiara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3517703', 'SP', 'Guará', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3517802', 'SP', 'Guaraçaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3517901', 'SP', 'Guaraci', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3518008', 'SP', 'Guarani d''Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3518107', 'SP', 'Guarantã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3518206', 'SP', 'Guararapes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3518305', 'SP', 'Guararema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3518404', 'SP', 'Guaratinguetá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3518503', 'SP', 'Guareí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3518602', 'SP', 'Guariba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3518701', 'SP', 'Guarujá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3518800', 'SP', 'Guarulhos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3518859', 'SP', 'Guatapará', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3518909', 'SP', 'Guzolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3519006', 'SP', 'Herculândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3519055', 'SP', 'Holambra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3519071', 'SP', 'Hortolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3519105', 'SP', 'Iacanga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3519204', 'SP', 'Iacri', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3519253', 'SP', 'Iaras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3519303', 'SP', 'Ibaté', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3519402', 'SP', 'Ibirá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3519501', 'SP', 'Ibirarema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3519600', 'SP', 'Ibitinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3519709', 'SP', 'Ibiúna', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3519808', 'SP', 'Icém', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3519907', 'SP', 'Iepê', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3520004', 'SP', 'Igaraçu do Tietê', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3520103', 'SP', 'Igarapava', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3520202', 'SP', 'Igaratá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3520301', 'SP', 'Iguape', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3520426', 'SP', 'Ilha Comprida', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3520442', 'SP', 'Ilha Solteira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3520400', 'SP', 'Ilhabela', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3520509', 'SP', 'Indaiatuba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3520608', 'SP', 'Indiana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3520707', 'SP', 'Indiaporã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3520806', 'SP', 'Inúbia Paulista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3520905', 'SP', 'Ipaussu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3521002', 'SP', 'Iperó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3521101', 'SP', 'Ipeúna', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3521150', 'SP', 'Ipiguá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3521200', 'SP', 'Iporanga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3521309', 'SP', 'Ipuã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3521408', 'SP', 'Iracemápolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3521507', 'SP', 'Irapuã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3521606', 'SP', 'Irapuru', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3521705', 'SP', 'Itaberá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3521804', 'SP', 'Itaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3521903', 'SP', 'Itajobi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3522000', 'SP', 'Itaju', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3522109', 'SP', 'Itanhaém', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3522158', 'SP', 'Itaóca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3522208', 'SP', 'Itapecerica da Serra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3522307', 'SP', 'Itapetininga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3522406', 'SP', 'Itapeva', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3522505', 'SP', 'Itapevi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3522604', 'SP', 'Itapira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3522653', 'SP', 'Itapirapuã Paulista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3522703', 'SP', 'Itápolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3522802', 'SP', 'Itaporanga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3522901', 'SP', 'Itapuí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3523008', 'SP', 'Itapura', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3523107', 'SP', 'Itaquaquecetuba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3523206', 'SP', 'Itararé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3523305', 'SP', 'Itariri', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3523404', 'SP', 'Itatiba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3523503', 'SP', 'Itatinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3523602', 'SP', 'Itirapina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3523701', 'SP', 'Itirapuã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3523800', 'SP', 'Itobi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3523909', 'SP', 'Itu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3524006', 'SP', 'Itupeva', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3524105', 'SP', 'Ituverava', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3524204', 'SP', 'Jaborandi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3524303', 'SP', 'Jaboticabal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3524402', 'SP', 'Jacareí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3524501', 'SP', 'Jaci', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3524600', 'SP', 'Jacupiranga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3524709', 'SP', 'Jaguariúna', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3524808', 'SP', 'Jales', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3524907', 'SP', 'Jambeiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3525003', 'SP', 'Jandira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3525102', 'SP', 'Jardinópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3525201', 'SP', 'Jarinu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3525300', 'SP', 'Jaú', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3525409', 'SP', 'Jeriquara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3525508', 'SP', 'Joanópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3525607', 'SP', 'João Ramalho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3525706', 'SP', 'José Bonifácio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3525805', 'SP', 'Júlio Mesquita', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3525854', 'SP', 'Jumirim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3525904', 'SP', 'Jundiaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3526001', 'SP', 'Junqueirópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3526100', 'SP', 'Juquiá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3526209', 'SP', 'Juquitiba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3526308', 'SP', 'Lagoinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3526407', 'SP', 'Laranjal Paulista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3526506', 'SP', 'Lavínia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3526605', 'SP', 'Lavrinhas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3526704', 'SP', 'Leme', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3526803', 'SP', 'Lençóis Paulista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3526902', 'SP', 'Limeira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3527009', 'SP', 'Lindóia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3527108', 'SP', 'Lins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3527207', 'SP', 'Lorena', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3527256', 'SP', 'Lourdes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3527306', 'SP', 'Louveira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3527405', 'SP', 'Lucélia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3527504', 'SP', 'Lucianópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3527603', 'SP', 'Luís Antônio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3527702', 'SP', 'Luiziânia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3527801', 'SP', 'Lupércio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3527900', 'SP', 'Lutécia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3528007', 'SP', 'Macatuba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3528106', 'SP', 'Macaubal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3528205', 'SP', 'Macedônia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3528304', 'SP', 'Magda', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3528403', 'SP', 'Mairinque', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3528502', 'SP', 'Mairiporã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3528601', 'SP', 'Manduri', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3528700', 'SP', 'Marabá Paulista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3528809', 'SP', 'Maracaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3528858', 'SP', 'Marapoama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3528908', 'SP', 'Mariápolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3529005', 'SP', 'Marília', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3529104', 'SP', 'Marinópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3529203', 'SP', 'Martinópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3529302', 'SP', 'Matão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3529401', 'SP', 'Mauá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3529500', 'SP', 'Mendonça', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3529609', 'SP', 'Meridiano', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3529658', 'SP', 'Mesópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3529708', 'SP', 'Miguelópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3529807', 'SP', 'Mineiros do Tietê', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3530003', 'SP', 'Mira Estrela', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3529906', 'SP', 'Miracatu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3530102', 'SP', 'Mirandópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3530201', 'SP', 'Mirante do Paranapanema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3530300', 'SP', 'Mirassol', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3530409', 'SP', 'Mirassolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3530508', 'SP', 'Mococa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3530607', 'SP', 'Mogi das Cruzes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3530706', 'SP', 'Mogi Guaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3530805', 'SP', 'Moji Mirim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3530904', 'SP', 'Mombuca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3531001', 'SP', 'Monções', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3531100', 'SP', 'Mongaguá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3531209', 'SP', 'Monte Alegre do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3531308', 'SP', 'Monte Alto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3531407', 'SP', 'Monte Aprazível', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3531506', 'SP', 'Monte Azul Paulista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3531605', 'SP', 'Monte Castelo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3531803', 'SP', 'Monte Mor', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3531704', 'SP', 'Monteiro Lobato', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3531902', 'SP', 'Morro Agudo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3532009', 'SP', 'Morungaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3532058', 'SP', 'Motuca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3532108', 'SP', 'Murutinga do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3532157', 'SP', 'Nantes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3532207', 'SP', 'Narandiba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3532306', 'SP', 'Natividade da Serra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3532405', 'SP', 'Nazaré Paulista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3532504', 'SP', 'Neves Paulista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3532603', 'SP', 'Nhandeara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3532702', 'SP', 'Nipoã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3532801', 'SP', 'Nova Aliança', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3532827', 'SP', 'Nova Campina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3532843', 'SP', 'Nova Canaã Paulista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3532868', 'SP', 'Nova Castilho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3532900', 'SP', 'Nova Europa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3533007', 'SP', 'Nova Granada', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3533106', 'SP', 'Nova Guataporanga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3533205', 'SP', 'Nova Independência', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3533304', 'SP', 'Nova Luzitânia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3533403', 'SP', 'Nova Odessa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3533254', 'SP', 'Novais', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3533502', 'SP', 'Novo Horizonte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3533601', 'SP', 'Nuporanga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3533700', 'SP', 'Ocauçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3533809', 'SP', 'Óleo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3533908', 'SP', 'Olímpia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3534005', 'SP', 'Onda Verde', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3534104', 'SP', 'Oriente', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3534203', 'SP', 'Orindiúva', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3534302', 'SP', 'Orlândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3534401', 'SP', 'Osasco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3534500', 'SP', 'Oscar Bressane', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3534609', 'SP', 'Osvaldo Cruz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3534708', 'SP', 'Ourinhos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3534807', 'SP', 'Ouro Verde', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3534757', 'SP', 'Ouroeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3534906', 'SP', 'Pacaembu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3535002', 'SP', 'Palestina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3535101', 'SP', 'Palmares Paulista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3535200', 'SP', 'Palmeira d''Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3535309', 'SP', 'Palmital', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3535408', 'SP', 'Panorama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3535507', 'SP', 'Paraguaçu Paulista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3535606', 'SP', 'Paraibuna', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3535705', 'SP', 'Paraíso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3535804', 'SP', 'Paranapanema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3535903', 'SP', 'Paranapuã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3536000', 'SP', 'Parapuã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3536109', 'SP', 'Pardinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3536208', 'SP', 'Pariquera-Açu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3536257', 'SP', 'Parisi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3536307', 'SP', 'Patrocínio Paulista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3536406', 'SP', 'Paulicéia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3536505', 'SP', 'Paulínia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3536570', 'SP', 'Paulistânia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3536604', 'SP', 'Paulo de Faria', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3536703', 'SP', 'Pederneiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3536802', 'SP', 'Pedra Bela', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3536901', 'SP', 'Pedranópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3537008', 'SP', 'Pedregulho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3537107', 'SP', 'Pedreira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3537156', 'SP', 'Pedrinhas Paulista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3537206', 'SP', 'Pedro de Toledo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3537305', 'SP', 'Penápolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3537404', 'SP', 'Pereira Barreto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3537503', 'SP', 'Pereiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3537602', 'SP', 'Peruíbe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3537701', 'SP', 'Piacatu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3537800', 'SP', 'Piedade', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3537909', 'SP', 'Pilar do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3538006', 'SP', 'Pindamonhangaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3538105', 'SP', 'Pindorama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3538204', 'SP', 'Pinhalzinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3538303', 'SP', 'Piquerobi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3538501', 'SP', 'Piquete', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3538600', 'SP', 'Piracaia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3538709', 'SP', 'Piracicaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3538808', 'SP', 'Piraju', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3538907', 'SP', 'Pirajuí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3539004', 'SP', 'Pirangi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3539103', 'SP', 'Pirapora do Bom Jesus', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3539202', 'SP', 'Pirapozinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3539301', 'SP', 'Pirassununga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3539400', 'SP', 'Piratininga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3539509', 'SP', 'Pitangueiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3539608', 'SP', 'Planalto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3539707', 'SP', 'Platina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3539806', 'SP', 'Poá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3539905', 'SP', 'Poloni', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3540002', 'SP', 'Pompéia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3540101', 'SP', 'Pongaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3540200', 'SP', 'Pontal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3540259', 'SP', 'Pontalinda', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3540309', 'SP', 'Pontes Gestal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3540408', 'SP', 'Populina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3540507', 'SP', 'Porangaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3540606', 'SP', 'Porto Feliz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3540705', 'SP', 'Porto Ferreira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3540754', 'SP', 'Potim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3540804', 'SP', 'Potirendaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3540853', 'SP', 'Pracinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3540903', 'SP', 'Pradópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3541000', 'SP', 'Praia Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3541059', 'SP', 'Pratânia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3541109', 'SP', 'Presidente Alves', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3541208', 'SP', 'Presidente Bernardes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3541307', 'SP', 'Presidente Epitácio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3541406', 'SP', 'Presidente Prudente', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3541505', 'SP', 'Presidente Venceslau', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3541604', 'SP', 'Promissão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3541653', 'SP', 'Quadra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3541703', 'SP', 'Quatá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3541802', 'SP', 'Queiroz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3541901', 'SP', 'Queluz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3542008', 'SP', 'Quintana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3542107', 'SP', 'Rafard', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3542206', 'SP', 'Rancharia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3542305', 'SP', 'Redenção da Serra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3542404', 'SP', 'Regente Feijó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3542503', 'SP', 'Reginópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3542602', 'SP', 'Registro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3542701', 'SP', 'Restinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3542800', 'SP', 'Ribeira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3542909', 'SP', 'Ribeirão Bonito', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3543006', 'SP', 'Ribeirão Branco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3543105', 'SP', 'Ribeirão Corrente', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3543204', 'SP', 'Ribeirão do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3543238', 'SP', 'Ribeirão dos Índios', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3543253', 'SP', 'Ribeirão Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3543303', 'SP', 'Ribeirão Pires', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3543402', 'SP', 'Ribeirão Preto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3543600', 'SP', 'Rifaina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3543709', 'SP', 'Rincão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3543808', 'SP', 'Rinópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3543907', 'SP', 'Rio Claro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3544004', 'SP', 'Rio das Pedras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3544103', 'SP', 'Rio Grande da Serra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3544202', 'SP', 'Riolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3543501', 'SP', 'Riversul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3544251', 'SP', 'Rosana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3544301', 'SP', 'Roseira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3544400', 'SP', 'Rubiácea', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3544509', 'SP', 'Rubinéia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3544608', 'SP', 'Sabino', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3544707', 'SP', 'Sagres', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3544806', 'SP', 'Sales', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3544905', 'SP', 'Sales Oliveira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3545001', 'SP', 'Salesópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3545100', 'SP', 'Salmourão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3545159', 'SP', 'Saltinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3545209', 'SP', 'Salto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3545308', 'SP', 'Salto de Pirapora', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3545407', 'SP', 'Salto Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3545506', 'SP', 'Sandovalina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3545605', 'SP', 'Santa Adélia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3545704', 'SP', 'Santa Albertina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3545803', 'SP', 'Santa Bárbara d''Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3546009', 'SP', 'Santa Branca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3546108', 'SP', 'Santa Clara d''Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3546207', 'SP', 'Santa Cruz da Conceição', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3546256', 'SP', 'Santa Cruz da Esperança', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3546306', 'SP', 'Santa Cruz das Palmeiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3546405', 'SP', 'Santa Cruz do Rio Pardo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3546504', 'SP', 'Santa Ernestina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3546603', 'SP', 'Santa Fé do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3546702', 'SP', 'Santa Gertrudes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3546801', 'SP', 'Santa Isabel', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3546900', 'SP', 'Santa Lúcia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3547007', 'SP', 'Santa Maria da Serra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3547106', 'SP', 'Santa Mercedes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3547502', 'SP', 'Santa Rita do Passa Quatro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3547403', 'SP', 'Santa Rita d''Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3547601', 'SP', 'Santa Rosa de Viterbo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3547650', 'SP', 'Santa Salete', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3547205', 'SP', 'Santana da Ponte Pensa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3547304', 'SP', 'Santana de Parnaíba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3547700', 'SP', 'Santo Anastácio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3547809', 'SP', 'Santo André', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3547908', 'SP', 'Santo Antônio da Alegria', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3548005', 'SP', 'Santo Antônio de Posse', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3548054', 'SP', 'Santo Antônio do Aracanguá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3548104', 'SP', 'Santo Antônio do Jardim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3548203', 'SP', 'Santo Antônio do Pinhal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3548302', 'SP', 'Santo Expedito', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3548401', 'SP', 'Santópolis do Aguapeí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3548500', 'SP', 'Santos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3548609', 'SP', 'São Bento do Sapucaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3548708', 'SP', 'São Bernardo do Campo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3548807', 'SP', 'São Caetano do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3548906', 'SP', 'São Carlos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3549003', 'SP', 'São Francisco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3549102', 'SP', 'São João da Boa Vista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3549201', 'SP', 'São João das Duas Pontes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3549250', 'SP', 'São João de Iracema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3549300', 'SP', 'São João do Pau d''Alho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3549409', 'SP', 'São Joaquim da Barra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3549508', 'SP', 'São José da Bela Vista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3549607', 'SP', 'São José do Barreiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3549706', 'SP', 'São José do Rio Pardo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3549805', 'SP', 'São José do Rio Preto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3549904', 'SP', 'São José dos Campos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3549953', 'SP', 'São Lourenço da Serra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3550001', 'SP', 'São Luís do Paraitinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3550100', 'SP', 'São Manuel', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3550209', 'SP', 'São Miguel Arcanjo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3550308', 'SP', 'São Paulo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3550407', 'SP', 'São Pedro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3550506', 'SP', 'São Pedro do Turvo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3550605', 'SP', 'São Roque', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3550704', 'SP', 'São Sebastião', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3550803', 'SP', 'São Sebastião da Grama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3550902', 'SP', 'São Simão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3551009', 'SP', 'São Vicente', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3551108', 'SP', 'Sarapuí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3551207', 'SP', 'Sarutaiá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3551306', 'SP', 'Sebastianópolis do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3551405', 'SP', 'Serra Azul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3551603', 'SP', 'Serra Negra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3551504', 'SP', 'Serrana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3551702', 'SP', 'Sertãozinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3551801', 'SP', 'Sete Barras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3551900', 'SP', 'Severínia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3552007', 'SP', 'Silveiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3552106', 'SP', 'Socorro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3552205', 'SP', 'Sorocaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3552304', 'SP', 'Sud Mennucci', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3552403', 'SP', 'Sumaré', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3552551', 'SP', 'Suzanápolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3552502', 'SP', 'Suzano', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3552601', 'SP', 'Tabapuã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3552700', 'SP', 'Tabatinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3552809', 'SP', 'Taboão da Serra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3552908', 'SP', 'Taciba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3553005', 'SP', 'Taguaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3553104', 'SP', 'Taiaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3553203', 'SP', 'Taiúva', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3553302', 'SP', 'Tambaú', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3553401', 'SP', 'Tanabi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3553500', 'SP', 'Tapiraí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3553609', 'SP', 'Tapiratiba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3553658', 'SP', 'Taquaral', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3553708', 'SP', 'Taquaritinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3553807', 'SP', 'Taquarituba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3553856', 'SP', 'Taquarivaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3553906', 'SP', 'Tarabai', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3553955', 'SP', 'Tarumã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3554003', 'SP', 'Tatuí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3554102', 'SP', 'Taubaté', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3554201', 'SP', 'Tejupá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3554300', 'SP', 'Teodoro Sampaio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3554409', 'SP', 'Terra Roxa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3554508', 'SP', 'Tietê', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3554607', 'SP', 'Timburi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3554656', 'SP', 'Torre de Pedra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3554706', 'SP', 'Torrinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3554755', 'SP', 'Trabiju', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3554805', 'SP', 'Tremembé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3554904', 'SP', 'Três Fronteiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3554953', 'SP', 'Tuiuti', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3555000', 'SP', 'Tupã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3555109', 'SP', 'Tupi Paulista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3555208', 'SP', 'Turiúba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3555307', 'SP', 'Turmalina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3555356', 'SP', 'Ubarana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3555406', 'SP', 'Ubatuba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3555505', 'SP', 'Ubirajara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3555604', 'SP', 'Uchoa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3555703', 'SP', 'União Paulista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3555802', 'SP', 'Urânia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3555901', 'SP', 'Uru', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3556008', 'SP', 'Urupês', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3556107', 'SP', 'Valentim Gentil', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3556206', 'SP', 'Valinhos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3556305', 'SP', 'Valparaíso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3556354', 'SP', 'Vargem', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3556404', 'SP', 'Vargem Grande do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3556453', 'SP', 'Vargem Grande Paulista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3556503', 'SP', 'Várzea Paulista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3556602', 'SP', 'Vera Cruz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3556701', 'SP', 'Vinhedo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3556800', 'SP', 'Viradouro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3556909', 'SP', 'Vista Alegre do Alto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3556958', 'SP', 'Vitória Brasil', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3557006', 'SP', 'Votorantim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3557105', 'SP', 'Votuporanga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('3557154', 'SP', 'Zacarias', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4100103', 'PR', 'Abatiá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4100202', 'PR', 'Adrianópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4100301', 'PR', 'Agudos do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4100400', 'PR', 'Almirante Tamandaré', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4100459', 'PR', 'Altamira do Paraná', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4128625', 'PR', 'Alto Paraíso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4100608', 'PR', 'Alto Paraná', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4100707', 'PR', 'Alto Piquiri', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4100509', 'PR', 'Altônia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4100806', 'PR', 'Alvorada do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4100905', 'PR', 'Amaporã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4101002', 'PR', 'Ampére', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4101051', 'PR', 'Anahy', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4101101', 'PR', 'Andirá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4101150', 'PR', 'Ângulo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4101200', 'PR', 'Antonina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4101309', 'PR', 'Antônio Olinto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4101408', 'PR', 'Apucarana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4101507', 'PR', 'Arapongas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4101606', 'PR', 'Arapoti', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4101655', 'PR', 'Arapuã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4101705', 'PR', 'Araruna', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4101804', 'PR', 'Araucária', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4101853', 'PR', 'Ariranha do Ivaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4101903', 'PR', 'Assaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4102000', 'PR', 'Assis Chateaubriand', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4102109', 'PR', 'Astorga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4102208', 'PR', 'Atalaia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4102307', 'PR', 'Balsa Nova', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4102406', 'PR', 'Bandeirantes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4102505', 'PR', 'Barbosa Ferraz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4102703', 'PR', 'Barra do Jacaré', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4102604', 'PR', 'Barracão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4102752', 'PR', 'Bela Vista da Caroba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4102802', 'PR', 'Bela Vista do Paraíso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4102901', 'PR', 'Bituruna', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4103008', 'PR', 'Boa Esperança', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4103024', 'PR', 'Boa Esperança do Iguaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4103040', 'PR', 'Boa Ventura de São Roque', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4103057', 'PR', 'Boa Vista da Aparecida', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4103107', 'PR', 'Bocaiúva do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4103156', 'PR', 'Bom Jesus do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4103206', 'PR', 'Bom Sucesso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4103222', 'PR', 'Bom Sucesso do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4103305', 'PR', 'Borrazópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4103354', 'PR', 'Braganey', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4103370', 'PR', 'Brasilândia do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4103404', 'PR', 'Cafeara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4103453', 'PR', 'Cafelândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4103479', 'PR', 'Cafezal do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4103503', 'PR', 'Califórnia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4103602', 'PR', 'Cambará', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4103701', 'PR', 'Cambé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4103800', 'PR', 'Cambira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4103909', 'PR', 'Campina da Lagoa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4103958', 'PR', 'Campina do Simão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4104006', 'PR', 'Campina Grande do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4104055', 'PR', 'Campo Bonito', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4104105', 'PR', 'Campo do Tenente', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4104204', 'PR', 'Campo Largo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4104253', 'PR', 'Campo Magro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4104303', 'PR', 'Campo Mourão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4104402', 'PR', 'Cândido de Abreu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4104428', 'PR', 'Candói', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4104451', 'PR', 'Cantagalo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4104501', 'PR', 'Capanema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4104600', 'PR', 'Capitão Leônidas Marques', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4104659', 'PR', 'Carambeí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4104709', 'PR', 'Carlópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4104808', 'PR', 'Cascavel', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4104907', 'PR', 'Castro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4105003', 'PR', 'Catanduvas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4105102', 'PR', 'Centenário do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4105201', 'PR', 'Cerro Azul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4105300', 'PR', 'Céu Azul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4105409', 'PR', 'Chopinzinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4105508', 'PR', 'Cianorte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4105607', 'PR', 'Cidade Gaúcha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4105706', 'PR', 'Clevelândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4105805', 'PR', 'Colombo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4105904', 'PR', 'Colorado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4106001', 'PR', 'Congonhinhas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4106100', 'PR', 'Conselheiro Mairinck', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4106209', 'PR', 'Contenda', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4106308', 'PR', 'Corbélia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4106407', 'PR', 'Cornélio Procópio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4106456', 'PR', 'Coronel Domingos Soares', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4106506', 'PR', 'Coronel Vivida', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4106555', 'PR', 'Corumbataí do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4106803', 'PR', 'Cruz Machado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4106571', 'PR', 'Cruzeiro do Iguaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4106605', 'PR', 'Cruzeiro do Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4106704', 'PR', 'Cruzeiro do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4106852', 'PR', 'Cruzmaltina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4106902', 'PR', 'Curitiba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4107009', 'PR', 'Curiúva', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4107108', 'PR', 'Diamante do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4107124', 'PR', 'Diamante do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4107157', 'PR', 'Diamante D''Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4107207', 'PR', 'Dois Vizinhos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4107256', 'PR', 'Douradina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4107306', 'PR', 'Doutor Camargo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4128633', 'PR', 'Doutor Ulysses', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4107405', 'PR', 'Enéas Marques', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4107504', 'PR', 'Engenheiro Beltrão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4107538', 'PR', 'Entre Rios do Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4107520', 'PR', 'Esperança Nova', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4107546', 'PR', 'Espigão Alto do Iguaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4107553', 'PR', 'Farol', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4107603', 'PR', 'Faxinal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4107652', 'PR', 'Fazenda Rio Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4107702', 'PR', 'Fênix', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4107736', 'PR', 'Fernandes Pinheiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4107751', 'PR', 'Figueira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4107850', 'PR', 'Flor da Serra do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4107801', 'PR', 'Floraí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4107900', 'PR', 'Floresta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4108007', 'PR', 'Florestópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4108106', 'PR', 'Flórida', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4108205', 'PR', 'Formosa do Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4108304', 'PR', 'Foz do Iguaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4108452', 'PR', 'Foz do Jordão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4108320', 'PR', 'Francisco Alves', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4108403', 'PR', 'Francisco Beltrão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4108502', 'PR', 'General Carneiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4108551', 'PR', 'Godoy Moreira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4108601', 'PR', 'Goioerê', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4108650', 'PR', 'Goioxim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4108700', 'PR', 'Grandes Rios', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4108809', 'PR', 'Guaíra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4108908', 'PR', 'Guairaçá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4108957', 'PR', 'Guamiranga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4109005', 'PR', 'Guapirama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4109104', 'PR', 'Guaporema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4109203', 'PR', 'Guaraci', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4109302', 'PR', 'Guaraniaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4109401', 'PR', 'Guarapuava', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4109500', 'PR', 'Guaraqueçaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4109609', 'PR', 'Guaratuba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4109658', 'PR', 'Honório Serpa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4109708', 'PR', 'Ibaiti', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4109757', 'PR', 'Ibema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4109807', 'PR', 'Ibiporã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4109906', 'PR', 'Icaraíma', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4110003', 'PR', 'Iguaraçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4110052', 'PR', 'Iguatu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4110078', 'PR', 'Imbaú', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4110102', 'PR', 'Imbituva', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4110201', 'PR', 'Inácio Martins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4110300', 'PR', 'Inajá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4110409', 'PR', 'Indianópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4110508', 'PR', 'Ipiranga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4110607', 'PR', 'Iporã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4110656', 'PR', 'Iracema do Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4110706', 'PR', 'Irati', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4110805', 'PR', 'Iretama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4110904', 'PR', 'Itaguajé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4110953', 'PR', 'Itaipulândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4111001', 'PR', 'Itambaracá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4111100', 'PR', 'Itambé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4111209', 'PR', 'Itapejara d''Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4111258', 'PR', 'Itaperuçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4111308', 'PR', 'Itaúna do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4111407', 'PR', 'Ivaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4111506', 'PR', 'Ivaiporã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4111555', 'PR', 'Ivaté', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4111605', 'PR', 'Ivatuba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4111704', 'PR', 'Jaboti', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4111803', 'PR', 'Jacarezinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4111902', 'PR', 'Jaguapitã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4112009', 'PR', 'Jaguariaíva', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4112108', 'PR', 'Jandaia do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4112207', 'PR', 'Janiópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4112306', 'PR', 'Japira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4112405', 'PR', 'Japurá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4112504', 'PR', 'Jardim Alegre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4112603', 'PR', 'Jardim Olinda', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4112702', 'PR', 'Jataizinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4112751', 'PR', 'Jesuítas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4112801', 'PR', 'Joaquim Távora', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4112900', 'PR', 'Jundiaí do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4112959', 'PR', 'Juranda', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4113007', 'PR', 'Jussara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4113106', 'PR', 'Kaloré', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4113205', 'PR', 'Lapa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4113254', 'PR', 'Laranjal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4113304', 'PR', 'Laranjeiras do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4113403', 'PR', 'Leópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4113429', 'PR', 'Lidianópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4113452', 'PR', 'Lindoeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4113502', 'PR', 'Loanda', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4113601', 'PR', 'Lobato', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4113700', 'PR', 'Londrina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4113734', 'PR', 'Luiziana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4113759', 'PR', 'Lunardelli', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4113809', 'PR', 'Lupionópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4113908', 'PR', 'Mallet', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4114005', 'PR', 'Mamborê', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4114104', 'PR', 'Mandaguaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4114203', 'PR', 'Mandaguari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4114302', 'PR', 'Mandirituba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4114351', 'PR', 'Manfrinópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4114401', 'PR', 'Mangueirinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4114500', 'PR', 'Manoel Ribas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4114609', 'PR', 'Marechal Cândido Rondon', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4114708', 'PR', 'Maria Helena', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4114807', 'PR', 'Marialva', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4114906', 'PR', 'Marilândia do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4115002', 'PR', 'Marilena', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4115101', 'PR', 'Mariluz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4115200', 'PR', 'Maringá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4115309', 'PR', 'Mariópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4115358', 'PR', 'Maripá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4115408', 'PR', 'Marmeleiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4115457', 'PR', 'Marquinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4115507', 'PR', 'Marumbi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4115606', 'PR', 'Matelândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4115705', 'PR', 'Matinhos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4115739', 'PR', 'Mato Rico', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4115754', 'PR', 'Mauá da Serra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4115804', 'PR', 'Medianeira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4115853', 'PR', 'Mercedes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4115903', 'PR', 'Mirador', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4116000', 'PR', 'Miraselva', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4116059', 'PR', 'Missal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4116109', 'PR', 'Moreira Sales', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4116208', 'PR', 'Morretes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4116307', 'PR', 'Munhoz de Melo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4116406', 'PR', 'Nossa Senhora das Graças', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4116505', 'PR', 'Nova Aliança do Ivaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4116604', 'PR', 'Nova América da Colina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4116703', 'PR', 'Nova Aurora', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4116802', 'PR', 'Nova Cantu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4116901', 'PR', 'Nova Esperança', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4116950', 'PR', 'Nova Esperança do Sudoeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4117008', 'PR', 'Nova Fátima', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4117057', 'PR', 'Nova Laranjeiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4117107', 'PR', 'Nova Londrina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4117206', 'PR', 'Nova Olímpia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4117255', 'PR', 'Nova Prata do Iguaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4117214', 'PR', 'Nova Santa Bárbara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4117222', 'PR', 'Nova Santa Rosa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4117271', 'PR', 'Nova Tebas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4117297', 'PR', 'Novo Itacolomi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4117305', 'PR', 'Ortigueira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4117404', 'PR', 'Ourizona', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4117453', 'PR', 'Ouro Verde do Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4117503', 'PR', 'Paiçandu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4117602', 'PR', 'Palmas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4117701', 'PR', 'Palmeira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4117800', 'PR', 'Palmital', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4117909', 'PR', 'Palotina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4118006', 'PR', 'Paraíso do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4118105', 'PR', 'Paranacity', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4118204', 'PR', 'Paranaguá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4118303', 'PR', 'Paranapoema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4118402', 'PR', 'Paranavaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4118451', 'PR', 'Pato Bragado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4118501', 'PR', 'Pato Branco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4118600', 'PR', 'Paula Freitas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4118709', 'PR', 'Paulo Frontin', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4118808', 'PR', 'Peabiru', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4118857', 'PR', 'Perobal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4118907', 'PR', 'Pérola', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4119004', 'PR', 'Pérola d''Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4119103', 'PR', 'Piên', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4119152', 'PR', 'Pinhais', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4119251', 'PR', 'Pinhal de São Bento', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4119202', 'PR', 'Pinhalão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4119301', 'PR', 'Pinhão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4119400', 'PR', 'Piraí do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4119509', 'PR', 'Piraquara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4119608', 'PR', 'Pitanga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4119657', 'PR', 'Pitangueiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4119707', 'PR', 'Planaltina do Paraná', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4119806', 'PR', 'Planalto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4119905', 'PR', 'Ponta Grossa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4119954', 'PR', 'Pontal do Paraná', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4120002', 'PR', 'Porecatu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4120101', 'PR', 'Porto Amazonas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4120150', 'PR', 'Porto Barreiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4120200', 'PR', 'Porto Rico', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4120309', 'PR', 'Porto Vitória', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4120333', 'PR', 'Prado Ferreira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4120358', 'PR', 'Pranchita', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4120408', 'PR', 'Presidente Castelo Branco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4120507', 'PR', 'Primeiro de Maio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4120606', 'PR', 'Prudentópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4120655', 'PR', 'Quarto Centenário', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4120705', 'PR', 'Quatiguá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4120804', 'PR', 'Quatro Barras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4120853', 'PR', 'Quatro Pontes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4120903', 'PR', 'Quedas do Iguaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4121000', 'PR', 'Querência do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4121109', 'PR', 'Quinta do Sol', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4121208', 'PR', 'Quitandinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4121257', 'PR', 'Ramilândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4121307', 'PR', 'Rancho Alegre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4121356', 'PR', 'Rancho Alegre D''Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4121406', 'PR', 'Realeza', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4121505', 'PR', 'Rebouças', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4121604', 'PR', 'Renascença', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4121703', 'PR', 'Reserva', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4121752', 'PR', 'Reserva do Iguaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4121802', 'PR', 'Ribeirão Claro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4121901', 'PR', 'Ribeirão do Pinhal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4122008', 'PR', 'Rio Azul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4122107', 'PR', 'Rio Bom', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4122156', 'PR', 'Rio Bonito do Iguaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4122172', 'PR', 'Rio Branco do Ivaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4122206', 'PR', 'Rio Branco do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4122305', 'PR', 'Rio Negro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4122404', 'PR', 'Rolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4122503', 'PR', 'Roncador', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4122602', 'PR', 'Rondon', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4122651', 'PR', 'Rosário do Ivaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4122701', 'PR', 'Sabáudia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4122800', 'PR', 'Salgado Filho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4122909', 'PR', 'Salto do Itararé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4123006', 'PR', 'Salto do Lontra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4123105', 'PR', 'Santa Amélia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4123204', 'PR', 'Santa Cecília do Pavão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4123303', 'PR', 'Santa Cruz de Monte Castelo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4123402', 'PR', 'Santa Fé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4123501', 'PR', 'Santa Helena', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4123600', 'PR', 'Santa Inês', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4123709', 'PR', 'Santa Isabel do Ivaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4123808', 'PR', 'Santa Izabel do Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4123824', 'PR', 'Santa Lúcia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4123857', 'PR', 'Santa Maria do Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4123907', 'PR', 'Santa Mariana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4123956', 'PR', 'Santa Mônica', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4124020', 'PR', 'Santa Tereza do Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4124053', 'PR', 'Santa Terezinha de Itaipu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4124004', 'PR', 'Santana do Itararé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4124103', 'PR', 'Santo Antônio da Platina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4124202', 'PR', 'Santo Antônio do Caiuá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4124301', 'PR', 'Santo Antônio do Paraíso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4124400', 'PR', 'Santo Antônio do Sudoeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4124509', 'PR', 'Santo Inácio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4124608', 'PR', 'São Carlos do Ivaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4124707', 'PR', 'São Jerônimo da Serra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4124806', 'PR', 'São João', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4124905', 'PR', 'São João do Caiuá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4125001', 'PR', 'São João do Ivaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4125100', 'PR', 'São João do Triunfo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4125308', 'PR', 'São Jorge do Ivaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4125357', 'PR', 'São Jorge do Patrocínio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4125209', 'PR', 'São Jorge d''Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4125407', 'PR', 'São José da Boa Vista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4125456', 'PR', 'São José das Palmeiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4125506', 'PR', 'São José dos Pinhais', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4125555', 'PR', 'São Manoel do Paraná', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4125605', 'PR', 'São Mateus do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4125704', 'PR', 'São Miguel do Iguaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4125753', 'PR', 'São Pedro do Iguaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4125803', 'PR', 'São Pedro do Ivaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4125902', 'PR', 'São Pedro do Paraná', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4126009', 'PR', 'São Sebastião da Amoreira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4126108', 'PR', 'São Tomé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4126207', 'PR', 'Sapopema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4126256', 'PR', 'Sarandi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4126272', 'PR', 'Saudade do Iguaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4126306', 'PR', 'Sengés', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4126355', 'PR', 'Serranópolis do Iguaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4126405', 'PR', 'Sertaneja', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4126504', 'PR', 'Sertanópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4126603', 'PR', 'Siqueira Campos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4126652', 'PR', 'Sulina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4126678', 'PR', 'Tamarana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4126702', 'PR', 'Tamboara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4126801', 'PR', 'Tapejara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4126900', 'PR', 'Tapira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4127007', 'PR', 'Teixeira Soares', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4127106', 'PR', 'Telêmaco Borba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4127205', 'PR', 'Terra Boa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4127304', 'PR', 'Terra Rica', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4127403', 'PR', 'Terra Roxa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4127502', 'PR', 'Tibagi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4127601', 'PR', 'Tijucas do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4127700', 'PR', 'Toledo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4127809', 'PR', 'Tomazina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4127858', 'PR', 'Três Barras do Paraná', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4127882', 'PR', 'Tunas do Paraná', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4127908', 'PR', 'Tuneiras do Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4127957', 'PR', 'Tupãssi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4127965', 'PR', 'Turvo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4128005', 'PR', 'Ubiratã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4128104', 'PR', 'Umuarama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4128203', 'PR', 'União da Vitória', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4128302', 'PR', 'Uniflor', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4128401', 'PR', 'Uraí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4128534', 'PR', 'Ventania', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4128559', 'PR', 'Vera Cruz do Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4128609', 'PR', 'Verê', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4128658', 'PR', 'Virmond', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4128708', 'PR', 'Vitorino', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4128500', 'PR', 'Wenceslau Braz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4128807', 'PR', 'Xambrê', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4200051', 'SC', 'Abdon Batista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4200101', 'SC', 'Abelardo Luz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4200200', 'SC', 'Agrolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4200309', 'SC', 'Agronômica', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4200408', 'SC', 'Água Doce', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4200507', 'SC', 'Águas de Chapecó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4200556', 'SC', 'Águas Frias', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4200606', 'SC', 'Águas Mornas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4200705', 'SC', 'Alfredo Wagner', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4200754', 'SC', 'Alto Bela Vista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4200804', 'SC', 'Anchieta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4200903', 'SC', 'Angelina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4201000', 'SC', 'Anita Garibaldi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4201109', 'SC', 'Anitápolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4201208', 'SC', 'Antônio Carlos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4201257', 'SC', 'Apiúna', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4201273', 'SC', 'Arabutã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4201307', 'SC', 'Araquari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4201406', 'SC', 'Araranguá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4201505', 'SC', 'Armazém', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4201604', 'SC', 'Arroio Trinta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4201653', 'SC', 'Arvoredo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4201703', 'SC', 'Ascurra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4201802', 'SC', 'Atalanta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4201901', 'SC', 'Aurora', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4201950', 'SC', 'Balneário Arroio do Silva', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4202057', 'SC', 'Balneário Barra do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4202008', 'SC', 'Balneário Camboriú', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4202073', 'SC', 'Balneário Gaivota', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4212809', 'SC', 'Balneário Piçarras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4202081', 'SC', 'Bandeirante', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4202099', 'SC', 'Barra Bonita', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4202107', 'SC', 'Barra Velha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4202131', 'SC', 'Bela Vista do Toldo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4202156', 'SC', 'Belmonte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4202206', 'SC', 'Benedito Novo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4202305', 'SC', 'Biguaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4202404', 'SC', 'Blumenau', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4202438', 'SC', 'Bocaina do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4202503', 'SC', 'Bom Jardim da Serra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4202537', 'SC', 'Bom Jesus', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4202578', 'SC', 'Bom Jesus do Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4202602', 'SC', 'Bom Retiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4202453', 'SC', 'Bombinhas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4202701', 'SC', 'Botuverá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4202800', 'SC', 'Braço do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4202859', 'SC', 'Braço do Trombudo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4202875', 'SC', 'Brunópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4202909', 'SC', 'Brusque', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4203006', 'SC', 'Caçador', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4203105', 'SC', 'Caibi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4203154', 'SC', 'Calmon', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4203204', 'SC', 'Camboriú', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4203303', 'SC', 'Campo Alegre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4203402', 'SC', 'Campo Belo do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4203501', 'SC', 'Campo Erê', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4203600', 'SC', 'Campos Novos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4203709', 'SC', 'Canelinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4203808', 'SC', 'Canoinhas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4203253', 'SC', 'Capão Alto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4203907', 'SC', 'Capinzal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4203956', 'SC', 'Capivari de Baixo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4204004', 'SC', 'Catanduvas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4204103', 'SC', 'Caxambu do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4204152', 'SC', 'Celso Ramos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4204178', 'SC', 'Cerro Negro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4204194', 'SC', 'Chapadão do Lageado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4204202', 'SC', 'Chapecó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4204251', 'SC', 'Cocal do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4204301', 'SC', 'Concórdia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4204350', 'SC', 'Cordilheira Alta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4204400', 'SC', 'Coronel Freitas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4204459', 'SC', 'Coronel Martins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4204558', 'SC', 'Correia Pinto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4204509', 'SC', 'Corupá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4204608', 'SC', 'Criciúma', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4204707', 'SC', 'Cunha Porã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4204756', 'SC', 'Cunhataí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4204806', 'SC', 'Curitibanos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4204905', 'SC', 'Descanso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4205001', 'SC', 'Dionísio Cerqueira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4205100', 'SC', 'Dona Emma', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4205159', 'SC', 'Doutor Pedrinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4205175', 'SC', 'Entre Rios', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4205191', 'SC', 'Ermo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4205209', 'SC', 'Erval Velho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4205308', 'SC', 'Faxinal dos Guedes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4205357', 'SC', 'Flor do Sertão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4205407', 'SC', 'Florianópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4205431', 'SC', 'Formosa do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4205456', 'SC', 'Forquilhinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4205506', 'SC', 'Fraiburgo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4205555', 'SC', 'Frei Rogério', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4205605', 'SC', 'Galvão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4205704', 'SC', 'Garopaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4205803', 'SC', 'Garuva', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4205902', 'SC', 'Gaspar', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4206009', 'SC', 'Governador Celso Ramos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4206108', 'SC', 'Grão Pará', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4206207', 'SC', 'Gravatal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4206306', 'SC', 'Guabiruba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4206405', 'SC', 'Guaraciaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4206504', 'SC', 'Guaramirim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4206603', 'SC', 'Guarujá do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4206652', 'SC', 'Guatambú', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4206702', 'SC', 'Herval d''Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4206751', 'SC', 'Ibiam', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4206801', 'SC', 'Ibicaré', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4206900', 'SC', 'Ibirama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4207007', 'SC', 'Içara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4207106', 'SC', 'Ilhota', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4207205', 'SC', 'Imaruí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4207304', 'SC', 'Imbituba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4207403', 'SC', 'Imbuia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4207502', 'SC', 'Indaial', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4207577', 'SC', 'Iomerê', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4207601', 'SC', 'Ipira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4207650', 'SC', 'Iporã do Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4207684', 'SC', 'Ipuaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4207700', 'SC', 'Ipumirim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4207759', 'SC', 'Iraceminha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4207809', 'SC', 'Irani', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4207858', 'SC', 'Irati', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4207908', 'SC', 'Irineópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4208005', 'SC', 'Itá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4208104', 'SC', 'Itaiópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4208203', 'SC', 'Itajaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4208302', 'SC', 'Itapema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4208401', 'SC', 'Itapiranga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4208450', 'SC', 'Itapoá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4208500', 'SC', 'Ituporanga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4208609', 'SC', 'Jaborá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4208708', 'SC', 'Jacinto Machado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4208807', 'SC', 'Jaguaruna', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4208906', 'SC', 'Jaraguá do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4208955', 'SC', 'Jardinópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4209003', 'SC', 'Joaçaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4209102', 'SC', 'Joinville', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4209151', 'SC', 'José Boiteux', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4209177', 'SC', 'Jupiá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4209201', 'SC', 'Lacerdópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4209300', 'SC', 'Lages', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4209409', 'SC', 'Laguna', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4209458', 'SC', 'Lajeado Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4209508', 'SC', 'Laurentino', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4209607', 'SC', 'Lauro Muller', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4209706', 'SC', 'Lebon Régis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4209805', 'SC', 'Leoberto Leal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4209854', 'SC', 'Lindóia do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4209904', 'SC', 'Lontras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4210001', 'SC', 'Luiz Alves', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4210035', 'SC', 'Luzerna', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4210050', 'SC', 'Macieira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4210100', 'SC', 'Mafra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4210209', 'SC', 'Major Gercino', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4210308', 'SC', 'Major Vieira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4210407', 'SC', 'Maracajá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4210506', 'SC', 'Maravilha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4210555', 'SC', 'Marema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4210605', 'SC', 'Massaranduba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4210704', 'SC', 'Matos Costa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4210803', 'SC', 'Meleiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4210852', 'SC', 'Mirim Doce', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4210902', 'SC', 'Modelo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4211009', 'SC', 'Mondaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4211058', 'SC', 'Monte Carlo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4211108', 'SC', 'Monte Castelo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4211207', 'SC', 'Morro da Fumaça', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4211256', 'SC', 'Morro Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4211306', 'SC', 'Navegantes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4211405', 'SC', 'Nova Erechim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4211454', 'SC', 'Nova Itaberaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4211504', 'SC', 'Nova Trento', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4211603', 'SC', 'Nova Veneza', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4211652', 'SC', 'Novo Horizonte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4211702', 'SC', 'Orleans', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4211751', 'SC', 'Otacílio Costa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4211801', 'SC', 'Ouro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4211850', 'SC', 'Ouro Verde', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4211876', 'SC', 'Paial', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4211892', 'SC', 'Painel', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4211900', 'SC', 'Palhoça', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4212007', 'SC', 'Palma Sola', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4212056', 'SC', 'Palmeira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4212106', 'SC', 'Palmitos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4212205', 'SC', 'Papanduva', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4212239', 'SC', 'Paraíso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4212254', 'SC', 'Passo de Torres', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4212270', 'SC', 'Passos Maia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4212304', 'SC', 'Paulo Lopes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4212403', 'SC', 'Pedras Grandes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4212502', 'SC', 'Penha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4212601', 'SC', 'Peritiba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4212700', 'SC', 'Petrolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4212908', 'SC', 'Pinhalzinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4213005', 'SC', 'Pinheiro Preto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4213104', 'SC', 'Piratuba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4213153', 'SC', 'Planalto Alegre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4213203', 'SC', 'Pomerode', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4213302', 'SC', 'Ponte Alta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4213351', 'SC', 'Ponte Alta do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4213401', 'SC', 'Ponte Serrada', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4213500', 'SC', 'Porto Belo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4213609', 'SC', 'Porto União', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4213708', 'SC', 'Pouso Redondo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4213807', 'SC', 'Praia Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4213906', 'SC', 'Presidente Castello Branco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4214003', 'SC', 'Presidente Getúlio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4214102', 'SC', 'Presidente Nereu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4214151', 'SC', 'Princesa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4214201', 'SC', 'Quilombo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4214300', 'SC', 'Rancho Queimado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4214409', 'SC', 'Rio das Antas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4214508', 'SC', 'Rio do Campo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4214607', 'SC', 'Rio do Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4214805', 'SC', 'Rio do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4214706', 'SC', 'Rio dos Cedros', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4214904', 'SC', 'Rio Fortuna', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4215000', 'SC', 'Rio Negrinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4215059', 'SC', 'Rio Rufino', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4215075', 'SC', 'Riqueza', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4215109', 'SC', 'Rodeio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4215208', 'SC', 'Romelândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4215307', 'SC', 'Salete', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4215356', 'SC', 'Saltinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4215406', 'SC', 'Salto Veloso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4215455', 'SC', 'Sangão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4215505', 'SC', 'Santa Cecília', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4215554', 'SC', 'Santa Helena', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4215604', 'SC', 'Santa Rosa de Lima', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4215653', 'SC', 'Santa Rosa do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4215679', 'SC', 'Santa Terezinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4215687', 'SC', 'Santa Terezinha do Progresso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4215695', 'SC', 'Santiago do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4215703', 'SC', 'Santo Amaro da Imperatriz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4215802', 'SC', 'São Bento do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4215752', 'SC', 'São Bernardino', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4215901', 'SC', 'São Bonifácio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4216008', 'SC', 'São Carlos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4216057', 'SC', 'São Cristovão do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4216107', 'SC', 'São Domingos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4216206', 'SC', 'São Francisco do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4216305', 'SC', 'São João Batista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4216354', 'SC', 'São João do Itaperiú', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4216255', 'SC', 'São João do Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4216404', 'SC', 'São João do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4216503', 'SC', 'São Joaquim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4216602', 'SC', 'São José', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4216701', 'SC', 'São José do Cedro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4216800', 'SC', 'São José do Cerrito', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4216909', 'SC', 'São Lourenço do Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4217006', 'SC', 'São Ludgero', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4217105', 'SC', 'São Martinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4217154', 'SC', 'São Miguel da Boa Vista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4217204', 'SC', 'São Miguel do Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4217253', 'SC', 'São Pedro de Alcântara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4217303', 'SC', 'Saudades', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4217402', 'SC', 'Schroeder', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4217501', 'SC', 'Seara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4217550', 'SC', 'Serra Alta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4217600', 'SC', 'Siderópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4217709', 'SC', 'Sombrio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4217758', 'SC', 'Sul Brasil', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4217808', 'SC', 'Taió', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4217907', 'SC', 'Tangará', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4217956', 'SC', 'Tigrinhos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4218004', 'SC', 'Tijucas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4218103', 'SC', 'Timbé do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4218202', 'SC', 'Timbó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4218251', 'SC', 'Timbó Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4218301', 'SC', 'Três Barras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4218350', 'SC', 'Treviso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4218400', 'SC', 'Treze de Maio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4218509', 'SC', 'Treze Tílias', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4218608', 'SC', 'Trombudo Central', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4218707', 'SC', 'Tubarão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4218756', 'SC', 'Tunápolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4218806', 'SC', 'Turvo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4218855', 'SC', 'União do Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4218905', 'SC', 'Urubici', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4218954', 'SC', 'Urupema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4219002', 'SC', 'Urussanga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4219101', 'SC', 'Vargeão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4219150', 'SC', 'Vargem', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4219176', 'SC', 'Vargem Bonita', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4219200', 'SC', 'Vidal Ramos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4219309', 'SC', 'Videira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4219358', 'SC', 'Vitor Meireles', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4219408', 'SC', 'Witmarsum', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4219507', 'SC', 'Xanxerê', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4219606', 'SC', 'Xavantina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4219705', 'SC', 'Xaxim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4219853', 'SC', 'Zortéa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4300034', 'RS', 'Aceguá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4300059', 'RS', 'Água Santa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4300109', 'RS', 'Agudo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4300208', 'RS', 'Ajuricaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4300307', 'RS', 'Alecrim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4300406', 'RS', 'Alegrete', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4300455', 'RS', 'Alegria', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4300471', 'RS', 'Almirante Tamandaré do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4300505', 'RS', 'Alpestre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4300554', 'RS', 'Alto Alegre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4300570', 'RS', 'Alto Feliz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4300604', 'RS', 'Alvorada', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4300638', 'RS', 'Amaral Ferrador', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4300646', 'RS', 'Ametista do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4300661', 'RS', 'André da Rocha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4300703', 'RS', 'Anta Gorda', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4300802', 'RS', 'Antônio Prado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4300851', 'RS', 'Arambaré', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4300877', 'RS', 'Araricá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4300901', 'RS', 'Aratiba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4301008', 'RS', 'Arroio do Meio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4301073', 'RS', 'Arroio do Padre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4301057', 'RS', 'Arroio do Sal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4301206', 'RS', 'Arroio do Tigre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4301107', 'RS', 'Arroio dos Ratos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4301305', 'RS', 'Arroio Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4301404', 'RS', 'Arvorezinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4301503', 'RS', 'Augusto Pestana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4301552', 'RS', 'Áurea', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4301602', 'RS', 'Bagé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4301636', 'RS', 'Balneário Pinhal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4301651', 'RS', 'Barão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4301701', 'RS', 'Barão de Cotegipe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4301750', 'RS', 'Barão do Triunfo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4301859', 'RS', 'Barra do Guarita', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4301875', 'RS', 'Barra do Quaraí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4301909', 'RS', 'Barra do Ribeiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4301925', 'RS', 'Barra do Rio Azul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4301958', 'RS', 'Barra Funda', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4301800', 'RS', 'Barracão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4302006', 'RS', 'Barros Cassal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4302055', 'RS', 'Benjamin Constant do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4302105', 'RS', 'Bento Gonçalves', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4302154', 'RS', 'Boa Vista das Missões', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4302204', 'RS', 'Boa Vista do Buricá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4302220', 'RS', 'Boa Vista do Cadeado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4302238', 'RS', 'Boa Vista do Incra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4302253', 'RS', 'Boa Vista do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4302303', 'RS', 'Bom Jesus', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4302352', 'RS', 'Bom Princípio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4302378', 'RS', 'Bom Progresso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4302402', 'RS', 'Bom Retiro do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4302451', 'RS', 'Boqueirão do Leão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4302501', 'RS', 'Bossoroca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4302584', 'RS', 'Bozano', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4302600', 'RS', 'Braga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4302659', 'RS', 'Brochier', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4302709', 'RS', 'Butiá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4302808', 'RS', 'Caçapava do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4302907', 'RS', 'Cacequi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4303004', 'RS', 'Cachoeira do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4303103', 'RS', 'Cachoeirinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4303202', 'RS', 'Cacique Doble', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4303301', 'RS', 'Caibaté', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4303400', 'RS', 'Caiçara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4303509', 'RS', 'Camaquã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4303558', 'RS', 'Camargo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4303608', 'RS', 'Cambará do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4303673', 'RS', 'Campestre da Serra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4303707', 'RS', 'Campina das Missões', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4303806', 'RS', 'Campinas do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4303905', 'RS', 'Campo Bom', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4304002', 'RS', 'Campo Novo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4304101', 'RS', 'Campos Borges', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4304200', 'RS', 'Candelária', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4304309', 'RS', 'Cândido Godói', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4304358', 'RS', 'Candiota', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4304408', 'RS', 'Canela', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4304507', 'RS', 'Canguçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4304606', 'RS', 'Canoas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4304614', 'RS', 'Canudos do Vale', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4304622', 'RS', 'Capão Bonito do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4304630', 'RS', 'Capão da Canoa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4304655', 'RS', 'Capão do Cipó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4304663', 'RS', 'Capão do Leão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4304689', 'RS', 'Capela de Santana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4304697', 'RS', 'Capitão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4304671', 'RS', 'Capivari do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4304713', 'RS', 'Caraá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4304705', 'RS', 'Carazinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4304804', 'RS', 'Carlos Barbosa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4304853', 'RS', 'Carlos Gomes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4304903', 'RS', 'Casca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4304952', 'RS', 'Caseiros', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4305009', 'RS', 'Catuípe', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4305108', 'RS', 'Caxias do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4305116', 'RS', 'Centenário', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4305124', 'RS', 'Cerrito', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4305132', 'RS', 'Cerro Branco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4305157', 'RS', 'Cerro Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4305173', 'RS', 'Cerro Grande do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4305207', 'RS', 'Cerro Largo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4305306', 'RS', 'Chapada', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4305355', 'RS', 'Charqueadas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4305371', 'RS', 'Charrua', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4305405', 'RS', 'Chiapetta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4305439', 'RS', 'Chuí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4305447', 'RS', 'Chuvisca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4305454', 'RS', 'Cidreira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4305504', 'RS', 'Ciríaco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4305587', 'RS', 'Colinas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4305603', 'RS', 'Colorado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4305702', 'RS', 'Condor', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4305801', 'RS', 'Constantina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4305835', 'RS', 'Coqueiro Baixo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4305850', 'RS', 'Coqueiros do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4305871', 'RS', 'Coronel Barros', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4305900', 'RS', 'Coronel Bicaco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4305934', 'RS', 'Coronel Pilar', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4305959', 'RS', 'Cotiporã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4305975', 'RS', 'Coxilha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4306007', 'RS', 'Crissiumal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4306056', 'RS', 'Cristal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4306072', 'RS', 'Cristal do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4306106', 'RS', 'Cruz Alta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4306130', 'RS', 'Cruzaltense', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4306205', 'RS', 'Cruzeiro do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4306304', 'RS', 'David Canabarro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4306320', 'RS', 'Derrubadas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4306353', 'RS', 'Dezesseis de Novembro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4306379', 'RS', 'Dilermando de Aguiar', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4306403', 'RS', 'Dois Irmãos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4306429', 'RS', 'Dois Irmãos das Missões', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4306452', 'RS', 'Dois Lajeados', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4306502', 'RS', 'Dom Feliciano', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4306601', 'RS', 'Dom Pedrito', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4306551', 'RS', 'Dom Pedro de Alcântara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4306700', 'RS', 'Dona Francisca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4306734', 'RS', 'Doutor Maurício Cardoso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4306759', 'RS', 'Doutor Ricardo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4306767', 'RS', 'Eldorado do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4306809', 'RS', 'Encantado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4306908', 'RS', 'Encruzilhada do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4306924', 'RS', 'Engenho Velho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4306957', 'RS', 'Entre Rios do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4306932', 'RS', 'Entre-Ijuís', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4306973', 'RS', 'Erebango', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4307005', 'RS', 'Erechim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4307054', 'RS', 'Ernestina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4307203', 'RS', 'Erval Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4307302', 'RS', 'Erval Seco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4307401', 'RS', 'Esmeralda', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4307450', 'RS', 'Esperança do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4307500', 'RS', 'Espumoso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4307559', 'RS', 'Estação', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4307609', 'RS', 'Estância Velha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4307708', 'RS', 'Esteio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4307807', 'RS', 'Estrela', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4307815', 'RS', 'Estrela Velha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4307831', 'RS', 'Eugênio de Castro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4307864', 'RS', 'Fagundes Varela', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4307906', 'RS', 'Farroupilha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4308003', 'RS', 'Faxinal do Soturno', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4308052', 'RS', 'Faxinalzinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4308078', 'RS', 'Fazenda Vilanova', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4308102', 'RS', 'Feliz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4308201', 'RS', 'Flores da Cunha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4308250', 'RS', 'Floriano Peixoto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4308300', 'RS', 'Fontoura Xavier', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4308409', 'RS', 'Formigueiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4308433', 'RS', 'Forquetinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4308458', 'RS', 'Fortaleza dos Valos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4308508', 'RS', 'Frederico Westphalen', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4308607', 'RS', 'Garibaldi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4308656', 'RS', 'Garruchos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4308706', 'RS', 'Gaurama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4308805', 'RS', 'General Câmara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4308854', 'RS', 'Gentil', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4308904', 'RS', 'Getúlio Vargas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4309001', 'RS', 'Giruá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4309050', 'RS', 'Glorinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4309100', 'RS', 'Gramado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4309126', 'RS', 'Gramado dos Loureiros', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4309159', 'RS', 'Gramado Xavier', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4309209', 'RS', 'Gravataí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4309258', 'RS', 'Guabiju', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4309308', 'RS', 'Guaíba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4309407', 'RS', 'Guaporé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4309506', 'RS', 'Guarani das Missões', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4309555', 'RS', 'Harmonia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4307104', 'RS', 'Herval', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4309571', 'RS', 'Herveiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4309605', 'RS', 'Horizontina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4309654', 'RS', 'Hulha Negra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4309704', 'RS', 'Humaitá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4309753', 'RS', 'Ibarama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4309803', 'RS', 'Ibiaçá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4309902', 'RS', 'Ibiraiaras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4309951', 'RS', 'Ibirapuitã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4310009', 'RS', 'Ibirubá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4310108', 'RS', 'Igrejinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4310207', 'RS', 'Ijuí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4310306', 'RS', 'Ilópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4310330', 'RS', 'Imbé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4310363', 'RS', 'Imigrante', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4310405', 'RS', 'Independência', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4310413', 'RS', 'Inhacorá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4310439', 'RS', 'Ipê', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4310462', 'RS', 'Ipiranga do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4310504', 'RS', 'Iraí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4310538', 'RS', 'Itaara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4310553', 'RS', 'Itacurubi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4310579', 'RS', 'Itapuca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4310603', 'RS', 'Itaqui', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4310652', 'RS', 'Itati', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4310702', 'RS', 'Itatiba do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4310751', 'RS', 'Ivorá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4310801', 'RS', 'Ivoti', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4310850', 'RS', 'Jaboticaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4310876', 'RS', 'Jacuizinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4310900', 'RS', 'Jacutinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4311007', 'RS', 'Jaguarão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4311106', 'RS', 'Jaguari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4311122', 'RS', 'Jaquirana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4311130', 'RS', 'Jari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4311155', 'RS', 'Jóia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4311205', 'RS', 'Júlio de Castilhos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4311239', 'RS', 'Lagoa Bonita do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4311270', 'RS', 'Lagoa dos Três Cantos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4311304', 'RS', 'Lagoa Vermelha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4311254', 'RS', 'Lagoão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4311403', 'RS', 'Lajeado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4311429', 'RS', 'Lajeado do Bugre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4311502', 'RS', 'Lavras do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4311601', 'RS', 'Liberato Salzano', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4311627', 'RS', 'Lindolfo Collor', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4311643', 'RS', 'Linha Nova', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4311718', 'RS', 'Maçambará', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4311700', 'RS', 'Machadinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4311734', 'RS', 'Mampituba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4311759', 'RS', 'Manoel Viana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4311775', 'RS', 'Maquiné', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4311791', 'RS', 'Maratá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4311809', 'RS', 'Marau', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4311908', 'RS', 'Marcelino Ramos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4311981', 'RS', 'Mariana Pimentel', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4312005', 'RS', 'Mariano Moro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4312054', 'RS', 'Marques de Souza', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4312104', 'RS', 'Mata', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4312138', 'RS', 'Mato Castelhano', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4312153', 'RS', 'Mato Leitão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4312179', 'RS', 'Mato Queimado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4312203', 'RS', 'Maximiliano de Almeida', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4312252', 'RS', 'Minas do Leão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4312302', 'RS', 'Miraguaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4312351', 'RS', 'Montauri', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4312377', 'RS', 'Monte Alegre dos Campos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4312385', 'RS', 'Monte Belo do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4312401', 'RS', 'Montenegro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4312427', 'RS', 'Mormaço', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4312443', 'RS', 'Morrinhos do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4312450', 'RS', 'Morro Redondo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4312476', 'RS', 'Morro Reuter', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4312500', 'RS', 'Mostardas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4312609', 'RS', 'Muçum', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4312617', 'RS', 'Muitos Capões', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4312625', 'RS', 'Muliterno', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4312658', 'RS', 'Não-Me-Toque', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4312674', 'RS', 'Nicolau Vergueiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4312708', 'RS', 'Nonoai', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4312757', 'RS', 'Nova Alvorada', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4312807', 'RS', 'Nova Araçá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4312906', 'RS', 'Nova Bassano', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4312955', 'RS', 'Nova Boa Vista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4313003', 'RS', 'Nova Bréscia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4313011', 'RS', 'Nova Candelária', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4313037', 'RS', 'Nova Esperança do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4313060', 'RS', 'Nova Hartz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4313086', 'RS', 'Nova Pádua', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4313102', 'RS', 'Nova Palma', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4313201', 'RS', 'Nova Petrópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4313300', 'RS', 'Nova Prata', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4313334', 'RS', 'Nova Ramada', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4313359', 'RS', 'Nova Roma do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4313375', 'RS', 'Nova Santa Rita', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4313490', 'RS', 'Novo Barreiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4313391', 'RS', 'Novo Cabrais', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4313409', 'RS', 'Novo Hamburgo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4313425', 'RS', 'Novo Machado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4313441', 'RS', 'Novo Tiradentes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4313466', 'RS', 'Novo Xingu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4313508', 'RS', 'Osório', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4313607', 'RS', 'Paim Filho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4313656', 'RS', 'Palmares do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4313706', 'RS', 'Palmeira das Missões', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4313805', 'RS', 'Palmitinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4313904', 'RS', 'Panambi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4313953', 'RS', 'Pantano Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4314001', 'RS', 'Paraí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4314027', 'RS', 'Paraíso do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4314035', 'RS', 'Pareci Novo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4314050', 'RS', 'Parobé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4314068', 'RS', 'Passa Sete', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4314076', 'RS', 'Passo do Sobrado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4314100', 'RS', 'Passo Fundo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4314134', 'RS', 'Paulo Bento', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4314159', 'RS', 'Paverama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4314175', 'RS', 'Pedras Altas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4314209', 'RS', 'Pedro Osório', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4314308', 'RS', 'Pejuçara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4314407', 'RS', 'Pelotas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4314423', 'RS', 'Picada Café', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4314456', 'RS', 'Pinhal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4314464', 'RS', 'Pinhal da Serra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4314472', 'RS', 'Pinhal Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4314498', 'RS', 'Pinheirinho do Vale', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4314506', 'RS', 'Pinheiro Machado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4314555', 'RS', 'Pirapó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4314605', 'RS', 'Piratini', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4314704', 'RS', 'Planalto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4314753', 'RS', 'Poço das Antas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4314779', 'RS', 'Pontão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4314787', 'RS', 'Ponte Preta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4314803', 'RS', 'Portão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4314902', 'RS', 'Porto Alegre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4315008', 'RS', 'Porto Lucena', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4315057', 'RS', 'Porto Mauá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4315073', 'RS', 'Porto Vera Cruz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4315107', 'RS', 'Porto Xavier', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4315131', 'RS', 'Pouso Novo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4315149', 'RS', 'Presidente Lucena', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4315156', 'RS', 'Progresso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4315172', 'RS', 'Protásio Alves', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4315206', 'RS', 'Putinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4315305', 'RS', 'Quaraí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4315313', 'RS', 'Quatro Irmãos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4315321', 'RS', 'Quevedos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4315354', 'RS', 'Quinze de Novembro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4315404', 'RS', 'Redentora', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4315453', 'RS', 'Relvado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4315503', 'RS', 'Restinga Seca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4315552', 'RS', 'Rio dos Índios', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4315602', 'RS', 'Rio Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4315701', 'RS', 'Rio Pardo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4315750', 'RS', 'Riozinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4315800', 'RS', 'Roca Sales', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4315909', 'RS', 'Rodeio Bonito', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4315958', 'RS', 'Rolador', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4316006', 'RS', 'Rolante', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4316105', 'RS', 'Ronda Alta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4316204', 'RS', 'Rondinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4316303', 'RS', 'Roque Gonzales', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4316402', 'RS', 'Rosário do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4316428', 'RS', 'Sagrada Família', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4316436', 'RS', 'Saldanha Marinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4316451', 'RS', 'Salto do Jacuí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4316477', 'RS', 'Salvador das Missões', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4316501', 'RS', 'Salvador do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4316600', 'RS', 'Sananduva', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4316709', 'RS', 'Santa Bárbara do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4316733', 'RS', 'Santa Cecília do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4316758', 'RS', 'Santa Clara do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4316808', 'RS', 'Santa Cruz do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4316972', 'RS', 'Santa Margarida do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4316907', 'RS', 'Santa Maria', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4316956', 'RS', 'Santa Maria do Herval', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4317202', 'RS', 'Santa Rosa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4317251', 'RS', 'Santa Tereza', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4317301', 'RS', 'Santa Vitória do Palmar', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4317004', 'RS', 'Santana da Boa Vista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4317103', 'RS', 'Sant''Ana do Livramento', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4317400', 'RS', 'Santiago', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4317509', 'RS', 'Santo Ângelo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4317608', 'RS', 'Santo Antônio da Patrulha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4317707', 'RS', 'Santo Antônio das Missões', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4317558', 'RS', 'Santo Antônio do Palma', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4317756', 'RS', 'Santo Antônio do Planalto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4317806', 'RS', 'Santo Augusto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4317905', 'RS', 'Santo Cristo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4317954', 'RS', 'Santo Expedito do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4318002', 'RS', 'São Borja', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4318051', 'RS', 'São Domingos do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4318101', 'RS', 'São Francisco de Assis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4318200', 'RS', 'São Francisco de Paula', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4318309', 'RS', 'São Gabriel', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4318408', 'RS', 'São Jerônimo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4318424', 'RS', 'São João da Urtiga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4318432', 'RS', 'São João do Polêsine', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4318440', 'RS', 'São Jorge', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4318457', 'RS', 'São José das Missões', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4318465', 'RS', 'São José do Herval', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4318481', 'RS', 'São José do Hortêncio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4318499', 'RS', 'São José do Inhacorá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4318507', 'RS', 'São José do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4318606', 'RS', 'São José do Ouro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4318614', 'RS', 'São José do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4318622', 'RS', 'São José dos Ausentes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4318705', 'RS', 'São Leopoldo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4318804', 'RS', 'São Lourenço do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4318903', 'RS', 'São Luiz Gonzaga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4319000', 'RS', 'São Marcos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4319109', 'RS', 'São Martinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4319125', 'RS', 'São Martinho da Serra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4319158', 'RS', 'São Miguel das Missões', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4319208', 'RS', 'São Nicolau', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4319307', 'RS', 'São Paulo das Missões', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4319356', 'RS', 'São Pedro da Serra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4319364', 'RS', 'São Pedro das Missões', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4319372', 'RS', 'São Pedro do Butiá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4319406', 'RS', 'São Pedro do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4319505', 'RS', 'São Sebastião do Caí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4319604', 'RS', 'São Sepé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4319703', 'RS', 'São Valentim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4319711', 'RS', 'São Valentim do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4319737', 'RS', 'São Valério do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4319752', 'RS', 'São Vendelino', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4319802', 'RS', 'São Vicente do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4319901', 'RS', 'Sapiranga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4320008', 'RS', 'Sapucaia do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4320107', 'RS', 'Sarandi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4320206', 'RS', 'Seberi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4320230', 'RS', 'Sede Nova', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4320263', 'RS', 'Segredo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4320305', 'RS', 'Selbach', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4320321', 'RS', 'Senador Salgado Filho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4320354', 'RS', 'Sentinela do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4320404', 'RS', 'Serafina Corrêa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4320453', 'RS', 'Sério', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4320503', 'RS', 'Sertão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4320552', 'RS', 'Sertão Santana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4320578', 'RS', 'Sete de Setembro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4320602', 'RS', 'Severiano de Almeida', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4320651', 'RS', 'Silveira Martins', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4320677', 'RS', 'Sinimbu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4320701', 'RS', 'Sobradinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4320800', 'RS', 'Soledade', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4320859', 'RS', 'Tabaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4320909', 'RS', 'Tapejara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4321006', 'RS', 'Tapera', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4321105', 'RS', 'Tapes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4321204', 'RS', 'Taquara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4321303', 'RS', 'Taquari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4321329', 'RS', 'Taquaruçu do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4321352', 'RS', 'Tavares', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4321402', 'RS', 'Tenente Portela', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4321436', 'RS', 'Terra de Areia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4321451', 'RS', 'Teutônia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4321469', 'RS', 'Tio Hugo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4321477', 'RS', 'Tiradentes do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4321493', 'RS', 'Toropi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4321501', 'RS', 'Torres', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4321600', 'RS', 'Tramandaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4321626', 'RS', 'Travesseiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4321634', 'RS', 'Três Arroios', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4321667', 'RS', 'Três Cachoeiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4321709', 'RS', 'Três Coroas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4321808', 'RS', 'Três de Maio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4321832', 'RS', 'Três Forquilhas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4321857', 'RS', 'Três Palmeiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4321907', 'RS', 'Três Passos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4321956', 'RS', 'Trindade do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4322004', 'RS', 'Triunfo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4322103', 'RS', 'Tucunduva', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4322152', 'RS', 'Tunas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4322186', 'RS', 'Tupanci do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4322202', 'RS', 'Tupanciretã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4322251', 'RS', 'Tupandi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4322301', 'RS', 'Tuparendi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4322327', 'RS', 'Turuçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4322343', 'RS', 'Ubiretama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4322350', 'RS', 'União da Serra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4322376', 'RS', 'Unistalda', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4322400', 'RS', 'Uruguaiana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4322509', 'RS', 'Vacaria', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4322533', 'RS', 'Vale do Sol', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4322541', 'RS', 'Vale Real', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4322525', 'RS', 'Vale Verde', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4322558', 'RS', 'Vanini', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4322608', 'RS', 'Venâncio Aires', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4322707', 'RS', 'Vera Cruz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4322806', 'RS', 'Veranópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4322855', 'RS', 'Vespasiano Correa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4322905', 'RS', 'Viadutos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4323002', 'RS', 'Viamão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4323101', 'RS', 'Vicente Dutra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4323200', 'RS', 'Victor Graeff', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4323309', 'RS', 'Vila Flores', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4323358', 'RS', 'Vila Lângaro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4323408', 'RS', 'Vila Maria', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4323457', 'RS', 'Vila Nova do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4323507', 'RS', 'Vista Alegre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4323606', 'RS', 'Vista Alegre do Prata', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4323705', 'RS', 'Vista Gaúcha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4323754', 'RS', 'Vitória das Missões', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4323770', 'RS', 'Westfalia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('4323804', 'RS', 'Xangri-lá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5000203', 'MS', 'Água Clara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5000252', 'MS', 'Alcinópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5000609', 'MS', 'Amambai', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5000708', 'MS', 'Anastácio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5000807', 'MS', 'Anaurilândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5000856', 'MS', 'Angélica', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5000906', 'MS', 'Antônio João', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5001003', 'MS', 'Aparecida do Taboado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5001102', 'MS', 'Aquidauana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5001243', 'MS', 'Aral Moreira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5001508', 'MS', 'Bandeirantes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5001904', 'MS', 'Bataguassu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5002001', 'MS', 'Batayporã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5002100', 'MS', 'Bela Vista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5002159', 'MS', 'Bodoquena', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5002209', 'MS', 'Bonito', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5002308', 'MS', 'Brasilândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5002407', 'MS', 'Caarapó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5002605', 'MS', 'Camapuã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5002704', 'MS', 'Campo Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5002803', 'MS', 'Caracol', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5002902', 'MS', 'Cassilândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5002951', 'MS', 'Chapadão do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5003108', 'MS', 'Corguinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5003157', 'MS', 'Coronel Sapucaia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5003207', 'MS', 'Corumbá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5003256', 'MS', 'Costa Rica', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5003306', 'MS', 'Coxim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5003454', 'MS', 'Deodápolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5003488', 'MS', 'Dois Irmãos do Buriti', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5003504', 'MS', 'Douradina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5003702', 'MS', 'Dourados', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5003751', 'MS', 'Eldorado', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5003801', 'MS', 'Fátima do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5003900', 'MS', 'Figueirão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5004007', 'MS', 'Glória de Dourados', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5004106', 'MS', 'Guia Lopes da Laguna', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5004304', 'MS', 'Iguatemi', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5004403', 'MS', 'Inocência', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5004502', 'MS', 'Itaporã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5004601', 'MS', 'Itaquiraí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5004700', 'MS', 'Ivinhema', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5004809', 'MS', 'Japorã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5004908', 'MS', 'Jaraguari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5005004', 'MS', 'Jardim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5005103', 'MS', 'Jateí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5005152', 'MS', 'Juti', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5005202', 'MS', 'Ladário', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5005251', 'MS', 'Laguna Carapã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5005400', 'MS', 'Maracaju', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5005608', 'MS', 'Miranda', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5005681', 'MS', 'Mundo Novo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5005707', 'MS', 'Naviraí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5005806', 'MS', 'Nioaque', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5006002', 'MS', 'Nova Alvorada do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5006200', 'MS', 'Nova Andradina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5006259', 'MS', 'Novo Horizonte do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5006309', 'MS', 'Paranaíba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5006358', 'MS', 'Paranhos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5006408', 'MS', 'Pedro Gomes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5006606', 'MS', 'Ponta Porã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5006903', 'MS', 'Porto Murtinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5007109', 'MS', 'Ribas do Rio Pardo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5007208', 'MS', 'Rio Brilhante', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5007307', 'MS', 'Rio Negro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5007406', 'MS', 'Rio Verde de Mato Grosso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5007505', 'MS', 'Rochedo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5007554', 'MS', 'Santa Rita do Pardo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5007695', 'MS', 'São Gabriel do Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5007802', 'MS', 'Selvíria', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5007703', 'MS', 'Sete Quedas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5007901', 'MS', 'Sidrolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5007935', 'MS', 'Sonora', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5007950', 'MS', 'Tacuru', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5007976', 'MS', 'Taquarussu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5008008', 'MS', 'Terenos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5008305', 'MS', 'Três Lagoas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5008404', 'MS', 'Vicentina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5100102', 'MT', 'Acorizal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5100201', 'MT', 'Água Boa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5100250', 'MT', 'Alta Floresta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5100300', 'MT', 'Alto Araguaia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5100359', 'MT', 'Alto Boa Vista', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5100409', 'MT', 'Alto Garças', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5100508', 'MT', 'Alto Paraguai', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5100607', 'MT', 'Alto Taquari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5100805', 'MT', 'Apiacás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5101001', 'MT', 'Araguaiana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5101209', 'MT', 'Araguainha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5101258', 'MT', 'Araputanga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5101308', 'MT', 'Arenápolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5101407', 'MT', 'Aripuanã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5101605', 'MT', 'Barão de Melgaço', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5101704', 'MT', 'Barra do Bugres', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5101803', 'MT', 'Barra do Garças', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5101852', 'MT', 'Bom Jesus do Araguaia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5101902', 'MT', 'Brasnorte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5102504', 'MT', 'Cáceres', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5102603', 'MT', 'Campinápolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5102637', 'MT', 'Campo Novo do Parecis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5102678', 'MT', 'Campo Verde', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5102686', 'MT', 'Campos de Júlio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5102694', 'MT', 'Canabrava do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5102702', 'MT', 'Canarana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5102793', 'MT', 'Carlinda', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5102850', 'MT', 'Castanheira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5103007', 'MT', 'Chapada dos Guimarães', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5103056', 'MT', 'Cláudia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5103106', 'MT', 'Cocalinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5103205', 'MT', 'Colíder', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5103254', 'MT', 'Colniza', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5103304', 'MT', 'Comodoro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5103353', 'MT', 'Confresa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5103361', 'MT', 'Conquista D''Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5103379', 'MT', 'Cotriguaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5103403', 'MT', 'Cuiabá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5103437', 'MT', 'Curvelândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5103452', 'MT', 'Denise', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5103502', 'MT', 'Diamantino', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5103601', 'MT', 'Dom Aquino', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5103700', 'MT', 'Feliz Natal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5103809', 'MT', 'Figueirópolis D''Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5103858', 'MT', 'Gaúcha do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5103908', 'MT', 'General Carneiro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5103957', 'MT', 'Glória D''Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5104104', 'MT', 'Guarantã do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5104203', 'MT', 'Guiratinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5104500', 'MT', 'Indiavaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5104526', 'MT', 'Ipiranga do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5104542', 'MT', 'Itanhangá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5104559', 'MT', 'Itaúba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5104609', 'MT', 'Itiquira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5104807', 'MT', 'Jaciara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5104906', 'MT', 'Jangada', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5105002', 'MT', 'Jauru', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5105101', 'MT', 'Juara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5105150', 'MT', 'Juína', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5105176', 'MT', 'Juruena', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5105200', 'MT', 'Juscimeira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5105234', 'MT', 'Lambari D''Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5105259', 'MT', 'Lucas do Rio Verde', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5105309', 'MT', 'Luciara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5105580', 'MT', 'Marcelândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5105606', 'MT', 'Matupá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5105622', 'MT', 'Mirassol d''Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5105903', 'MT', 'Nobres', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5106000', 'MT', 'Nortelândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5106109', 'MT', 'Nossa Senhora do Livramento', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5106158', 'MT', 'Nova Bandeirantes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5106208', 'MT', 'Nova Brasilândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5106216', 'MT', 'Nova Canaã do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5108808', 'MT', 'Nova Guarita', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5106182', 'MT', 'Nova Lacerda', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5108857', 'MT', 'Nova Marilândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5108907', 'MT', 'Nova Maringá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5108956', 'MT', 'Nova Monte Verde', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5106224', 'MT', 'Nova Mutum', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5106174', 'MT', 'Nova Nazaré', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5106232', 'MT', 'Nova Olímpia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5106190', 'MT', 'Nova Santa Helena', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5106240', 'MT', 'Nova Ubiratã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5106257', 'MT', 'Nova Xavantina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5106273', 'MT', 'Novo Horizonte do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5106265', 'MT', 'Novo Mundo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5106315', 'MT', 'Novo Santo Antônio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5106281', 'MT', 'Novo São Joaquim', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5106299', 'MT', 'Paranaíta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5106307', 'MT', 'Paranatinga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5106372', 'MT', 'Pedra Preta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5106422', 'MT', 'Peixoto de Azevedo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5106455', 'MT', 'Planalto da Serra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5106505', 'MT', 'Poconé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5106653', 'MT', 'Pontal do Araguaia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5106703', 'MT', 'Ponte Branca', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5106752', 'MT', 'Pontes e Lacerda', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5106778', 'MT', 'Porto Alegre do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5106802', 'MT', 'Porto dos Gaúchos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5106828', 'MT', 'Porto Esperidião', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5106851', 'MT', 'Porto Estrela', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5107008', 'MT', 'Poxoréo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5107040', 'MT', 'Primavera do Leste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5107065', 'MT', 'Querência', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5107156', 'MT', 'Reserva do Cabaçal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5107180', 'MT', 'Ribeirão Cascalheira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5107198', 'MT', 'Ribeirãozinho', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5107206', 'MT', 'Rio Branco', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5107578', 'MT', 'Rondolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5107602', 'MT', 'Rondonópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5107701', 'MT', 'Rosário Oeste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5107750', 'MT', 'Salto do Céu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5107248', 'MT', 'Santa Carmem', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5107743', 'MT', 'Santa Cruz do Xingu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5107768', 'MT', 'Santa Rita do Trivelato', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5107776', 'MT', 'Santa Terezinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5107263', 'MT', 'Santo Afonso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5107792', 'MT', 'Santo Antônio do Leste', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5107800', 'MT', 'Santo Antônio do Leverger', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5107859', 'MT', 'São Félix do Araguaia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5107297', 'MT', 'São José do Povo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5107305', 'MT', 'São José do Rio Claro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5107354', 'MT', 'São José do Xingu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5107107', 'MT', 'São José dos Quatro Marcos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5107404', 'MT', 'São Pedro da Cipa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5107875', 'MT', 'Sapezal', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5107883', 'MT', 'Serra Nova Dourada', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5107909', 'MT', 'Sinop', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5107925', 'MT', 'Sorriso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5107941', 'MT', 'Tabaporã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5107958', 'MT', 'Tangará da Serra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5108006', 'MT', 'Tapurah', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5108055', 'MT', 'Terra Nova do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5108105', 'MT', 'Tesouro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5108204', 'MT', 'Torixoréu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5108303', 'MT', 'União do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5108352', 'MT', 'Vale de São Domingos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5108402', 'MT', 'Várzea Grande', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5108501', 'MT', 'Vera', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5105507', 'MT', 'Vila Bela da Santíssima Trindade', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5108600', 'MT', 'Vila Rica', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5200050', 'GO', 'Abadia de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5200100', 'GO', 'Abadiânia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5200134', 'GO', 'Acreúna', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5200159', 'GO', 'Adelândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5200175', 'GO', 'Água Fria de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5200209', 'GO', 'Água Limpa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5200258', 'GO', 'Águas Lindas de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5200308', 'GO', 'Alexânia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5200506', 'GO', 'Aloândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5200555', 'GO', 'Alto Horizonte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5200605', 'GO', 'Alto Paraíso de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5200803', 'GO', 'Alvorada do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5200829', 'GO', 'Amaralina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5200852', 'GO', 'Americano do Brasil', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5200902', 'GO', 'Amorinópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5201108', 'GO', 'Anápolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5201207', 'GO', 'Anhanguera', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5201306', 'GO', 'Anicuns', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5201405', 'GO', 'Aparecida de Goiânia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5201454', 'GO', 'Aparecida do Rio Doce', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5201504', 'GO', 'Aporé', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5201603', 'GO', 'Araçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5201702', 'GO', 'Aragarças', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5201801', 'GO', 'Aragoiânia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5202155', 'GO', 'Araguapaz', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5202353', 'GO', 'Arenópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5202502', 'GO', 'Aruanã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5202601', 'GO', 'Aurilândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5202809', 'GO', 'Avelinópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5203104', 'GO', 'Baliza', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5203203', 'GO', 'Barro Alto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5203302', 'GO', 'Bela Vista de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5203401', 'GO', 'Bom Jardim de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5203500', 'GO', 'Bom Jesus de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5203559', 'GO', 'Bonfinópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5203575', 'GO', 'Bonópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5203609', 'GO', 'Brazabrantes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5203807', 'GO', 'Britânia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5203906', 'GO', 'Buriti Alegre', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5203939', 'GO', 'Buriti de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5203962', 'GO', 'Buritinópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5204003', 'GO', 'Cabeceiras', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5204102', 'GO', 'Cachoeira Alta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5204201', 'GO', 'Cachoeira de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5204250', 'GO', 'Cachoeira Dourada', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5204300', 'GO', 'Caçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5204409', 'GO', 'Caiapônia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5204508', 'GO', 'Caldas Novas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5204557', 'GO', 'Caldazinha', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5204607', 'GO', 'Campestre de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5204656', 'GO', 'Campinaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5204706', 'GO', 'Campinorte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5204805', 'GO', 'Campo Alegre de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5204854', 'GO', 'Campo Limpo de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5204904', 'GO', 'Campos Belos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5204953', 'GO', 'Campos Verdes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5205000', 'GO', 'Carmo do Rio Verde', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5205059', 'GO', 'Castelândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5205109', 'GO', 'Catalão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5205208', 'GO', 'Caturaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5205307', 'GO', 'Cavalcante', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5205406', 'GO', 'Ceres', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5205455', 'GO', 'Cezarina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5205471', 'GO', 'Chapadão do Céu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5205497', 'GO', 'Cidade Ocidental', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5205513', 'GO', 'Cocalzinho de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5205521', 'GO', 'Colinas do Sul', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5205703', 'GO', 'Córrego do Ouro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5205802', 'GO', 'Corumbá de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5205901', 'GO', 'Corumbaíba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5206206', 'GO', 'Cristalina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5206305', 'GO', 'Cristianópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5206404', 'GO', 'Crixás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5206503', 'GO', 'Cromínia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5206602', 'GO', 'Cumari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5206701', 'GO', 'Damianópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5206800', 'GO', 'Damolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5206909', 'GO', 'Davinópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5207105', 'GO', 'Diorama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5208301', 'GO', 'Divinópolis de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5207253', 'GO', 'Doverlândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5207352', 'GO', 'Edealina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5207402', 'GO', 'Edéia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5207501', 'GO', 'Estrela do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5207535', 'GO', 'Faina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5207600', 'GO', 'Fazenda Nova', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5207808', 'GO', 'Firminópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5207907', 'GO', 'Flores de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5208004', 'GO', 'Formosa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5208103', 'GO', 'Formoso', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5208152', 'GO', 'Gameleira de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5208400', 'GO', 'Goianápolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5208509', 'GO', 'Goiandira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5208608', 'GO', 'Goianésia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5208707', 'GO', 'Goiânia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5208806', 'GO', 'Goianira', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5208905', 'GO', 'Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5209101', 'GO', 'Goiatuba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5209150', 'GO', 'Gouvelândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5209200', 'GO', 'Guapó', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5209291', 'GO', 'Guaraíta', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5209408', 'GO', 'Guarani de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5209457', 'GO', 'Guarinos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5209606', 'GO', 'Heitoraí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5209705', 'GO', 'Hidrolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5209804', 'GO', 'Hidrolina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5209903', 'GO', 'Iaciara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5209937', 'GO', 'Inaciolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5209952', 'GO', 'Indiara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5210000', 'GO', 'Inhumas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5210109', 'GO', 'Ipameri', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5210158', 'GO', 'Ipiranga de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5210208', 'GO', 'Iporá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5210307', 'GO', 'Israelândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5210406', 'GO', 'Itaberaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5210562', 'GO', 'Itaguari', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5210604', 'GO', 'Itaguaru', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5210802', 'GO', 'Itajá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5210901', 'GO', 'Itapaci', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5211008', 'GO', 'Itapirapuã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5211206', 'GO', 'Itapuranga', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5211305', 'GO', 'Itarumã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5211404', 'GO', 'Itauçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5211503', 'GO', 'Itumbiara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5211602', 'GO', 'Ivolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5211701', 'GO', 'Jandaia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5211800', 'GO', 'Jaraguá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5211909', 'GO', 'Jataí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5212006', 'GO', 'Jaupaci', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5212055', 'GO', 'Jesúpolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5212105', 'GO', 'Joviânia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5212204', 'GO', 'Jussara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5212253', 'GO', 'Lagoa Santa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5212303', 'GO', 'Leopoldo de Bulhões', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5212501', 'GO', 'Luziânia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5212600', 'GO', 'Mairipotaba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5212709', 'GO', 'Mambaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5212808', 'GO', 'Mara Rosa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5212907', 'GO', 'Marzagão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5212956', 'GO', 'Matrinchã', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5213004', 'GO', 'Maurilândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5213053', 'GO', 'Mimoso de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5213087', 'GO', 'Minaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5213103', 'GO', 'Mineiros', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5213400', 'GO', 'Moiporá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5213509', 'GO', 'Monte Alegre de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5213707', 'GO', 'Montes Claros de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5213756', 'GO', 'Montividiu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5213772', 'GO', 'Montividiu do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5213806', 'GO', 'Morrinhos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5213855', 'GO', 'Morro Agudo de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5213905', 'GO', 'Mossâmedes', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5214002', 'GO', 'Mozarlândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5214051', 'GO', 'Mundo Novo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5214101', 'GO', 'Mutunópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5214408', 'GO', 'Nazário', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5214507', 'GO', 'Nerópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5214606', 'GO', 'Niquelândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5214705', 'GO', 'Nova América', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5214804', 'GO', 'Nova Aurora', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5214838', 'GO', 'Nova Crixás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5214861', 'GO', 'Nova Glória', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5214879', 'GO', 'Nova Iguaçu de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5214903', 'GO', 'Nova Roma', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5215009', 'GO', 'Nova Veneza', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5215207', 'GO', 'Novo Brasil', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5215231', 'GO', 'Novo Gama', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5215256', 'GO', 'Novo Planalto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5215306', 'GO', 'Orizona', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5215405', 'GO', 'Ouro Verde de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5215504', 'GO', 'Ouvidor', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5215603', 'GO', 'Padre Bernardo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5215652', 'GO', 'Palestina de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5215702', 'GO', 'Palmeiras de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5215801', 'GO', 'Palmelo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5215900', 'GO', 'Palminópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5216007', 'GO', 'Panamá', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5216304', 'GO', 'Paranaiguara', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5216403', 'GO', 'Paraúna', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5216452', 'GO', 'Perolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5216809', 'GO', 'Petrolina de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5216908', 'GO', 'Pilar de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5217104', 'GO', 'Piracanjuba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5217203', 'GO', 'Piranhas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5217302', 'GO', 'Pirenópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5217401', 'GO', 'Pires do Rio', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5217609', 'GO', 'Planaltina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5217708', 'GO', 'Pontalina', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5218003', 'GO', 'Porangatu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5218052', 'GO', 'Porteirão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5218102', 'GO', 'Portelândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5218300', 'GO', 'Posse', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5218391', 'GO', 'Professor Jamil', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5218508', 'GO', 'Quirinópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5218607', 'GO', 'Rialma', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5218706', 'GO', 'Rianápolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5218789', 'GO', 'Rio Quente', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5218805', 'GO', 'Rio Verde', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5218904', 'GO', 'Rubiataba', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5219001', 'GO', 'Sanclerlândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5219100', 'GO', 'Santa Bárbara de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5219209', 'GO', 'Santa Cruz de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5219258', 'GO', 'Santa Fé de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5219308', 'GO', 'Santa Helena de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5219357', 'GO', 'Santa Isabel', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5219407', 'GO', 'Santa Rita do Araguaia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5219456', 'GO', 'Santa Rita do Novo Destino', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5219506', 'GO', 'Santa Rosa de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5219605', 'GO', 'Santa Tereza de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5219704', 'GO', 'Santa Terezinha de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5219712', 'GO', 'Santo Antônio da Barra', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5219738', 'GO', 'Santo Antônio de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5219753', 'GO', 'Santo Antônio do Descoberto', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5219803', 'GO', 'São Domingos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5219902', 'GO', 'São Francisco de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5220058', 'GO', 'São João da Paraúna', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5220009', 'GO', 'São João d''Aliança', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5220108', 'GO', 'São Luís de Montes Belos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5220157', 'GO', 'São Luíz do Norte', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5220207', 'GO', 'São Miguel do Araguaia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5220264', 'GO', 'São Miguel do Passa Quatro', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5220280', 'GO', 'São Patrício', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5220405', 'GO', 'São Simão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5220454', 'GO', 'Senador Canedo', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5220504', 'GO', 'Serranópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5220603', 'GO', 'Silvânia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5220686', 'GO', 'Simolândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5220702', 'GO', 'Sítio d''Abadia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5221007', 'GO', 'Taquaral de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5221080', 'GO', 'Teresina de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5221197', 'GO', 'Terezópolis de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5221304', 'GO', 'Três Ranchos', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5221403', 'GO', 'Trindade', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5221452', 'GO', 'Trombas', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5221502', 'GO', 'Turvânia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5221551', 'GO', 'Turvelândia', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5221577', 'GO', 'Uirapuru', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5221601', 'GO', 'Uruaçu', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5221700', 'GO', 'Uruana', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5221809', 'GO', 'Urutaí', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5221858', 'GO', 'Valparaíso de Goiás', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5221908', 'GO', 'Varjão', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5222005', 'GO', 'Vianópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5222054', 'GO', 'Vicentinópolis', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5222203', 'GO', 'Vila Boa', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5222302', 'GO', 'Vila Propício', 1);\
		    INSERT INTO municipios (id_municipio, id_uf, nome, id_pais) VALUES ('5300108', 'DF', 'Brasília', 1);\
		    INSERT INTO uf (id_uf, nome, cod_ibge) SELECT DISTINCT id_uf, id_uf, substr(id_municipio,1,2)\
		    	FROM municipios;";

int create_db(){
	int rc;
	char *err = NULL;

	rc = db_exec(create_sql, &err);
	if(rc){
		fprintf(stderr, "livrenfe: SQL Error: %s\n", err);
		return -ESQL;
	}
	rc = db_exec(insert_sql, &err);
	if(rc){
		fprintf(stderr, "livrenfe: SQL Error: %s\n", err);
		return -ESQL;
	}
	return 0;
}
