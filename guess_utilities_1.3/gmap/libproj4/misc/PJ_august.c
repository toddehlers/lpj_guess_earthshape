/*
** libproj -- library of cartographic projections
**
** Copyright (c) 2003   Gerald I. Evenden
*/
static const char
LIBPROJ_ID[] = "$Id: PJ_august.c,v 2.1 2003/03/28 01:46:39 gie Distr. gie $";
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
PROJ_HEAD(august, "August Epicycloidal") "\n\tMisc Sph, no inv.";
#define M 1.333333333333333
FORWARD(s_forward); /* spheroid */
	double t, c1, c, x1, x12, y1, y12;

	t = tan(.5 * lp.phi);
	c1 = sqrt(1. - t * t);
	c = 1. + c1 * cos(lp.lam *= .5);
	x1 = sin(lp.lam) *  c1 / c;
	y1 =  t / c;
	xy.x = M * x1 * (3. + (x12 = x1 * x1) - 3. * (y12 = y1 *  y1));
	xy.y = M * y1 * (3. + 3. * x12 - y12);
	return (xy);
}
FREEUP; if (P) free(P); }
ENTRY0(august) P->inv = 0; P->fwd = s_forward; P->es = 0.; ENDENTRY(P)
/*
** $Log: PJ_august.c,v $
** Revision 2.1  2003/03/28 01:46:39  gie
** Initial
**
*/
