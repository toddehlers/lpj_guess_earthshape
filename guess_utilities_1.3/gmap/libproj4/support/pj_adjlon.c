/*
** libproj -- library of cartographic projections
**
** Copyright (c) 2003   Gerald I. Evenden
*/
static const char
LIBPROJ_ID[] = "$Id: pj_adjlon.c,v 2.1 2003/03/28 01:20:31 gie Distr. gie $";
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
/*
** Reduce argument to range +/- PI
*/

#include <math.h>
#define L_TWOPI 6.28318530717958647692528677
#define L_RPI   0.3183098861837906715377675
#define TOLPI 1e-14
	double
pj_adjlon(double lon) {
	double x;

	if ((fabs(x = lon * L_RPI) - 1.) > TOLPI) {
		x = 0.5 * (x + 1);
		lon = ((x - floor(x)) - 0.5) * L_TWOPI;
	}
	return( lon );
}
/*
** $Log: pj_adjlon.c,v $
** Revision 2.1  2003/03/28 01:20:31  gie
** Initial
**
*/
