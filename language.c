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

#include <simpleparser.h>

#include "debug.h"
#include "language.h"

struct context {
	struct graph	*g;
	int				xrangen;
	int				yrangen;
	int				groupn;
	int				ptn;
	int				cszn;
	double			w, h;
	int				plotg_n;
	int				plotg_g;
	point_func		plotg_sym;
	int				lsz;
	int				fl_n;
	double			fl_xmin, fl_xmax;
	double			fl_x, fl_y;
	int				fl_g;
	int				fl_pts[20];
	/* FIXME: alocare dinamica pentru fl_pts */
};

#define C(x) ((struct context *)x)

static int drawgrid_s(void *c) {
	double w,h;
	int ih, x, i, *col;
	init_graph(C(c)->g, (int)rint(w=C(c)->w*C(c)->g->dres), ih=(int)rint(h=C(c)->h*C(c)->g->dres));
	/* FIXME: trebuie stabilit clar cand se face alocarea pozei; keyword care marcheaza inceputul
	 * zonei de comenzi ??? */
	/* FIXME: rezolutia trebuie sa fie cunoscuta, la fel dimensiunile panzei */

	/* calculez coord. fizice ale pct. de coord xmin, ymin */
	/* w si h sunt in dots */
	C(c)->g->ox=(int)rint((w-(C(c)->g->xmax-C(c)->g->xmin)*C(c)->g->sx)/2);
	C(c)->g->lfh=(int)rint(C(c)->g->lh*C(c)->g->dres);
	C(c)->g->lfw=(int)rint(C(c)->g->lw*C(c)->g->dres);
	C(c)->g->ly=ih-(int)rint((h-(C(c)->g->ymax-C(c)->g->ymin)*C(c)->g->sy-
				(C(c)->g->lh+C(c)->g->ls)*C(c)->g->dres)/2)-C(c)->g->lfh;
	C(c)->g->oy=C(c)->g->ly-(int)rint(C(c)->g->ls*C(c)->g->dres);
	C(c)->g->lx=(int)rint((w-C(c)->g->lfw)/2);
	/* aici sunt de baza explicatiile din dreptul fiecarui camp din structura graph (vezi graph.h)
	 * ideea e ca centrez graficul si legenda pe x, iar pe y centrez grafic+legenda+spatiul_dintre_ele
	 * si calculez g->ly si g->oy in functie de ele
	 */
	debug(1, "Setting physical coordinates of (xmin, ymin) to (%d, %d).\n", C(c)->g->ox, C(c)->g->oy);
	debug(1, "Setting physical coordinates of legend to (%d, %d).\n", C(c)->g->lx, C(c)->g->ly);
	debug(1, "Setting physical size of legend to (%d, %d).\n", C(c)->g->lfw, C(c)->g->lfh);

	/* aloc culorile pentru grupuri */
	for (i=1; i<=C(c)->groupn; i++) {
		col=&C(c)->g->grps[i].col;
		x=*col;
		*col=gdImageColorAllocate(C(c)->g->img, CR(x), CG(x), CB(x));
		debug(5, "Allocated color (%X %X %X) for group %d as %d\n", CR(x),CG(x),CB(x),i,*col);
		/* FIXME: test daca alocarea culorii a avut succes */
	}
	/* FIXME: am presupus ca grupurile sunt de la 1 la C(c)->groupn */

	draw_grid(C(c)->g);
	return 0;
}

static int xrange_s(void *c) {
	C(c)->xrangen=0;
	return 0;
}

static int xrange_a(void *c, char *s) {
	switch (C(c)->xrangen++) {
	case 0:
		sscanf(s, "%lf", &C(c)->g->xmin);
		break;
	case 1:
		sscanf(s, "%lf", &C(c)->g->xmax);
		break;
	}
	return 0;
}

static int yrange_s(void *c) {
	C(c)->yrangen=0;
	return 0;
}

static int yrange_a(void *c, char *s) {
	switch (C(c)->yrangen++) {
	case 0:
		sscanf(s, "%lf", &C(c)->g->ymin);
		break;
	case 1:
		sscanf(s, "%lf", &C(c)->g->ymax);
		break;
	}
	return 0;
}

static int xgridunit_a(void *c, char *s) {
	sscanf(s, "%lf", &C(c)->g->dx);
	return 0;
}

