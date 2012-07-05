/******************************************************************************
 * $Id: PJ_healpix.c 1504 2011-10-18 14:58:57Z landcare $
 *
 * Project:  PROJ.4
 * Purpose:  Implementation of the healpix projection.
 *           Definition: http://code.scenzgrid.org/index.php/p/scenzgrid-py/source/tree/master/docs/scenzgrid.pdf
 * Author:   Alex Raichev & Michael Speth , spethm@landcareresearch.co.nz
 *
 ******************************************************************************
 * Copyright (c) 2001, Thomas Flemming, tf@ttqv.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *****************************************************************************/

#define PROJ_PARMS__ \
    int npole;\
    int spole;

#define PJ_LIB__
# include	<projects.h>
PROJ_HEAD(healpix, "HEALPix") "\n\tSph., Ellps.";
PROJ_HEAD(rhealpix, "rHEALPix") "\n\tSph., Ellps.\n\tnpole= spole=";
# include	<stdio.h>
# define R1 {{ 0,-1},{ 1, 0}}	/** Matrix for anticlockwise rotation by pi/2 **/
# define R2 {{-1, 0},{ 0,-1}}	/** Matrix for anticlockwise rotation by pi (R1 X R1) X = dot product **/
# define R3 {{ 0, 1},{-1, 0}}	/** Matrix for anticlockwise rotation by 3*pi/2 (R2 X R1) **/
# define IDENT {{1,0},{0,1}}
/**
 * 0 - Identity matrix<br>
 * 1 - Counter-clockwise rotation by PI/2<br>
 * 2 - Counter-clockwise rotation by PI<br>
 * 3 - Counter-clockwise rotation by 3*PI/2<br>
 * 4 - Counter-clockwise rotation by 3*PI/2<br>
 * 5 - Counter-clockwise rotation by PI<br>
 * 6 - Counter-clockwise rotation by PI/2<br>
 **/
# define ROT { IDENT, R1, R2, R3, R3, R2, R1}
# define RFACTOR 3 /** Used for returning the rotation matrix **/
/** Used for calculating if a point is within the HEALPix projection for sphere. **/
# define EPS 1e-12
typedef struct {
    int cn; // the number 0 -> 4 indicating the position of the polar cap.
    double x,y;  // the coordinates of the pole points (point of most extreme latitude on the polar caps).
    enum Region { north, south, equatorial } region;
} CapMap;
typedef struct {
    double x,y;
} Point;
double rot[7][2][2] = ROT;

/**
    NOTES:  Alex Raichev implemented the math in python and this is a port of his work.
	    The healpix projection is a Lambert cylindrical equal-area projection for
	    equaltorial latitudes and an interrupted Colignon projection for polar  
	    latitudes.
 **/

/**
 * Returns the sign of the double.
 * @param v the parameter whose sign is returned.
 * @return 1 for positive number, -1 for negative, and 0 for zero.
 **/
double sign (double v) {
    return v > 0 ? 1 : (v < 0 ? -1 : 0);
}
/**
 * Scales the number by a factor.
 * @param num the number to be scaled.
 * @param factor the factor to scale the number by.
 * @param isInverse 1 for scaling the number by 1 / factor and 0 for scaling by the factor.
 * @return the scaled number.
 **/
double scale_number(double num, double factor, int isInverse){
    if(isInverse == 1){
	return num * 1.0/factor;
    }
    return num * factor;
}
/**
 * Scales all the items of the array by a factor.
 * @param xy
 **/
void scale_array(XY *array, double k, int inverse){
    double c = 0;
    if (inverse == 1) {
	c = 1.0/k;
    }else{
	c = k;
    }
    array->x *= c;
    array->y *= c;
}
/**
 * Given an angle return its equivalent angle.
 * @param x the angle to convert
 * @return the equivalent angle such that -PI <= the angle returend <= PI
 **/
