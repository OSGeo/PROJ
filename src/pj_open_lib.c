/* standard location file open procedure */
#ifndef lint
static const char SCCSID[]="@(#)pj_open_lib.c	4.5   94/10/30 GIE REL";
#endif
#define PJ_LIB__
#include <projects.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

static const char *(*pj_finder)(const char *) = NULL;
static char * proj_lib_name =
#ifdef PROJ_LIB
PROJ_LIB;
#else
0;
#endif

/************************************************************************/
/*                           pj_set_finder()                            */
/************************************************************************/

void pj_set_finder( const char *(*new_finder)(const char *) )

{
    pj_finder = new_finder;
}

/************************************************************************/
/*                            pj_open_lib()                             */
/************************************************************************/

FILE *
pj_open_lib(char *name, char *mode) {
	char fname[MAX_PATH_FILENAME+1], *sysname;
	FILE *fid;
	int n = 0;

	/* check if ~/name */
	if (*name == '~' && name[1] == DIR_CHAR)
		if (sysname = getenv("HOME")) {
			(void)strcpy(fname, sysname);
			fname[n = strlen(fname)] = DIR_CHAR;
			fname[++n] = '\0';
			(void)strcpy(fname+n, name + 1);
			sysname = fname;
		} else
			return NULL;

	/* or fixed path: /name, ./name or ../name */
	else if (*name == DIR_CHAR || (*name == '.' && name[1] == DIR_CHAR) ||
		(!strncmp(name, "..", 2) && name[2] == DIR_CHAR) )
		sysname = name;

        /* or try to use application provided file finder */
        else if( pj_finder != NULL && pj_finder( name ) != NULL )
            sysname = pj_finder( name );
        
	/* or is environment PROJ_LIB defined */
	else if ((sysname = getenv("PROJ_LIB")) || (sysname = proj_lib_name)) {
		(void)strcpy(fname, sysname);
		fname[n = strlen(fname)] = DIR_CHAR;
		fname[++n] = '\0';
		(void)strcpy(fname+n, name);
		sysname = fname;
	} else /* just try it bare bones */
		sysname = name;

        if( getenv( "PROJ_DEBUG" ) != NULL )
            fprintf( stderr, "pj_open_lib(%s): call fopen(%s).\n",
                     name, sysname);
        
	if (fid = fopen(sysname, mode))
		errno = 0;
	return(fid);
}
