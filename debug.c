/*****************************************************************************
 *                                                                           *
 * graph - a tool to plot experimental data graphs                           *
 * Copyright (C) 2001 Radu Constantin Rendec, Ioan Petru Nicu                *
 *                                                                           *
 * This program is free software; you can redistribute it and/or modify      *
 * it under the terms of the GNU General Public License as published by      *
 * the Free Software Foundation; either version 2 of the License, or         *
 * (at your option) any later version.                                       *
 *                                                                           *
 * This program is distributed in the hope that it will be useful,           *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of            *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
 * GNU General Public License for more details.                              *
 *                                                                           *
 * You should have received a copy of the GNU General Public License         *
 * along with this program; if not, write to the Free Software               *
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA *
 *                                                                           *
 * The full text of the license can be found in gpl.txt                      *
 *                                                                           *
 * Contact:                                                                  *
 * radu@rendec.ines.ro                                                       *
 * ionut_nicu@yahoo.com                                                      * 
 *                                                                           *
 *****************************************************************************/

#include <stdarg.h>
#include <stdio.h>

int debug_level=DEBUG_LEVEL;

void debug(int level, ...) {
	va_list ap;
	char *fmt;

	if (debug_level < level) return;
	va_start(ap, level);
	fmt=va_arg(ap, char *);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
}