double standardize_lon(double x){
    if(x < -1*PI || x >= PI){
	x = x - 2*PI*floor(x/(2*PI));	
	if(x >= PI){
	    x = x - 2*PI;
	}
    }
    return x;
}
/**
 * Given an angle, return its unit-circle equivalent angle.
 * @param x the angel to convert.
 * @return the equivalent angle such that -PI/2 <= the angle returned <= PI/2.
 **/
double standardize_lat(double x){
    if( x < -PI/2.0 || x > PI/2){
	x = x-2.0*PI*floor(x/(2.0*PI));
	if(x > PI/2.0 && x <= 3.0*PI/2){
	    x = PI - x;
	}else{
	    x = x - 2*PI;
	}
    }
    return x;
}
/**
 * Returns the index of the 2d array in rot.
 * @param index range from -3 to 3.
 * @return the index into the rot 3d array.
 */
static int get_rotate_index(int index){
    switch(index){
	case 0:
	    return 0;
	case 1:
	    return 1;
	case 2: 
	    return 2;
	case 3: 
	    return 3;
	case -1:
	    return 4;
	case -2:
	    return 5;
	case -3:
	    return 6;
    }
    return 0;
}
/**
 * Calculates if the point lies on or within the polygon.
 * Very good explination of how this works: http://paulbourke.net/geometry/insidepoly/
 * @param nvert the number of vertices in the polygon.
 * @param vert the x,y-coordinates of the polygon's vertices
 * @param testx the x-coordinate of the test point.
 * @param testy the y-coordinate of the test point.
 * @return 1 if on or within the bounds of the polygon, and 0 otherwise.
 **/
static
int pnpoly(int nvert, double vert[][2], double testx, double testy){
    
    int i,j,c = 0;
    int counter = 0;
    double xinters;
    Point p1,p2;

    // check for boundrary cases
    for(i = 0; i < nvert; i++){
	if(testx == vert[i][0] && testy == vert[i][1]){
	    return 1;
	}
    }

    // initialize p1
    p1.x = vert[0][0];
    p1.y = vert[0][1];
    
    for(i = 1; i < nvert; i++){
	p2.x = vert[i % nvert][0];
	p2.y = vert[i % nvert][1];

	if(testy > MIN(p1.y,p2.y)){
	    if (testy <= MAX(p1.y,p2.y)) {
		if (testx <= MAX(p1.x,p2.x)) {
		    if (p1.y != p2.y) {
			xinters = (testy-p1.y)*(p2.x-p1.x)/(p2.y-p1.y)+p1.x;
			if (p1.x == p2.x || testx <= xinters){
			    counter++;
			}
		    }
		}
	    }
	}
	p1 = p2;
    }
    if(counter % 2 == 0){
	return 0;
    }else{
	return 1;
    }
    return c;
}
/**
 * Calculates if the coordinates are within the image of projection.
 * @param x the x-coordinate to check.
 * @param y the y-coordinate to check.
 * @param proj 0 for healpix and 1 for rhealpix.
 * @param npole the positions of the polar squares, only used for rhealpix.
 * @param spole the positions of the polar squares, only used for rhealpix.
 * @return 1 if the coordinate is within the projection and 0 otherwise.
 **/
