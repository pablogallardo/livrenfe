/* Copyright (c) 2016 Pablo G. Gallardo <pggllrd@gmail.com>
 *
 * This file is part of LivreNFE.
 *
 * LivreNFE is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LivreNFE is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with LivreNFE.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#define _XOPEN_SOURCE
#define _GNU_SOURCE
#define __USE_XOPEN
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define DHDEFAULT  "%Y-%m-%d-T-%H:%M:%S-"

char *timetostr(time_t t){
	char *buffer;
	buffer = malloc(sizeof(char) * 20);
	struct tm *tm_info;

	tm_info = localtime(&t);

	strftime(buffer, 19, "%Y-%m-%d %H:%M:%S", tm_info);

	return buffer;
}

time_t strtotime(const char *s){
	struct tm mytm;
	time_t t;
	strptime(s, "%d/%m/%Y %H:%M:%S", &mytm);
	t = mktime(&mytm);
	return t;
}

char *itoa(int i){
	char *s = malloc(10);
	sprintf(s, "%d", i);
	return s;
}

char *dtoa(double d){
	char *s = malloc(10);
	char *formated;
	sprintf(s, "%.2f", d);

	formated = str_replace(".", ",", s);
	free(s);
	return formated;
}

char *strrev(char *s){
	if(!s)
		return NULL;
	int i, j;
	char *rev = malloc(sizeof(char) * strlen(s) + 1);
	for(i = 0, j = strlen(s) - 1; i < strlen(s); i++, j--){
		rev[j] = s[i];	
	}
	rev[strlen(s)] = '\0';
	return rev;
}

char *timef(time_t t, char *format, int chars){
	char *buffer;
	buffer = malloc(sizeof(char) * ++chars);
	struct tm *tm_info;

	tm_info = localtime(&t);

	strftime(buffer, chars, format, tm_info);

	return buffer;
}

/**
 * PHP's str_replace ported to C
 * @author Silver Moon (m00n.silv3r@gmail.com)
 */
char *str_replace(char *search , char *replace , char *subject){
    char  *p = NULL , *old = NULL , *new_subject = NULL ;
    int c = 0 , search_size;
     
    search_size = strlen(search);
    //Count how many occurences
    for(p = strstr(subject , search) ; p != NULL ; p = strstr(p + search_size , search))
    {
        c++;
    }
    //Final size
    c = ( strlen(replace) - search_size )*c + strlen(subject);
    //New subject with new size
    new_subject = malloc(sizeof(char) * (c + 1));
    //Set it to blank
    strcpy(new_subject , "");
    //The start position
    old = subject;
    for(p = strstr(subject , search) ; p != NULL ; p = strstr(p + search_size , search)){
    	char *aux = new_subject + strlen(new_subject);
        //move ahead and copy some text from original subject , from a certain position
        strncpy(aux , old , p - old);
        //move ahead and copy the replacement text
        strcpy(aux + (p - old) , replace);
        //The new start position after this search match
        old = p + search_size;
    }
    //Copy the part after the last search match
    strcpy(new_subject + strlen(new_subject) , old);
    return new_subject;
}

/*Data e hora do evento no formato AAAA-MM-DDThh:mm:ssTZD (UTC - Universal Coordinated
 * Time , onde TZD pode ser 
 * -02:00 (Fernando de Noronha), 
 * -03:00 (Brasília) ou 
 * -04:00 (Manaus), no  horário de verão serão 
 * -01:00, -02:00 e -03:00. Ex.:
 * 2010-08-19T13:00:15-03:00.
 * */
 char  *dhnfe(int tzd, int hverao,char **str){
 	 char *aux;

	if (hverao == 1){

		switch (tzd){
			case -2: /*Fuso de Fernando de Noronha */
				aux = "2:00";
				break;
			case -3:/*Fuso de Brasilia*/
			default:		
				aux = "3:00";
				break;
			case -4:/*Fuso de manaus*/
				aux = "4:00";
				break;
		}
	}else if(hverao == 0){
		switch (tzd){
			case -2:/*Fuso de Fernando de Noronha Horario de Verao*/
				aux = "1:00";
				break;
			case -3:/*Fuso de Brasilia Horario de Verao*/
			default:		
				aux = "2:00";
				break;
			case -4:/*Fuso de Manaus Hoario de Verao*/
				aux = "3:00";
				break;
		}
	}
	strcat(*str,DHDEFAULT);
	return strcat(*str,aux);
 }
