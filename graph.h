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

#ifndef GRAPH_H
#define GRAPH_H

#include <gd.h>
#include <math.h>

/* colors */

#define C_NONE				-1
#define C_BACKGROUND		0
#define C_AXES				1
#define C_GRID				2
#define C_SUBGRID			3
#define C_AXISUNIT			4
#define C_AXISUNITNAME		5

#define N_COL				256

struct color {
	int r, g, b;
};

struct color_def {
	int col;
	struct color def;
};

struct point {
	double					x;
	double					y;
};

struct group {
	int						col;				/* culoarea de reprezentare */
	struct point			pts[20];			/* puncte */
	/* FIXME: ar trebui sa fie struct point * si alocat dinamic la parsing */
	int						npts;
	char					legend[50];
};

/* conventii de numire:
 * coord. logica = coord. in marimea reprezentata pe una din axe
 * coord. fizica = coord. in sistemul de coordonate al imaginii (1=pixel, originea
 * in coltul din stanga sus, sensul pe axa verticala de sus in jos)
 */

struct graph {
	int						col[N_COL];			/* harta de trecere de la culori la paleta */
	struct gdImageStruct	*img;
	int						ox, oy;				/* coordonatele fizice ale punctului de coord. xmin, ymin */
	/* lucrez cu coordonatele punctului (xmin,ymin) si nu cu originea logica pt. ca daca am un
	 * xrange de genul (400000 : 400000.1) obtin niste coordonate fizice ale originii logice foarte
	 * "interesante" (overflow) -- multumiri deosebite lui Val Petru! */
	double					xmin, xmax;			/* domeniul de reprezentare pe X */
	double					ymin, ymax;			/* domeniul de reprezentare pe Y */
	double					sx;					/* raport unitate fizica/unitate logica pe X */
	double					sy;					/* raport unitate fizica/unitate logica pe Y */
	double					dx, dy;				/* dimensiunea unei diviziuni a grilei in scara logica */
	double					sdx, sdy;			/* dimensiunea unei subdiviziuni a grilei in scara logica */
	int						res;				/* rezolutia in DPI */
	double					dres;				/* rezolutia in DotsPerMeter */
	char					x_unitname[10];		/* denumirea unitatii de masura pe x */
	char					y_unitname[10];
	char					x_title[10];		/* denumirea marimii reprezentata pe x */
	char					y_title[10];
	/* FIXME: ar trebui sa fie char * si alocate dinamic la parsing */
	struct group			grps[20];
	int						ngrps;
	/* FIXME: ar trebui sa fie struct group * si alocat dinamic la parsing */
	int						font_size;
	double					lw, lh;				/* dimensiunile legendei, in metri */
	double					ls;					/* spatiul dintre grafic si legenda, in metri */
	int						lx, ly;				/* coord. fizice ale coltului stanga-sus al legendei */
	int						lfw, lfh;			/* latimea si inaltimea fizica ale legendei */
	char					legend_title[50];	/* titlul legendei */
};

#define GW(graph)			(gdImageSX((graph)->img))
#define GH(graph)			(gdImageSY((graph)->img))

#define abs_floor(x)		((x)>0?floor(x):ceil(x))
#define abs_ceil(x)			((x)>0?ceil(x):floor(x))

#define XC(g,x)				((int)rint(((x)-(g)->xmin)*(g)->sx)+(g)->ox)
#define YC(g,y)				(-(int)rint(((y)-(g)->ymin)*(g)->sy)+(g)->oy)
/* macrouri de trecere de la coordonate logice la coordonate fizice */

#define CR(x)				(((x)&0xff0000)>>16)
#define CG(x)				(((x)&0x00ff00)>>8)
#define CB(x)				( (x)&0x0000ff)

/* functii pentru reprezentarea punctelor */

extern void point_cross		(struct graph *, double, double, int);

typedef void				(*point_func)(struct graph *, double, double, int);

extern const point_func solve_sym (const char *);
extern void draw_legend(struct graph *);

#endif