int in_image(double x, double y, int proj, int npole, int spole){
    if(proj == 0){
	double healpixVertsJit[][2] = {
	    {-1.0*PI-EPS    ,PI/4.0},
	    {-3.0*PI/4.0    ,PI/2.0+EPS},
	    {-1.0*PI/2.0    ,PI/4.0+EPS},
	    {-1.0*PI/4.0    ,PI/2.0+EPS},
	    {0.0	    ,PI/4.0+EPS},
	    {PI/4.0	    ,PI/2.0+EPS},
	    {PI/2.0	    ,PI/4.0+EPS},
	    {3.0*PI/4.0	    ,PI/2.0+EPS},
	    {PI+EPS	    ,PI/4.0},
	    {PI+EPS	    ,-1.0*PI/4.0},
	    {3.0*PI/4.0	    ,-1.0*PI/2.0-EPS},
	    {PI/2.0	    ,-1.0*PI/4.0-EPS},
	    {PI/4.0	    ,-1.0*PI/2.0-EPS},
	    {0.0	    ,-1.0*PI/4.0-EPS},
	    {-1.0*PI/4.0    ,-1.0*PI/2.0-EPS},
	    {-1.0*PI/2.0    ,-1.0*PI/4.0-EPS},
	    {-3.0*PI/4.0    ,-1.0*PI/2.0-EPS},
	    {-1.0*PI-EPS    ,-1.0*PI/4.0}};
	return pnpoly((int)sizeof(healpixVertsJit)/sizeof(healpixVertsJit[0]),
                  healpixVertsJit,x,y);
    }else{
	// Used for calculating if a point is within the rHEALPix projection for sphere.
	double rhealpixVertsJit[][2] = {
	    {-1.0*PI-EPS			,PI/4.0+EPS},
	    {-1.0*PI + npole*PI/2.0-EPS		,PI/4.0+EPS},
	    {-1.0*PI + npole*PI/2.0-EPS		,3*PI/4.0+EPS},
	    {-1.0*PI + (npole + 1.0)*PI/2.0+EPS ,3*PI/4.0+EPS},
	    {-1.0*PI + (npole + 1.0)*PI/2.0+EPS ,PI/4.0+EPS},
	    {PI+EPS				,PI/4.0+EPS},
	    {PI+EPS				,-1.0*PI/4.0-EPS},
	    {-1.0*PI + (spole + 1.0)*PI/2.0+EPS ,-1.0*PI/4.0-EPS},
	    {-1.0*PI + (spole + 1.0)*PI/2.0+EPS ,-3.0*PI/4.0-EPS},
	    {-1.0*PI + spole*PI/2.0-EPS		,-3.0*PI/4.0-EPS},
	    {-1.0*PI + spole*PI/2.0-EPS		,-1.0*PI/4.0-EPS},
	    {-1.0*PI-EPS			,-1.0*PI/4.0-EPS}};
	return pnpoly((int)sizeof(rhealpixVertsJit)/sizeof(rhealpixVertsJit[0]),
                  rhealpixVertsJit,x,y);
    }
}
/**
 * Returns an authalic latitude of the point given a point of geographic 
 * latitude phi on an ellipse of eccentricity e.
 * pj_authlat is the inverse of the alex's auth_lat.
 * @param phi
 * @param e
 * @param inverse 1 for inverse or 0 otherwise.
 * @return the authalic latitude of the point.
 **/
