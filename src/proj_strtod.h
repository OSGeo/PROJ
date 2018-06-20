/* Internal header for proj_strtod.c */

#ifndef PROJ_STRTOD_H
#define PROJ_STRTOD_H

#ifdef __cplusplus
extern "C" {
#endif

double proj_strtod(const char *str, char **endptr);
double proj_atof(const char *str);

#ifdef __cplusplus
}
#endif

#endif /* PROJ_STRTOD_H */