static int xgridsubunit_a(void *c, char *s) {
	sscanf(s, "%lf", &C(c)->g->sdx);
	return 0;
}

static int ygridunit_a(void *c, char *s) {
	sscanf(s, "%lf", &C(c)->g->dy);
	return 0;
}

static int ygridsubunit_a(void *c, char *s) {
	sscanf(s, "%lf", &C(c)->g->sdy);
	return 0;
}

static int group_a(void *c, char *s) {
	sscanf(s, "%d", &C(c)->groupn);
	debug(5, "group_a: switching to group %d\n", C(c)->groupn);

	C(c)->g->grps[C(c)->groupn].npts=0;
	/* FIXME: asta e un dirty quick hack; la alocare dinamica ar trebui sa fie initializat
	 * cu 0 doar daca grupul nu exista deja */
	return 0;
}

static int pt_s(void *c) {
	C(c)->ptn=0;
	return 0;
}

static int pt_a(void *c, char *s) {
	struct point *pt=&C(c)->g->grps[C(c)->groupn].pts[C(c)->g->grps[C(c)->groupn].npts];

	switch (C(c)->ptn++) {
	case 0:
		sscanf(s, "%lf", &pt->x);
		break;
	case 1:
		sscanf(s, "%lf", &pt->y);
		C(c)->g->grps[C(c)->groupn].npts++;
		/* FIXME: aici incrementez orbeste numarul de puncte, dar asta presupune ca fiecare
		 * instructiune "Pt" primeste exact 2 argumente; daca are alt nr. de argumente, apar
		 * rezultate nedorite; pt_a si pt_e ar trebui sa faca verificarile necesare */
		debug(5, "pt_a: added point (%lg, %lg) to group %d\n", pt->x, pt->y, C(c)->groupn);
		break;
	}
	return 0;
}

static int color_a(void *c, char *s) {
	int *col=&C(c)->g->grps[C(c)->groupn].col;
	sscanf(s, "%x", col);
	debug(5, "color_a: setting color for group %d to %X\n", C(c)->groupn, *col);
	return 0;
}

static int resolution_a(void *c, char *s) {
	sscanf(s, "%d", &C(c)->g->res);
	C(c)->g->dres=C(c)->g->res/0.0254;
	return 0;
}

static int xscale_a(void *c, char *s) {
	sscanf(s, "%lf", &C(c)->g->sx);
	C(c)->g->sx*=C(c)->g->dres;
	/* FIXME: daca nu a fost inca definita rezolutia? */
	return 0;
}

static int yscale_a(void *c, char *s) {
	sscanf(s, "%lf", &C(c)->g->sy);
	C(c)->g->sy*=C(c)->g->dres;
	/* FIXME: daca nu a fost inca definita rezolutia? */
	return 0;
}

static int xtitle_a(void *c, char *s) {
	strcpy(C(c)->g->x_title, s);
	/* FIXME: daca depaseste lungimea definita ? */
	return 0;
}

static int ytitle_a(void *c, char *s) {
	strcpy(C(c)->g->y_title, s);
	/* FIXME: daca depaseste lungimea definita ? */
	return 0;
}

static int xunitname_a(void *c, char *s) {
	strcpy(C(c)->g->x_unitname, s);
	/* FIXME: daca depaseste lungimea definita ? */
	return 0;
}

static int yunitname_a(void *c, char *s) {
	strcpy(C(c)->g->y_unitname, s);
	/* FIXME: daca depaseste lungimea definita ? */
	return 0;
}

static int canvassize_s(void *c) {
	C(c)->cszn=0;
	return 0;
}

static int canvassize_a(void *c, char *s) {
	switch (C(c)->cszn++) {
	case 0:
		sscanf(s, "%lf", &C(c)->w);
		break;
	case 1:
		sscanf(s, "%lf", &C(c)->h);
		break;
	}
	return 0;
}

static int fontsize_a(void *c, char *s) {
	sscanf(s, "%d", &C(c)->g->font_size);
	debug(5, "fontsize_a: setting font size to %d\n", C(c)->g->font_size);
	return 0;
}

static int plotgroup_s(void *c) {
	C(c)->plotg_n=0;
	return 0;
}

static int plotgroup_a(void *c, char *s) {
	switch (C(c)->plotg_n++) {
	case 0:
		sscanf(s, "%d", &C(c)->plotg_g);
		break;
	case 1:
		C(c)->plotg_sym=solve_sym(s);
		break;
	}
	return 0;
}