double auth_lat(double phi, double e, int inverse){
    if(inverse == 0){
	double q_numerator = ((1.0 - pow(e,2.0)) * sin(phi));
	double q_demonitor =  (1.0 - (pow(e*sin(phi),2.0)));
	double q_subtractor =  - (1.0 - pow(e,2.0)) / (2.0*e) * log((1.0 - e*sin(phi)) / (1.0+e*sin(phi)));
	double q = ((1.0 - pow(e,2.0)) * sin(phi)) / (1.0 - (pow(e*sin(phi),2.0))) - 
	(1.0 - pow(e,2.0)) / (2.0*e) * log((1.0 - e*sin(phi)) / (1.0+e*sin(phi)));

	double qp = 1.0 - (1.0-pow(e,2.0)) / (2.0*e)*log((1.0 - e) / (1.0 + e));
	double ratio = q/qp;
	// Rounding errors
	if( fabsl(ratio) > 1){
	    ratio = sign(ratio);
	}
	return asin(ratio);
    }
    return phi + (pow(e,2) / 3.0 + 31*pow(e,4) / 180.0 + 517.0*pow(e,6)/5040.0) * sin(2.0*phi)
    + (23.0*pow(e,4)/360.0 + 251.0*pow(e,6)/3780.0)*sin(4.0*phi)
    + 761.0*pow(e,6)/45360.0 * sin(6.0*phi);
}
/**
 * Compute the forward signature functions of the HEALPix 
 * projection of a sphere with radius `R` and central meridian `lon0`. 
**/
XY healpix_sphere(LP lp, PJ *P){
    double lam = standardize_lon(lp.lam);
    double phi = standardize_lat(lp.phi);
    double phi0 = aasin(P->ctx, 2.0/3.0);
    XY xy;
    // equatorial region
    if( fabsl(phi) <= phi0) {
	xy.x = lam;
	xy.y = 3.0*PI/8.0*sin(phi);
    } else {
	double lamc;
	double sigma = sqrt(3.0 * (1 - fabsl(sin(phi))));
	double cn = floor(2 * lam / PI + 2);
	if (cn >= 4) {
	    cn = 3;
	}
	lamc = -3*PI/4 + (PI/2)*cn;
	xy.x = lamc + (lam - lamc) * sigma;
	xy.y = sign(phi)*PI/4 * (2 - sigma);
    }
    xy.x = scale_number(xy.x,P->a,0);
    xy.y = scale_number(xy.y,P->a,0);
    return xy;
}
/**
 * Compute the inverse signature functions of the HEALPix 
 * projection of a sphere with radius `R` and central meridian `lon0`. 
**/
LP healpix_sphere_inv(XY xy, PJ *P){
    double x,y,y0;
    double cn;
    double xc;
    double tau;
    LP lp; 
    // Scale down to radius 1 sphere
    x = scale_number(xy.x,P->a,1);
    y = scale_number(xy.y,P->a,1);
    y0 = PI/4.0;
    // Equatorial region.
    if(fabsl(y) <= y0){
	lp.lam = x;
	lp.phi = asin(8.0*y/(3.0*PI));	
    } else if(fabsl(y) < PI/2.0){
	cn = floor(2.0 * x/PI + 2.0);
	if(cn >= 4){
	    cn = 3;
	}
	xc = -3.0 * PI/4.0 + (PI/2.0)*cn;
	tau = 2.0 - 4.0*fabsl(y)/PI;
	lp.lam = xc + (x - xc)/tau;	
	lp.phi = sign(y)*asin(1.0 - pow(tau , 2.0)/3.0);
    } else {
	lp.lam = -1.0*PI - P->lam0;
	lp.phi = sign(y)*PI/2.0;
    }
    return (lp);
}
/**
 * Adds one vector to another of length 2.
 * @param a the first term.
 * @param b the second term.
 * @param ret holds the summation of the vectors.
 **/
static void vector_add(double a[], double b[],double * ret){
    int i;
    for(i = 0; i < 2; i++){
	ret[i] = a[i] + b[i];
    }
}
/**
 * Subs tracts one vector from another of length 2.
 * @param a the minuend.
 * @param b the subtrahend.
 * @param ret the difference of the vectors where the difference is the result of a minus b.
 **/
static void vector_sub(double a[], double b[], double * ret){
    int i;
    for(i = 0; i < 2; i++){
	ret[i] = a[i] - b[i];
    }
}
/**
 * Calculates the dot product of the arrays.
 * @param a the array that will be used to calculate the dot product.
 *  Must contain the same number of columns as b's rows.  Must be a matrix with equal lengthed rows and columns.
 * @param b the array that will be used to calculate the dot product; must contain the same number of rows as a's columns.
 * @param length the size of the b array.  Note, a's column size must equal b's length.
 * @param ret the dot product of a and b.
 **/
