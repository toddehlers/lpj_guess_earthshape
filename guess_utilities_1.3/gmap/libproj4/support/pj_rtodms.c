/* Convert radian argument to DMS ascii format */
/*
** Copyright (c) 2003   Gerald I. Evenden
*/
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
static const char RCS_ID[] = "$Id: pj_rtodms.c,v 2.4 2003/04/28 14:09:59 gie Exp gie $";
#include <lib_proj.h>
#include <stdio.h>
#include <string.h>
/*
** RES is fractional second figures
** RES60 = 60 * RES
** CONV = 180 * 3600 * RES / PI (radians to RES seconds)
*/
	static double
RES = 1000.,
RES60 = 60000.,
CONV = 206264806.24709635515796003417;
	static char
format[50] = "%dd%d'%.3f\"%c";
	static int
dolong = 0;
	void
pj_set_rtodms(int fract, int con_w) {
	int i;

	if (fract >= 0 && fract < 9 ) {
		RES = 1.;
		/* following not very elegant, but used infrequently */
		for (i = 0; i < fract; ++i)
			RES *= 10.;
		RES60 = RES * 60.;
		CONV = 180. * 3600. * RES / PI;
		if (! con_w)
			(void)sprintf(format,"%%dd%%d'%%.%df\"%%c", fract);
		else
			(void)sprintf(format,"%%dd%%02d'%%0%d.%df\"%%c",
				fract+2+(fract?1:0), fract);
		dolong = con_w;
	}
}
	char *
pj_rtodms(char *s, double r, const char * csign) {
	int deg, min, sign;
	char *ss = s;
	double sec;

	if (r < 0) {
		r = -r;
		if  (!csign) {
			*ss++ = '-';
			sign = 0;
		} else 
			sign = csign[1];
	} else
		sign = csign ? csign[0] : 0;
	r = floor(r * CONV + .5);
	sec = fmod(r / RES, 60.);
	r = floor(r / RES60);
	min = (int)fmod(r, 60.);
	deg = (int)(r / 60.);
	if (dolong)
		(void)sprintf(ss,format,deg,min,sec,sign);
	else if (sec) {
		char *p, *q;

		(void)sprintf(ss,format,deg,min,sec,sign);
		for (q = p = ss + strlen(ss) - (sign ? 3 : 2); *p == '0'; --p) ;
		if (*p != '.')
			++p;
		if (++q != p)
			(void)strcpy(p, q);
	} else if (min)
		(void)sprintf(ss,"%dd%d'%c",deg,min,sign);
	else
		(void)sprintf(ss,"%dd%c",deg, sign);
	return s;
}
/*
** $Log: pj_rtodms.c,v $
** Revision 2.4  2003/04/28 14:09:59  gie
** changed sign suffix input
**
** Revision 2.3  2003/04/23 17:48:13  gie
** corrected include file name
**
** Revision 2.2  2003/04/23 17:31:50  gie
** changed entry names to pj_*
**
** Revision 2.1  2003/03/28 15:38:31  gie
** Initial
**
*/

