/*
 * This code was entirely written by Nathan Wagner
 * and is in the public domain.
 */

#include <errno.h>
#include <math.h>
#include <float.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PJ_LIB__
#include "proj_internal.h"
#include "proj_math.h"
#include "proj.h"
#include "projects.h"

#define DEG36 0.62831853071795864768
#define DEG72 1.25663706143591729537
#define DEG90 M_PI_2
#define DEG108 1.88495559215387594306
#define DEG120 2.09439510239319549229
#define DEG144 2.51327412287183459075
#define DEG180 M_PI

/* sqrt(5)/M_PI */
#define ISEA_SCALE 0.8301572857837594396028083

/* 26.565051177 degrees */
#define V_LAT 0.46364760899944494524

/* 52.62263186 */
#define E_RAD 0.91843818702186776133

/* 10.81231696 */
#define F_RAD 0.18871053072122403508

/* R tan(g) sin(60) */
#define TABLE_G 0.6615845383

/* H = 0.25 R tan g = */
#define TABLE_H 0.1909830056

/* in radians */
#define ISEA_STD_LAT 1.01722196792335072101
#define ISEA_STD_LON .19634954084936207740

struct hex {
        int iso;
        long x, y, z;
};

/* y *must* be positive down as the xy /iso conversion assumes this */
static void hex_xy(struct hex *h) {
    if (!h->iso) return;
    if (h->x >= 0) {
        h->y = -h->y - (h->x+1)/2;
    } else {
        /* need to round toward -inf, not toward zero, so x-1 */
        h->y = -h->y - h->x/2;
    }
    h->iso = 0;
}

static void hex_iso(struct hex *h) {
    if (h->iso) return;

    if (h->x >= 0) {
        h->y = (-h->y - (h->x+1)/2);
    } else {
        /* need to round toward -inf, not toward zero, so x-1 */
        h->y = (-h->y - (h->x)/2);
    }

    h->z = -h->x - h->y;
    h->iso = 1;
}

static void hexbin2(double width, double x, double y, long *i, long *j) {
    double z, rx, ry, rz;
    double abs_dx, abs_dy, abs_dz;
    long ix, iy, iz, s;
    struct hex h;

    x = x / cos(30 * M_PI / 180.0); /* rotated X coord */
    y = y - x / 2.0; /* adjustment for rotated X */

    /* adjust for actual hexwidth */
    x /= width;
    y /= width;

    z = -x - y;

    rx = floor(x + 0.5);
    ix = lround(rx);
    ry = floor(y + 0.5);
    iy = lround(ry);
    rz = floor(z + 0.5);
    iz = lround(rz);

    s = ix + iy + iz;

    if (s) {
        abs_dx = fabs(rx - x);
        abs_dy = fabs(ry - y);
        abs_dz = fabs(rz - z);

        if (abs_dx >= abs_dy && abs_dx >= abs_dz) {
            ix -= s;
        } else if (abs_dy >= abs_dx && abs_dy >= abs_dz) {
            iy -= s;
        } else {
            iz -= s;
        }
    }
    h.x = ix;
    h.y = iy;
    h.z = iz;
    h.iso = 1;

    hex_xy(&h);
    *i = h.x;
    *j = h.y;
}

enum isea_poly { ISEA_NONE, ISEA_ICOSAHEDRON = 20 };
enum isea_topology { ISEA_HEXAGON=6, ISEA_TRIANGLE=3, ISEA_DIAMOND=4 };
enum isea_address_form { ISEA_GEO, ISEA_Q2DI, ISEA_SEQNUM, ISEA_INTERLEAVE,
    ISEA_PLANE, ISEA_Q2DD, ISEA_PROJTRI, ISEA_VERTEX2DD, ISEA_HEX
};

struct isea_dgg {
    int polyhedron; /* ignored, icosahedron */
    double  o_lat, o_lon, o_az; /* orientation, radians */
    int pole; /* true if standard snyder */
    int topology; /* ignored, hexagon */
    int aperture; /* valid values depend on partitioning method */
    int resolution;
    double  radius; /* radius of the earth in meters, ignored 1.0 */
    int output; /* an isea_address_form */
    int triangle; /* triangle of last transformed point */
    int quad; /* quad of last transformed point */
    unsigned long serial;
};