static void dot_product(double a[2][2], double b[], double * ret){
    int i,j;
    int length = 2;
    for(i = 0; i < length; i++){
	ret[i] = 0;
	for(j = 0; j < length; j++){
	    ret[i] += a[i][j]*b[j];
	}
    }
}
/**
 * Returns the polar cap number, pole point coordinates, and region
 * for x,y in the HEALPix projection of the sphere of radius R.
 * @param x coordinate in the HEALPix or rHEALPix.
 * @param y coordinate in the HEALPix or rHEALPix.
 * @param npole integer between 0 and 3 indicating the position of the north pole.
 * @param spole integer between 0 and 3 indicating teh position of the south pole.
 * @param inverse 1 computes the rHEALPix projection and 0 computes forward.
 * @return a structure containing the cap poles.
 **/
static CapMap get_cap(double x, double y, double R, int npole, int spole, int inverse){
    CapMap capmap;
    double c;

    capmap.x = x;
    capmap.y = y;

    if(inverse == 0){
	if(y > R*PI/4.0){
	    capmap.region = north;
	    c = R*PI/2.0; 
	}else if(y < -1*R*PI/4.0){
	    capmap.region = south;
	    c = -1*R*PI/2.0;
	}else{
	    capmap.region = equatorial;
	    capmap.cn = 0;
	    return capmap;
	}
	// polar region
	if(x < -1*R*PI/2.0){
	    capmap.cn = 0;
	    capmap.x = (-1*R*3.0*PI/4.0);
	    capmap.y = c;
	}else if(x >= -1*R*PI/2.0 && x < 0){
	    capmap.cn = 1;
	    capmap.x = -1*R*PI/4.0;
	    capmap.y = c;
	}else if(x >= 0 && x < R*PI/2.0){
	    capmap.cn = 2;
	    capmap.x = R*PI/4.0;
	    capmap.y = c;
	}else{
	    capmap.cn = 3;
	    capmap.x = R*3.0*PI/4.0;
	    capmap.y = c;
	}
	return capmap;
    }else{
	double c;
	double eps;
	if(y > R*PI/4.0){
	    capmap.region = north;
	    capmap.x = R*(-3.0*PI/4.0 + npole*PI/2.0); 
	    capmap.y = R*PI/2.0;
	    x = x - npole*R*PI/2.0;
	}else if(y < -1*R*PI/4.0){
	    capmap.region = south;
	    capmap.x = R*(-3.0*PI/4.0 + spole*PI/2); 
	    capmap.y = -1*R*PI/2.0;
	    x = x - spole*R*PI/2.0;
	}else{
	    capmap.region = equatorial;
	    capmap.cn = 0;
	    return capmap;
	}
	// Polar Region, find # of HEALPix polar cap number that
	// x,y moves to when rHEALPix polar square is disassembled.
	eps = R*1e-15; // Kludge.  Fuzz to avoid some rounding errors.
	if(capmap.region == north){
	    if(y >= -1*x - R*PI/4.0 - eps && y < x + R*5.0*PI/4.0 - eps){
		capmap.cn = (npole + 1) % 4;
	    }else if(y > -1*x -1*R*PI/4.0 + eps && y >= x + R*5.0*PI/4.0 - eps){
		capmap.cn = (npole + 2) % 4;
	    }else if(y <= -1*x -1*R*PI/4.0 + eps && y > x + R*5.0*PI/4.0 + eps){
		capmap.cn = (npole + 3) % 4;
	    }else{
		capmap.cn = npole;
	    }
	}else if(capmap.region == south){
	    if(y <= x + R*PI/4.0 + eps && y > -1*x - R*5.0*PI/4 + eps){
		capmap.cn = (spole + 1) % 4;
	    }else if(y < x + R*PI/4.0 - eps && y <= -1*x - R*5.0*PI/4.0 + eps){
		capmap.cn = (spole + 2) % 4;
	    }else if(y >= x + R*PI/4.0 - eps && y < -1*x - R*5.0*PI/4.0 - eps){
		capmap.cn = (spole + 3) % 4;
	    }else {
		capmap.cn = spole;
	    }
	}
	return capmap;
    }
}
/**
 * Rearrange point x,y in the HEALPix projection by
 * combining the polar caps into two polar squares.
 * Put the north polar square in position npole and
 * the south polar square in position spole.
 * @param x coordinate in the HEALPix projection of the sphere.
 * @param y coordinate in the HEALPix projection of the sphere.
 * @param R - the Sphere's radius.
 * @param npole integer between 0 and 3 indicating the position
 * of the north polar square.
 * @param spole integer between 0 and 3 indicating the position
 * of the south polar square.
 * @param inverse 1 to uncombine the polar caps and 0 to combine.
 **/
