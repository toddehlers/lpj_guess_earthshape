/*
** libproj -- library of cartographic projections
**
** Copyright (c) 2003   Gerald I. Evenden
*/
static const char
LIBPROJ_ID[] = "$Id: PJ_wink2.c,v 2.1 2003/03/28 01:46:52 gie Distr. gie $";
/*
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/
#define PROJ_PARMS__ \
	double	cosphi1;
#define PJ_LIB__
# include	<lib_proj.h>
PROJ_HEAD(wink2, "Winkel II") "\n\tPCyl., Sph., no inv.\n\tlat_1=";
#define MAX_ITER    10
#define LOOP_TOL    1e-7
#define TWO_D_PI 0.636619772367581343
FORWARD(s_forward); /* spheroid */
	double k, V;
	int i;

	xy.y = lp.phi * TWO_D_PI;
	k = PI * sin(lp.phi);
	lp.phi *= 1.8;
	for (i = MAX_ITER; i ; --i) {
		lp.phi -= V = (lp.phi + sin(lp.phi) - k) /
			(1. + cos(lp.phi));
		if (fabs(V) < LOOP_TOL)
			break;
	}
	if (!i)
		lp.phi = (lp.phi < 0.) ? -HALFPI : HALFPI;
	else
		lp.phi *= 0.5;
	xy.x = 0.5 * lp.lam * (cos(lp.phi) + P->cosphi1);
	xy.y = FORTPI * (sin(lp.phi) + xy.y);
	return (xy);
}
FREEUP; if (P) free(P); }
ENTRY0(wink2)
	P->cosphi1 = cos(pj_param(P->params, "rlat_1").f);
	P->es = 0.; P->inv = 0; P->fwd = s_forward;
ENDENTRY(P)
/*
** $Log: PJ_wink2.c,v $
** Revision 2.1  2003/03/28 01:46:52  gie
** Initial
**
*/
