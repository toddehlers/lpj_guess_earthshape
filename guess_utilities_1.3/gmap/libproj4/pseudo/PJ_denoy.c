/*
** libproj -- library of cartographic projections
**
** Copyright (c) 2003   Gerald I. Evenden
*/
static const char
LIBPROJ_ID[] = "$Id: PJ_denoy.c,v 2.1 2003/03/28 01:46:50 gie Distr. gie $";
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
PROJ_HEAD(denoy, "Denoyer Semi-Elliptical") "\n\tPCyl., no inv., Sph.";
#define C0	0.95
#define C1	-.08333333333333333333
#define C3	.00166666666666666666
#define D1	0.9
#define D5	0.03
FORWARD(s_forward); /* spheroid */
	xy.y = lp.phi;
	xy.x = lp.lam;
	lp.lam = fabs(lp.lam);
	xy.x *= cos((C0 + lp.lam * (C1 + lp.lam * lp.lam * C3)) *
		(lp.phi * (D1 + D5 * lp.phi * lp.phi * lp.phi * lp.phi)));
	return (xy);
}
FREEUP; if (P) free(P); }
ENTRY0(denoy) P->es = 0.; P->fwd = s_forward; ENDENTRY(P)
/*
** $Log: PJ_denoy.c,v $
** Revision 2.1  2003/03/28 01:46:50  gie
** Initial
**
*/