struct isea_pt {
    double x, y;
};

struct isea_geo {
    double lon, lat;
};

/* ENDINC */

enum snyder_polyhedron {
    SNYDER_POLY_HEXAGON, SNYDER_POLY_PENTAGON,
    SNYDER_POLY_TETRAHEDRON, SNYDER_POLY_CUBE,
    SNYDER_POLY_OCTAHEDRON, SNYDER_POLY_DODECAHEDRON,
    SNYDER_POLY_ICOSAHEDRON
};

struct snyder_constants {
    double          g, G, theta;
    /* cppcheck-suppress unusedStructMember */
    double ea_w, ea_a, ea_b, g_w, g_a, g_b;
};

/* TODO put these in radians to avoid a later conversion */
static const struct snyder_constants constants[] = {
    {23.80018260, 62.15458023, 60.0, 3.75, 1.033, 0.968, 5.09, 1.195, 1.0},
    {20.07675127, 55.69063953, 54.0, 2.65, 1.030, 0.983, 3.59, 1.141, 1.027},
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
    {37.37736814, 36.0, 30.0, 17.27, 1.163, 0.860, 13.14, 1.584, 1.0},
};

static struct isea_geo vertex[] = {
    {0.0, DEG90},
    {DEG180, V_LAT},
    {-DEG108, V_LAT},
    {-DEG36, V_LAT},
    {DEG36, V_LAT},
    {DEG108, V_LAT},
    {-DEG144, -V_LAT},
    {-DEG72, -V_LAT},
    {0.0, -V_LAT},
    {DEG72, -V_LAT},
    {DEG144, -V_LAT},
    {0.0, -DEG90}
};

/* TODO make an isea_pt array of the vertices as well */

static int tri_v1[] = {0, 0, 0, 0, 0, 0, 6, 7, 8, 9, 10, 2, 3, 4, 5, 1, 11, 11, 11, 11, 11};

/* triangle Centers */
static const struct isea_geo icostriangles[] = {
    {0.0, 0.0},
    {-DEG144, E_RAD},
    {-DEG72, E_RAD},
    {0.0, E_RAD},
    {DEG72, E_RAD},
    {DEG144, E_RAD},
    {-DEG144, F_RAD},
    {-DEG72, F_RAD},
    {0.0, F_RAD},
    {DEG72, F_RAD},
    {DEG144, F_RAD},
    {-DEG108, -F_RAD},
    {-DEG36, -F_RAD},
    {DEG36, -F_RAD},
    {DEG108, -F_RAD},
    {DEG180, -F_RAD},
    {-DEG108, -E_RAD},
    {-DEG36, -E_RAD},
    {DEG36, -E_RAD},
    {DEG108, -E_RAD},
    {DEG180, -E_RAD},
};

static double az_adjustment(int triangle)
{
    double          adj;

    struct isea_geo v;
    struct isea_geo c;

    v = vertex[tri_v1[triangle]];
    c = icostriangles[triangle];

    /* TODO looks like the adjustment is always either 0 or 180 */
    /* at least if you pick your vertex carefully */
    adj = atan2(cos(v.lat) * sin(v.lon - c.lon),
            cos(c.lat) * sin(v.lat)
            - sin(c.lat) * cos(v.lat) * cos(v.lon - c.lon));
    return adj;
}

static struct isea_pt isea_triangle_xy(int triangle)
{
    struct isea_pt  c;
    const double Rprime = 0.91038328153090290025;

    triangle = (triangle - 1) % 20;

