/*
** libproj -- library of cartographic projections
**
** Copyright (c) 2003   Gerald I. Evenden
*/
static const char
LIBPROJ_ID[] = "$Id: PJ_putp2.c,v 2.1 2003/03/28 01:46:51 gie Distr. gie $";
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
PROJ_HEAD(putp2, "Putnins P2") "\n\tPCyl., Sph.";
#define C_x	1.89490
#define C_y	1.71848
#define C_p 0.6141848493043784
#define EPS	1e-10
#define NITER	10
#define PI_DIV_3	1.0471975511965977
FORWARD(s_forward); /* spheroid */
	double p, c, s, V;
	int i;

	p = C_p * sin(lp.phi);
	s = lp.phi * lp.phi;
	lp.phi *= 0.615709 + s * ( 0.00909953 + s * 0.0046292 );
	for (i = NITER; i ; --i) {
		c = cos(lp.phi);
		s = sin(lp.phi);
		lp.phi -= V = (lp.phi + s * (c - 1.) - p) /
			(1. + c * (c - 1.) - s * s);
		if (fabs(V) < EPS)
			break;
	}
	if (!i)
		lp.phi = lp.phi < 0 ? - PI_DIV_3 : PI_DIV_3;
	xy.x = C_x * lp.lam * (cos(lp.phi) - 0.5);
	xy.y = C_y * sin(lp.phi);
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	double c;

	lp.phi = pj_asin(xy.y / C_y);
	lp.lam = xy.x / (C_x * ((c = cos(lp.phi)) - 0.5));
	lp.phi = pj_asin((lp.phi + sin(lp.phi) * (c - 1.)) / C_p);
	return (lp);
}
FREEUP; if (P) free(P); }
ENTRY0(putp2) P->es = 0.; P->inv = s_inverse; P->fwd = s_forward; ENDENTRY(P)
/*
** $Log: PJ_putp2.c,v $
** Revision 2.1  2003/03/28 01:46:51  gie
** Initial
**
*/
