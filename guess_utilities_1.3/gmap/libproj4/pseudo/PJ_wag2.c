/*
** libproj -- library of cartographic projections
**
** Copyright (c) 2003   Gerald I. Evenden
*/
static const char
LIBPROJ_ID[] = "$Id: PJ_wag2.c,v 2.2 2004/11/27 16:34:42 gie Exp gie $";
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
# include	<lib_proj.h>
PROJ_HEAD(wag2, "Wagner II") "\n\tPCyl., Sph.";
#define CX 0.9248327337222211159780313106
#define CY 1.387249100583331673967046966
#define CM2 0.8855017059025996450524064573
#define CM1 0.8802234877744129284498330453
FORWARD(s_forward); /* spheroid */
	lp.phi = pj_asin(CM1 * sin(CM2 * lp.phi));
	xy.x = CX * lp.lam * cos(lp.phi);
	xy.y = CY * lp.phi;
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	lp.phi = xy.y / CY;
	lp.lam = xy.x / (CX * cos(lp.phi));
	lp.phi = pj_asin(sin(lp.phi) / CM1) / CM2;
	return (lp);
}
FREEUP; if (P) free(P); }
ENTRY0(wag2) P->es = 0.; P->inv = s_inverse; P->fwd = s_forward; ENDENTRY(P)
/*
** $Log: PJ_wag2.c,v $
** Revision 2.2  2004/11/27 16:34:42  gie
** improved constant precision
**
** Revision 2.1  2003/03/28 01:46:52  gie
** Initial
**
*/