    c.x = TABLE_G * ((triangle % 5) - 2) * 2.0;
    if (triangle > 9) {
        c.x += TABLE_G;
    }
    switch (triangle / 5) {
    case 0:
        c.y = 5.0 * TABLE_H;
        break;
    case 1:
        c.y = TABLE_H;
        break;
    case 2:
        c.y = -TABLE_H;
        break;
    case 3:
        c.y = -5.0 * TABLE_H;
        break;
    default:
        /* should be impossible */
        exit(EXIT_FAILURE);
    };
    c.x *= Rprime;
    c.y *= Rprime;

    return c;
}

/* snyder eq 14 */
static double sph_azimuth(double f_lon, double f_lat,
                          double t_lon, double t_lat)
{
    double          az;

    az = atan2(cos(t_lat) * sin(t_lon - f_lon),
           cos(f_lat) * sin(t_lat)
           - sin(f_lat) * cos(t_lat) * cos(t_lon - f_lon)
        );
    return az;
}

#ifdef _MSC_VER
#pragma warning( push )
/* disable unreachable code warning for return 0 */
#pragma warning( disable : 4702 )
#endif

/* coord needs to be in radians */
static int isea_snyder_forward(struct isea_geo * ll, struct isea_pt * out)
{
    int             i;

    /*
     * spherical distance from center of polygon face to any of its
     * vertexes on the globe
     */
    double          g;

    /*
     * spherical angle between radius vector to center and adjacent edge
     * of spherical polygon on the globe
     */
    double          G;

    /*
     * plane angle between radius vector to center and adjacent edge of
     * plane polygon
     */
    double          theta;

    /* additional variables from snyder */
    double          q, Rprime, H, Ag, Azprime, Az, dprime, f, rho,
                    x, y;

    /* variables used to store intermediate results */
    double          cot_theta, tan_g, az_offset;

    /* how many multiples of 60 degrees we adjust the azimuth */
    int             Az_adjust_multiples;

    struct snyder_constants c;

    /*
     * TODO by locality of reference, start by trying the same triangle
     * as last time
     */

    /* TODO put these constants in as radians to begin with */
    c = constants[SNYDER_POLY_ICOSAHEDRON];
    theta = PJ_TORAD(c.theta);
    g = PJ_TORAD(c.g);
    G = PJ_TORAD(c.G);

    for (i = 1; i <= 20; i++) {
        double          z;
        struct isea_geo center;

        center = icostriangles[i];

        /* step 1 */
        z = acos(sin(center.lat) * sin(ll->lat)
             + cos(center.lat) * cos(ll->lat) * cos(ll->lon - center.lon));
        /* not on this triangle */
        if (z > g + 0.000005) { /* TODO DBL_EPSILON */
            continue;
        }

        Az = sph_azimuth(center.lon, center.lat, ll->lon, ll->lat);

        /* step 2 */

        /* This calculates "some" vertex coordinate */
        az_offset = az_adjustment(i);

        Az -= az_offset;

        /* TODO I don't know why we do this.  It's not in snyder */
        /* maybe because we should have picked a better vertex */
        if (Az < 0.0) {
            Az += 2.0 * M_PI;
        }
        /*
         * adjust Az for the point to fall within the range of 0 to
         * 2(90 - theta) or 60 degrees for the hexagon, by
         * and therefore 120 degrees for the triangle
         * of the icosahedron
         * subtracting or adding multiples of 60 degrees to Az and
         * recording the amount of adjustment
         */

        Az_adjust_multiples = 0;
        while (Az < 0.0) {
            Az += DEG120;
            Az_adjust_multiples--;
        }
        while (Az > DEG120 + DBL_EPSILON) {
            Az -= DEG120;
            Az_adjust_multiples++;
        }

        /* step 3 */
        cot_theta = 1.0 / tan(theta);
        tan_g = tan(g); /* TODO this is a constant */

        /* Calculate q from eq 9. */
        /* TODO cot_theta is cot(30) */
        q = atan2(tan_g, cos(Az) + sin(Az) * cot_theta);

        /* not in this triangle */
        if (z > q + 0.000005) {
            continue;
        }
        /* step 4 */

        /* Apply equations 5-8 and 10-12 in order */

        /* eq 5 */
        /* Rprime = 0.9449322893 * R; */
        /* R' in the paper is for the truncated */
        Rprime = 0.91038328153090290025;

        /* eq 6 */
        H = acos(sin(Az) * sin(G) * cos(g) - cos(Az) * cos(G));

        /* eq 7 */
        /* Ag = (Az + G + H - DEG180) * M_PI * R * R / DEG180; */
        Ag = Az + G + H - DEG180;

        /* eq 8 */
        Azprime = atan2(2.0 * Ag, Rprime * Rprime * tan_g * tan_g - 2.0 * Ag * cot_theta);

        /* eq 10 */
        /* cot(theta) = 1.73205080756887729355 */
        dprime = Rprime * tan_g / (cos(Azprime) + sin(Azprime) * cot_theta);

        /* eq 11 */
        f = dprime / (2.0 * Rprime * sin(q / 2.0));

        /* eq 12 */
        rho = 2.0 * Rprime * f * sin(z / 2.0);

        /*
         * add back the same 60 degree multiple adjustment from step
         * 2 to Azprime
         */

        Azprime += DEG120 * Az_adjust_multiples;

        /* calculate rectangular coordinates */

        x = rho * sin(Azprime);
        y = rho * cos(Azprime);

        /*
         * TODO
         * translate coordinates to the origin for the particular
         * hexagon on the flattened polyhedral map plot
         */

        out->x = x;
        out->y = y;

        return i;
    }

    /*
     * should be impossible, this implies that the coordinate is not on
     * any triangle
     */

    fprintf(stderr, "impossible transform: %f %f is not on any triangle\n",
            PJ_TODEG(ll->lon), PJ_TODEG(ll->lat));

    exit(EXIT_FAILURE);

    /* not reached */
    return 0;       /* suppresses a warning */
}

