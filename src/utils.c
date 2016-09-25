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

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

char *timetostr(time_t t){
	char *buffer;
	buffer = malloc(sizeof(char) * 11);
	struct tm* tm_info;

	tm_info = localtime(&t);

	strftime(buffer, 11, "%Y-%m-%d", tm_info);

	return buffer;
}
