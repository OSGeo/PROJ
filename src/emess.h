/* Error message processing header file */
#ifndef EMESS_H
#define EMESS_H

#ifdef __cplusplus
extern "C" {
#endif

struct EMESS {
	char	*File_name,	/* input file name */
			*Prog_name;	/* name of program */
	int		File_line;	/* approximate line read
							where error occurred */
};

#ifdef EMESS_ROUTINE	/* use type */
/* for emess procedure */
struct EMESS PROJ_DLL emess_dat = { (char *)0, (char *)0, 0 };

#ifdef sun /* Archaic SunOs 4.1.1, etc. */
extern char *sys_errlist[];
#define strerror(n) (sys_errlist[n])
#endif

#else	/* for for calling procedures */

extern struct EMESS PROJ_DLL emess_dat;

#endif /* use type */

void PROJ_DLL emess(int, const char *, ...);

#ifdef __cplusplus
}
#endif

#endif /* end EMESS_H */
