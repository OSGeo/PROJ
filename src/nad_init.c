/* open structure for NAD27<->NAD83 conversion */
#ifndef lint
static const char SCCSID[]="@(#)nad_init.c	4.5   94/10/30 GIE REL";
#endif
#define PJ_LIB__
#include <projects.h>
#include <stdio.h>
#include <errno.h>
extern FILE *pj_open_lib(char *, char *);
	struct CTABLE *
nad_init(char *name) {
	char fname[MAX_PATH_FILENAME+1];
	struct CTABLE *ct;
	FILE *fid;
	size_t i;

	errno = pj_errno = 0;
	strcpy(fname, "nad2783/");
	strcat(fname, name);
	if (!(fid = pj_open_lib(fname, "rb"))) {
		pj_errno = errno;
		return 0;
	}
	if (!(ct = pj_malloc(sizeof(struct CTABLE))) ||
		fread(ct, sizeof(struct CTABLE), 1, fid) != 1 ||
		!(ct->cvs = (FLP *)pj_malloc(i=sizeof(FLP)*ct->lim.lam*ct->lim.phi)) ||
		fread(ct->cvs, i, 1, fid) != 1) {
		nad_free(ct);
		pj_errno = -38;
		ct = 0;
		return 0;
	}
	fclose(fid);
	pj_errno = 0;
	return ct;
}
	void
nad_free(struct CTABLE *ct) {
	if (ct) {
		pj_dalloc(ct->cvs);
		pj_dalloc(ct);
	}
}
