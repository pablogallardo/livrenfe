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

#include "livrenfe.h"
#include <time.h>

extern NFE *new_nfe(int num, int serie, time_t dh_emis, time_t dh_saida,
		int f_pag, char *nat_op, MUNICIPIO munic, DESTINATARIO dest, int orig, 
		ITEM *i);

extern PRODUTO *new_produto(int codigo, char *desc, unsigned int ncm, unsigned int cfop,
		char *unidade_comercial, float valor);

extern ITEM *new_item(PRODUTO *p, IMPOSTO *i, float valor, float quantidade, 
		unsigned int ordem);

extern ICMS *new_icms(int origem, unsigned int tipo, float aliquota,
		float valor);

extern IMPOSTO *new_imposto(ICMS *i, PIS *p, COFINS *c);

extern DESTINATARIO *new_destinatario(char *rua, int num, char *complemento, 
		char *bairro, MUNICIPIO *m, unsigned int cep, PAIS *p);

extern int add_item(NFE *nfe, ITEM *i);