#ifdef _MSC_VER
#pragma warning( pop )
#endif

/*
 * return the new coordinates of any point in original coordinate system.
 * Define a point (newNPold) in original coordinate system as the North Pole in
 * new coordinate system, and the great circle connect the original and new
 * North Pole as the lon0 longitude in new coordinate system, given any point
 * in original coordinate system, this function return the new coordinates.
 */

/* formula from Snyder, Map Projections: A working manual, p31 */
/*
 * old north pole at np in new coordinates
 * could be simplified a bit with fewer intermediates
 *
 * TODO take a result pointer
 */
static struct isea_geo snyder_ctran(struct isea_geo * np, struct isea_geo * pt)
{
    struct isea_geo npt;
    double          alpha, phi, lambda, lambda0, beta, lambdap, phip;
    double          sin_phip;
    double          lp_b;   /* lambda prime minus beta */
    double          cos_p, sin_a;

    phi = pt->lat;
    lambda = pt->lon;
    alpha = np->lat;
    beta = np->lon;
    lambda0 = beta;

    cos_p = cos(phi);
    sin_a = sin(alpha);

    /* mpawm 5-7 */
    sin_phip = sin_a * sin(phi) - cos(alpha) * cos_p * cos(lambda - lambda0);

    /* mpawm 5-8b */

    /* use the two argument form so we end up in the right quadrant */
    lp_b = atan2(cos_p * sin(lambda - lambda0),
       (sin_a * cos_p * cos(lambda - lambda0) + cos(alpha) * sin(phi)));

    lambdap = lp_b + beta;

    /* normalize longitude */
    /* TODO can we just do a modulus ? */
    lambdap = fmod(lambdap, 2 * M_PI);
    while (lambdap > M_PI)
        lambdap -= 2 * M_PI;
    while (lambdap < -M_PI)
        lambdap += 2 * M_PI;

    phip = asin(sin_phip);

    npt.lat = phip;
    npt.lon = lambdap;

    return npt;
}

