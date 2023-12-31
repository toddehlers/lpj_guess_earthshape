/*
** libproj -- library of cartographic projections
**
** Copyright (c) 2003, 2005   Gerald I. Evenden
*/
static const char
LIBPROJ_ID[] = "$Id: pj_translate.c,v 2.3 2005/02/03 16:52:32 gie Exp gie $";
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
** Procedure to translate axis on a sphere
*/
#define PJ_LIB__
#include <lib_proj.h>

struct TRANS {
	double cosal, sinal, beta; \
	int mode;
};
#define TOL 1e-12
#define EN ((struct TRANS *)en)
	LP
pj_translate(LP in, const void *en) {
	LP out;
	double cp, sp, cl;

	cp = cos(in.phi);
	cl = cos(in.lam);
	switch (EN->mode) {
	case 0:
		sp = sin(in.phi);
		out.phi = pj_asin(EN->sinal * sp - EN->cosal * cp * cl);
		out.lam = pj_adjlon(
			atan2(cp * sin(in.lam) , (EN->sinal * cp * cl + EN->cosal * sp))
			+ EN->beta);
		break;
	case 1:
		out.phi = pj_asin(- cp * sin(in.lam));
		out.lam = atan2(-cl, tan(in.phi));
		break;
	case 2:
		out.phi = pj_asin(- cp * sin(in.lam));
		out.lam = atan2(tan(in.phi), cl);
		break;
	}	
	return (out);
}
	LP
pj_inv_translate(LP in, const void *en) {
	LP out;
	double cp, sp, cl;

	cp = cos(in.phi);
	cl = cos(in.lam);
	switch (EN->mode) {
	case 0:
		in.lam -= EN->beta;
		sp = sin(in.phi);
		out.phi = pj_asin(EN->sinal * sp + EN->cosal * cp * cl);
		out.lam = atan(cp * sin(in.lam) / 
			(EN->sinal * cp * cl - EN->cosal * sp));
		break;
	case 1:
		out.phi = cp * cl;
		out.lam = atan2(-sin(in.lam),tan(in.phi));
		break;
	case 2:
		out.phi = cp * sin(in.lam);
		out.lam = atan2(cl,tan(in.phi));
		break;
	}
	return (out);
}
	void *
pj_translate_ini(double alpha, double beta) {
	struct TRANS *en;

	if ((en = (struct TRANS *)malloc(sizeof(struct TRANS))) == NULL)
		return (NULL);
	EN->mode = 0;
	if ( fabs(alpha) < TOL ) {
		if ( (fabs(beta) < TOL) )
			EN->mode = 1;
		else if ( (fabs(beta - HALFPI) < TOL) )
			EN->mode = 2;
	}
	if (!EN->mode) {
		EN->sinal = sin(alpha);
		EN->cosal = cos(alpha);
		EN->beta = beta;
	}
	return ((void *)en);
}
