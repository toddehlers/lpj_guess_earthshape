/*
** libproj -- library of cartographic projections
**
** Copyright (c) 2003   Gerald I. Evenden
*/
static const char
LIBPROJ_ID[] = "$Id: PJ_oxford.c,v 2.1 2003/03/28 01:46:51 gie Distr. gie $";
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
	int oxford_mode;
#define PJ_LIB__
# include	<lib_proj.h>
PROJ_HEAD(oxford, "Oxford Atlas") "\n\tPCyl., Sph.";
PROJ_HEAD(times, "Times Atlas") "\n\tPCyl., Sph.";
#define C_Y 1.70710678118654752440084436
#define C_X 0.707106781186547524400844362
FORWARD(s_forward); /* spheroid */
	double c, t;

	xy.y = C_Y * (t = tan(0.5 * lp.phi));
	if (P->oxford_mode) {
		c = lp.phi * lp.phi;
		xy.x = lp.lam * C_X * (1. - 0.04 * c * c);
	} else
		xy.x = lp.lam * 0.74 * sqrt(1 - 0.5 * t * t);
	return (xy);
}
FREEUP; if (P) free(P); }
	static PJ *
setup(PJ *P, int mode) {
	P->es = 0.;
	P->fwd = s_forward;
	P->inv = 0;
	P->oxford_mode = mode;
	return P;
}
ENTRY0(oxford) ENDENTRY(setup(P, 1))
ENTRY0(times) ENDENTRY(setup(P, 0))
/*
** $Log: PJ_oxford.c,v $
** Revision 2.1  2003/03/28 01:46:51  gie
** Initial
**
*/
