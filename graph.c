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

#include <gdfontt.h>
#include <gdfonts.h>
#include <gdfontmb.h>
#include <gdfontl.h>
#include <gdfontg.h>

#include "graph.h"
#include "debug.h"

gdFont *fonts[5];

const struct color_def color_defaults[]={
	C_BACKGROUND,			255,	255,	255,
	C_AXES,					0,		0,		0,
	C_GRID,					192,	192,	192,
	C_SUBGRID,				224,	224,	224,
	C_AXISUNIT,				0,		0,		0,
	C_AXISUNITNAME,			255,	0,		0,
	C_NONE,					0,		0,		0
};

int init_graph(struct graph *g, int w, int h) {
	const struct color_def *c;

	debug(1, "init_graph: Allocating %dx%d image.\n", w, h);
	if ((g->img=gdImageCreate(w, h))==NULL) return 1;
	for (c=color_defaults; c->col!=C_NONE; c++) {
		g->col[c->col]=gdImageColorAllocate(g->img, c->def.r, c->def.g, c->def.b);
		/* FIXME: test succes alocare */
	}

	return 0;
}

typedef void (*_w_val)(struct graph *, double, int, int);

static void _draw_grid(struct graph *g, double dx, double dy, int col,
		_w_val xval, _w_val yval, int xmin, int ymin, int xmax, int ymax) {
	int x1, y1, x2, y2;
	int x, y;

	x2=abs_floor(g->xmax/dx);
	y2=abs_floor(g->ymax/dy);

	for (x1=abs_ceil(g->xmin/dx); x1<=x2; x1++) {
		x=XC(g, x1*dx);
		gdImageLine(g->img, x, ymin, x, ymax, col);
		if (xval!=NULL) xval(g, x1*dx, x, ymin);
	}

	for (y1=abs_ceil(g->ymin/dy); y1<=y2; y1++) {
		y=YC(g, y1*dy);
		gdImageLine(g->img, xmin, y, xmax, y, col);
		if (yval!=NULL) yval(g, y1*dy, xmin, y);
	}
}

#define COORD_DIST 5
/* distanta dintre grafic si valorile coordonatelor, in pixeli */

static void _xcoord(struct graph *g, double val, int x, int y) {
	char buf[15];
	/* FIXME: daca 15 nu e destul? */
	gdFont *fnt=fonts[g->font_size];

	sprintf(buf, "%lg", val);
	x-=strlen(buf)*fnt->w/2;
	y+=COORD_DIST;
	gdImageString(g->img, fnt, x, y, buf, g->col[C_AXISUNIT]);
}

static void _ycoord(struct graph *g, double val, int x, int y) {
	char buf[15];
	/* FIXME: daca 15 nu e destul? */
	gdFont *fnt=fonts[g->font_size];

	sprintf(buf, "%lg", val);
	x-=strlen(buf)*fnt->w+COORD_DIST;
	y-=fnt->h/2;
	gdImageString(g->img, fnt, x, y, buf, g->col[C_AXISUNIT]);
}

static void _xunit(struct graph *g, int x, int y) {
	char *buf;
	int l;
	
	gdFont *fnt=fonts[g->font_size];
	buf=(char *)alloca(3+(l=strlen(g->x_unitname)));
	buf[0]='('; strcpy(buf+1, g->x_unitname);
	buf[++l]=')'; buf[++l]='\0';
	x+=COORD_DIST;
	y-=fnt->h;
	gdImageString(g->img, fnt, x, y, buf, g->col[C_AXISUNITNAME]);
	y-=fnt->h;
	gdImageString(g->img, fnt, x, y, g->x_title, g->col[C_AXISUNITNAME]);
}

static void _yunit(struct graph *g, int x, int y) {
	char *buf;
	int l;
	
	gdFont *fnt=fonts[g->font_size];
	buf=(char *)alloca(3+(l=strlen(g->y_unitname)));
	buf[0]='('; strcpy(buf+1, g->y_unitname);
	buf[++l]=')'; buf[++l]='\0';
	y-=fnt->h+COORD_DIST;
	gdImageString(g->img, fnt, x, y, buf, g->col[C_AXISUNITNAME]);
	y-=fnt->h;
	gdImageString(g->img, fnt, x, y, g->y_title, g->col[C_AXISUNITNAME]);
}

void draw_grid(struct graph *g) {
	int xmin, xmax, ymin, ymax;
	
	xmin=XC(g, g->xmin); ymin=YC(g, g->ymin);
	xmax=XC(g, g->xmax); ymax=YC(g, g->ymax);
	_draw_grid(g, g->sdx, g->sdy, C_SUBGRID, NULL, NULL, xmin, ymin, xmax, ymax);
	_draw_grid(g, g->dx, g->dy, C_GRID, _xcoord, _ycoord, xmin, ymin, xmax, ymax);
	_xunit(g, xmax, ymin);
	_yunit(g, xmin, ymax);
}