static int plotgroup_e(void *c) {
	int i;
	struct group *grp=&C(c)->g->grps[C(c)->plotg_g];

	debug(4, "plotgroup_e: plotting group %d\n", C(c)->plotg_g);
	for (i=0; i<grp->npts; i++) {
		debug(8, "plotgroup_e: point(%lg, %lg)\n", grp->pts[i].x, grp->pts[i].y);
		C(c)->plotg_sym(C(c)->g, grp->pts[i].x, grp->pts[i].y, grp->col);
	}
	return 0;
}

static int legendsize_s(void *c) {
	C(c)->lsz=0;
	return 0;
}

static int legendsize_a(void *c, char *s) {
	switch(C(c)->lsz++) {
	case 0:
		sscanf(s, "%lf", &C(c)->g->lw);
		break;
	case 1:
		sscanf(s, "%lf", &C(c)->g->lh);
		break;
	}
	return 0;
}

static int legendspace_a(void *c, char *s) {
	sscanf(s, "%lf", &C(c)->g->ls);
	return 0;
}

static int legendtitle_a(void *c, char *s) {
	strcpy(C(c)->g->legend_title, s);
	return 0;
}

static int legend_a(void *c, char *s) {
	strcpy(C(c)->g->grps[C(c)->groupn].legend, s);
	return 0;
}

static int drawlegend_s(void *c) {
	C(c)->g->ngrps=C(c)->groupn;
	/* FIXME: C(c)->groupn ar trebui sa fie direct in C(c)->g->ngrps
	 * nu are sens de schimbat daca se implementeaza alocare dinamica pentru grupuri */
	draw_legend(C(c)->g);
	
	return 0;
}

static int fitline_s(void *c) {
	C(c)->fl_n=0;
	return 0;
}

static int fitline_a(void *c, char *s) {
	switch (C(c)->fl_n) {
	case 0:
		sscanf(s, "%d", &C(c)->fl_g);
		break;
	case 1:
		sscanf(s, "%lf", &C(c)->fl_x);
		break;
	case 2:
		sscanf(s, "%lf", &C(c)->fl_y);
		break;
	case 3:
		sscanf(s, "%lf", &C(c)->fl_xmin);
		break;
	case 4:
		sscanf(s, "%lf", &C(c)->fl_xmax);
		break;
	default:
		sscanf(s, "%d", &C(c)->fl_pts[C(c)->fl_n-5]);
		break;
	}
	C(c)->fl_n++;
	return 0;
}

static int fitline_e(void *c) {
	int i;
	double a, b, s_xy=0, s_x=0;
	struct point *p;
	int x1, y1, x2, y2;

	C(c)->fl_n-=5;

	for (i=0; i<C(c)->fl_n; i++) {
		p=&C(c)->g->grps[C(c)->fl_g].pts[C(c)->fl_pts[i]-1];
		s_x+=(p->x-C(c)->fl_x)*(p->x-C(c)->fl_x);
		s_xy+=(p->x-C(c)->fl_x)*(p->y-C(c)->fl_y);
	}
	if (!s_x) return 1;
	a=s_xy/s_x;
	b=C(c)->fl_y-a*C(c)->fl_x;
	debug(1, "fitline_e: group %d, slope %g\n", C(c)->fl_g, a);
	x1=XC(C(c)->g, C(c)->fl_xmin);
	y1=YC(C(c)->g, a*C(c)->fl_xmin+b);
	x2=XC(C(c)->g, C(c)->fl_xmax);
	y2=YC(C(c)->g, a*C(c)->fl_xmax+b);
	gdImageLine(C(c)->g->img, x1, y1, x2, y2, C(c)->g->grps[C(c)->fl_g].col);
	return 0;
}

static int cmmpline_s(void *c) {
	C(c)->fl_n=0;
	return 0;
}

static int cmmpline_a(void *c, char *s) {
	switch (C(c)->fl_n) {
	case 0:
		sscanf(s, "%d", &C(c)->fl_g);
		break;
	case 1:
		sscanf(s, "%lf", &C(c)->fl_xmin);
		break;
	case 2:
		sscanf(s, "%lf", &C(c)->fl_xmax);
		break;
	default:
		sscanf(s, "%d", &C(c)->fl_pts[C(c)->fl_n-3]);
		break;
	}
	C(c)->fl_n++;
	return 0;
}

