/*
** libproj -- library of cartographic projections
**
** Copyright (c) 2003   Gerald I. Evenden
*/
static const char
LIBPROJ_ID[] = "$Id: PJ_dummy.c,v 1.1 2003/05/13 15:40:09 gie Exp gie $";
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
/* This procedure is to provide two dummy entries in the list of
 * projections that can be used when developing a new projection
 * without recompiling the projection list until the new projection
 * is developed and vetted.  The new projection should be called "dummy1"
 * or "dummy2" during this test period and simply linked in with
 * with "make lproj T=PJ_new.o" exection.
 * 
 * If these projections are selected they will return a failed,
 * null P value.
*/
#define PROJ_PARMS__ \
	double	temp;
#define PJ_LIB__
#include	<lib_proj.h>
PROJ_HEAD(dummy, "Dummy projection") "\n\tfor development purposes";
FREEUP; if (P) free(P); }
ENTRY0(dummy)
	E_ERROR_0;
ENDENTRY(0)
/*
** $Log: PJ_dummy.c,v $
** Revision 1.1  2003/05/13 15:40:09  gie
** Initial revision
**
*/