static struct isea_geo isea_ctran(struct isea_geo * np, struct isea_geo * pt,
                                  double lon0)
{
    struct isea_geo npt;

    np->lon += M_PI;
    npt = snyder_ctran(np, pt);
    np->lon -= M_PI;

    npt.lon -= (M_PI - lon0 + np->lon);

    /*
     * snyder is down tri 3, isea is along side of tri1 from vertex 0 to
     * vertex 1 these are 180 degrees apart
     */
    npt.lon += M_PI;
    /* normalize longitude */
    npt.lon = fmod(npt.lon, 2 * M_PI);
    while (npt.lon > M_PI)
        npt.lon -= 2 * M_PI;
    while (npt.lon < -M_PI)
        npt.lon += 2 * M_PI;

    return npt;
}

/* fuller's at 5.2454 west, 2.3009 N, adjacent at 7.46658 deg */

static int isea_grid_init(struct isea_dgg * g)
{
    if (!g)
        return 0;

    g->polyhedron = 20;
    g->o_lat = ISEA_STD_LAT;
    g->o_lon = ISEA_STD_LON;
    g->o_az = 0.0;
    g->aperture = 4;
    g->resolution = 6;
    g->radius = 1.0;
    g->topology = 6;

    return 1;
}

static void isea_orient_isea(struct isea_dgg * g)
{
    if (!g)
        return;
    g->o_lat = ISEA_STD_LAT;
    g->o_lon = ISEA_STD_LON;
    g->o_az = 0.0;
}

static void isea_orient_pole(struct isea_dgg * g)
{
    if (!g)
        return;
    g->o_lat = M_PI / 2.0;
    g->o_lon = 0.0;
    g->o_az = 0;
}

static int isea_transform(struct isea_dgg * g, struct isea_geo * in,
                          struct isea_pt * out)
{
    struct isea_geo i, pole;
    int             tri;

    pole.lat = g->o_lat;
    pole.lon = g->o_lon;

    i = isea_ctran(&pole, in, g->o_az);

    tri = isea_snyder_forward(&i, out);
    out->x *= g->radius;
    out->y *= g->radius;
    g->triangle = tri;

    return tri;
}

#define DOWNTRI(tri) (((tri - 1) / 5) % 2 == 1)

static void isea_rotate(struct isea_pt * pt, double degrees)
{
    double          rad;

    double          x, y;

    rad = -degrees * M_PI / 180.0;
    while (rad >= 2.0 * M_PI) rad -= 2.0 * M_PI;
    while (rad <= -2.0 * M_PI) rad += 2.0 * M_PI;

    x = pt->x * cos(rad) + pt->y * sin(rad);
    y = -pt->x * sin(rad) + pt->y * cos(rad);

    pt->x = x;
    pt->y = y;
}

static int isea_tri_plane(int tri, struct isea_pt *pt, double radius) {
    struct isea_pt tc; /* center of triangle */

    if (DOWNTRI(tri)) {
        isea_rotate(pt, 180.0);
    }
    tc = isea_triangle_xy(tri);
    tc.x *= radius;
    tc.y *= radius;
    pt->x += tc.x;
    pt->y += tc.y;

    return tri;
}

/* convert projected triangle coords to quad xy coords, return quad number */
static int isea_ptdd(int tri, struct isea_pt *pt) {
    int             downtri, quad;

    downtri = (((tri - 1) / 5) % 2 == 1);
    quad = ((tri - 1) % 5) + ((tri - 1) / 10) * 5 + 1;

    isea_rotate(pt, downtri ? 240.0 : 60.0);
    if (downtri) {
        pt->x += 0.5;
        /* pt->y += cos(30.0 * M_PI / 180.0); */
        pt->y += .86602540378443864672;
    }
    return quad;
}

static int isea_dddi_ap3odd(struct isea_dgg *g, int quad, struct isea_pt *pt,
                            struct isea_pt *di)
{
    struct isea_pt  v;
    double          hexwidth;
    double          sidelength; /* in hexes */
    long             d, i;
    long            maxcoord;
    struct hex      h;

    /* This is the number of hexes from apex to base of a triangle */
    sidelength = (pow(2.0, g->resolution) + 1.0) / 2.0;

    /* apex to base is cos(30deg) */
    hexwidth = cos(M_PI / 6.0) / sidelength;

