/* print projection's list of parameters */
#ifndef lint
static const char SCCSID[]="@(#)pj_pr_list.c	4.6   94/03/19 GIE REL";
#endif
#include <projects.h>
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
