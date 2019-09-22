/* Error message processing header file */
#ifndef EMESS_H
#define EMESS_H

struct EMESS {
	char	*File_name,	/* input file name */
			*Prog_name;	/* name of program */
	int		File_line;	/* approximate line read
							where error occurred */
};

#ifdef EMESS_ROUTINE	/* use type */
/* for emess procedure */
struct EMESS emess_dat = { nullptr, nullptr, 0 };

#else	/* for for calling procedures */

extern struct EMESS emess_dat;

#endif /* use type */

void emess(int, const char *, ...);

#endif /* end EMESS_H */