void point_cross(struct graph *g, double xl, double yl, int col) {
	int a=5, x=XC(g,xl), y=YC(g,yl);

	gdImageLine(g->img, x-a, y, x+a, y, col);
	gdImageLine(g->img, x, y-a, x, y+a, col);
}

void point_x(struct graph *g, double xl, double yl, int col) {
	int a=4, x=XC(g,xl), y=YC(g,yl);

	gdImageLine(g->img, x-a, y-a, x+a, y+a, col);
	gdImageLine(g->img, x-a, y+a, x+a, y-a, col);
}

void point_circle(struct graph *g, double xl, double yl, int col) {
	int a=10, x=XC(g,xl), y=YC(g,yl);

	gdImageArc(g->img, x, y, a, a, 0, 360, col);
}

void point_delta(struct graph *g, double xl, double yl, int col) {
	int a=5, x=XC(g,xl), y=YC(g,yl);

	gdImageLine(g->img, x-a, y+a, x, y-a, col);
	gdImageLine(g->img, x, y-a, x+a, y+a, col);
	gdImageLine(g->img, x-a, y+a, x+a, y+a, col);
}

void point_nabla(struct graph *g, double xl, double yl, int col) {
	int a=5, x=XC(g,xl), y=YC(g,yl);

	gdImageLine(g->img, x-a, y-a, x, y+a, col);
	gdImageLine(g->img, x, y+a, x+a, y-a, col);
	gdImageLine(g->img, x-a, y-a, x+a, y-a, col);
}

void point_square(struct graph *g, double xl, double yl, int col) {
	int a=5, x=XC(g,xl), y=YC(g,yl);

	gdImageLine(g->img, x-a, y-a, x+a, y-a, col);
	gdImageLine(g->img, x-a, y-a+1, x-a, y+a-1, col);
	gdImageLine(g->img, x+a, y-a+1, x+a, y+a-1, col);
	gdImageLine(g->img, x-a, y+a, x+a, y+a, col);
}

/* FIXME: dimensiunea simbolurilor, "a", ar trebui sa fie calculata in functie de rezolutie */

const point_func solve_sym(const char *s) {
	/* FIXME: un algoritm mai eficient */

	if (!strcasecmp(s, "cross")) return point_cross;
	if (!strcasecmp(s, "x")) return point_x;
	if (!strcasecmp(s, "circle")) return point_circle;
	if (!strcasecmp(s, "delta")) return point_delta;
	if (!strcasecmp(s, "nabla")) return point_nabla;
	if (!strcasecmp(s, "square")) return point_square;
	return NULL;
}

void draw_legend(struct graph *g) {
    int col=g->col[C_AXES]; /* FIXME: quick hack */
	gdFont *fnt=fonts[g->font_size];
	int w=80, h=8; /* dimensiunile "mostrei" */
	int s=15; /* spatiul dintre mostra si explicatie */
	int y, x, i, max, l, y1;

	gdImageRectangle(g->img, g->lx, g->ly, g->lx+g->lfw, g->ly+g->lfh, col);
	y=g->ly+(g->lfh-(g->ngrps+2)*fnt->h)/2;
	x=g->lx+g->lfw/2;
	gdImageString(g->img, fnt, x-fnt->w*strlen(g->legend_title)/2, y, g->legend_title, col);

	for(max=0,i=1; i<=g->ngrps; i++) if ((l=strlen(g->grps[i].legend))>max) max=l;
	x-=(max*fnt->w+s+w)/2;
	y+=2*fnt->h;
	for(i=1; i<=g->ngrps; i++, y+=fnt->h) {
		y1=y+(fnt->h-h)/2;
		gdImageFilledRectangle(g->img, x, y1, x+w, y1+h, g->grps[i].col);
		gdImageString(g->img, fnt, x+w+s, y, g->grps[i].legend, col);
	}
}

int main(int argc, char **argv) {
	struct graph gr;
	FILE *s, *d;
	int err;

	fonts[0]=gdFontTiny;
	fonts[1]=gdFontSmall;
	fonts[2]=gdFontMediumBold;
	fonts[3]=gdFontLarge;
	fonts[4]=gdFontGiant;

	s=fopen("test.graph", "r");
	err=run_script(s, &gr);
	fclose(s);
	if (!err) {
		d=fopen("test.png", "w");
		gdImagePng(gr.img, d);
		fclose(d);
	}
	
	return 0;
}
