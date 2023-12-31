/*
** libproj -- library of cartographic projections
**
** Copyright (c) 2003   Gerald I. Evenden
*/
static const char
LIBPROJ_ID[] = "$Id: PJ_mbtfpp.c,v 2.1 2003/03/28 01:46:51 gie Distr. gie $";
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
#define PJ_LIB__
#include	<lib_proj.h>
PROJ_HEAD(mbtfpp, "McBride-Thomas Flat-Polar Parabolic") "\n\tCyl., Sph.";
#define CS	.95257934441568037152
#define FXC	.92582009977255146156
#define FYC	3.40168025708304504493
#define C23	.66666666666666666666
#define C13	.33333333333333333333
#define ONEEPS	1.0000001
FORWARD(s_forward); /* spheroid */
	lp.phi = asin(CS * sin(lp.phi));
	xy.x = FXC * lp.lam * (2. * cos(C23 * lp.phi) - 1.);
	xy.y = FYC * sin(C13 * lp.phi);
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	lp.phi = xy.y / FYC;
	if (fabs(lp.phi) >= 1.) {
		if (fabs(lp.phi) > ONEEPS)	I_ERROR
		else	lp.phi = (lp.phi < 0.) ? -HALFPI : HALFPI;
	} else
		lp.phi = asin(lp.phi);
	lp.lam = xy.x / ( FXC * (2. * cos(C23 * (lp.phi *= 3.)) - 1.) );
	if (fabs(lp.phi = sin(lp.phi) / CS) >= 1.) {
		if (fabs(lp.phi) > ONEEPS)	I_ERROR
		else	lp.phi = (lp.phi < 0.) ? -HALFPI : HALFPI;
	} else
		lp.phi = asin(lp.phi);
	return (lp);
}
FREEUP; if (P) free(P); }
ENTRY0(mbtfpp) P->es = 0.; P->inv = s_inverse; P->fwd = s_forward; ENDENTRY(P)
/*
** $Log: PJ_mbtfpp.c,v $
** Revision 2.1  2003/03/28 01:46:51  gie
** Initial
**
*/
