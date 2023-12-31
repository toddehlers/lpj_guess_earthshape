/*
** libproj -- library of cartographic projections
**
** Copyright (c) 2003   Gerald I. Evenden
*/
static const char
LIBPROJ_ID[] = "$Id: PJ_eck5.c,v 2.1 2003/03/28 01:46:51 gie Distr. gie $";
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
PROJ_HEAD(eck5, "Eckert V") "\n\tPCyl, Sph.";
#define XF	0.44101277172455148219
#define RXF	2.26750802723822639137
#define YF	0.88202554344910296438
#define RYF	1.13375401361911319568
FORWARD(s_forward); /* spheroid */
	xy.x = XF * (1. + cos(lp.phi)) * lp.lam;
	xy.y = YF * lp.phi;
	return (xy);
}
INVERSE(s_inverse); /* spheroid */
	lp.lam = RXF * xy.x / (1. + cos( lp.phi = RYF * xy.y));
	return (lp);
}
FREEUP; if (P) free(P); }
ENTRY0(eck5); P->es = 0.; P->inv = s_inverse; P->fwd = s_forward; ENDENTRY(P)
/*
** $Log: PJ_eck5.c,v $
** Revision 2.1  2003/03/28 01:46:51  gie
** Initial
**
*/