    /* TODO I think sidelength is always x.5, so
     * (int)sidelength * 2 + 1 might be just as good
     */
    maxcoord = lround((sidelength * 2.0));

    v = *pt;
    hexbin2(hexwidth, v.x, v.y, &h.x, &h.y);
    h.iso = 0;
    hex_iso(&h);

    d = h.x - h.z;
    i = h.x + h.y + h.y;

    /*
     * you want to test for max coords for the next quad in the same
     * "row" first to get the case where both are max
     */
    if (quad <= 5) {
        if (d == 0 && i == maxcoord) {
            /* north pole */
            quad = 0;
            d = 0;
            i = 0;
        } else if (i == maxcoord) {
            /* upper right in next quad */
            quad += 1;
            if (quad == 6)
                quad = 1;
            i = maxcoord - d;
            d = 0;
        } else if (d == maxcoord) {
            /* lower right in quad to lower right */
            quad += 5;
            d = 0;
        }
    } else if (quad >= 6) {
        if (i == 0 && d == maxcoord) {
            /* south pole */
            quad = 11;
            d = 0;
            i = 0;
        } else if (d == maxcoord) {
            /* lower right in next quad */
            quad += 1;
            if (quad == 11)
                quad = 6;
            d = maxcoord - i;
            i = 0;
        } else if (i == maxcoord) {
            /* upper right in quad to upper right */
            quad = (quad - 4) % 5;
            i = 0;
        }
    }

    di->x = d;
    di->y = i;

    g->quad = quad;
    return quad;
}

static int isea_dddi(struct isea_dgg *g, int quad, struct isea_pt *pt,
                     struct isea_pt *di) {
    struct isea_pt  v;
    double          hexwidth;
    long            sidelength; /* in hexes */
    struct hex      h;

    if (g->aperture == 3 && g->resolution % 2 != 0) {
        return isea_dddi_ap3odd(g, quad, pt, di);
    }
    /* todo might want to do this as an iterated loop */
    if (g->aperture >0) {
        sidelength = lround(pow(g->aperture, g->resolution / 2.0));
    } else {
        sidelength = g->resolution;
    }

    hexwidth = 1.0 / sidelength;

    v = *pt;
    isea_rotate(&v, -30.0);
    hexbin2(hexwidth, v.x, v.y, &h.x, &h.y);
    h.iso = 0;
    hex_iso(&h);

    /* we may actually be on another quad */
    if (quad <= 5) {
        if (h.x == 0 && h.z == -sidelength) {
            /* north pole */
            quad = 0;
            h.z = 0;
            h.y = 0;
            h.x = 0;
        } else if (h.z == -sidelength) {
            quad = quad + 1;
            if (quad == 6)
                quad = 1;
            h.y = sidelength - h.x;
            h.z = h.x - sidelength;
            h.x = 0;
        } else if (h.x == sidelength) {
            quad += 5;
            h.y = -h.z;
            h.x = 0;
        }
    } else if (quad >= 6) {
        if (h.z == 0 && h.x == sidelength) {
            /* south pole */
            quad = 11;
            h.x = 0;
            h.y = 0;
            h.z = 0;
        } else if (h.x == sidelength) {
            quad = quad + 1;
            if (quad == 11)
                quad = 6;
            h.x = h.y + sidelength;
            h.y = 0;
            h.z = -h.x;
        } else if (h.y == -sidelength) {
            quad -= 4;
            h.y = 0;
            h.z = -h.x;
        }
    }
    di->x = h.x;
    di->y = -h.z;

    g->quad = quad;
    return quad;
}

static int isea_ptdi(struct isea_dgg *g, int tri, struct isea_pt *pt,
                     struct isea_pt *di) {
    struct isea_pt  v;
    int             quad;

    v = *pt;
    quad = isea_ptdd(tri, &v);
    quad = isea_dddi(g, quad, &v, di);
    return quad;
}

/* q2di to seqnum */

