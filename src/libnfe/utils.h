/* Copyright (c) 2016, 2017 Pablo G. Gallardo <pggllrd@gmail.com>
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

#ifndef	UTILS_H
#define UTILS_H

#include <time.h>
/*Gera parametros para gerar data e hora no formato exigido pela Sefaz
* O resuldato deve ser usado no 3º parametro da função  strftime(). 
*/

extern char  *dhnfe(int tzd, int hverao,char **str);

extern char *timetostr(time_t);

extern time_t strtotime(const char *);

extern char *itoa(int);

extern char *strrev(char *);

extern char *timef(time_t t, char *format, int chars);

extern char *str_replace(char *search , char *replace , char *subject);

#endif
