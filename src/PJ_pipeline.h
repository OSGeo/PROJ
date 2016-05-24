#ifndef PJ_PIPELINE_H
#define PJ_PIPELINE_H


#ifdef __cplusplus
extern "C" {
#endif


/* View one type as the other. Definitely a hack - perhaps even a kludge */
/* Using a union is more appropriate (see below) */
#define ASXYZ(lpz) (*(XYZ *)(&lpz))
#define ASLPZ(xyz) (*(LPZ *)(&xyz))
#define ASXY(lp)   (*(XY  *)(&lp ))
#define ASLP(xy)   (*(LP  *)(&xy ))

/* Avoid explicit type-punning: Use a union */
typedef union {
    XYZ xyz;
    LPZ lpz;
    XY  xy;
    LP  lp;
} COORDINATE;


/* Apply the most appropriate projection function. No-op if none appropriate */
COORDINATE pj_apply_projection (COORDINATE point, int direction, PJ *P);

/* For debugging etc. */
int pj_show_coordinate (char *banner, COORDINATE point, int angular);



#ifdef __cplusplus
}
#endif

#endif /* end of basic projections header */