static long isea_disn(struct isea_dgg *g, int quad, struct isea_pt *di) {
    long             sidelength;
    long             sn, height;
    long             hexes;

    if (quad == 0) {
        g->serial = 1;
        return g->serial;
    }
    /* hexes in a quad */
    hexes = lround(pow(g->aperture, g->resolution));
    if (quad == 11) {
        g->serial = 1 + 10 * hexes + 1;
        return g->serial;
    }
    if (g->aperture == 3 && g->resolution % 2 == 1) {
        height = lround(floor((pow(g->aperture, (g->resolution - 1) / 2.0))));
        sn = ((long)di->x) * height;
        sn += ((long)di->y) / height;
        sn += (quad - 1) * hexes;
        sn += 2;
    } else {
        sidelength = lround((pow(g->aperture, g->resolution / 2.0)));
        sn = lround(floor(((quad - 1) * hexes + sidelength * di->x + di->y + 2)));
    }

    g->serial = sn;
    return sn;
}

/* TODO just encode the quad in the d or i coordinate
 * quad is 0-11, which can be four bits.
 * d' = d << 4 + q, d = d' >> 4, q = d' & 0xf
 */
/* convert a q2di to global hex coord */
static int isea_hex(struct isea_dgg *g, int tri,
                    struct isea_pt *pt, struct isea_pt *hex) {
    struct isea_pt v;
#ifdef FIXME
    long sidelength;
    long d, i, x, y;
#endif
    int quad;

    quad = isea_ptdi(g, tri, pt, &v);

    hex->x = ((int)v.x << 4) + quad;
    hex->y = v.y;

    return 1;
#ifdef FIXME
    d = lround(floor(v.x));
    i = lround(floor(v.y));

    /* Aperture 3 odd resolutions */
    if (g->aperture == 3 && g->resolution % 2 != 0) {
        long offset = lround((pow(3.0, g->resolution - 1) + 0.5));

        d += offset * ((g->quad-1) % 5);
        i += offset * ((g->quad-1) % 5);

        if (quad == 0) {
            d = 0;
            i = offset;
        } else if (quad == 11) {
            d = 2 * offset;
            i = 0;
        } else if (quad > 5) {
            d += offset;
        }

        x = (2*d - i) /3;
        y = (2*i - d) /3;

        hex->x = x + offset / 3;
        hex->y = y + 2 * offset / 3;
        return 1;
    }

    /* aperture 3 even resolutions and aperture 4 */
    sidelength = lround((pow(g->aperture, g->resolution / 2.0)));
    if (g->quad == 0) {
        hex->x = 0;
        hex->y = sidelength;
    } else if (g->quad == 11) {
        hex->x = sidelength * 2;
        hex->y = 0;
    } else {
        hex->x = d + sidelength * ((g->quad-1) % 5);
        if (g->quad > 5) hex->x += sidelength;
        hex->y = i + sidelength * ((g->quad-1) % 5);
    }

    return 1;
#endif
}

static struct isea_pt isea_forward(struct isea_dgg *g, struct isea_geo *in)
{
    int             tri;
    struct isea_pt  out, coord;

    tri = isea_transform(g, in, &out);

    if (g->output == ISEA_PLANE) {
        isea_tri_plane(tri, &out, g->radius);
        return out;
    }

    /* convert to isea standard triangle size */
    out.x = out.x / g->radius * ISEA_SCALE;
    out.y = out.y / g->radius * ISEA_SCALE;
    out.x += 0.5;
    out.y += 2.0 * .14433756729740644112;

    switch (g->output) {
    case ISEA_PROJTRI:
        /* nothing to do, already in projected triangle */
        break;
    case ISEA_VERTEX2DD:
        g->quad = isea_ptdd(tri, &out);
        break;
    case ISEA_Q2DD:
        /* Same as above, we just don't print as much */
        g->quad = isea_ptdd(tri, &out);
        break;
    case ISEA_Q2DI:
        g->quad = isea_ptdi(g, tri, &out, &coord);
        return coord;
        break;
    case ISEA_SEQNUM:
        isea_ptdi(g, tri, &out, &coord);
        /* disn will set g->serial */
        isea_disn(g, g->quad, &coord);
        return coord;
        break;
    case ISEA_HEX:
        isea_hex(g, tri, &out, &coord);
        return coord;
        break;
    }