static XY combine_caps(double x, double y, double R, int npole, int spole, int inverse){
    XY xy;
    double v[2];
    double a[2];
    double vector[2];
    double tmpVect[2];
    double v_min_c[2];
    double ret_dot[2];
    double ret_add[2];
    CapMap capmap = get_cap(x,y,R,npole,spole,inverse);

    if(capmap.region == equatorial){
	xy.x = capmap.x;
	xy.y = capmap.y;
	return xy;
    }
    v[0] = x;
    v[1] = y;
    if(inverse == 0){
	// compute forward function by rotating, translating, and shifting xy.
	int pole = 0;
	double (*tmpRot)[2];
	double c[2] = {capmap.x,capmap.y};
	if(capmap.region == north){
	    pole = npole;
	    a[0] = R * (-3.0*PI/4.0 + pole * PI/2);
	    a[1] = R * (PI/2.0 + pole * 0);

	    tmpRot = rot[get_rotate_index(capmap.cn - pole)];
	    vector_sub(v,c,v_min_c);	
	    dot_product(tmpRot,v_min_c,ret_dot);
	    vector_add(ret_dot, a, vector);

	}else {
	    pole = spole;
	    a[0] = R * (-3.0*PI/4.0 + pole * PI/2);
	    a[1] = R * (PI/-2.0 + pole * 0);

	    tmpRot = rot[get_rotate_index(-1*(capmap.cn - pole))];
	    vector_sub(v,c,v_min_c);	
	    dot_product(tmpRot,v_min_c,ret_dot);

	    vector_add(ret_dot, a, vector);
	}

	xy.x = vector[0];
	xy.y = vector[1];
	return xy;
    }else{
	// compute inverse function.
	// get the current position of rHEALPix polar squares
	int cn;
	int pole = 0;
	double (*tmpRot)[2];
	double c[2] = {capmap.x,capmap.y};
	// disassemble
	if(capmap.region == north){
	    pole = npole;
	    a[0] = R * (-3.0*PI/4.0 + capmap.cn * PI/2);
	    a[1] = R * (PI/2.0 + capmap.cn * 0);

	    tmpRot = rot[get_rotate_index(-1*(capmap.cn - pole))];
	    vector_sub(v,c,v_min_c);	
	    dot_product(tmpRot,v_min_c,ret_dot);
	    vector_add(ret_dot, a, vector);

	}else{
	    pole = spole;
	    a[0] = R * (-3.0*PI/4.0 + capmap.cn * PI/2);
	    a[1] = R * (PI/-2.0 + capmap.cn * 0);

	    tmpRot = rot[get_rotate_index(capmap.cn - pole)];
	    vector_sub(v,c,v_min_c);	
	    dot_product(tmpRot,v_min_c,ret_dot);
	    vector_add(ret_dot, a, vector);
	}
	xy.x = vector[0];
	xy.y = vector[1];
	return xy;
    }
}
FORWARD(e_healpix_forward); /* ellipsoidal */
    //int r1[][2] = R1;
    double bet = auth_lat(lp.phi, P->e, 0);
    lp.phi = bet;
    P->a = P->ra;
    return healpix_sphere(lp,P);
}
FORWARD(s_healpix_forward); /* spheroid */
    return healpix_sphere(lp, P);
}
INVERSE(e_healpix_inverse); /* ellipsoidal */
    double bet, x, y;
    P->a = P->ra;

    // Scale down to radius 1 sphere before checking x,y
    x = scale_number(xy.x,P->a,1);
    y = scale_number(xy.y,P->a,1);
    // check if the point is in the image
    if(in_image(x,y,0,0,0) == 0){
	lp.lam = HUGE_VAL;
	lp.phi = HUGE_VAL;
	pj_ctx_set_errno( P->ctx, -15);
	return lp;
    }

    lp = healpix_sphere_inv(xy, P);

    lp.phi = auth_lat(lp.phi,P->e,1);
    
    return (lp);
}
INVERSE(s_healpix_inverse); /* spheroid */
    double x = xy.x;
    double y = xy.y;
    // Scale down to radius 1 sphere before checking x,y
    x = scale_number(x,P->a,1);
    y = scale_number(y,P->a,1);
    // check if the point is in the image
    if(in_image(x,y,0,0,0) == 0){
	lp.lam = HUGE_VAL;
	lp.phi = HUGE_VAL;
	pj_ctx_set_errno( P->ctx, -15);
	return lp;
    }
    return healpix_sphere_inv(xy, P);
}
FORWARD(e_rhealpix_forward); /* ellipsoidal */
    double bet = auth_lat(lp.phi,P->e,0);
    lp.phi = bet;
    xy = healpix_sphere(lp,P);
    return combine_caps(xy.x, xy.y, P->a, P->npole, P->spole, 0);
}
FORWARD(s_rhealpix_forward); /* spheroid */
    // Compute forward function.
    xy = healpix_sphere(lp,P);
    return combine_caps(xy.x, xy.y, P->a, P->npole, P->spole, 0);
}
INVERSE(e_rhealpix_inverse); /* ellipsoidal */
    double x = scale_number(xy.x,P->a,1);
    double y = scale_number(xy.y,P->a,1);
    // check for out of bounds coordinates
    if(in_image(x,y,1,P->npole,P->spole) == 0){
	lp.lam = HUGE_VAL;
	lp.phi = HUGE_VAL;
	pj_ctx_set_errno( P->ctx, -15);
	return lp;
    }

    xy = combine_caps(xy.x,xy.y,P->a,P->npole,P->spole,1);
    lp = healpix_sphere_inv(xy, P);
    lp.phi = auth_lat(lp.phi,P->e,1);
    return lp;
}
INVERSE(s_rhealpix_inverse); /* spheroid */
    double x = scale_number(xy.x,P->a,1);
    double y = scale_number(xy.y,P->a,1);
    // check for out of bounds coordinates
    if(in_image(x,y,1,P->npole,P->spole) == 0){
	lp.lam = HUGE_VAL;
	lp.phi = HUGE_VAL;
	pj_ctx_set_errno( P->ctx, -15);
	return lp;
    }
    xy = combine_caps(xy.x,xy.y,P->a,P->npole,P->spole,1);
    return healpix_sphere_inv(xy, P);
}
FREEUP;
    if (P) {
	pj_dalloc(P);
    }
}
ENTRY0(healpix)
    if(P->es){
	P->inv = e_healpix_inverse; P->fwd = e_healpix_forward;
    }else{
	P->inv = s_healpix_inverse; P->fwd = s_healpix_forward;
    }
ENDENTRY(P)
ENTRY0(rhealpix)
    P->npole = pj_param(P->ctx, P->params,"inpole").i;
    P->spole = pj_param(P->ctx,P->params,"ispole").i;
    
    // check for valid npole and spole inputs
    if(P->npole < 0 || P->npole > 3){
	E_ERROR(-47);
    }
    if(P->spole < 0 || P->spole > 3){
	E_ERROR(-47);
    }

    if(P->es){
	P->inv = e_rhealpix_inverse; P->fwd = e_rhealpix_forward;
    }else{
	P->inv = s_rhealpix_inverse; P->fwd = s_rhealpix_forward;
    }
ENDENTRY(P)
