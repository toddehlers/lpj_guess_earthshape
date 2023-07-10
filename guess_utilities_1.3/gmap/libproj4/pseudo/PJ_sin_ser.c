/*
** libproj -- library of cartographic projections
**
** Copyright (c) 2003   Gerald I. Evenden
*/
static const char
LIBPROJ_ID[] = "$Id: PJ_sin_ser.c,v 1.1 2004/02/22 21:31:03 gie Exp gie $";
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
	double	fact, rfact;
#define PJ_LIB__
#include	<lib_proj.h>
#define SINSER2	0.9
#define SINSER3	0.75
#define SINSER4	0.6666666666666666667
PROJ_HEAD(sinser2, "McBryde-Thomas Sine Series #2") "\n\tPCyl, Sph., No Inv.";
PROJ_HEAD(sinser3, "McBryde-Thomas Sine Series #3") "\n\tPCyl, Sph., No Inv.";
PROJ_HEAD(sinser4, "McBryde-Thomas Sine Series #4") "\n\tPCyl, Sph., No Inv.";

FORWARD(s_forward); /* sphere */
	double theta = lp.phi * P->fact;
	
	xy.x = lp.lam * cos(lp.phi) / cos(theta);
	xy.y = P->rfact * sin(theta);
	return (xy);
}
FREEUP; if (P) free(P); }
	static void
setup(PJ *P) {
	P->es = 0;
	P->rfact = 1.0 / P->fact;
	P->fwd = s_forward;
}
ENTRY0(sinser2)
	P->fact = SINSER2;
	setup(P);
ENDENTRY(P)
ENTRY0(sinser3)
	P->fact = SINSER3;
	setup(P);
ENDENTRY(P)
ENTRY0(sinser4)
	P->fact = SINSER4;
	setup(P);
ENDENTRY(P)
/*
** $Log: PJ_sin_ser.c,v $
** Revision 1.1  2004/02/22 21:31:03  gie
** Initial revision
**
*/
