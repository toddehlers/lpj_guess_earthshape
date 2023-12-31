/*
** libproj -- library of cartographic projections
**
** Copyright (c) 2003   Gerald I. Evenden
*/
static const char
LIBPROJ_ID[] = "$Id: pj_pr_list.c,v 2.1 2003/03/28 01:44:30 gie Distr. gie $";
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
** Print projection's list of parameters
*/
#include <lib_proj.h>
#include <stdio.h>
#include <string.h>
#define LINE_LEN 72
	static int
pr_list(PJ *P, int not_used) {
	paralist *t;
	int l, n = 1, flag = 0;

	(void)putchar('#');
	for (t = P->params; t; t = t->next)
		if ((!not_used && t->used) || (not_used && !t->used)) {
			l = strlen(t->param) + 1;
			if (n + l > LINE_LEN) {
				(void)fputs("\n#", stdout);
				n = 2;
			}
			(void)putchar(' ');
			if (*(t->param) != '+')
				(void)putchar('+');
			(void)fputs(t->param, stdout);
			n += l;
		} else
			flag = 1;
	if (n > 1)
		(void)putchar('\n');
	return flag;
}
	void /* print link list of projection parameters */
pj_pr_list(PJ *P) {
	char const *s;

	(void)putchar('#');
	for (s = P->descr; *s ; ++s) {
		(void)putchar(*s);
		if (*s == '\n')
			(void)putchar('#');
	}
	(void)putchar('\n');
	if (pr_list(P, 0)) {
		(void)fputs("#--- following specified but NOT used\n", stdout);
		(void)pr_list(P, 1);
	}
}
/*
** $Log: pj_pr_list.c,v $
** Revision 2.1  2003/03/28 01:44:30  gie
** Initial
**
*/