    return out;
}

/*
 * Proj 4 integration code follows
 */

PROJ_HEAD(isea, "Icosahedral Snyder Equal Area") "\n\tSph";

struct pj_opaque {
    struct isea_dgg dgg;
};


static XY s_forward (LP lp, PJ *P) {           /* Spheroidal, forward */
    XY xy = {0.0,0.0};
    struct pj_opaque *Q = P->opaque;
    struct isea_pt out;
    struct isea_geo in;

    in.lon = lp.lam;
    in.lat = lp.phi;

    out = isea_forward(&Q->dgg, &in);

    xy.x = out.x;
    xy.y = out.y;

    return xy;
}


PJ *PROJECTION(isea) {
    char *opt;
    struct pj_opaque *Q = pj_calloc (1, sizeof (struct pj_opaque));
    if (0==Q)
        return pj_default_destructor (P, ENOMEM);
    P->opaque = Q;


    P->fwd = s_forward;
    isea_grid_init(&Q->dgg);

    Q->dgg.output = ISEA_PLANE;
/*      P->dgg.radius = P->a; / * otherwise defaults to 1 */
    /* calling library will scale, I think */

    opt = pj_param(P->ctx,P->params, "sorient").s;
    if (opt) {
        if (!strcmp(opt, "isea")) {
            isea_orient_isea(&Q->dgg);
        } else if (!strcmp(opt, "pole")) {
            isea_orient_pole(&Q->dgg);
        } else {
            return pj_default_destructor(P, PJD_ERR_ELLIPSOID_USE_REQUIRED);
        }
    }

    if (pj_param(P->ctx,P->params, "tazi").i) {
        Q->dgg.o_az = pj_param(P->ctx,P->params, "razi").f;
    }

    if (pj_param(P->ctx,P->params, "tlon_0").i) {
        Q->dgg.o_lon = pj_param(P->ctx,P->params, "rlon_0").f;
    }

    if (pj_param(P->ctx,P->params, "tlat_0").i) {
        Q->dgg.o_lat = pj_param(P->ctx,P->params, "rlat_0").f;
    }

    if (pj_param(P->ctx,P->params, "taperture").i) {
        Q->dgg.aperture = pj_param(P->ctx,P->params, "iaperture").i;
    }

    if (pj_param(P->ctx,P->params, "tresolution").i) {
        Q->dgg.resolution = pj_param(P->ctx,P->params, "iresolution").i;
    }

    opt = pj_param(P->ctx,P->params, "smode").s;
    if (opt) {
        if (!strcmp(opt, "plane")) {
            Q->dgg.output = ISEA_PLANE;
        } else if (!strcmp(opt, "di")) {
            Q->dgg.output = ISEA_Q2DI;
        }
        else if (!strcmp(opt, "dd")) {
            Q->dgg.output = ISEA_Q2DD;
        }
        else if (!strcmp(opt, "hex")) {
            Q->dgg.output = ISEA_HEX;
        }
        else {
            /* TODO verify error code.  Possibly eliminate magic */
            return pj_default_destructor(P, PJD_ERR_ELLIPSOID_USE_REQUIRED);
        }
    }

    if (pj_param(P->ctx,P->params, "trescale").i) {
        Q->dgg.radius = ISEA_SCALE;
    }

    if (pj_param(P->ctx,P->params, "tresolution").i) {
        Q->dgg.resolution = pj_param(P->ctx,P->params, "iresolution").i;
    } else {
        Q->dgg.resolution = 4;
    }

    if (pj_param(P->ctx,P->params, "taperture").i) {
        Q->dgg.aperture = pj_param(P->ctx,P->params, "iaperture").i;
    } else {
        Q->dgg.aperture = 3;
    }

    return P;
}