static int cmmpline_e(void *c) {
	int i;
	double a, b, s_xy=0, s_x=0, s_y=0, s_x2=0, n;
	struct point *p;
	int x1, y1, x2, y2;

	C(c)->fl_n-=3;
	n=C(c)->fl_n;

	for (i=0; i<C(c)->fl_n; i++) {
		p=&C(c)->g->grps[C(c)->fl_g].pts[C(c)->fl_pts[i]-1];
		s_x+=p->x;
		s_x2+=p->x*p->x;
		s_y+=p->y;
		s_xy+=p->x*p->y;
	}
	if (!s_x) return 1;
	a=(n*s_xy-s_x*s_y)/(n*s_x2-s_x*s_x);
	b=(s_x2*s_y-s_x*s_xy)/(n*s_x2-s_x*s_x);
	debug(1, "cmmpline_e: group %d, slope %g\n", C(c)->fl_g, a);
	x1=XC(C(c)->g, C(c)->fl_xmin);
	y1=YC(C(c)->g, a*C(c)->fl_xmin+b);
	x2=XC(C(c)->g, C(c)->fl_xmax);
	y2=YC(C(c)->g, a*C(c)->fl_xmax+b);
	gdImageLine(C(c)->g->img, x1, y1, x2, y2, C(c)->g->grps[C(c)->fl_g].col);
	return 0;
}

const struct spa_keyword lang_main[]={
/*	keyword				start			arg				end				bloc_start		bloc_end */
	"canvassize",		canvassize_s,	canvassize_a,	NULL,			NULL,			NULL,
	"cmmpline",			cmmpline_s,		cmmpline_a,		cmmpline_e,		NULL,			NULL,
	"color",			NULL,			color_a,		NULL,			NULL,			NULL,
	"drawgrid",			drawgrid_s,		NULL,			NULL,			NULL,			NULL,
	"drawlegend",		drawlegend_s,	NULL,			NULL,			NULL,			NULL,
	"fitline",			fitline_s,		fitline_a,		fitline_e,		NULL,			NULL,
	"fontsize",			NULL,			fontsize_a,		NULL,			NULL,			NULL,
	"group",			NULL,			group_a,		NULL,			NULL,			NULL,
	"legend",			NULL,			legend_a,		NULL,			NULL,			NULL,
	"legendsize",		legendsize_s,	legendsize_a,	NULL,			NULL,			NULL,
	"legendspace",		NULL,			legendspace_a,	NULL,			NULL,			NULL,
	"legendtitle",		NULL,			legendtitle_a,	NULL,			NULL,			NULL,
	"plotgroup",		plotgroup_s,	plotgroup_a,	plotgroup_e,	NULL,			NULL,
	"pt",				pt_s,			pt_a,			NULL,			NULL,			NULL,
	"resolution",		NULL,			resolution_a,	NULL,			NULL,			NULL,
	"xgridsubunit",		NULL,			xgridsubunit_a,	NULL,			NULL,			NULL,
	"xgridunit",		NULL,			xgridunit_a,	NULL,			NULL,			NULL,
	"xrange",			xrange_s,		xrange_a,		NULL,			NULL,			NULL,
	"xscale",			NULL,			xscale_a,		NULL,			NULL,			NULL,
	"xtitle",			NULL,			xtitle_a,		NULL,			NULL,			NULL,
	"xunitname",		NULL,			xunitname_a,	NULL,			NULL,			NULL,
	"ygridsubunit",		NULL,			ygridsubunit_a,	NULL,			NULL,			NULL,
	"ygridunit",		NULL,			ygridunit_a,	NULL,			NULL,			NULL,
	"yrange",			yrange_s,		yrange_a,		NULL,			NULL,			NULL,
	"yscale",			NULL,			yscale_a,		NULL,			NULL,			NULL,
	"ytitle",			NULL,			ytitle_a,		NULL,			NULL,			NULL,
	"yunitname",		NULL,			yunitname_a,	NULL,			NULL,			NULL,
	NULL,				NULL,			NULL,			NULL,			NULL,			NULL
};

int run_script(FILE *f, struct graph *g) {
	struct context ctx;
	struct spa_vars pv;
	
	ctx.g=g;
	if (spa_parse(f, &pv, &ctx, lang_main)) {
		spa_error(stderr, &pv);
		return 1;
	}
	return 0;
}
