/*
** libproj -- library of cartographic projections
**
** Copyright (c) 2003   Gerald I. Evenden
*/
static const char
LIBPROJ_ID[] = "$Id: PJ_putp3.c,v 2.1 2003/03/28 01:46:51 gie Distr. gie $";
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
	double	A;
#define PJ_LIB__
# include	<lib_proj.h>
PROJ_HEAD(putp3, "Putnins P3") "\n\tPCyl., Sph.";
PROJ_HEAD(putp3p, "Putnins P3'") "\n\tPCyl., no inv., Sph.";
#define C	0.79788456
#define RPISQ	0.1013211836
FORWARD(s_forward); /* spheroid */
	xy.x = C * lp.lam * (1. - P->A * lp.phi * lp.phi);
	xy.y = C * lp.phi;
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	lp.phi = xy.y / C;
	lp.lam = xy.x / (C * (1. - P->A * lp.phi * lp.phi));
	return (lp);
}
FREEUP; if (P) free(P); }
	static PJ *
setup(PJ *P) {
	P->es = 0.; P->inv = s_inverse; P->fwd = s_forward;
	return P;
}
ENTRY0(putp3)  P->A = 4. * RPISQ; ENDENTRY(setup(P))
ENTRY0(putp3p) P->A = 2. * RPISQ; ENDENTRY(setup(P))
/*
** $Log: PJ_putp3.c,v $
** Revision 2.1  2003/03/28 01:46:51  gie
** Initial
**
*/
