#include <stdio.h>
/* #include <proj.h>*/

int OK = 0, KO = 0;

static void test_update (double distance, double tolerance, char *definition, PJ_COORD ci, PJ_COORD ce, PJ_COORD co) {
    (void) ci;
    (void) ce;
    (void) co;
    (void) definition;

    if (distance < 1000*tolerance)
        OK++;
    else {
        KO++;
        printf ("%80.80s: %.3f\n", definition, distance);
    }
}

int test_main (void);
int test_main (void) {
    double d, tolerance;
    enum PJ_DIRECTION direction = 1;
    PJ_COORD ci, ce, co;
    PJ *P;


    P = proj_create (0, "proj=axisswap order=1,2,3,4");
    tolerance = 0.000001000000;
    ci.xyzt.x = 1.000000000000000;
    ci.xyzt.y = 2.000000000000000;
    ci.xyzt.z = 3.000000000000000;
    ci.xyzt.t = 4.000000000000000;
    ce.xyzt.x = 1.000000000000000;
    ce.xyzt.y = 2.000000000000000;
    ce.xyzt.z = 3.000000000000000;
    ce.xyzt.t = 4.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=axisswap order=1,2,3,4", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=axisswap order=4,3,2,1");
    tolerance = 0.000001000000;
    ci.xyzt.x = 1.000000000000000;
    ci.xyzt.y = 2.000000000000000;
    ci.xyzt.z = 3.000000000000000;
    ci.xyzt.t = 4.000000000000000;
    ce.xyzt.x = 4.000000000000000;
    ce.xyzt.y = 3.000000000000000;
    ce.xyzt.z = 2.000000000000000;
    ce.xyzt.t = 1.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=axisswap order=4,3,2,1", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=axisswap order=-1,-2,-3,-4");
    tolerance = 0.000001000000;
    ci.xyzt.x = 1.000000000000000;
    ci.xyzt.y = 2.000000000000000;
    ci.xyzt.z = 3.000000000000000;
    ci.xyzt.t = 4.000000000000000;
    ce.xyzt.x = -1.000000000000000;
    ce.xyzt.y = -2.000000000000000;
    ce.xyzt.z = -3.000000000000000;
    ce.xyzt.t = -4.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=axisswap order=-1,-2,-3,-4", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=axisswap order=1,2,-3,4");
    tolerance = 0.000001000000;
    ci.xyzt.x = 1.000000000000000;
    ci.xyzt.y = 2.000000000000000;
    ci.xyzt.z = 3.000000000000000;
    ci.xyzt.t = 4.000000000000000;
    ce.xyzt.x = 1.000000000000000;
    ce.xyzt.y = 2.000000000000000;
    ce.xyzt.z = -3.000000000000000;
    ce.xyzt.t = 4.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=axisswap order=1,2,-3,4", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=axisswap order=-1,2,3,4");
    tolerance = 0.000001000000;
    ci.xyzt.x = 1.000000000000000;
    ci.xyzt.y = 2.000000000000000;
    ci.xyzt.z = 3.000000000000000;
    ci.xyzt.t = 4.000000000000000;
    ce.xyzt.x = -1.000000000000000;
    ce.xyzt.y = 2.000000000000000;
    ce.xyzt.z = 3.000000000000000;
    ce.xyzt.t = 4.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=axisswap order=-1,2,3,4", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=axisswap order=1,2,3,-4");
    tolerance = 0.000001000000;
    ci.xyzt.x = 1.000000000000000;
    ci.xyzt.y = 2.000000000000000;
    ci.xyzt.z = 3.000000000000000;
    ci.xyzt.t = 4.000000000000000;
    ce.xyzt.x = 1.000000000000000;
    ce.xyzt.y = 2.000000000000000;
    ce.xyzt.z = 3.000000000000000;
    ce.xyzt.t = -4.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=axisswap order=1,2,3,-4", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=axisswap order=-2,1");
    tolerance = 0.000001000000;
    ci.xyzt.x = 1.000000000000000;
    ci.xyzt.y = 2.000000000000000;
    ci.xyzt.z = 3.000000000000000;
    ci.xyzt.t = 4.000000000000000;
    ce.xyzt.x = -2.000000000000000;
    ce.xyzt.y = 1.000000000000000;
    ce.xyzt.z = 3.000000000000000;
    ce.xyzt.t = 4.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=axisswap order=-2,1", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=axisswap order=3,-2,1");
    tolerance = 0.000001000000;
    ci.xyzt.x = 1.000000000000000;
    ci.xyzt.y = 2.000000000000000;
    ci.xyzt.z = 3.000000000000000;
    ci.xyzt.t = 4.000000000000000;
    ce.xyzt.x = 3.000000000000000;
    ce.xyzt.y = -2.000000000000000;
    ce.xyzt.z = 1.000000000000000;
    ce.xyzt.t = 4.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=axisswap order=3,-2,1", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=pipeline step proj=latlong step proj=axisswap order=1,2,3,4 angularunits");
    tolerance = 0.000010000000;
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.209439510239320;
    ce.xyzt.y = 0.959931088596881;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "proj=pipeline step proj=latlong step proj=axisswap order=1,2,3,4 angularunits", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=pipeline step proj=latlong step proj=axisswap order=-2,-1,3,4 angularunits");
    tolerance = 0.000010000000;
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.959931088596881;
    ce.xyzt.y = -0.209439510239320;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "proj=pipeline step proj=latlong step proj=axisswap order=-2,-1,3,4 angularunits", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=aea ellps=GRS80 +lat_1=0 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222571.608757106010000;
    ce.xyzt.y = 110653.326743030000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aea ellps=GRS80 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222706.306508390990000;
    ce.xyzt.y = -110484.267144400000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aea ellps=GRS80 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222571.608757106010000;
    ce.xyzt.y = 110653.326743030000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aea ellps=GRS80 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222706.306508390990000;
    ce.xyzt.y = -110484.267144400000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aea ellps=GRS80 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aea ellps=GRS80 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357108940;
    ce.xyzt.y = -0.000015784234156;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aea ellps=GRS80 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aea ellps=GRS80 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357108940;
    ce.xyzt.y = -0.000015784234156;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aea ellps=GRS80 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=aea R=6400000 +lat_1=0 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223334.085170885000000;
    ce.xyzt.y = 111780.431884472000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aea R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223470.154991687010000;
    ce.xyzt.y = -111610.339430990000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aea R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223334.085170885000000;
    ce.xyzt.y = 111780.431884472000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aea R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223470.154991687010000;
    ce.xyzt.y = -111610.339430990000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aea R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031250015537;
    ce.xyzt.y = 0.000015624990315;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aea R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aea R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031250015537;
    ce.xyzt.y = 0.000015624990315;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aea R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aea R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=aeqd ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222616.522190052000000;
    ce.xyzt.y = 110596.996549549990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aeqd ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222616.522190052000000;
    ce.xyzt.y = -110596.996549549990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aeqd ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222616.522190052000000;
    ce.xyzt.y = 110596.996549549990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aeqd ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222616.522190052000000;
    ce.xyzt.y = -110596.996549549990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aeqd ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aeqd ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aeqd ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aeqd ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aeqd ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=aeqd R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223379.456047271000000;
    ce.xyzt.y = 111723.757570854000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aeqd R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223379.456047271000000;
    ce.xyzt.y = -111723.757570854000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aeqd R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223379.456047271000000;
    ce.xyzt.y = 111723.757570854000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aeqd R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223379.456047271000000;
    ce.xyzt.y = -111723.757570854000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aeqd R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aeqd R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aeqd R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aeqd R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aeqd R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=airy a=6400000 +lat_1=0 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 189109.886908621000000;
    ce.xyzt.y = 94583.752387504006000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=airy a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 189109.886908621000000;
    ce.xyzt.y = -94583.752387504006000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=airy a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -189109.886908621000000;
    ce.xyzt.y = 94583.752387504006000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=airy a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -189109.886908621000000;
    ce.xyzt.y = -94583.752387504006000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=airy a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=aitoff R=6400000 +lat_1=0 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223379.458811696010000;
    ce.xyzt.y = 111706.742883853000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aitoff R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223379.458811696010000;
    ce.xyzt.y = -111706.742883853000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aitoff R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223379.458811696010000;
    ce.xyzt.y = 111706.742883853000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aitoff R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223379.458811696010000;
    ce.xyzt.y = -111706.742883853000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=aitoff R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aitoff R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aitoff R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aitoff R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=aitoff R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=alsk +ellps=clrk66");
    tolerance = 0.000000010000;
    ci.xyzt.x = -2.792526803190927;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -513253.146950842000000;
    ce.xyzt.y = -968928.031867943010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=alsk +ellps=clrk66", ci, ce, co);

    ci.xyzt.x = -2.792526803190927;
    ci.xyzt.y = 1.221730476396031;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -305001.133897637020000;
    ce.xyzt.y = 687494.464958650990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=alsk +ellps=clrk66", ci, ce, co);

    ci.xyzt.x = -2.530727415391778;
    ci.xyzt.y = 1.221730476396031;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 266454.305088600030000;
    ce.xyzt.y = 683423.477493030950000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=alsk +ellps=clrk66", ci, ce, co);

    ci.xyzt.x = -2.530727415391778;
    ci.xyzt.y = 1.047197551196598;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 389141.322439244020000;
    ce.xyzt.y = -423913.251230397030000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=alsk +ellps=clrk66", ci, ce, co);

    direction = -1;
    ci.xyzt.x = -500000.000000000000000;
    ci.xyzt.y = -950000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.789573781198071;
    ce.xyzt.y = 0.963128449092835;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=alsk +ellps=clrk66", ci, ce, co);

    ci.xyzt.x = -305000.000000000000000;
    ci.xyzt.y = 700000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.793263387217860;
    ce.xyzt.y = 1.223669307928546;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=alsk +ellps=clrk66", ci, ce, co);

    ci.xyzt.x = 250000.000000000000000;
    ci.xyzt.y = 700000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.537377879950219;
    ce.xyzt.y = 1.224591086887639;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=alsk +ellps=clrk66", ci, ce, co);

    ci.xyzt.x = 400000.000000000000000;
    ci.xyzt.y = -400000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.526520918141052;
    ce.xyzt.y = 1.050739333902489;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=alsk +ellps=clrk66", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=alsk +R=6370997");
    tolerance = 0.000000100000;
    ci.xyzt.x = -2.792526803190927;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -511510.319410844010000;
    ce.xyzt.y = -967150.991676077950000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=alsk +R=6370997", ci, ce, co);

    ci.xyzt.x = -2.792526803190927;
    ci.xyzt.y = 1.221730476396031;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -303744.771290368980000;
    ce.xyzt.y = 685439.745941122990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=alsk +R=6370997", ci, ce, co);

    ci.xyzt.x = -2.530727415391778;
    ci.xyzt.y = 1.221730476396031;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 265354.974019663000000;
    ce.xyzt.y = 681386.892874573010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=alsk +R=6370997", ci, ce, co);

    ci.xyzt.x = -2.530727415391778;
    ci.xyzt.y = 1.047197551196598;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 387711.995394026980000;
    ce.xyzt.y = -422980.685505462990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=alsk +R=6370997", ci, ce, co);

    direction = -1;
    ci.xyzt.x = -500000.000000000000000;
    ci.xyzt.y = -950000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.789978874822718;
    ce.xyzt.y = 0.962822293680533;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=alsk +R=6370997", ci, ce, co);

    ci.xyzt.x = -305000.000000000000000;
    ci.xyzt.y = 700000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.793963774163304;
    ce.xyzt.y = 1.223969866785522;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=alsk +R=6370997", ci, ce, co);

    ci.xyzt.x = 250000.000000000000000;
    ci.xyzt.y = 700000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.536798148872602;
    ce.xyzt.y = 1.224899416945283;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=alsk +R=6370997", ci, ce, co);

    ci.xyzt.x = 400000.000000000000000;
    ci.xyzt.y = -400000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.526089025352258;
    ce.xyzt.y = 1.050575893103191;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=alsk +R=6370997", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=apian a=6400000 +lat_1=0 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223374.577355252990000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=apian a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223374.577355252990000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=apian a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223374.577355252990000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=apian a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223374.577355252990000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=apian a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=august a=6400000 +lat_1=0 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223404.978180971990000;
    ce.xyzt.y = 111722.340289763000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=august a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223404.978180971990000;
    ce.xyzt.y = -111722.340289763000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=august a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223404.978180971990000;
    ce.xyzt.y = 111722.340289763000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=august a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223404.978180971990000;
    ce.xyzt.y = -111722.340289763000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=august a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=bacon a=6400000 +lat_1=0 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223334.132555965010000;
    ce.xyzt.y = 175450.725922666000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bacon a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223334.132555965010000;
    ce.xyzt.y = -175450.725922666000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bacon a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223334.132555965010000;
    ce.xyzt.y = 175450.725922666000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bacon a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223334.132555965010000;
    ce.xyzt.y = -175450.725922666000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bacon a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=bipc ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2452160.217725756100000;
    ce.xyzt.y = -14548450.759654747000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bipc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2447915.213725341000000;
    ce.xyzt.y = -14763427.212798730000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bipc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2021695.522934909000000;
    ce.xyzt.y = -14540413.695283702000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bipc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2018090.503004699000000;
    ce.xyzt.y = -14755620.651414108000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bipc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.274765801350571;
    ce.xyzt.y = 0.301036457005734;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=bipc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.274741427024715;
    ce.xyzt.y = 0.301059085408925;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=bipc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.274716808003699;
    ce.xyzt.y = 0.300991391504890;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=bipc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.274692433677843;
    ce.xyzt.y = 0.301014019576469;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=bipc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=bipc a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2460565.740974965000000;
    ce.xyzt.y = -14598319.989330800000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bipc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2456306.185935200200000;
    ce.xyzt.y = -14814033.339502094000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bipc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2028625.497819099100000;
    ce.xyzt.y = -14590255.375482792000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bipc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2025008.120589142900000;
    ce.xyzt.y = -14806200.018759441000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bipc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.274765676035930;
    ce.xyzt.y = 0.301036418678303;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=bipc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.274741384979734;
    ce.xyzt.y = 0.301058969780862;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=bipc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.274716850048680;
    ce.xyzt.y = 0.300991507132953;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=bipc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.274692558992483;
    ce.xyzt.y = 0.301014057903899;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=bipc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=boggs a=6400000 +lat_1=0 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 211949.700808182010000;
    ce.xyzt.y = 117720.998305411010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=boggs a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 211949.700808182010000;
    ce.xyzt.y = -117720.998305411010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=boggs a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -211949.700808182010000;
    ce.xyzt.y = 117720.998305411010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=boggs a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -211949.700808182010000;
    ce.xyzt.y = -117720.998305411010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=boggs a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=bonne ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222605.296097157000000;
    ce.xyzt.y = 55321.139565495003000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bonne ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222605.296099239000000;
    ce.xyzt.y = -165827.647799052010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bonne ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222605.296097157000000;
    ce.xyzt.y = 55321.139565495003000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bonne ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222605.296099239000000;
    ce.xyzt.y = -165827.647799052010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bonne ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031358313217;
    ce.xyzt.y = 0.008742430476675;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=bonne ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031358295764;
    ce.xyzt.y = 0.008710862043269;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=bonne ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031358313217;
    ce.xyzt.y = 0.008742430476675;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=bonne ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031358295764;
    ce.xyzt.y = 0.008710862043269;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=bonne ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=bonne R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223368.115572528010000;
    ce.xyzt.y = 55884.555246393997000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bonne R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223368.115574631990000;
    ce.xyzt.y = -167517.599369694010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bonne R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223368.115572528010000;
    ce.xyzt.y = 55884.555246393997000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bonne R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223368.115574631990000;
    ce.xyzt.y = -167517.599369694010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=bonne R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031251202361;
    ce.xyzt.y = 0.008742271250287;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=bonne R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031251184908;
    ce.xyzt.y = 0.008711021252203;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=bonne R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031251202361;
    ce.xyzt.y = 0.008742271250287;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=bonne R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031251184908;
    ce.xyzt.y = 0.008711021252203;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=bonne R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=calcofi ellps=GRS80 +lat_1=0.5 +lat_2=2 +no_defs");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 508.444872149999980;
    ce.xyzt.y = -1171.764860417999900;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=calcofi ellps=GRS80 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 514.999168152000040;
    ce.xyzt.y = -1145.821981468000100;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=calcofi ellps=GRS80 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 500.685384124999980;
    ce.xyzt.y = -1131.445377920000100;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=calcofi ellps=GRS80 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 507.369719137000000;
    ce.xyzt.y = -1106.178201483000100;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=calcofi ellps=GRS80 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.926203096683601;
    ce.xyzt.y = 0.209999010018752;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=calcofi ellps=GRS80 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.718364069739549;
    ce.xyzt.y = 0.326354293490859;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=calcofi ellps=GRS80 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -3.620633601060157;
    ce.xyzt.y = 1.419198586179172;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=calcofi ellps=GRS80 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.090592071266480;
    ce.xyzt.y = 1.535553869633825;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=calcofi ellps=GRS80 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=calcofi R=6400000 +lat_1=0.5 +lat_2=2 +no_defs");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 507.090507488000010;
    ce.xyzt.y = -1164.727375197999900;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=calcofi R=6400000 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 513.686136375000050;
    ce.xyzt.y = -1138.999268217000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=calcofi R=6400000 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 499.336261476000000;
    ce.xyzt.y = -1124.435130997000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=calcofi R=6400000 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 506.060570392999980;
    ce.xyzt.y = -1099.375665067000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=calcofi R=6400000 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.925188754693774;
    ce.xyzt.y = 0.209999010018752;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=calcofi R=6400000 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.716048926911800;
    ce.xyzt.y = 0.326354293490859;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=calcofi R=6400000 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -3.622341969649115;
    ce.xyzt.y = 1.419198586179172;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=calcofi R=6400000 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.092173819848491;
    ce.xyzt.y = 1.535553869633825;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=calcofi R=6400000 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=cass ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222605.285776990990000;
    ce.xyzt.y = 110642.229253999000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cass ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222605.285776990990000;
    ce.xyzt.y = -110642.229253999000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cass ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222605.285776990990000;
    ce.xyzt.y = 110642.229253999000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cass ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222605.285776990990000;
    ce.xyzt.y = -110642.229253999000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cass ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cass ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cass ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cass ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cass ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=cass R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223368.105203484010000;
    ce.xyzt.y = 111769.145040585990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cass R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223368.105203484010000;
    ce.xyzt.y = -111769.145040585990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cass R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223368.105203484010000;
    ce.xyzt.y = 111769.145040585990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cass R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223368.105203484010000;
    ce.xyzt.y = -111769.145040585990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cass R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cass R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cass R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cass R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cass R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=pipeline +step +proj=ccon +lat_1=52 +lat_0=52 +lon_0=19 +a=6390000 +x_0=330000 +y_0=-350000 +step +proj=axisswap +order=1,-2,3,4");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.418879020478639;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 650031.541094132230000;
    ce.xyzt.y = 4106.161777064360600;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=pipeline +step +proj=ccon +lat_1=52 +lat_0=52 +lon_0=19 +a=6390000 +x_0=330000 +y_0=-350000 +step +proj=axisswap +order=1,-2,3,4", ci, ce, co);

    ci.xyzt.x = 0.261799387799149;
    ci.xyzt.y = 0.855211333477221;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 37074.189007307468000;
    ce.xyzt.y = 676826.235592700420000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=pipeline +step +proj=ccon +lat_1=52 +lat_0=52 +lon_0=19 +a=6390000 +x_0=330000 +y_0=-350000 +step +proj=axisswap +order=1,-2,3,4", ci, ce, co);

    ci.xyzt.x = 0.418879020478639;
    ci.xyzt.y = 0.855211333477221;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 696053.360616178480000;
    ce.xyzt.y = 672294.567958271950000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=pipeline +step +proj=ccon +lat_1=52 +lat_0=52 +lon_0=19 +a=6390000 +x_0=330000 +y_0=-350000 +step +proj=axisswap +order=1,-2,3,4", ci, ce, co);

    ci.xyzt.x = 0.331612557878923;
    ci.xyzt.y = 0.907571211037051;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 330000.000000000000000;
    ce.xyzt.y = 350000.000000000000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=pipeline +step +proj=ccon +lat_1=52 +lat_0=52 +lon_0=19 +a=6390000 +x_0=330000 +y_0=-350000 +step +proj=axisswap +order=1,-2,3,4", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 0.000000000000000;
    ci.xyzt.y = 0.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.241557535932658;
    ce.xyzt.y = 0.960461738391808;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=pipeline +step +proj=ccon +lat_1=52 +lat_0=52 +lon_0=19 +a=6390000 +x_0=330000 +y_0=-350000 +step +proj=axisswap +order=1,-2,3,4", ci, ce, co);

    ci.xyzt.x = 0.000000000000000;
    ci.xyzt.y = 700000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.253325004353958;
    ce.xyzt.y = 0.851264233583056;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=pipeline +step +proj=ccon +lat_1=52 +lat_0=52 +lon_0=19 +a=6390000 +x_0=330000 +y_0=-350000 +step +proj=axisswap +order=1,-2,3,4", ci, ce, co);

    ci.xyzt.x = 700000.000000000000000;
    ci.xyzt.y = 0.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.432539837923184;
    ce.xyzt.y = 0.959992445737815;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=pipeline +step +proj=ccon +lat_1=52 +lat_0=52 +lon_0=19 +a=6390000 +x_0=330000 +y_0=-350000 +step +proj=axisswap +order=1,-2,3,4", ci, ce, co);

    ci.xyzt.x = 700000.000000000000000;
    ci.xyzt.y = 700000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.419360919211760;
    ce.xyzt.y = 0.850856319344845;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=pipeline +step +proj=ccon +lat_1=52 +lat_0=52 +lon_0=19 +a=6390000 +x_0=330000 +y_0=-350000 +step +proj=axisswap +order=1,-2,3,4", ci, ce, co);

    ci.xyzt.x = 330000.000000000000000;
    ci.xyzt.y = 350000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.331612557878923;
    ce.xyzt.y = 0.907571211037051;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=pipeline +step +proj=ccon +lat_1=52 +lat_0=52 +lon_0=19 +a=6390000 +x_0=330000 +y_0=-350000 +step +proj=axisswap +order=1,-2,3,4", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=cc a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = 111712.415540593010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = -111712.415540593010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = 111712.415540593010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = -111712.415540593010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=cea ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222638.981586546990000;
    ce.xyzt.y = 110568.812396267010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cea ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222638.981586546990000;
    ce.xyzt.y = -110568.812396266000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cea ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222638.981586546990000;
    ce.xyzt.y = 110568.812396267010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cea ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222638.981586546990000;
    ce.xyzt.y = -110568.812396266000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cea ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cea ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cea ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cea ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cea ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=cea R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = 111695.401198614000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cea R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = -111695.401198614000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cea R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = 111695.401198614000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cea R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = -111695.401198614000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=cea R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cea R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cea R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cea R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=cea R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=chamb R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -27864.779586801000000;
    ce.xyzt.y = -223364.324593274010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=chamb R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -251312.283053493010000;
    ce.xyzt.y = -223402.145526208010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=chamb R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -27864.785649105001000;
    ce.xyzt.y = 223364.327328827000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=chamb R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -251312.289116442990000;
    ce.xyzt.y = 223402.142197287000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=chamb R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=collg a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 249872.921577930010000;
    ce.xyzt.y = 99423.174788460004000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=collg a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 254272.532301245000000;
    ce.xyzt.y = -98559.307760743002000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=collg a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -249872.921577930010000;
    ce.xyzt.y = 99423.174788460004000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=collg a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -254272.532301245000000;
    ce.xyzt.y = -98559.307760743002000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=collg a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000027694832211;
    ce.xyzt.y = 0.000017630844865;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=collg a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000027694343519;
    ce.xyzt.y = -0.000017631001944;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=collg a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000027694832211;
    ce.xyzt.y = 0.000017630844865;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=collg a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000027694343519;
    ce.xyzt.y = -0.000017631001944;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=collg a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=comill a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = 110611.859089459000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=comill a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = -110611.859089459000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=comill a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = 110611.859089459000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=comill a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = -110611.859089459000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=comill a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015779643940;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=comill a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015779643940;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=comill a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015779643940;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=comill a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015779643940;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=comill a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=crast a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 218280.142056781010000;
    ce.xyzt.y = 114306.045604280010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=crast a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 218280.142056781010000;
    ce.xyzt.y = -114306.045604280010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=crast a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -218280.142056781010000;
    ce.xyzt.y = 114306.045604280010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=crast a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -218280.142056781010000;
    ce.xyzt.y = -114306.045604280010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=crast a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031978952299;
    ce.xyzt.y = 0.000015268820975;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=crast a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031978952299;
    ce.xyzt.y = -0.000015268820975;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=crast a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031978952299;
    ce.xyzt.y = 0.000015268820975;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=crast a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031978952299;
    ce.xyzt.y = -0.000015268820975;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=crast a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=denoy a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223377.422876953990000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=denoy a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223377.422876953990000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=denoy a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223377.422876953990000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=denoy a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223377.422876953990000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=denoy a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=eck1 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 204680.888202951000000;
    ce.xyzt.y = 102912.178426065000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 204680.888202951000000;
    ce.xyzt.y = -102912.178426065000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -204680.888202951000000;
    ce.xyzt.y = 102912.178426065000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -204680.888202951000000;
    ce.xyzt.y = -102912.178426065000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000033918990483;
    ce.xyzt.y = 0.000016959399248;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000033918990483;
    ce.xyzt.y = -0.000016959399248;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000033918990483;
    ce.xyzt.y = 0.000016959399248;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000033918990483;
    ce.xyzt.y = -0.000016959399248;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=eck2 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 204472.870907959990000;
    ce.xyzt.y = 121633.734975242000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 204472.870907959990000;
    ce.xyzt.y = -121633.734975242000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -204472.870907959990000;
    ce.xyzt.y = 121633.734975242000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -204472.870907959990000;
    ce.xyzt.y = -121633.734975242000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000033918990483;
    ce.xyzt.y = 0.000014395545484;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000033918990483;
    ce.xyzt.y = -0.000014395545484;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000033918990483;
    ce.xyzt.y = 0.000014395545484;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000033918990483;
    ce.xyzt.y = -0.000014395545484;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=eck3 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 188652.015721537990000;
    ce.xyzt.y = 94328.919337030995000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 188652.015721537990000;
    ce.xyzt.y = -94328.919337030995000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -188652.015721537990000;
    ce.xyzt.y = 94328.919337030995000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -188652.015721537990000;
    ce.xyzt.y = -94328.919337030995000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000037005186386;
    ce.xyzt.y = 0.000018502584466;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000037005186386;
    ce.xyzt.y = -0.000018502584466;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000037005186386;
    ce.xyzt.y = 0.000018502584466;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000037005186386;
    ce.xyzt.y = -0.000018502584466;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=eck4 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 188646.389356416010000;
    ce.xyzt.y = 132268.540174065010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck4 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 188646.389356416010000;
    ce.xyzt.y = -132268.540174065010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck4 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -188646.389356416010000;
    ce.xyzt.y = 132268.540174065010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck4 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -188646.389356416010000;
    ce.xyzt.y = -132268.540174065010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck4 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000037005186386;
    ce.xyzt.y = 0.000013194950944;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck4 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000037005186386;
    ce.xyzt.y = -0.000013194950944;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck4 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000037005186386;
    ce.xyzt.y = 0.000013194950944;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck4 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000037005186386;
    ce.xyzt.y = -0.000013194950944;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck4 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=eck5 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 197031.392134061000000;
    ce.xyzt.y = 98523.198847227002000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 197031.392134061000000;
    ce.xyzt.y = -98523.198847227002000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -197031.392134061000000;
    ce.xyzt.y = 98523.198847227002000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -197031.392134061000000;
    ce.xyzt.y = -98523.198847227002000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000035429817296;
    ce.xyzt.y = 0.000017714899922;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000035429817296;
    ce.xyzt.y = -0.000017714899922;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000035429817296;
    ce.xyzt.y = 0.000017714899922;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000035429817296;
    ce.xyzt.y = -0.000017714899922;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=eck6 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 197021.605628991990000;
    ce.xyzt.y = 126640.420733174000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 197021.605628991990000;
    ce.xyzt.y = -126640.420733174000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -197021.605628991990000;
    ce.xyzt.y = 126640.420733174000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -197021.605628991990000;
    ce.xyzt.y = -126640.420733174000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eck6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000035429817296;
    ce.xyzt.y = 0.000013781643373;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000035429817296;
    ce.xyzt.y = -0.000013781643373;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000035429817296;
    ce.xyzt.y = 0.000013781643373;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000035429817296;
    ce.xyzt.y = -0.000013781643373;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eck6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=eqc a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eqc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eqc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eqc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eqc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eqc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eqc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eqc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eqc a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=eqdc ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222588.440269285990000;
    ce.xyzt.y = 110659.134907347000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eqdc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222756.836702042000000;
    ce.xyzt.y = -110489.578087221000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eqdc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222588.440269285990000;
    ce.xyzt.y = 110659.134907347000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eqdc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222756.836702042000000;
    ce.xyzt.y = -110489.578087221000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eqdc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031352379098;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eqdc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031352361645;
    ce.xyzt.y = -0.000015784234156;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eqdc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031352379098;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eqdc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031352361645;
    ce.xyzt.y = -0.000015784234156;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eqdc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=eqdc R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223351.088175114010000;
    ce.xyzt.y = 111786.108747174000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eqdc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223521.200266734990000;
    ce.xyzt.y = -111615.970741241010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eqdc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223351.088175114010000;
    ce.xyzt.y = 111786.108747174000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eqdc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223521.200266734990000;
    ce.xyzt.y = -111615.970741241010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=eqdc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031245250788;
    ce.xyzt.y = 0.000015624990315;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eqdc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031245233335;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eqdc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031245250788;
    ce.xyzt.y = 0.000015624990315;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eqdc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031245233335;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=eqdc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=euler ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222597.634659108010000;
    ce.xyzt.y = 111404.240549919010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=euler ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222767.165631876010000;
    ce.xyzt.y = -111234.676491018010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=euler ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222597.634659108010000;
    ce.xyzt.y = 111404.240549919010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=euler ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222767.165631876010000;
    ce.xyzt.y = -111234.676491018010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=euler ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031351017741;
    ce.xyzt.y = 0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=euler ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031350982834;
    ce.xyzt.y = -0.000015678571923;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=euler ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031351017741;
    ce.xyzt.y = 0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=euler ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031350982834;
    ce.xyzt.y = -0.000015678571923;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=euler ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=euler a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223360.655598693990000;
    ce.xyzt.y = 111786.112389791000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=euler a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223530.767690315990000;
    ce.xyzt.y = -111615.967098623990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=euler a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223360.655598693990000;
    ce.xyzt.y = 111786.112389791000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=euler a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223530.767690315990000;
    ce.xyzt.y = -111615.967098623990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=euler a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031243906885;
    ce.xyzt.y = 0.000015624990315;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=euler a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031243889432;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=euler a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031243906885;
    ce.xyzt.y = 0.000015624990315;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=euler a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031243889432;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=euler a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=etmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222650.796797585990000;
    ce.xyzt.y = 110642.229411933000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=etmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222650.796797585990000;
    ce.xyzt.y = -110642.229411933000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=etmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222650.796797585990000;
    ce.xyzt.y = 110642.229411933000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=etmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222650.796797585990000;
    ce.xyzt.y = -110642.229411933000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=etmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=etmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=etmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=etmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=etmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=fahey a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 182993.344649123990000;
    ce.xyzt.y = 101603.193569884000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=fahey a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 182993.344649123990000;
    ce.xyzt.y = -101603.193569884000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=fahey a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -182993.344649123990000;
    ce.xyzt.y = 101603.193569884000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=fahey a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -182993.344649123990000;
    ce.xyzt.y = -101603.193569884000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=fahey a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000038149214804;
    ce.xyzt.y = 0.000017178333350;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=fahey a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000038149214804;
    ce.xyzt.y = -0.000017178333350;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=fahey a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000038149214804;
    ce.xyzt.y = 0.000017178333350;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=fahey a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000038149214804;
    ce.xyzt.y = -0.000017178333350;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=fahey a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=fouc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222588.120675891990000;
    ce.xyzt.y = 111322.316700694010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=fouc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222588.120675891990000;
    ce.xyzt.y = -111322.316700694010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=fouc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222588.120675891990000;
    ce.xyzt.y = 111322.316700694010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=fouc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222588.120675891990000;
    ce.xyzt.y = -111322.316700694010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=fouc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=fouc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=fouc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=fouc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=fouc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=fouc a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223351.109003413990000;
    ce.xyzt.y = 111703.907721713000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=fouc a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223351.109003413990000;
    ce.xyzt.y = -111703.907721713000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=fouc a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223351.109003413990000;
    ce.xyzt.y = 111703.907721713000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=fouc a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223351.109003413990000;
    ce.xyzt.y = -111703.907721713000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=fouc a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=fouc a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=fouc a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=fouc a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=fouc a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=fouc_s a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = 111695.401198614000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=fouc_s a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = -111695.401198614000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=fouc_s a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = 111695.401198614000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=fouc_s a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = -111695.401198614000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=fouc_s a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=fouc_s a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=fouc_s a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=fouc_s a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=fouc_s a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=gall a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 157969.171134520000000;
    ce.xyzt.y = 95345.249178386002000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gall a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 157969.171134520000000;
    ce.xyzt.y = -95345.249178386002000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gall a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -157969.171134520000000;
    ce.xyzt.y = 95345.249178386002000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gall a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -157969.171134520000000;
    ce.xyzt.y = -95345.249178386002000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gall a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000044194180121;
    ce.xyzt.y = 0.000018305833500;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gall a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000044194180121;
    ce.xyzt.y = -0.000018305833500;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gall a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000044194180121;
    ce.xyzt.y = 0.000018305833500;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gall a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000044194180121;
    ce.xyzt.y = -0.000018305833500;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gall a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=geocent ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222638.981586546990000;
    ce.xyzt.y = 111319.490793274000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=geocent ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222638.981586546990000;
    ce.xyzt.y = -111319.490793274000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=geocent ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222638.981586546990000;
    ce.xyzt.y = 111319.490793274000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=geocent ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222638.981586546990000;
    ce.xyzt.y = -111319.490793274000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=geocent ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=geocent ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=geocent ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=geocent ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=geocent ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=geocent a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=geos ellps=GRS80 +lat_1=0.5 +lat_2=2 +h=35785831");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222527.070365800000000;
    ce.xyzt.y = 110551.303413329010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=geos ellps=GRS80 +lat_1=0.5 +lat_2=2 +h=35785831", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222527.070365800000000;
    ce.xyzt.y = -110551.303413329010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=geos ellps=GRS80 +lat_1=0.5 +lat_2=2 +h=35785831", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222527.070365800000000;
    ce.xyzt.y = 110551.303413329010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=geos ellps=GRS80 +lat_1=0.5 +lat_2=2 +h=35785831", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222527.070365800000000;
    ce.xyzt.y = -110551.303413329010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=geos ellps=GRS80 +lat_1=0.5 +lat_2=2 +h=35785831", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=geos ellps=GRS80 +lat_1=0.5 +lat_2=2 +h=35785831", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=geos ellps=GRS80 +lat_1=0.5 +lat_2=2 +h=35785831", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=geos ellps=GRS80 +lat_1=0.5 +lat_2=2 +h=35785831", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=geos ellps=GRS80 +lat_1=0.5 +lat_2=2 +h=35785831", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=geos R=6400000 +lat_1=0.5 +lat_2=2 +h=35785831");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223289.457635795000000;
    ce.xyzt.y = 111677.657456537000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=geos R=6400000 +lat_1=0.5 +lat_2=2 +h=35785831", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223289.457635795000000;
    ce.xyzt.y = -111677.657456537000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=geos R=6400000 +lat_1=0.5 +lat_2=2 +h=35785831", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223289.457635795000000;
    ce.xyzt.y = 111677.657456537000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=geos R=6400000 +lat_1=0.5 +lat_2=2 +h=35785831", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223289.457635795000000;
    ce.xyzt.y = -111677.657456537000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=geos R=6400000 +lat_1=0.5 +lat_2=2 +h=35785831", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=geos R=6400000 +lat_1=0.5 +lat_2=2 +h=35785831", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=geos R=6400000 +lat_1=0.5 +lat_2=2 +h=35785831", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=geos R=6400000 +lat_1=0.5 +lat_2=2 +h=35785831", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=geos R=6400000 +lat_1=0.5 +lat_2=2 +h=35785831", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=gins8 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 194350.250939590010000;
    ce.xyzt.y = 111703.907635335000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gins8 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 194350.250939590010000;
    ce.xyzt.y = -111703.907635335000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gins8 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -194350.250939590010000;
    ce.xyzt.y = 111703.907635335000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gins8 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -194350.250939590010000;
    ce.xyzt.y = -111703.907635335000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gins8 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=gn_sinu a=6400000 +lat_1=0.5 +lat_2=2 +m=1 n=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223385.132504696000000;
    ce.xyzt.y = 111698.236447187000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gn_sinu a=6400000 +lat_1=0.5 +lat_2=2 +m=1 n=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223385.132504696000000;
    ce.xyzt.y = -111698.236447187000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gn_sinu a=6400000 +lat_1=0.5 +lat_2=2 +m=1 n=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223385.132504696000000;
    ce.xyzt.y = 111698.236447187000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gn_sinu a=6400000 +lat_1=0.5 +lat_2=2 +m=1 n=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223385.132504696000000;
    ce.xyzt.y = -111698.236447187000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gn_sinu a=6400000 +lat_1=0.5 +lat_2=2 +m=1 n=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gn_sinu a=6400000 +lat_1=0.5 +lat_2=2 +m=1 n=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gn_sinu a=6400000 +lat_1=0.5 +lat_2=2 +m=1 n=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gn_sinu a=6400000 +lat_1=0.5 +lat_2=2 +m=1 n=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gn_sinu a=6400000 +lat_1=0.5 +lat_2=2 +m=1 n=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=gnom a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223492.924747185000000;
    ce.xyzt.y = 111780.509206592990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gnom a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223492.924747185000000;
    ce.xyzt.y = -111780.509206592990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gnom a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223492.924747185000000;
    ce.xyzt.y = 111780.509206592990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gnom a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223492.924747185000000;
    ce.xyzt.y = -111780.509206592990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gnom a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gnom a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gnom a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gnom a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gnom a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=goode a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223368.119026631990000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=goode a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223368.119026631990000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=goode a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223368.119026631990000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=goode a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223368.119026631990000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=goode a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=goode a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=goode a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=goode a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=goode a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=gs48 +R=6370997");
    tolerance = 0.000000100000;
    ci.xyzt.x = -2.076941809873252;
    ci.xyzt.y = 0.698131700797732;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1923908.446529346100000;
    ce.xyzt.y = 355874.658944479020000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gs48 +R=6370997", ci, ce, co);

    ci.xyzt.x = -1.221730476396031;
    ci.xyzt.y = 1.117010721276371;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1354020.375109297900000;
    ce.xyzt.y = 3040846.007866525100000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gs48 +R=6370997", ci, ce, co);

    ci.xyzt.x = -1.396263401595464;
    ci.xyzt.y = 0.436332312998582;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1625139.160484320000000;
    ce.xyzt.y = -1413614.894029108100000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gs48 +R=6370997", ci, ce, co);

    ci.xyzt.x = -1.658062789394613;
    ci.xyzt.y = 0.610865238198015;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 90241.658071458005000;
    ce.xyzt.y = -439595.048485902020000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gs48 +R=6370997", ci, ce, co);

    direction = -1;
    ci.xyzt.x = -1923000.000000000000000;
    ci.xyzt.y = 355000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.076716882538123;
    ce.xyzt.y = 0.698034831341601;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gs48 +R=6370997", ci, ce, co);

    ci.xyzt.x = 1354000.000000000000000;
    ci.xyzt.y = 3040000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.221821390579314;
    ce.xyzt.y = 1.116895317243889;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gs48 +R=6370997", ci, ce, co);

    ci.xyzt.x = 1625000.000000000000000;
    ci.xyzt.y = -1413000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.396269451081184;
    ce.xyzt.y = 0.436430095890230;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gs48 +R=6370997", ci, ce, co);

    ci.xyzt.x = 90000.000000000000000;
    ci.xyzt.y = -439000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.658108280930327;
    ce.xyzt.y = 0.610959917161215;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gs48 +R=6370997", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=gs50 +ellps=clrk66");
    tolerance = 0.000000100000;
    ci.xyzt.x = -2.792526803190927;
    ci.xyzt.y = 1.134464013796314;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1874628.537740232900000;
    ce.xyzt.y = 2660907.942291014800000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gs50 +ellps=clrk66", ci, ce, co);

    ci.xyzt.x = -2.268928027592629;
    ci.xyzt.y = 0.785398163397448;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -771831.518853335990000;
    ce.xyzt.y = 48465.166491304997000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gs50 +ellps=clrk66", ci, ce, co);

    ci.xyzt.x = -1.134464013796314;
    ci.xyzt.y = 0.785398163397448;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 4030931.833981508900000;
    ce.xyzt.y = 1323687.864777399000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gs50 +ellps=clrk66", ci, ce, co);

    ci.xyzt.x = -1.396263401595464;
    ci.xyzt.y = 0.628318530717959;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 3450764.261536100900000;
    ce.xyzt.y = -175619.041820732000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gs50 +ellps=clrk66", ci, ce, co);

    direction = -1;
    ci.xyzt.x = -1800000.000000000000000;
    ci.xyzt.y = 2600000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.757433206121689;
    ce.xyzt.y = 1.131873240247870;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gs50 +ellps=clrk66", ci, ce, co);

    ci.xyzt.x = -800000.000000000000000;
    ci.xyzt.y = 500000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.289372648068253;
    ce.xyzt.y = 0.856694335309505;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gs50 +ellps=clrk66", ci, ce, co);

    ci.xyzt.x = 4000000.000000000000000;
    ci.xyzt.y = 1300000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.143043505849263;
    ce.xyzt.y = 0.785273161589970;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gs50 +ellps=clrk66", ci, ce, co);

    ci.xyzt.x = 3900000.000000000000000;
    ci.xyzt.y = -170000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.318607770643029;
    ce.xyzt.y = 0.596747515551179;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gs50 +ellps=clrk66", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=gs50 +R=6370997");
    tolerance = 0.000000100000;
    ci.xyzt.x = -2.792526803190927;
    ci.xyzt.y = 1.134464013796314;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1867268.253460008900000;
    ce.xyzt.y = 2656506.230401822800000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gs50 +R=6370997", ci, ce, co);

    ci.xyzt.x = -2.268928027592629;
    ci.xyzt.y = 0.785398163397448;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -769572.189672993960000;
    ce.xyzt.y = 48324.312440864000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gs50 +R=6370997", ci, ce, co);

    ci.xyzt.x = -1.134464013796314;
    ci.xyzt.y = 0.785398163397448;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 4019393.068680791200000;
    ce.xyzt.y = 1320191.309350288900000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gs50 +R=6370997", ci, ce, co);

    ci.xyzt.x = -1.396263401595464;
    ci.xyzt.y = 0.628318530717959;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 3442685.615172346100000;
    ce.xyzt.y = -178760.423489429000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gs50 +R=6370997", ci, ce, co);

    direction = -1;
    ci.xyzt.x = -1800000.000000000000000;
    ci.xyzt.y = 2600000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.760470254338483;
    ce.xyzt.y = 1.131920866007100;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gs50 +R=6370997", ci, ce, co);

    ci.xyzt.x = -800000.000000000000000;
    ci.xyzt.y = 500000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.289990957013537;
    ce.xyzt.y = 0.856658479352618;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gs50 +R=6370997", ci, ce, co);

    ci.xyzt.x = 4000000.000000000000000;
    ci.xyzt.y = 1300000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.140554256811863;
    ce.xyzt.y = 0.784652780083652;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gs50 +R=6370997", ci, ce, co);

    ci.xyzt.x = 3900000.000000000000000;
    ci.xyzt.y = -170000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.316795423383205;
    ce.xyzt.y = 0.596647894775469;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gs50 +R=6370997", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=hammer a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223373.788703241010000;
    ce.xyzt.y = 111703.907397767010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=hammer a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223373.788703241010000;
    ce.xyzt.y = -111703.907397767010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=hammer a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223373.788703241010000;
    ce.xyzt.y = 111703.907397767010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=hammer a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223373.788703241010000;
    ce.xyzt.y = -111703.907397767010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=hammer a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=hammer a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=hammer a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=hammer a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=hammer a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=hatano a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 189878.878946528010000;
    ce.xyzt.y = 131409.802440625990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=hatano a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 189881.081952445010000;
    ce.xyzt.y = -131409.142276074010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=hatano a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -189878.878946528010000;
    ce.xyzt.y = 131409.802440625990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=hatano a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -189881.081952445010000;
    ce.xyzt.y = -131409.142276074010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=hatano a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000036764697468;
    ce.xyzt.y = 0.000013281205116;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=hatano a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000036764697468;
    ce.xyzt.y = -0.000013281222569;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=hatano a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000036764697468;
    ce.xyzt.y = 0.000013281205116;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=hatano a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000036764697468;
    ce.xyzt.y = -0.000013281222569;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=hatano a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=healpix ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222390.103949239010000;
    ce.xyzt.y = 130406.588664482000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=healpix ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222390.103949239010000;
    ce.xyzt.y = -130406.588664480990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=healpix ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222390.103949239010000;
    ce.xyzt.y = 130406.588664482000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=healpix ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222390.103949239010000;
    ce.xyzt.y = -130406.588664480990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=healpix ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031392207511;
    ce.xyzt.y = 0.000013383097438;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=healpix ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031392207511;
    ce.xyzt.y = -0.000013383097438;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=healpix ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031392207511;
    ce.xyzt.y = 0.000013383097438;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=healpix ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031392207511;
    ce.xyzt.y = -0.000013383097438;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=healpix ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=healpix R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = 131588.044441998990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=healpix R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = -131588.044441998990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=healpix R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = 131588.044441998990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=healpix R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = -131588.044441998990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=healpix R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000013262914066;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=healpix R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000013262914066;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=healpix R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000013262914066;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=healpix R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000013262914066;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=healpix R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=rhealpix ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222390.103949239010000;
    ce.xyzt.y = 130406.588664482000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=rhealpix ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222390.103949239010000;
    ce.xyzt.y = -130406.588664480990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=rhealpix ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222390.103949239010000;
    ce.xyzt.y = 130406.588664482000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=rhealpix ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222390.103949239010000;
    ce.xyzt.y = -130406.588664480990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=rhealpix ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031392207511;
    ce.xyzt.y = 0.000013383097438;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=rhealpix ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031392207511;
    ce.xyzt.y = -0.000013383097438;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=rhealpix ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031392207511;
    ce.xyzt.y = 0.000013383097438;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=rhealpix ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031392207511;
    ce.xyzt.y = -0.000013383097438;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=rhealpix ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=rhealpix R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = 131588.044441998990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=rhealpix R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = -131588.044441998990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=rhealpix R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = 131588.044441998990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=rhealpix R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = -131588.044441998990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=rhealpix R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000013262914066;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=rhealpix R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000013262914066;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=rhealpix R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000013262914066;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=rhealpix R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000013262914066;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=rhealpix R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=igh a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223878.497456271000000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=igh a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223708.371313058010000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=igh a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222857.740596991990000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=igh a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223027.866740205010000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=igh a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249928271;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=igh a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249963177;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=igh a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031250067897;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=igh a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031250050444;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=igh a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=imw_p ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222588.441139375990000;
    ce.xyzt.y = 55321.128653810003000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=imw_p ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222756.906377687000000;
    ce.xyzt.y = -165827.584288324000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=imw_p ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222588.441139375990000;
    ce.xyzt.y = 55321.128653810003000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=imw_p ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222756.906377687000000;
    ce.xyzt.y = -165827.584288324000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=imw_p ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031358313217;
    ce.xyzt.y = 0.008742440163252;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=imw_p ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031358295764;
    ce.xyzt.y = 0.008710852356691;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=imw_p ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031358313217;
    ce.xyzt.y = 0.008742440163252;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=imw_p ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031358295764;
    ce.xyzt.y = 0.008710852356691;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=imw_p ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=isea a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1097074.948022474100000;
    ce.xyzt.y = 3442909.309037182900000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=isea a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1097074.948264794900000;
    ce.xyzt.y = 3233611.728585708000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=isea a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1575486.353641554000000;
    ce.xyzt.y = 3442168.342028188100000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=isea a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1575486.353880283000000;
    ce.xyzt.y = 3234352.695594706100000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=isea a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=kav5 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 200360.905308829010000;
    ce.xyzt.y = 123685.082476998000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=kav5 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 200360.905308829010000;
    ce.xyzt.y = -123685.082476998000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=kav5 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -200360.905308829010000;
    ce.xyzt.y = 123685.082476998000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=kav5 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -200360.905308829010000;
    ce.xyzt.y = -123685.082476998000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=kav5 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000034841292273;
    ce.xyzt.y = 0.000014110690296;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=kav5 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000034841292273;
    ce.xyzt.y = -0.000014110690296;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=kav5 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000034841292273;
    ce.xyzt.y = 0.000014110690296;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=kav5 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000034841292273;
    ce.xyzt.y = -0.000014110690296;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=kav5 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=kav5 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 201047.703110878010000;
    ce.xyzt.y = 124109.050629171000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=kav5 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 201047.703110878010000;
    ce.xyzt.y = -124109.050629171000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=kav5 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -201047.703110878010000;
    ce.xyzt.y = 124109.050629171000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=kav5 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -201047.703110878010000;
    ce.xyzt.y = -124109.050629171000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=kav5 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000034722278271;
    ce.xyzt.y = 0.000014062484302;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=kav5 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000034722278271;
    ce.xyzt.y = -0.000014062484302;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=kav5 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000034722278271;
    ce.xyzt.y = 0.000014062484302;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=kav5 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000034722278271;
    ce.xyzt.y = -0.000014062484302;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=kav5 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=kav7 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 193462.974943729000000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=kav7 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 193462.974943729000000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=kav7 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -193462.974943729000000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=kav7 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -193462.974943729000000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=kav7 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000036084385579;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=kav7 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000036084385579;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=kav7 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000036084385579;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=kav7 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000036084385579;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=kav7 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=krovak +ellps=GRS80 no_defs");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -3196535.232563640900000;
    ce.xyzt.y = -6617878.867551444100000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=krovak +ellps=GRS80 no_defs", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -3260035.440552108900000;
    ce.xyzt.y = -6898873.614878031400000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=krovak +ellps=GRS80 no_defs", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -3756305.328869175200000;
    ce.xyzt.y = -6478142.561571511400000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=krovak +ellps=GRS80 no_defs", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -3831703.658501982200000;
    ce.xyzt.y = -6759107.170155394800000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=krovak +ellps=GRS80 no_defs", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.433473793882657;
    ce.xyzt.y = 1.042980904367579;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=krovak +ellps=GRS80 no_defs", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.433475479277302;
    ce.xyzt.y = 1.042954453780592;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=krovak +ellps=GRS80 no_defs", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.433373067947011;
    ce.xyzt.y = 1.042980904367579;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=krovak +ellps=GRS80 no_defs", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.433371382552366;
    ce.xyzt.y = 1.042954453780592;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=krovak +ellps=GRS80 no_defs", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=labrd ellps=GRS80 +lon_0=0.5 +lat_0=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 166973.166090228010000;
    ce.xyzt.y = -110536.912730266010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=labrd ellps=GRS80 +lon_0=0.5 +lat_0=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 166973.168287156990000;
    ce.xyzt.y = -331761.993650884020000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=labrd ellps=GRS80 +lon_0=0.5 +lat_0=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -278345.500519976020000;
    ce.xyzt.y = -110469.032642032010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=labrd ellps=GRS80 +lon_0=0.5 +lat_0=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -278345.504185269990000;
    ce.xyzt.y = -331829.870790274990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=labrd ellps=GRS80 +lon_0=0.5 +lat_0=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.008758022375547;
    ce.xyzt.y = 0.034922369047150;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=labrd ellps=GRS80 +lon_0=0.5 +lat_0=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.008758022340641;
    ce.xyzt.y = 0.034890800997717;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=labrd ellps=GRS80 +lon_0=0.5 +lat_0=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.008695270144396;
    ce.xyzt.y = 0.034922369047150;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=labrd ellps=GRS80 +lon_0=0.5 +lat_0=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.008695270179303;
    ce.xyzt.y = 0.034890800997717;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=labrd ellps=GRS80 +lon_0=0.5 +lat_0=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=laea ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222602.471450095010000;
    ce.xyzt.y = 110589.827224409990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=laea ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222602.471450095010000;
    ce.xyzt.y = -110589.827224409000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=laea ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222602.471450095010000;
    ce.xyzt.y = 110589.827224409990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=laea ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222602.471450095010000;
    ce.xyzt.y = -110589.827224409000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=laea ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=laea ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=laea ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=laea ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=laea ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=laea R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223365.281370125010000;
    ce.xyzt.y = 111716.668072916000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=laea R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223365.281370125010000;
    ce.xyzt.y = -111716.668072916000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=laea R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223365.281370125010000;
    ce.xyzt.y = 111716.668072916000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=laea R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223365.281370125010000;
    ce.xyzt.y = -111716.668072916000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=laea R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=laea R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=laea R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=laea R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=laea R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=lagrng a=6400000 +W=2 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 111703.375917226000000;
    ce.xyzt.y = 27929.831908033000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lagrng a=6400000 +W=2 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 111699.122088816000000;
    ce.xyzt.y = -83784.178013358003000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lagrng a=6400000 +W=2 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -111703.375917226000000;
    ce.xyzt.y = 27929.831908033000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lagrng a=6400000 +W=2 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -111699.122088816000000;
    ce.xyzt.y = -83784.178013358003000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lagrng a=6400000 +W=2 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=larr a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223393.637624200990000;
    ce.xyzt.y = 111707.215961256010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=larr a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223393.637624200990000;
    ce.xyzt.y = -111707.215961256010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=larr a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223393.637624200990000;
    ce.xyzt.y = 111707.215961256010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=larr a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223393.637624200990000;
    ce.xyzt.y = -111707.215961256010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=larr a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=lask a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 217928.275907355010000;
    ce.xyzt.y = 112144.329220142000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lask a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 217928.275907355010000;
    ce.xyzt.y = -112144.329220142000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lask a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -217928.275907355010000;
    ce.xyzt.y = 112144.329220142000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lask a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -217928.275907355010000;
    ce.xyzt.y = -112144.329220142000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lask a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=lcc ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222588.439735967990000;
    ce.xyzt.y = 110660.533870800010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lcc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222756.879700279010000;
    ce.xyzt.y = -110532.797660827000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lcc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222588.439735967990000;
    ce.xyzt.y = 110660.533870800010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lcc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222756.879700279010000;
    ce.xyzt.y = -110532.797660827000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lcc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031352379098;
    ce.xyzt.y = 0.000015781825602;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=lcc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031352361645;
    ce.xyzt.y = -0.000015781843055;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=lcc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031352379098;
    ce.xyzt.y = 0.000015781825602;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=lcc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031352361645;
    ce.xyzt.y = -0.000015781843055;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=lcc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=lcca ellps=GRS80 +lat_0=1 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222605.285770237010000;
    ce.xyzt.y = 67.806007272000002;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lcca ellps=GRS80 +lat_0=1 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222740.037637937000000;
    ce.xyzt.y = -221125.539829602000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lcca ellps=GRS80 +lat_0=1 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222605.285770237010000;
    ce.xyzt.y = 67.806007272000002;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lcca ellps=GRS80 +lat_0=1 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222740.037637937000000;
    ce.xyzt.y = -221125.539829602000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lcca ellps=GRS80 +lat_0=1 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031361873689;
    ce.xyzt.y = 0.017469076684286;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=lcca ellps=GRS80 +lat_0=1 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031361856236;
    ce.xyzt.y = 0.017437508338147;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=lcca ellps=GRS80 +lat_0=1 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031361873689;
    ce.xyzt.y = 0.017469076684286;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=lcca ellps=GRS80 +lat_0=1 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031361856236;
    ce.xyzt.y = 0.017437508338147;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=lcca ellps=GRS80 +lat_0=1 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=leac ellps=GRS80 +lat_1=0 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 220685.140542979000000;
    ce.xyzt.y = 112983.500889396000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=leac ellps=GRS80 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 224553.312279826000000;
    ce.xyzt.y = -108128.636744873000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=leac ellps=GRS80 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -220685.140542979000000;
    ce.xyzt.y = 112983.500889396000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=leac ellps=GRS80 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -224553.312279826000000;
    ce.xyzt.y = -108128.636744873000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=leac ellps=GRS80 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357370739;
    ce.xyzt.y = 0.000015783919997;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=leac ellps=GRS80 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031356864594;
    ce.xyzt.y = -0.000015784530862;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=leac ellps=GRS80 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357370739;
    ce.xyzt.y = 0.000015783919997;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=leac ellps=GRS80 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031356864594;
    ce.xyzt.y = -0.000015784530862;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=leac ellps=GRS80 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=leac R=6400000 +lat_1=0 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 221432.868592852000000;
    ce.xyzt.y = 114119.454526532000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=leac R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 225331.724127111000000;
    ce.xyzt.y = -109245.829435056000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=leac R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -221432.868592852000000;
    ce.xyzt.y = 114119.454526532000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=leac R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -225331.724127111000000;
    ce.xyzt.y = -109245.829435056000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=leac R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031250242430;
    ce.xyzt.y = 0.000015624693609;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=leac R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249753738;
    ce.xyzt.y = -0.000015625304475;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=leac R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031250242430;
    ce.xyzt.y = 0.000015624693609;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=leac R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249753738;
    ce.xyzt.y = -0.000015625304475;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=leac R=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=lee_os R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000001000000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -25564478.952605054000000;
    ce.xyzt.y = 154490848.828625500000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lee_os R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 30115393.938574638000000;
    ce.xyzt.y = 125193997.439701940000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lee_os R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -31039340.592166007000000;
    ce.xyzt.y = 57678685.044891536000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lee_os R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -3088419.939423570900000;
    ce.xyzt.y = 58150091.099111006000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lee_os R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.879749274033809;
    ce.xyzt.y = -0.174511263237408;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=lee_os R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.879749273702196;
    ce.xyzt.y = -0.174554586829845;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=lee_os R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.879837257547479;
    ce.xyzt.y = -0.174511263237408;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=lee_os R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.879837257879091;
    ce.xyzt.y = -0.174554586829845;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=lee_os R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=loxim a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223382.295791339010000;
    ce.xyzt.y = 55850.536063819003000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=loxim a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223393.637462243000000;
    ce.xyzt.y = -167551.608191456010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=loxim a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223382.295791339010000;
    ce.xyzt.y = 55850.536063819003000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=loxim a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223393.637462243000000;
    ce.xyzt.y = -167551.608191456010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=loxim a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031251184908;
    ce.xyzt.y = 0.008742271267740;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=loxim a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031251184908;
    ce.xyzt.y = 0.008711021252203;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=loxim a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031251184908;
    ce.xyzt.y = 0.008742271267740;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=loxim a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031251184908;
    ce.xyzt.y = 0.008711021252203;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=loxim a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=lsat +ellps=GRS80 +lat_1=0.5 +lat_2=2 +lsat=1 +path=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 18241950.014558554000000;
    ce.xyzt.y = 9998256.839822936800000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lsat +ellps=GRS80 +lat_1=0.5 +lat_2=2 +lsat=1 +path=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 18746856.253319457000000;
    ce.xyzt.y = 10215761.669925213000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lsat +ellps=GRS80 +lat_1=0.5 +lat_2=2 +lsat=1 +path=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 18565503.683633164000000;
    ce.xyzt.y = 9085039.146727051600000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lsat +ellps=GRS80 +lat_1=0.5 +lat_2=2 +lsat=1 +path=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 19019696.902028911000000;
    ce.xyzt.y = 9247763.039432821800000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=lsat +ellps=GRS80 +lat_1=0.5 +lat_2=2 +lsat=1 +path=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.199122254829090;
    ce.xyzt.y = 0.000030085671487;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=lsat +ellps=GRS80 +lat_1=0.5 +lat_2=2 +lsat=1 +path=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.199153494529732;
    ce.xyzt.y = 0.000032814895198;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=lsat +ellps=GRS80 +lat_1=0.5 +lat_2=2 +lsat=1 +path=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.199127676415159;
    ce.xyzt.y = -0.000032814895198;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=lsat +ellps=GRS80 +lat_1=0.5 +lat_2=2 +lsat=1 +path=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.199158916133253;
    ce.xyzt.y = -0.000030085671487;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=lsat +ellps=GRS80 +lat_1=0.5 +lat_2=2 +lsat=1 +path=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=mbt_s ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 204131.517850273000000;
    ce.xyzt.y = 121400.330225508010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbt_s ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 204131.517850273000000;
    ce.xyzt.y = -121400.330225508010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbt_s ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -204131.517850273000000;
    ce.xyzt.y = 121400.330225508010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbt_s ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -204131.517850273000000;
    ce.xyzt.y = -121400.330225508010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbt_s ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000034197684658;
    ce.xyzt.y = 0.000014376259595;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbt_s ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000034197684658;
    ce.xyzt.y = -0.000014376259595;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbt_s ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000034197684658;
    ce.xyzt.y = 0.000014376259595;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbt_s ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000034197684658;
    ce.xyzt.y = -0.000014376259595;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbt_s ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=mbt_s a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 204831.240570992000000;
    ce.xyzt.y = 121816.466696035000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbt_s a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 204831.240570992000000;
    ce.xyzt.y = -121816.466696035000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbt_s a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -204831.240570992000000;
    ce.xyzt.y = 121816.466696035000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbt_s a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -204831.240570992000000;
    ce.xyzt.y = -121816.466696035000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbt_s a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000034080852317;
    ce.xyzt.y = 0.000014327146030;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbt_s a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000034080852317;
    ce.xyzt.y = -0.000014327146030;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbt_s a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000034080852317;
    ce.xyzt.y = 0.000014327146030;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbt_s a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000034080852317;
    ce.xyzt.y = -0.000014327146030;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbt_s a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=mbt_fps a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 198798.176129850010000;
    ce.xyzt.y = 125512.017254531010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbt_fps a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 198798.176129850010000;
    ce.xyzt.y = -125512.017254531010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbt_fps a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -198798.176129850010000;
    ce.xyzt.y = 125512.017254531010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbt_fps a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -198798.176129850010000;
    ce.xyzt.y = -125512.017254531010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbt_fps a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000035115518405;
    ce.xyzt.y = 0.000013905247590;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbt_fps a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000035115518405;
    ce.xyzt.y = -0.000013905247590;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbt_fps a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000035115518405;
    ce.xyzt.y = 0.000013905247590;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbt_fps a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000035115518405;
    ce.xyzt.y = -0.000013905247590;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbt_fps a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=mbtfpp a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 206804.786929819990000;
    ce.xyzt.y = 120649.762565793000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbtfpp a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 206804.786929819990000;
    ce.xyzt.y = -120649.762565793000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbtfpp a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -206804.786929819990000;
    ce.xyzt.y = 120649.762565793000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbtfpp a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -206804.786929819990000;
    ce.xyzt.y = -120649.762565793000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbtfpp a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000033753864882;
    ce.xyzt.y = 0.000014465934612;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbtfpp a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000033753864882;
    ce.xyzt.y = -0.000014465934612;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbtfpp a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000033753864882;
    ce.xyzt.y = 0.000014465934612;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbtfpp a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000033753864882;
    ce.xyzt.y = -0.000014465934612;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbtfpp a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=mbtfpq a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 209391.854738393010000;
    ce.xyzt.y = 119161.040199055000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbtfpq a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 209391.854738393010000;
    ce.xyzt.y = -119161.040199055000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbtfpq a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -209391.854738393010000;
    ce.xyzt.y = 119161.040199055000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbtfpq a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -209391.854738393010000;
    ce.xyzt.y = -119161.040199055000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbtfpq a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000033337638762;
    ce.xyzt.y = 0.000014646541283;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbtfpq a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000033337638762;
    ce.xyzt.y = -0.000014646541283;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbtfpq a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000033337638762;
    ce.xyzt.y = 0.000014646541283;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbtfpq a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000033337638762;
    ce.xyzt.y = -0.000014646541283;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbtfpq a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=mbtfps a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 204740.117478572010000;
    ce.xyzt.y = 121864.729719340000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbtfps a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 204740.117478572010000;
    ce.xyzt.y = -121864.729719340000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbtfps a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -204740.117478572010000;
    ce.xyzt.y = 121864.729719340000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbtfps a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -204740.117478572010000;
    ce.xyzt.y = -121864.729719340000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mbtfps a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000034093523408;
    ce.xyzt.y = 0.000014321822776;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbtfps a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000034093523408;
    ce.xyzt.y = -0.000014321822776;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbtfps a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000034093523408;
    ce.xyzt.y = 0.000014321822776;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbtfps a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000034093523408;
    ce.xyzt.y = -0.000014321822776;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mbtfps a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=merc ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222638.981586546990000;
    ce.xyzt.y = 110579.965218250000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=merc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222638.981586546990000;
    ce.xyzt.y = -110579.965218249000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=merc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222638.981586546990000;
    ce.xyzt.y = 110579.965218250000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=merc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222638.981586546990000;
    ce.xyzt.y = -110579.965218249000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=merc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=merc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=merc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=merc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=merc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=merc R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = 111706.743574944000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=merc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = -111706.743574944000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=merc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = 111706.743574944000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=merc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = -111706.743574944000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=merc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=merc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=merc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=merc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=merc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=mil_os R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1908527.949594205000000;
    ce.xyzt.y = -1726237.473061448000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mil_os R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1916673.022918485100000;
    ce.xyzt.y = -1943133.888125523000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mil_os R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2344429.412089623000000;
    ce.xyzt.y = -1706258.051218912000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mil_os R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2354637.835532999100000;
    ce.xyzt.y = -1926468.605135417100000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mil_os R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.349101392179034;
    ce.xyzt.y = 0.314176166202431;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mil_os R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.349101391795061;
    ce.xyzt.y = 0.314142364149008;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mil_os R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.349030308618698;
    ce.xyzt.y = 0.314176166202431;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mil_os R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.349030309002671;
    ce.xyzt.y = 0.314142364149008;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mil_os R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=mill a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = 111704.701754394000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mill a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = -111704.701754396000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mill a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = 111704.701754394000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mill a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = -111704.701754396000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=mill a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mill a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mill a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mill a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=mill a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=misrsom ellps=GRS80 +lat_1=0.5 +lat_2=2 +path=1");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 18556630.368369825000000;
    ce.xyzt.y = 9533394.675311271100000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=misrsom ellps=GRS80 +lat_1=0.5 +lat_2=2 +path=1", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 19041866.006729737000000;
    ce.xyzt.y = 9707182.175323523600000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=misrsom ellps=GRS80 +lat_1=0.5 +lat_2=2 +path=1", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 18816810.130184799000000;
    ce.xyzt.y = 8647669.649802954900000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=misrsom ellps=GRS80 +lat_1=0.5 +lat_2=2 +path=1", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 19252610.784536727000000;
    ce.xyzt.y = 8778164.085801404000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=misrsom ellps=GRS80 +lat_1=0.5 +lat_2=2 +path=1", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.229823995305426;
    ce.xyzt.y = 0.000030284080516;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=misrsom ellps=GRS80 +lat_1=0.5 +lat_2=2 +path=1", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.229855262582269;
    ce.xyzt.y = 0.000032671970185;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=misrsom ellps=GRS80 +lat_1=0.5 +lat_2=2 +path=1", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.229828738865987;
    ce.xyzt.y = -0.000032671970185;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=misrsom ellps=GRS80 +lat_1=0.5 +lat_2=2 +path=1", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.229860006142830;
    ce.xyzt.y = -0.000030284080516;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=misrsom ellps=GRS80 +lat_1=0.5 +lat_2=2 +path=1", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=misrsom R=6400000 +lat_1=0.5 +lat_2=2 +path=1");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 18641249.279170383000000;
    ce.xyzt.y = 9563342.532334165600000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=misrsom R=6400000 +lat_1=0.5 +lat_2=2 +path=1", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 19130982.461581279000000;
    ce.xyzt.y = 9739539.593504635600000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=misrsom R=6400000 +lat_1=0.5 +lat_2=2 +path=1", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 18903483.515011538000000;
    ce.xyzt.y = 8675064.500617975400000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=misrsom R=6400000 +lat_1=0.5 +lat_2=2 +path=1", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 19343388.399800610000000;
    ce.xyzt.y = 8807471.904068488600000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=misrsom R=6400000 +lat_1=0.5 +lat_2=2 +path=1", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.229824015551245;
    ce.xyzt.y = 0.000029953881675;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=misrsom R=6400000 +lat_1=0.5 +lat_2=2 +path=1", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.229855172750173;
    ce.xyzt.y = 0.000032360516180;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=misrsom R=6400000 +lat_1=0.5 +lat_2=2 +path=1", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.229828828698083;
    ce.xyzt.y = -0.000032360516180;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=misrsom R=6400000 +lat_1=0.5 +lat_2=2 +path=1", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.229859985879557;
    ce.xyzt.y = -0.000029953881675;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=misrsom R=6400000 +lat_1=0.5 +lat_2=2 +path=1", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=moll a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 201113.698641813010000;
    ce.xyzt.y = 124066.283433859990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=moll a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 201113.698641813010000;
    ce.xyzt.y = -124066.283433859990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=moll a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -201113.698641813010000;
    ce.xyzt.y = 124066.283433859990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=moll a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -201113.698641813010000;
    ce.xyzt.y = -124066.283433859990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=moll a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000034710026060;
    ce.xyzt.y = 0.000014067441038;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=moll a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000034710026060;
    ce.xyzt.y = -0.000014067441038;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=moll a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000034710026060;
    ce.xyzt.y = 0.000014067441038;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=moll a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000034710026060;
    ce.xyzt.y = -0.000014067441038;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=moll a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=murd1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222600.813473554010000;
    ce.xyzt.y = 111404.244180546000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222770.349287864000000;
    ce.xyzt.y = -111234.672856675000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222600.813473554010000;
    ce.xyzt.y = 111404.244180546000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222770.349287864000000;
    ce.xyzt.y = -111234.672856675000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031350563955;
    ce.xyzt.y = 0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031350546502;
    ce.xyzt.y = -0.000015678571923;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031350563955;
    ce.xyzt.y = 0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031350546502;
    ce.xyzt.y = -0.000015678571923;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=murd1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223363.845309492000000;
    ce.xyzt.y = 111786.116032863010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223533.962259251010000;
    ce.xyzt.y = -111615.963451823000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223363.845309492000000;
    ce.xyzt.y = 111786.116032863010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223533.962259251010000;
    ce.xyzt.y = -111615.963451823000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031243470553;
    ce.xyzt.y = 0.000015624990315;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031243453099;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031243470553;
    ce.xyzt.y = 0.000015624990315;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031243453099;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=murd2 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222588.099751230010000;
    ce.xyzt.y = 111426.140027412000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd2 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222757.726267018010000;
    ce.xyzt.y = -111341.431317505000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd2 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222588.099751230010000;
    ce.xyzt.y = 111426.140027412000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd2 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222757.726267018010000;
    ce.xyzt.y = -111341.431317505000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd2 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031352344191;
    ce.xyzt.y = 0.000015671084461;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd2 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031352326738;
    ce.xyzt.y = -0.000015671101914;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd2 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031352344191;
    ce.xyzt.y = 0.000015671084461;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd2 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031352326738;
    ce.xyzt.y = -0.000015671101914;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd2 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=murd2 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223351.088007026990000;
    ce.xyzt.y = 111808.086934388000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd2 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223521.295969169990000;
    ce.xyzt.y = -111723.087859673000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd2 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223351.088007026990000;
    ce.xyzt.y = 111808.086934388000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd2 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223521.295969169990000;
    ce.xyzt.y = -111723.087859673000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd2 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031245250788;
    ce.xyzt.y = 0.000015617555213;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd2 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031245233335;
    ce.xyzt.y = -0.000015617572666;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd2 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031245250788;
    ce.xyzt.y = 0.000015617555213;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd2 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031245233335;
    ce.xyzt.y = -0.000015617572666;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd2 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=murd3 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222600.814077577000000;
    ce.xyzt.y = 111404.246601372000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd3 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222770.354733899000000;
    ce.xyzt.y = -111234.670432178000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd3 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222600.814077577000000;
    ce.xyzt.y = 111404.246601372000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd3 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222770.354733899000000;
    ce.xyzt.y = -111234.670432178000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd3 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031350563955;
    ce.xyzt.y = 0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd3 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031350546502;
    ce.xyzt.y = -0.000015678571923;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd3 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031350563955;
    ce.xyzt.y = 0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd3 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031350546502;
    ce.xyzt.y = -0.000015678571923;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd3 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=murd3 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223363.845915585000000;
    ce.xyzt.y = 111786.118461987000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd3 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223533.967723953010000;
    ce.xyzt.y = -111615.961019015000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd3 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223363.845915585000000;
    ce.xyzt.y = 111786.118461987000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd3 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223533.967723953010000;
    ce.xyzt.y = -111615.961019015000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=murd3 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031243470553;
    ce.xyzt.y = 0.000015624990315;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd3 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031243453099;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd3 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031243470553;
    ce.xyzt.y = 0.000015624990315;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd3 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031243453099;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=murd3 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=natearth a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 194507.265257889000000;
    ce.xyzt.y = 112508.737358294990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=natearth a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 194507.265257889000000;
    ce.xyzt.y = -112508.737358294990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=natearth a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -194507.265257889000000;
    ce.xyzt.y = 112508.737358294990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=natearth a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -194507.265257889000000;
    ce.xyzt.y = -112508.737358294990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=natearth a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000035890654032;
    ce.xyzt.y = 0.000015512905271;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=natearth a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000035890654032;
    ce.xyzt.y = -0.000015512905271;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=natearth a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000035890654032;
    ce.xyzt.y = 0.000015512905271;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=natearth a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000035890654032;
    ce.xyzt.y = -0.000015512905271;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=natearth a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=natearth2 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 189255.172934731000000;
    ce.xyzt.y = 113022.495810907000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=natearth2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 189255.172934731000000;
    ce.xyzt.y = -113022.495810907000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=natearth2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -189255.172934731000000;
    ce.xyzt.y = 113022.495810907000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=natearth2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -189255.172934731000000;
    ce.xyzt.y = -113022.495810907000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=natearth2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000036886643623;
    ce.xyzt.y = 0.000015442324156;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=natearth2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000036886643623;
    ce.xyzt.y = -0.000015442324156;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=natearth2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000036886643623;
    ce.xyzt.y = 0.000015442324156;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=natearth2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000036886643623;
    ce.xyzt.y = -0.000015442324156;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=natearth2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=nell a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223385.132504696000000;
    ce.xyzt.y = 111698.236447187000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nell a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223385.132504696000000;
    ce.xyzt.y = -111698.236447187000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nell a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223385.132504696000000;
    ce.xyzt.y = 111698.236447187000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nell a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223385.132504696000000;
    ce.xyzt.y = -111698.236447187000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nell a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=nell a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=nell a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=nell a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=nell a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=nell_h a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223385.131640953010000;
    ce.xyzt.y = 111698.236533562000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nell_h a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223385.131640953010000;
    ce.xyzt.y = -111698.236533562000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nell_h a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223385.131640953010000;
    ce.xyzt.y = 111698.236533562000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nell_h a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223385.131640953010000;
    ce.xyzt.y = -111698.236533562000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nell_h a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=nell_h a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=nell_h a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=nell_h a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=nell_h a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=nicol a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223374.561814140000000;
    ce.xyzt.y = 111732.553988545000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nicol a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223374.561814140000000;
    ce.xyzt.y = -111732.553988545000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nicol a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223374.561814140000000;
    ce.xyzt.y = 111732.553988545000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nicol a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223374.561814140000000;
    ce.xyzt.y = -111732.553988545000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nicol a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=nsper a=6400000 +h=1000000");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222239.816114099990000;
    ce.xyzt.y = 111153.763991925010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nsper a=6400000 +h=1000000", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222239.816114099990000;
    ce.xyzt.y = -111153.763991925010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nsper a=6400000 +h=1000000", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222239.816114099990000;
    ce.xyzt.y = 111153.763991925010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nsper a=6400000 +h=1000000", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222239.816114099990000;
    ce.xyzt.y = -111153.763991925010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nsper a=6400000 +h=1000000", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=nsper a=6400000 +h=1000000", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=nsper a=6400000 +h=1000000", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=nsper a=6400000 +h=1000000", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=nsper a=6400000 +h=1000000", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=nzmg ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000010000000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 3352675144.747424600000000;
    ce.xyzt.y = -7043205391.100244500000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nzmg ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 3691989502.779305500000000;
    ce.xyzt.y = -6729069415.332104700000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nzmg ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 4099000768.453239000000000;
    ce.xyzt.y = -7863208779.667248700000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nzmg ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 4466166927.369976000000000;
    ce.xyzt.y = -7502531736.628606800000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=nzmg ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200000.000000000000000;
    ci.xyzt.y = 100000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 3.062740193401719;
    ce.xyzt.y = -1.211654554191880;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=nzmg ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200000.000000000000000;
    ci.xyzt.y = -100000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 3.067535182637135;
    ce.xyzt.y = -1.213589756155136;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=nzmg ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200000.000000000000000;
    ci.xyzt.y = 100000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.349302520655395;
    ce.xyzt.y = -1.072679283418543;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=nzmg ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200000.000000000000000;
    ci.xyzt.y = -100000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.344565087648867;
    ce.xyzt.y = -1.075499001840615;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=nzmg ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=ob_tran +R=6400000 +o_proj=latlon +o_lon_p=20 +o_lat_p=20 +lon_0=180");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.685687214000000;
    ce.xyzt.y = 1.237430235000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ob_tran +R=6400000 +o_proj=latlon +o_lon_p=20 +o_lat_p=20 +lon_0=180", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.695406975000000;
    ce.xyzt.y = 1.202683395000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ob_tran +R=6400000 +o_proj=latlon +o_lon_p=20 +o_lat_p=20 +lon_0=180", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.899366393000000;
    ce.xyzt.y = 1.237430235000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ob_tran +R=6400000 +o_proj=latlon +o_lon_p=20 +o_lat_p=20 +lon_0=180", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.889646631000000;
    ce.xyzt.y = 1.202683395000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ob_tran +R=6400000 +o_proj=latlon +o_lon_p=20 +o_lat_p=20 +lon_0=180", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.121480427947509;
    ce.xyzt.y = -0.044263297899995;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ob_tran +R=6400000 +o_proj=latlon +o_lon_p=20 +o_lat_p=20 +lon_0=180", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1.104115955444076;
    ce.xyzt.y = 0.306921726668973;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ob_tran +R=6400000 +o_proj=latlon +o_lon_p=20 +o_lat_p=20 +lon_0=180", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2.462672371736954;
    ce.xyzt.y = 0.455386287205369;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ob_tran +R=6400000 +o_proj=latlon +o_lon_p=20 +o_lat_p=20 +lon_0=180", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.149515481920648;
    ce.xyzt.y = 0.904609301391311;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ob_tran +R=6400000 +o_proj=latlon +o_lon_p=20 +o_lat_p=20 +lon_0=180", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=ocea a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 19994423.837934088000000;
    ce.xyzt.y = 223322.760576728000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ocea a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 20217962.128015257000000;
    ce.xyzt.y = 223322.760576728990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ocea a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 19994423.837934088000000;
    ce.xyzt.y = -223322.760576727010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ocea a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 20217962.128015265000000;
    ce.xyzt.y = -223322.760576725010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ocea a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 3.141577028582025;
    ce.xyzt.y = 0.000031249998084;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ocea a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -3.141577028582025;
    ce.xyzt.y = 0.000031249998084;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ocea a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 3.141577028582025;
    ce.xyzt.y = -0.000031249998084;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ocea a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -3.141577028582025;
    ce.xyzt.y = -0.000031249998084;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ocea a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=oea a=6400000 +lat_1=0.5 +lat_2=2 +n=1 m=2 +theta=3");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 228926.872097863990000;
    ce.xyzt.y = 99870.488430076002000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=oea a=6400000 +lat_1=0.5 +lat_2=2 +n=1 m=2 +theta=3", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 217242.584036940010000;
    ce.xyzt.y = -123247.885607474990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=oea a=6400000 +lat_1=0.5 +lat_2=2 +n=1 m=2 +theta=3", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -217242.584036940010000;
    ce.xyzt.y = 123247.885607474990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=oea a=6400000 +lat_1=0.5 +lat_2=2 +n=1 m=2 +theta=3", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -228926.872097863990000;
    ce.xyzt.y = -99870.488430076002000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=oea a=6400000 +lat_1=0.5 +lat_2=2 +n=1 m=2 +theta=3", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000030389428590;
    ce.xyzt.y = 0.000017239088261;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=oea a=6400000 +lat_1=0.5 +lat_2=2 +n=1 m=2 +theta=3", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000032024924272;
    ce.xyzt.y = -0.000013968079443;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=oea a=6400000 +lat_1=0.5 +lat_2=2 +n=1 m=2 +theta=3", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000032024924272;
    ce.xyzt.y = 0.000013968079443;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=oea a=6400000 +lat_1=0.5 +lat_2=2 +n=1 m=2 +theta=3", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000030389428590;
    ce.xyzt.y = -0.000017239088261;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=oea a=6400000 +lat_1=0.5 +lat_2=2 +n=1 m=2 +theta=3", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=omerc ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222650.796885260990000;
    ce.xyzt.y = 110642.229314984000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=omerc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222650.796885260990000;
    ce.xyzt.y = -110642.229314984000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=omerc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222650.796885262010000;
    ce.xyzt.y = 110642.229314984000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=omerc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222650.796885262010000;
    ce.xyzt.y = -110642.229314984000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=omerc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=omerc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=omerc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=omerc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=omerc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=ortel a=6400000 +lat_1=0 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223374.577355252990000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ortel a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223374.577355252990000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ortel a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223374.577355252990000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ortel a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223374.577355252990000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ortel a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=ortho a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223322.760576727010000;
    ce.xyzt.y = 111695.401198614000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ortho a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223322.760576727010000;
    ce.xyzt.y = -111695.401198614000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ortho a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223322.760576727010000;
    ce.xyzt.y = 111695.401198614000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ortho a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223322.760576727010000;
    ce.xyzt.y = -111695.401198614000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ortho a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ortho a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ortho a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ortho a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ortho a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=pconic ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222588.098841617000000;
    ce.xyzt.y = 111416.604770067000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=pconic ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222757.718091090010000;
    ce.xyzt.y = -111331.881531080000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=pconic ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222588.098841617000000;
    ce.xyzt.y = 111416.604770067000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=pconic ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222757.718091090010000;
    ce.xyzt.y = -111331.881531080000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=pconic ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031352361645;
    ce.xyzt.y = 0.000015672428364;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=pconic ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031352326738;
    ce.xyzt.y = -0.000015672445818;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=pconic ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031352361645;
    ce.xyzt.y = 0.000015672428364;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=pconic ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031352326738;
    ce.xyzt.y = -0.000015672445818;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=pconic ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=pconic a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223351.087094295010000;
    ce.xyzt.y = 111798.518992055000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=pconic a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223521.287765217010000;
    ce.xyzt.y = -111713.505338457010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=pconic a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223351.087094295010000;
    ce.xyzt.y = 111798.518992055000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=pconic a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223521.287765217010000;
    ce.xyzt.y = -111713.505338457010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=pconic a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031245250788;
    ce.xyzt.y = 0.000015618899116;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=pconic a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031245233335;
    ce.xyzt.y = -0.000015618899116;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=pconic a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031245250788;
    ce.xyzt.y = 0.000015618899116;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=pconic a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031245233335;
    ce.xyzt.y = -0.000015618899116;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=pconic a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=patterson a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = 113354.250397779990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=patterson a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = -113354.250397779990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=patterson a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = 113354.250397779990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=patterson a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = -113354.250397779990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=patterson a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015397120128;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=patterson a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015397120128;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=patterson a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015397120128;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=patterson a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015397120128;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=patterson a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=poly ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222605.285770237010000;
    ce.xyzt.y = 110642.194561440000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=poly ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222605.285770237010000;
    ce.xyzt.y = -110642.194561440000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=poly ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222605.285770237010000;
    ce.xyzt.y = 110642.194561440000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=poly ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222605.285770237010000;
    ce.xyzt.y = -110642.194561440000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=poly ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=poly ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=poly ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=poly ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=poly ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=poly R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223368.105210218990000;
    ce.xyzt.y = 111769.110491225000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=poly R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223368.105210218990000;
    ce.xyzt.y = -111769.110491225000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=poly R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223368.105210218990000;
    ce.xyzt.y = 111769.110491225000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=poly R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223368.105210218990000;
    ce.xyzt.y = -111769.110491225000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=poly R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=poly R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=poly R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=poly R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=poly R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=putp1 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 211642.762754160010000;
    ce.xyzt.y = 105831.180787330000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 211642.762754160010000;
    ce.xyzt.y = -105831.180787330000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -211642.762754160010000;
    ce.xyzt.y = 105831.180787330000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -211642.762754160010000;
    ce.xyzt.y = -105831.180787330000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000032983267111;
    ce.xyzt.y = 0.000016491633555;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000032983267111;
    ce.xyzt.y = -0.000016491633555;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000032983267111;
    ce.xyzt.y = 0.000016491633555;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000032983267111;
    ce.xyzt.y = -0.000016491633555;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=putp2 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 211638.039634338990000;
    ce.xyzt.y = 117895.033043380000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 211638.039634338990000;
    ce.xyzt.y = -117895.033043380000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -211638.039634338990000;
    ce.xyzt.y = 117895.033043380000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -211638.039634338990000;
    ce.xyzt.y = -117895.033043380000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000032983267111;
    ce.xyzt.y = 0.000014803917622;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000032983267111;
    ce.xyzt.y = -0.000014803917622;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000032983267111;
    ce.xyzt.y = 0.000014803917622;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000032983267111;
    ce.xyzt.y = -0.000014803917622;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=putp3 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 178227.115507793990000;
    ce.xyzt.y = 89124.560786087997000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 178227.115507793990000;
    ce.xyzt.y = -89124.560786087997000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -178227.115507793990000;
    ce.xyzt.y = 89124.560786087997000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -178227.115507793990000;
    ce.xyzt.y = -89124.560786087997000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000039166061079;
    ce.xyzt.y = 0.000019583030540;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000039166061079;
    ce.xyzt.y = -0.000019583030540;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000039166061079;
    ce.xyzt.y = 0.000019583030540;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000039166061079;
    ce.xyzt.y = -0.000019583030540;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=putp3p a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 178238.118539985010000;
    ce.xyzt.y = 89124.560786087997000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp3p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 178238.118539985010000;
    ce.xyzt.y = -89124.560786087997000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp3p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -178238.118539985010000;
    ce.xyzt.y = 89124.560786087997000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp3p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -178238.118539985010000;
    ce.xyzt.y = -89124.560786087997000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp3p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000039166061079;
    ce.xyzt.y = 0.000019583030540;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp3p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000039166061079;
    ce.xyzt.y = -0.000019583030540;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp3p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000039166061079;
    ce.xyzt.y = 0.000019583030540;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp3p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000039166061079;
    ce.xyzt.y = -0.000019583030540;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp3p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=putp4p a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 195241.477349386000000;
    ce.xyzt.y = 127796.782307926000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp4p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 195241.477349386000000;
    ce.xyzt.y = -127796.782307926000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp4p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -195241.477349386000000;
    ce.xyzt.y = 127796.782307926000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp4p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -195241.477349386000000;
    ce.xyzt.y = -127796.782307926000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp4p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000035753558419;
    ce.xyzt.y = 0.000013656852331;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp4p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000035753558419;
    ce.xyzt.y = -0.000013656852331;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp4p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000035753558419;
    ce.xyzt.y = 0.000013656852331;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp4p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000035753558419;
    ce.xyzt.y = -0.000013656852331;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp4p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=putp5 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 226367.213380562000000;
    ce.xyzt.y = 113204.568558475000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 226367.213380562000000;
    ce.xyzt.y = -113204.568558475000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -226367.213380562000000;
    ce.xyzt.y = 113204.568558475000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -226367.213380562000000;
    ce.xyzt.y = -113204.568558475000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000030834958788;
    ce.xyzt.y = 0.000015417488121;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000030834958788;
    ce.xyzt.y = -0.000015417488121;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000030834958788;
    ce.xyzt.y = 0.000015417488121;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000030834958788;
    ce.xyzt.y = -0.000015417488121;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=putp5p a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 226388.175248755990000;
    ce.xyzt.y = 113204.568558475000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp5p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 226388.175248755990000;
    ce.xyzt.y = -113204.568558475000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp5p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -226388.175248755990000;
    ce.xyzt.y = 113204.568558475000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp5p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -226388.175248755990000;
    ce.xyzt.y = -113204.568558475000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp5p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000030834958788;
    ce.xyzt.y = 0.000015417488121;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp5p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000030834958788;
    ce.xyzt.y = -0.000015417488121;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp5p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000030834958788;
    ce.xyzt.y = 0.000015417488121;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp5p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000030834958788;
    ce.xyzt.y = -0.000015417488121;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp5p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=putp6 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 226369.395133403010000;
    ce.xyzt.y = 110218.523796521000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 226369.395133403010000;
    ce.xyzt.y = -110218.523796521000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -226369.395133403010000;
    ce.xyzt.y = 110218.523796521000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -226369.395133403010000;
    ce.xyzt.y = -110218.523796521000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000030834958788;
    ce.xyzt.y = 0.000015835302490;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000030834958788;
    ce.xyzt.y = -0.000015835302490;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000030834958788;
    ce.xyzt.y = 0.000015835302490;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000030834958788;
    ce.xyzt.y = -0.000015835302490;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=putp6p a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 198034.195132194990000;
    ce.xyzt.y = 125989.475461323000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp6p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 198034.195132194990000;
    ce.xyzt.y = -125989.475461323000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp6p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -198034.195132194990000;
    ce.xyzt.y = 125989.475461323000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp6p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -198034.195132194990000;
    ce.xyzt.y = -125989.475461323000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=putp6p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000035247814362;
    ce.xyzt.y = 0.000013852957526;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp6p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000035247814362;
    ce.xyzt.y = -0.000013852957526;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp6p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000035247814362;
    ce.xyzt.y = 0.000013852957526;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp6p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000035247814362;
    ce.xyzt.y = -0.000013852957526;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=putp6p a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=qua_aut ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222613.549033096990000;
    ce.xyzt.y = 111318.077887984010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=qua_aut ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222613.549033096990000;
    ce.xyzt.y = -111318.077887984010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=qua_aut ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222613.549033096990000;
    ce.xyzt.y = 111318.077887984010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=qua_aut ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222613.549033096990000;
    ce.xyzt.y = -111318.077887984010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=qua_aut ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=qua_aut ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=qua_aut ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=qua_aut ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=qua_aut ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=qua_aut a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223376.624524029000000;
    ce.xyzt.y = 111699.654379186000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=qua_aut a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223376.624524029000000;
    ce.xyzt.y = -111699.654379186000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=qua_aut a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223376.624524029000000;
    ce.xyzt.y = 111699.654379186000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=qua_aut a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223376.624524029000000;
    ce.xyzt.y = -111699.654379186000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=qua_aut a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=qua_aut a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=qua_aut a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=qua_aut a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=qua_aut a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=qsc ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 304638.450843852010000;
    ce.xyzt.y = 164123.870923793990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=qsc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 304638.450843852010000;
    ce.xyzt.y = -164123.870923793990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=qsc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -304638.450843852010000;
    ce.xyzt.y = 164123.870923793990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=qsc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -304638.450843852010000;
    ce.xyzt.y = -164123.870923793990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=qsc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000023061750992;
    ce.xyzt.y = 0.000010657905437;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=qsc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000023061750992;
    ce.xyzt.y = -0.000010657905437;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=qsc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000023061750992;
    ce.xyzt.y = 0.000010657905437;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=qsc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000023061750992;
    ce.xyzt.y = -0.000010657905437;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=qsc ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=qsc R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 305863.792402890980000;
    ce.xyzt.y = 165827.722754715010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=qsc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 305863.792402890980000;
    ce.xyzt.y = -165827.722754715010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=qsc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -305863.792402890980000;
    ce.xyzt.y = 165827.722754715010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=qsc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -305863.792402890980000;
    ce.xyzt.y = -165827.722754715010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=qsc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000022982966829;
    ce.xyzt.y = 0.000010550393155;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=qsc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000022982966829;
    ce.xyzt.y = -0.000010550393155;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=qsc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000022982966829;
    ce.xyzt.y = 0.000010550393155;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=qsc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000022982966829;
    ce.xyzt.y = -0.000010550393155;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=qsc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=robin a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 189588.423282508010000;
    ce.xyzt.y = 107318.530350703000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=robin a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 189588.423282508010000;
    ce.xyzt.y = -107318.530350703000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=robin a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -189588.423282508010000;
    ce.xyzt.y = 107318.530350703000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=robin a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -189588.423282508010000;
    ce.xyzt.y = -107318.530350703000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=robin a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000036821019243;
    ce.xyzt.y = 0.000016263082690;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=robin a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000036821019243;
    ce.xyzt.y = -0.000016263082690;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=robin a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000036821019243;
    ce.xyzt.y = 0.000016263082690;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=robin a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000036821019243;
    ce.xyzt.y = -0.000016263082690;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=robin a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=rouss ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222644.894131617010000;
    ce.xyzt.y = 110611.091868370000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=rouss ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222644.894131617010000;
    ce.xyzt.y = -110611.091868370000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=rouss ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222644.894131617010000;
    ce.xyzt.y = 110611.091868370000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=rouss ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222644.894131617010000;
    ce.xyzt.y = -110611.091868370000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=rouss ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=rouss ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=rouss ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=rouss ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=rouss ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=rpoly a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223368.098302014000000;
    ce.xyzt.y = 111769.110486991010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=rpoly a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223368.098302014000000;
    ce.xyzt.y = -111769.110486991010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=rpoly a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223368.098302014000000;
    ce.xyzt.y = 111769.110486991010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=rpoly a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223368.098302014000000;
    ce.xyzt.y = -111769.110486991010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=rpoly a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=sinu ellps=GRS80 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222605.299539466010000;
    ce.xyzt.y = 110574.388554153000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=sinu ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222605.299539466010000;
    ce.xyzt.y = -110574.388554153000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=sinu ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222605.299539466010000;
    ce.xyzt.y = 110574.388554153000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=sinu ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222605.299539466010000;
    ce.xyzt.y = -110574.388554153000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=sinu ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=sinu ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=sinu ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=sinu ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=sinu ellps=GRS80 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=sinu R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223368.119026631990000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=sinu R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223368.119026631990000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=sinu R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223368.119026631990000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=sinu R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223368.119026631990000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=sinu R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=sinu R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=sinu R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=sinu R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=sinu R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=somerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222638.981586546990000;
    ce.xyzt.y = 110579.965218249000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=somerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222638.981586546990000;
    ce.xyzt.y = -110579.965218251000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=somerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222638.981586546990000;
    ce.xyzt.y = 110579.965218249000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=somerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222638.981586546990000;
    ce.xyzt.y = -110579.965218251000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=somerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=somerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=somerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=somerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=somerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=somerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = 111706.743574944000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=somerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223402.144255274000000;
    ce.xyzt.y = -111706.743574944990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=somerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = 111706.743574944000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=somerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223402.144255274000000;
    ce.xyzt.y = -111706.743574944990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=somerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=somerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=somerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=somerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=somerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=stere ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222644.854550116990000;
    ce.xyzt.y = 110610.883474174000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=stere ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222644.854550116990000;
    ce.xyzt.y = -110610.883474174000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=stere ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222644.854550116990000;
    ce.xyzt.y = 110610.883474174000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=stere ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222644.854550116990000;
    ce.xyzt.y = -110610.883474174000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=stere ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=stere ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=stere ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=stere ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=stere ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=stere R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223407.810259507010000;
    ce.xyzt.y = 111737.938996443000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=stere R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223407.810259507010000;
    ce.xyzt.y = -111737.938996443000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=stere R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223407.810259507010000;
    ce.xyzt.y = 111737.938996443000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=stere R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223407.810259507010000;
    ce.xyzt.y = -111737.938996443000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=stere R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=stere R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=stere R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=stere R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=stere R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=sterea ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222644.894109198010000;
    ce.xyzt.y = 110611.091871736990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=sterea ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222644.894109198010000;
    ce.xyzt.y = -110611.091871738000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=sterea ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222644.894109198010000;
    ce.xyzt.y = 110611.091871736990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=sterea ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222644.894109198010000;
    ce.xyzt.y = -110611.091871738000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=sterea ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=sterea ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=sterea ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=sterea ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=sterea ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=sterea R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223407.810259507010000;
    ce.xyzt.y = 111737.938996443000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=sterea R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223407.810259507010000;
    ce.xyzt.y = -111737.938996443000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=sterea R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223407.810259507010000;
    ce.xyzt.y = 111737.938996443000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=sterea R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223407.810259507010000;
    ce.xyzt.y = -111737.938996443000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=sterea R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=sterea R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=sterea R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=sterea R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=sterea R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=gstmerc R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223413.466406322000000;
    ce.xyzt.y = 111769.145040585990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gstmerc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223413.466406322000000;
    ce.xyzt.y = -111769.145040587000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gstmerc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223413.466406322990000;
    ce.xyzt.y = 111769.145040585990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gstmerc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223413.466406322990000;
    ce.xyzt.y = -111769.145040587000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=gstmerc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gstmerc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gstmerc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gstmerc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=gstmerc R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=tcc a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223458.844192458000000;
    ce.xyzt.y = 111769.145040585990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tcc a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223458.844192458000000;
    ce.xyzt.y = -111769.145040585990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tcc a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223458.844192458000000;
    ce.xyzt.y = 111769.145040585990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tcc a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223458.844192458000000;
    ce.xyzt.y = -111769.145040585990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tcc a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=tcea a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223322.760576727010000;
    ce.xyzt.y = 111769.145040585990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tcea a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223322.760576727010000;
    ce.xyzt.y = -111769.145040585990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tcea a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223322.760576727010000;
    ce.xyzt.y = 111769.145040585990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tcea a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223322.760576727010000;
    ce.xyzt.y = -111769.145040585990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tcea a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tcea a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tcea a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tcea a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tcea a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=times +ellps=sphere");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.000000000000000;
    ci.xyzt.y = 0.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000000000000000;
    ce.xyzt.y = 0.000000000000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=times +ellps=sphere", ci, ce, co);

    ci.xyzt.x = 1.396263401595464;
    ci.xyzt.y = 1.221730476396031;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 5785183.576067095600000;
    ce.xyzt.y = 7615452.066120422400000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=times +ellps=sphere", ci, ce, co);

    ci.xyzt.x = 0.436332312998582;
    ci.xyzt.y = -0.174532925199433;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2065971.530107880900000;
    ce.xyzt.y = -951526.064849458980000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=times +ellps=sphere", ci, ce, co);

    ci.xyzt.x = -0.610865238198015;
    ci.xyzt.y = 0.349065850398866;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -2873054.045485095100000;
    ce.xyzt.y = 1917730.953000521100000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=times +ellps=sphere", ci, ce, co);

    ci.xyzt.x = -0.785398163397448;
    ci.xyzt.y = -0.523598775598299;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -3651383.203521486800000;
    ce.xyzt.y = -2914213.457815920900000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=times +ellps=sphere", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 0.000000000000000;
    ci.xyzt.y = 0.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000000000000000;
    ce.xyzt.y = 0.000000000000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=times +ellps=sphere", ci, ce, co);

    ci.xyzt.x = 5785183.576067095600000;
    ci.xyzt.y = 7615452.066120422400000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1.396263401595464;
    ce.xyzt.y = 1.221730476396031;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=times +ellps=sphere", ci, ce, co);

    ci.xyzt.x = 2065971.530107880900000;
    ci.xyzt.y = -951526.064849458980000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.436332312998582;
    ce.xyzt.y = -0.174532925199433;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=times +ellps=sphere", ci, ce, co);

    ci.xyzt.x = -2873054.045485095100000;
    ci.xyzt.y = 1917730.953000521100000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.610865238198015;
    ce.xyzt.y = 0.349065850398866;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=times +ellps=sphere", ci, ce, co);

    ci.xyzt.x = -3651383.203521486800000;
    ci.xyzt.y = -2914213.457815920900000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.785398163397448;
    ce.xyzt.y = -0.523598775598299;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=times +ellps=sphere", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=tissot ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222641.078699631010000;
    ce.xyzt.y = 54347.828487281004000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tissot ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222810.614513940990000;
    ce.xyzt.y = -168291.088549939010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tissot ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222641.078699631010000;
    ce.xyzt.y = 54347.828487281004000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tissot ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222810.614513940990000;
    ce.xyzt.y = -168291.088549939010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tissot ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031351017741;
    ce.xyzt.y = 0.008961304992504;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tissot ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031350982834;
    ce.xyzt.y = 0.008929947883564;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tissot ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031351017741;
    ce.xyzt.y = 0.008961304992504;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tissot ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031350982834;
    ce.xyzt.y = 0.008929947883564;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tissot ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=tissot a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223404.248556848990000;
    ce.xyzt.y = 54534.122161157997000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tissot a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223574.365506608010000;
    ce.xyzt.y = -168867.957323528010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tissot a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223404.248556848990000;
    ce.xyzt.y = 54534.122161157997000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tissot a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223574.365506608010000;
    ce.xyzt.y = -168867.957323528010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tissot a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031243906885;
    ce.xyzt.y = 0.008961251428349;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tissot a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031243889432;
    ce.xyzt.y = 0.008930001430265;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tissot a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031243906885;
    ce.xyzt.y = 0.008961251428349;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tissot a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031243889432;
    ce.xyzt.y = 0.008930001430265;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tissot a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=tmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222650.796795778010000;
    ce.xyzt.y = 110642.229411926990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222650.796795778010000;
    ce.xyzt.y = -110642.229411926990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222650.796795778010000;
    ce.xyzt.y = 110642.229411926990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222650.796795778010000;
    ce.xyzt.y = -110642.229411926990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = 0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031357126393;
    ce.xyzt.y = -0.000015784216703;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tmerc ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=tmerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223413.466406322000000;
    ce.xyzt.y = 111769.145040597000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tmerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223413.466406322000000;
    ce.xyzt.y = -111769.145040597000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tmerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223413.466406322000000;
    ce.xyzt.y = 111769.145040597000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tmerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223413.466406322000000;
    ce.xyzt.y = -111769.145040597000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tmerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tmerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tmerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tmerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tmerc R=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=tpeqd ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -27750.758831678999000;
    ce.xyzt.y = -222599.403691777000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tpeqd ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -250434.937024035990000;
    ce.xyzt.y = -222655.938193265990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tpeqd ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -27750.758831678999000;
    ce.xyzt.y = 222599.403691777000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tpeqd ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -250434.937024035990000;
    ce.xyzt.y = 222655.938193265990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tpeqd ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000015682760714;
    ce.xyzt.y = 0.021847972758869;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tpeqd ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000015682760714;
    ce.xyzt.y = 0.021847972758869;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tpeqd ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000015682725807;
    ce.xyzt.y = 0.021785258523536;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tpeqd ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000015682725807;
    ce.xyzt.y = 0.021785258523536;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tpeqd ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=tpeqd a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -27845.882978484999000;
    ce.xyzt.y = -223362.430695260000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tpeqd a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -251293.378764650990000;
    ce.xyzt.y = -223419.158985908000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tpeqd a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -27845.882978484999000;
    ce.xyzt.y = 223362.430695260000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tpeqd a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -251293.378764650990000;
    ce.xyzt.y = 223419.158985908000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tpeqd a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000015629179106;
    ce.xyzt.y = 0.021847865648013;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tpeqd a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000015629179106;
    ce.xyzt.y = 0.021847865648013;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tpeqd a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000015629161652;
    ce.xyzt.y = 0.021785365651845;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tpeqd a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000015629161652;
    ce.xyzt.y = 0.021785365651845;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tpeqd a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=tpers a=6400000 +h=1000000 +azi=20");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 170820.288955531000000;
    ce.xyzt.y = 180460.865555805010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tpers a=6400000 +h=1000000 +azi=20", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 246853.941538941990000;
    ce.xyzt.y = -28439.878035775000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tpers a=6400000 +h=1000000 +azi=20", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -246853.941538941990000;
    ce.xyzt.y = 28439.878035775000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tpers a=6400000 +h=1000000 +azi=20", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -170820.288955531000000;
    ce.xyzt.y = -180460.865555805010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=tpers a=6400000 +h=1000000 +azi=20", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000034709467554;
    ce.xyzt.y = 0.000003994569966;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tpers a=6400000 +h=1000000 +azi=20", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000024021333014;
    ce.xyzt.y = -0.000025370821592;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tpers a=6400000 +h=1000000 +azi=20", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000024021333014;
    ce.xyzt.y = 0.000025370821592;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tpers a=6400000 +h=1000000 +azi=20", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000034709467554;
    ce.xyzt.y = -0.000003994569966;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=tpers a=6400000 +h=1000000 +azi=20", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=ups ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2433455.563438467200000;
    ce.xyzt.y = -10412543.301512826000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ups ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2448749.118568199200000;
    ce.xyzt.y = -10850493.419804076000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ups ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1566544.436561533000000;
    ce.xyzt.y = -10412543.301512826000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ups ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1551250.881431801000000;
    ce.xyzt.y = -10850493.419804076000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ups ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.785373161521007;
    ce.xyzt.y = 1.133036967795808;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ups ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.785323161520582;
    ce.xyzt.y = 1.133015743963786;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ups ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.785473161521857;
    ce.xyzt.y = 1.132994521283680;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ups ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.785423161521432;
    ce.xyzt.y = 1.132973299772945;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ups ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=urm5 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223393.638433963990000;
    ce.xyzt.y = 111696.818785116990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=urm5 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223393.638433963990000;
    ce.xyzt.y = -111696.818785116990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=urm5 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223393.638433963990000;
    ce.xyzt.y = 111696.818785116990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=urm5 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223393.638433963990000;
    ce.xyzt.y = -111696.818785116990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=urm5 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=urmfps a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 196001.708134192010000;
    ce.xyzt.y = 127306.843329993000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=urmfps a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 196001.708134192010000;
    ce.xyzt.y = -127306.843329993000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=urmfps a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -196001.708134192010000;
    ce.xyzt.y = 127306.843329993000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=urmfps a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -196001.708134192010000;
    ce.xyzt.y = -127306.843329993000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=urmfps a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000035617300565;
    ce.xyzt.y = 0.000013709107489;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=urmfps a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000035617300565;
    ce.xyzt.y = -0.000013709107489;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=urmfps a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000035617300565;
    ce.xyzt.y = 0.000013709107489;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=urmfps a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000035617300565;
    ce.xyzt.y = -0.000013709107489;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=urmfps a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=utm ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1057002.405491298100000;
    ce.xyzt.y = 110955.141175949000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=utm ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1057002.405491298100000;
    ce.xyzt.y = -110955.141175949000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=utm ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 611263.812278904950000;
    ce.xyzt.y = 110547.105696804010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=utm ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 611263.812278904950000;
    ce.xyzt.y = -110547.105696804010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=utm ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.130671964787398;
    ce.xyzt.y = 0.000015741822655;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=utm ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.130671964787398;
    ce.xyzt.y = -0.000015741822655;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=utm ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.130734510389019;
    ce.xyzt.y = 0.000015741735389;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=utm ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.130734510389019;
    ce.xyzt.y = -0.000015741735389;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=utm ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5 +zone=30", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=vandg a=6400000 +lat_1=0.5 +lat_2=2 +no_defs");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223395.249543407000000;
    ce.xyzt.y = 111704.596633675000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vandg a=6400000 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223395.249543407000000;
    ce.xyzt.y = -111704.596633675000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vandg a=6400000 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223395.249543407000000;
    ce.xyzt.y = 111704.596633675000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vandg a=6400000 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223395.249543407000000;
    ce.xyzt.y = -111704.596633675000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vandg a=6400000 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031250015537;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=vandg a=6400000 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031250015537;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=vandg a=6400000 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031250015537;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=vandg a=6400000 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031250015537;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=vandg a=6400000 +lat_1=0.5 +lat_2=2 +no_defs", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=vandg2 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223395.247850437010000;
    ce.xyzt.y = 111718.491037226010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vandg2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223395.247850437010000;
    ce.xyzt.y = -111718.491037226010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vandg2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223395.247850437010000;
    ce.xyzt.y = 111718.491037226010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vandg2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223395.247850437010000;
    ce.xyzt.y = -111718.491037226010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vandg2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=vandg3 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223395.249552831000000;
    ce.xyzt.y = 111704.519904421000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vandg3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223395.249552831000000;
    ce.xyzt.y = -111704.519904421000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vandg3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223395.249552831000000;
    ce.xyzt.y = 111704.519904421000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vandg3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223395.249552831000000;
    ce.xyzt.y = -111704.519904421000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vandg3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=vandg4 R=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000010000000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223374.577294355000000;
    ce.xyzt.y = 111701.195484154000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vandg4 R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223374.577294355000000;
    ce.xyzt.y = -111701.195484154000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vandg4 R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223374.577294355000000;
    ce.xyzt.y = 111701.195484154000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vandg4 R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223374.577294355000000;
    ce.xyzt.y = -111701.195484154000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vandg4 R=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=vitk1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222607.171211457990000;
    ce.xyzt.y = 111404.251442434990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vitk1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 222776.716709598000000;
    ce.xyzt.y = -111234.665587444990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vitk1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222607.171211457990000;
    ce.xyzt.y = 111404.251442434990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vitk1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -222776.716709598000000;
    ce.xyzt.y = -111234.665587444990000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vitk1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031349673837;
    ce.xyzt.y = 0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=vitk1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031349638931;
    ce.xyzt.y = -0.000015678571923;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=vitk1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031349673837;
    ce.xyzt.y = 0.000015678554470;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=vitk1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031349638931;
    ce.xyzt.y = -0.000015678571923;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=vitk1 ellps=GRS80 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=vitk1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223370.224840471000000;
    ce.xyzt.y = 111786.123319644000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vitk1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223540.351507254990000;
    ce.xyzt.y = -111615.956157675000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vitk1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223370.224840471000000;
    ce.xyzt.y = 111786.123319644000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vitk1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223540.351507254990000;
    ce.xyzt.y = -111615.956157675000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=vitk1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031242580435;
    ce.xyzt.y = 0.000015624990315;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=vitk1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031242545528;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=vitk1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031242580435;
    ce.xyzt.y = 0.000015624990315;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=vitk1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031242545528;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=vitk1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=wag1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 195986.781561157990000;
    ce.xyzt.y = 127310.075060660000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 195986.781561157990000;
    ce.xyzt.y = -127310.075060660000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -195986.781561157990000;
    ce.xyzt.y = 127310.075060660000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -195986.781561157990000;
    ce.xyzt.y = -127310.075060660000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000035617300565;
    ce.xyzt.y = 0.000013709107489;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000035617300565;
    ce.xyzt.y = -0.000013709107489;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000035617300565;
    ce.xyzt.y = 0.000013709107489;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000035617300565;
    ce.xyzt.y = -0.000013709107489;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag1 a=6400000 +lat_1=0.5 +lat_2=2 +n=0.5", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=wag2 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 206589.888099962000000;
    ce.xyzt.y = 120778.040357547000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 206589.888099962000000;
    ce.xyzt.y = -120778.040357547000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -206589.888099962000000;
    ce.xyzt.y = 120778.040357547000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -206589.888099962000000;
    ce.xyzt.y = -120778.040357547000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000033789993198;
    ce.xyzt.y = 0.000014450593168;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000033789993198;
    ce.xyzt.y = -0.000014450593168;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000033789993198;
    ce.xyzt.y = 0.000014450593168;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000033789993198;
    ce.xyzt.y = -0.000014450593168;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=wag3 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223387.021718166010000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223387.021718166010000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223387.021718166010000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223387.021718166010000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag3 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=wag4 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 192801.218662384000000;
    ce.xyzt.y = 129416.216394803010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag4 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 192801.218662384000000;
    ce.xyzt.y = -129416.216394803010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag4 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -192801.218662384000000;
    ce.xyzt.y = 129416.216394803010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag4 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -192801.218662384000000;
    ce.xyzt.y = -129416.216394803010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag4 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000036206907692;
    ce.xyzt.y = 0.000013485862424;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag4 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000036206907692;
    ce.xyzt.y = -0.000013485862424;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag4 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000036206907692;
    ce.xyzt.y = 0.000013485862424;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag4 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000036206907692;
    ce.xyzt.y = -0.000013485862424;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag4 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=wag5 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 203227.051925325010000;
    ce.xyzt.y = 138651.631442713000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 203227.051925325010000;
    ce.xyzt.y = -138651.631442713000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -203227.051925325010000;
    ce.xyzt.y = 138651.631442713000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -203227.051925325010000;
    ce.xyzt.y = -138651.631442713000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000034349336316;
    ce.xyzt.y = 0.000012587593818;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000034349336316;
    ce.xyzt.y = -0.000012587593818;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000034349336316;
    ce.xyzt.y = 0.000012587593818;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000034349336316;
    ce.xyzt.y = -0.000012587593818;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag5 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=wag6 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 211652.562164409990000;
    ce.xyzt.y = 105831.180787330000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 211652.562164409990000;
    ce.xyzt.y = -105831.180787330000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -211652.562164409990000;
    ce.xyzt.y = 105831.180787330000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -211652.562164409990000;
    ce.xyzt.y = -105831.180787330000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000032983267111;
    ce.xyzt.y = 0.000016491633555;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000032983267111;
    ce.xyzt.y = -0.000016491633555;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000032983267111;
    ce.xyzt.y = 0.000016491633555;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000032983267111;
    ce.xyzt.y = -0.000016491633555;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wag6 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=wag7 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 198601.876957311990000;
    ce.xyzt.y = 125637.045714171010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag7 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 198601.876957311990000;
    ce.xyzt.y = -125637.045714171010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag7 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -198601.876957311990000;
    ce.xyzt.y = 125637.045714171010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag7 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -198601.876957311990000;
    ce.xyzt.y = -125637.045714171010000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wag7 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=weren a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223378.515757634010000;
    ce.xyzt.y = 146214.093042288000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=weren a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223378.515757634010000;
    ce.xyzt.y = -146214.093042288000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=weren a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223378.515757634010000;
    ce.xyzt.y = 146214.093042288000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=weren a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223378.515757634010000;
    ce.xyzt.y = -146214.093042288000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=weren a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000011936620914;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=weren a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000011936620914;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=weren a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000011936620914;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=weren a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000011936620914;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=weren a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=wink1 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223385.131640953010000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wink1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223385.131640953010000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wink1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223385.131640953010000;
    ce.xyzt.y = 111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wink1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223385.131640953010000;
    ce.xyzt.y = -111701.072127637000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wink1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wink1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wink1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wink1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wink1 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=wink2 a=6400000 +lat_1=0.5 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223387.396433785990000;
    ce.xyzt.y = 124752.032797445000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wink2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223387.396433785990000;
    ce.xyzt.y = -124752.032797445000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wink2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223387.396433785990000;
    ce.xyzt.y = 124752.032797445000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wink2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223387.396433785990000;
    ce.xyzt.y = -124752.032797445000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wink2 a=6400000 +lat_1=0.5 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=wintri a=6400000 +lat_1=0 +lat_2=2");
    tolerance = 0.000000100000;
    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223390.801533485010000;
    ce.xyzt.y = 111703.907505745000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wintri a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 223390.801533485010000;
    ce.xyzt.y = -111703.907505745000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wintri a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = 0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223390.801533485010000;
    ce.xyzt.y = 111703.907505745000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wintri a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -0.034906585039887;
    ci.xyzt.y = -0.017453292519943;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -223390.801533485010000;
    ce.xyzt.y = -111703.907505745000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=wintri a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wintri a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = 200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wintri a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = 100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = 0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wintri a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    ci.xyzt.x = -200.000000000000000;
    ci.xyzt.y = -100.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.000031249998084;
    ce.xyzt.y = -0.000015625007769;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=wintri a=6400000 +lat_1=0 +lat_2=2", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=deformation +xy_grids=alaska +t_epoch=2016.0 +t_obs=2000.0");
    tolerance = 0.000001000000;
    ci.xyzt.x = -3004295.588250307800000;
    ci.xyzt.y = -1093474.169060383200000;
    ci.xyzt.z = 5500477.133825145700000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -3004295.588876612000000;
    ce.xyzt.y = -1093474.168851342300000;
    ce.xyzt.z = 5500477.133825145700000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=deformation +xy_grids=alaska +t_epoch=2016.0 +t_obs=2000.0", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=deformation +z_grids=egm96_15.gtx +t_epoch=2016.0 +t_obs=2000.0");
    tolerance = 0.000001000000;
    ci.xyzt.x = -3004295.588250307800000;
    ci.xyzt.y = -1093474.169060383200000;
    ci.xyzt.z = 5500477.133825145700000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -3004295.588250307800000;
    ce.xyzt.y = -1093474.169060383200000;
    ce.xyzt.z = 5500234.008855661400000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=deformation +z_grids=egm96_15.gtx +t_epoch=2016.0 +t_obs=2000.0", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=deformation +xy_grids=alaska +z_grids=egm96_15.gtx +t_epoch=2016.0 +t_obs=2000.0");
    tolerance = 0.000001000000;
    ci.xyzt.x = -3004295.588250307800000;
    ci.xyzt.y = -1093474.169060383200000;
    ci.xyzt.z = 5500477.133825145700000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -3004295.588876612000000;
    ce.xyzt.y = -1093474.168851342300000;
    ce.xyzt.z = 5500234.008855661400000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=deformation +xy_grids=alaska +z_grids=egm96_15.gtx +t_epoch=2016.0 +t_obs=2000.0", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=deformation +xy_grids=alaska +t_epoch=2016.0");
    tolerance = 0.000001000000;
    ci.xyzt.x = -3004295.588250307800000;
    ci.xyzt.y = -1093474.169060383200000;
    ci.xyzt.z = 5500477.133825145700000;
    ci.xyzt.t = 2000.000000000000000;
    ce.xyzt.x = -3004295.588876612000000;
    ce.xyzt.y = -1093474.168851342300000;
    ce.xyzt.z = 5500477.133825145700000;
    ce.xyzt.t = 2000.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=deformation +xy_grids=alaska +t_epoch=2016.0", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=deformation +z_grids=egm96_15.gtx +t_epoch=2016.0");
    tolerance = 0.000001000000;
    ci.xyzt.x = -3004295.588250307800000;
    ci.xyzt.y = -1093474.169060383200000;
    ci.xyzt.z = 5500477.133825145700000;
    ci.xyzt.t = 2000.000000000000000;
    ce.xyzt.x = -3004295.588250307800000;
    ce.xyzt.y = -1093474.169060383200000;
    ce.xyzt.z = 5500234.008855661400000;
    ce.xyzt.t = 2000.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=deformation +z_grids=egm96_15.gtx +t_epoch=2016.0", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=deformation +xy_grids=alaska +z_grids=egm96_15.gtx +t_epoch=2016.0 +ellps=GRS80");
    tolerance = 0.000001000000;
    ci.xyzt.x = -3004295.588250307800000;
    ci.xyzt.y = -1093474.169060383200000;
    ci.xyzt.z = 5500477.133825145700000;
    ci.xyzt.t = 2000.000000000000000;
    ce.xyzt.x = -3004295.588876612000000;
    ce.xyzt.y = -1093474.168851342300000;
    ce.xyzt.z = 5500234.008855661400000;
    ce.xyzt.t = 2000.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=deformation +xy_grids=alaska +z_grids=egm96_15.gtx +t_epoch=2016.0 +ellps=GRS80", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=merc R=6400000");
    tolerance = 0.000000010000;
    ci.xyzt.x = 0.017453292519943;
    ci.xyzt.y = 0.034906585039887;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 111701.072127637100000;
    ce.xyzt.y = 223447.526203260500000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=merc R=6400000", ci, ce, co);

    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1340412.865531645200000;
    ce.xyzt.y = 7387101.143096734800000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=merc R=6400000", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=merc a=6400000 rf=297");
    tolerance = 0.000000010000;
    ci.xyzt.x = 0.017453292519943;
    ci.xyzt.y = 0.034906585039887;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 111701.072127637100000;
    ce.xyzt.y = 221945.968183208810000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=merc a=6400000 rf=297", ci, ce, co);

    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1340412.865531645200000;
    ce.xyzt.y = 7351803.915170588500000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=merc a=6400000 rf=297", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=merc ellps=GRS80");
    tolerance = 0.000000010000;
    ci.xyzt.x = 0.017453292519943;
    ci.xyzt.y = 0.034906585039887;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 111319.490793273600000;
    ce.xyzt.y = 221194.077160423710000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=merc ellps=GRS80", ci, ce, co);

    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1335833.889519282800000;
    ce.xyzt.y = 7326837.714873876400000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=merc ellps=GRS80", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=merc ellps=GRS80000000000");
    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=merc +a=-1");
    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=merc no_defs");
    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=merc");
    ci.xyzt.x = 0.000000000000000;
    ci.xyzt.y = 0.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000000000000000;
    ce.xyzt.y = 0.000000000000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=merc", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=merc +es=-1");
    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "");
    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "cobra");
    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=merc ellps=GRS80 R_A");
    tolerance = 0.000000010000;
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1334340.623729770300000;
    ce.xyzt.y = 7353636.629655202900000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=merc ellps=GRS80 R_A", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=merc ellps=GRS80 R_V");
    tolerance = 0.000000010000;
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1334339.285267565200000;
    ce.xyzt.y = 7353629.253304272000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=merc ellps=GRS80 R_V", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=merc ellps=GRS80 R_a");
    tolerance = 0.000000010000;
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1333594.490452750400000;
    ce.xyzt.y = 7349524.641382550800000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=merc ellps=GRS80 R_a", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=merc ellps=GRS80 R_g");
    tolerance = 0.000000010000;
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1333592.610229132700000;
    ce.xyzt.y = 7349514.279349780600000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=merc ellps=GRS80 R_g", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=merc ellps=GRS80 R_h");
    tolerance = 0.000000010000;
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1333590.730008165800000;
    ce.xyzt.y = 7349503.917331623800000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=merc ellps=GRS80 R_h", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=merc ellps=GRS80 R_lat_a=60");
    tolerance = 0.000000010000;
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1338073.743626892100000;
    ce.xyzt.y = 7374210.092480332600000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=merc ellps=GRS80 R_lat_a=60", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=merc ellps=GRS80 R_lat_g=60");
    tolerance = 0.000000010000;
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 1338073.269610159100000;
    ce.xyzt.y = 7374207.480143763100000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=merc ellps=GRS80 R_lat_g=60", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=healpix a=1 lon_0=0 ellps=WGS84");
    ci.xyzt.x = 0.000000000000000;
    ci.xyzt.y = 0.731953631859906;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.000000000000000;
    ce.xyzt.y = 0.784520000000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=healpix a=1 lon_0=0 ellps=WGS84", ci, ce, co);

    ci.xyzt.x = -1.570796326794897;
    ci.xyzt.y = 0.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1.569040000000000;
    ce.xyzt.y = 0.000000000000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=healpix a=1 lon_0=0 ellps=WGS84", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=utm zone=32 ellps=GRS80 rf=0");
    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=utm zone=32 ellps=GRS80 es=1");
    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=utm zone=32 ellps=GRS80 b=0");
    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=utm zone=32 ellps=GRS80 b=6000000");
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 699293.087999999990000;
    ce.xyzt.y = 5674591.529500000200000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=utm zone=32 ellps=GRS80 b=6000000", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=utm zone=32 ellps=GRS80 rf=300");
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 691873.121200000050000;
    ce.xyzt.y = 6099054.966099999800000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=utm zone=32 ellps=GRS80 rf=300", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=utm zone=32 ellps=GRS80 f=0.00333333333333");
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 691873.121200000050000;
    ce.xyzt.y = 6099054.966099999800000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=utm zone=32 ellps=GRS80 f=0.00333333333333", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=utm zone=32 ellps=GRS80 b=6000000");
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 699293.087999999990000;
    ce.xyzt.y = 5674591.529500000200000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=utm zone=32 ellps=GRS80 b=6000000", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=utm zone=32 a=6400000 b=6000000");
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 700416.589999999970000;
    ce.xyzt.y = 5669475.888399999600000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=utm zone=32 a=6400000 b=6000000", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=helmert x=0.06155 rx=-0.0394924 y=-0.01087 ry=-0.0327221 z=-0.04019 rz=-0.0328979 s=-0.009994");
    tolerance = 0.000075000000;
    ci.xyzt.x = -4052051.764299999900000;
    ci.xyzt.y = 4212836.201700000100000;
    ci.xyzt.z = -2545106.024499999800000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -4052052.737900000100000;
    ce.xyzt.y = 4212835.989699999800000;
    ce.xyzt.z = -2545104.589800000200000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=helmert x=0.06155 rx=-0.0394924 y=-0.01087 ry=-0.0327221 z=-0.04019 rz=-0.0328979 s=-0.009994", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=pipeline ellps=GRS80 step proj=cart step proj=helmert x=0.06155 rx=-0.0394924 y=-0.01087 ry=-0.0327221 z=-0.04019 rz=-0.0328979 s=-0.009994 step proj=cart inv");
    ci.xyzt.x = 2.336743027633126;
    ci.xyzt.y = -0.413121596235468;
    ci.xyzt.z = 603.346599999999970;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.336743172669987;
    ce.xyzt.y = -0.413121356252696;
    ce.xyzt.z = 603.248900000000050;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "proj=pipeline ellps=GRS80 step proj=cart step proj=helmert x=0.06155 rx=-0.0394924 y=-0.01087 ry=-0.0327221 z=-0.04019 rz=-0.0328979 s=-0.009994 step proj=cart inv", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=helmert ellps=GRS80 exact x=0 rx=0 dx=0 drx=0.00150379 y=0 ry=0 dy=0 dry=0.00118346 z=0 rz=0 dz=0 drz=0.00120716 s=0 ds=0 t_epoch=2020.0");
    tolerance = 0.000040000000;
    ci.xyzt.x = -4052052.658799999900000;
    ci.xyzt.y = 4212835.993800000300000;
    ci.xyzt.z = -2545104.694600000000000;
    ci.xyzt.t = 2018.000000000000000;
    ce.xyzt.x = -4052052.737300000200000;
    ce.xyzt.y = 4212835.983500000100000;
    ce.xyzt.z = -2545104.586699999900000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=helmert ellps=GRS80 exact x=0 rx=0 dx=0 drx=0.00150379 y=0 ry=0 dy=0 dry=0.00118346 z=0 rz=0 dz=0 drz=0.00120716 s=0 ds=0 t_epoch=2020.0", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "+proj=ob_tran o_proj=moll +R=6378137.0 +o_lon_p=0 +o_lat_p=0 +lon_0=180");
    tolerance = 0.001000000000;
    direction = -1;
    ci.xyzt.x = 300000.000000000000000;
    ci.xyzt.y = 400000.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -0.746236741984416;
    ce.xyzt.y = 1.493847101725128;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "+proj=ob_tran o_proj=moll +R=6378137.0 +o_lon_p=0 +o_lat_p=0 +lon_0=180", ci, ce, co);

    direction = 1;
    ci.xyzt.x = 0.174532925199433;
    ci.xyzt.y = 0.349065850398866;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = -1384841.187870000000000;
    ce.xyzt.y = 7581707.882399999500000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "+proj=ob_tran o_proj=moll +R=6378137.0 +o_lon_p=0 +o_lat_p=0 +lon_0=180", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=molodensky a=6378160 rf=298.25 da=-23 df=-8.120449e-8 dx=-134 dy=-48 dz=149 abridged");
    tolerance = 2.000000000000;
    ci.xyzt.x = 2.530146220750864;
    ci.xyzt.y = -0.659734457253856;
    ci.xyzt.z = 50.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.530168910031139;
    ce.xyzt.y = -0.659707928249226;
    ce.xyzt.z = 46.378000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "proj=molodensky a=6378160 rf=298.25 da=-23 df=-8.120449e-8 dx=-134 dy=-48 dz=149 abridged", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=molodensky a=6378160 rf=298.25 da=-23 df=-8.120449e-8 dx=-134 dy=-48 dz=149");
    tolerance = 2.000000000000;
    ci.xyzt.x = 2.530146220750864;
    ci.xyzt.y = -0.659734457253856;
    ci.xyzt.z = 50.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 2.530168910031139;
    ce.xyzt.y = -0.659707928249226;
    ce.xyzt.z = 46.378000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "proj=molodensky a=6378160 rf=298.25 da=-23 df=-8.120449e-8 dx=-134 dy=-48 dz=149", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=pipeline zone=32 step proj=utm ellps=GRS80 step proj=utm ellps=GRS80 inv");
    tolerance = 0.000100000000;
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.209439510239320;
    ce.xyzt.y = 0.959931088596881;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "proj=pipeline zone=32 step proj=utm ellps=GRS80 step proj=utm ellps=GRS80 inv", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.209439510239320;
    ce.xyzt.y = 0.959931088596881;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "proj=pipeline zone=32 step proj=utm ellps=GRS80 step proj=utm ellps=GRS80 inv", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=pipeline zone=32 ellps=GRS80 step proj=utm inv step proj=utm");
    ci.xyzt.x = 691875.632140000000000;
    ci.xyzt.y = 6098907.825009999800000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 691875.632140000000000;
    ce.xyzt.y = 6098907.825009999800000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=pipeline zone=32 ellps=GRS80 step proj=utm inv step proj=utm", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 691875.632140000000000;
    ci.xyzt.y = 6098907.825009999800000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 691875.632140000000000;
    ce.xyzt.y = 6098907.825009999800000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=pipeline zone=32 ellps=GRS80 step proj=utm inv step proj=utm", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=pipeline step proj=utm zone=32 ellps=GRS80");
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 691875.632140000000000;
    ce.xyzt.y = 6098907.825009999800000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=pipeline step proj=utm zone=32 ellps=GRS80", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 691875.632140000000000;
    ci.xyzt.y = 6098907.825009999800000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.209439510239320;
    ce.xyzt.y = 0.959931088596881;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "proj=pipeline step proj=utm zone=32 ellps=GRS80", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=pipeline step init=epsg:25832 inv step init=epsg:25833 step init=epsg:25833 inv step init=epsg:25832");
    ci.xyzt.x = 691875.632140000000000;
    ci.xyzt.y = 6098907.825009999800000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 691875.632140000000000;
    ce.xyzt.y = 6098907.825009999800000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=pipeline step init=epsg:25832 inv step init=epsg:25833 step init=epsg:25833 inv step init=epsg:25832", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 12.000000000000000;
    ci.xyzt.y = 55.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 12.000000000000000;
    ce.xyzt.y = 55.000000000000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=pipeline step init=epsg:25832 inv step init=epsg:25833 step init=epsg:25833 inv step init=epsg:25832", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=vgridshift grids=egm96_15.gtx ellps=GRS80");
    tolerance = 0.150000000000;
    ci.xyzt.x = 0.218166156499291;
    ci.xyzt.y = 0.968657734856853;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.218166156499291;
    ce.xyzt.y = 0.968657734856853;
    ce.xyzt.z = -36.021305084228523;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "proj=vgridshift grids=egm96_15.gtx ellps=GRS80", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=vgridshift");
    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=vgridshift grids=nonexistinggrid.gtx");
    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=hgridshift +grids=nzgd2kgrid0005.gsb ellps=GRS80");
    tolerance = 0.000000001000;
    ci.xyzt.x = 3.019419605950190;
    ci.xyzt.y = -0.785398163397448;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 3.019417724154021;
    ce.xyzt.y = -0.785426445270361;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "proj=hgridshift +grids=nzgd2kgrid0005.gsb ellps=GRS80", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 3.019417724154021;
    ci.xyzt.y = -0.785426445270361;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 3.019419605950190;
    ce.xyzt.y = -0.785398163397448;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "proj=hgridshift +grids=nzgd2kgrid0005.gsb ellps=GRS80", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=hgridshift grids=@nonexistinggrid.gsb,anothernonexistinggrid.gsb");
    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=hgridshift");
    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=helmert ellps=GRS80 x=0.67678 y=0.65495 z=-0.52827 rx=-0.022742 ry=0.012667 rz=0.022704 s=-0.01070");
    tolerance = 0.000001000000;
    ci.xyzt.x = 3565285.000000000000000;
    ci.xyzt.y = 855949.000000000000000;
    ci.xyzt.z = 5201383.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 3565285.413423509800000;
    ce.xyzt.y = 855948.679867589960000;
    ce.xyzt.z = 5201382.729397909700000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=helmert ellps=GRS80 x=0.67678 y=0.65495 z=-0.52827 rx=-0.022742 ry=0.012667 rz=0.022704 s=-0.01070", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=helmert ellps=GRS80 exact x=-081.0703 rx=-0.48488 y=-089.3603 ry=-0.02436 z=-115.7526 rz=-0.41321 s=-0.540645");
    tolerance = 0.000001000000;
    ci.xyzt.x = 3494994.301200000100000;
    ci.xyzt.y = 1056601.972499999900000;
    ci.xyzt.z = 5212382.166600000100000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 3494909.840263680100000;
    ce.xyzt.y = 1056506.789386329900000;
    ce.xyzt.z = 5212265.666997609700000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=helmert ellps=GRS80 exact x=-081.0703 rx=-0.48488 y=-089.3603 ry=-0.02436 z=-115.7526 rz=-0.41321 s=-0.540645", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=helmert ellps=GRS80 transpose x=0.0127 dx=-0.0029 rx=-0.00039 drx=-0.00011 y=0.0065 dy=-0.0002 ry=0.00080 dry=-0.00019 z=-0.0209 dz=-0.0006 rz=-0.00114 drz=0.00007 s=0.00195 ds=0.00001 t_epoch=1988.0");
    tolerance = 0.000030000000;
    ci.xyzt.x = 3370658.378000000000000;
    ci.xyzt.y = 711877.314000000010000;
    ci.xyzt.z = 5349787.086000000100000;
    ci.xyzt.t = 2017.000000000000000;
    ce.xyzt.x = 3370658.188900000000000;
    ce.xyzt.y = 711877.423700000040000;
    ce.xyzt.z = 5349787.124300000300000;
    ce.xyzt.t = 2017.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=helmert ellps=GRS80 transpose x=0.0127 dx=-0.0029 rx=-0.00039 drx=-0.00011 y=0.0065 dy=-0.0002 ry=0.00080 dry=-0.00019 z=-0.0209 dz=-0.0006 rz=-0.00114 drz=0.00007 s=0.00195 ds=0.00001 t_epoch=1988.0", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=helmert ellps=GRS80 x=-9597.3572 y=.6112 s=0.304794780637 theta=-1.244048");
    tolerance = 0.001000000000;
    ci.xyzt.x = 2546506.956999999900000;
    ci.xyzt.y = 542256.609000000050000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 766563.675000000050000;
    ce.xyzt.y = 165282.277000000000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=helmert ellps=GRS80 x=-9597.3572 y=.6112 s=0.304794780637 theta=-1.244048", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=helmert ellps=GRS80 transpose x=0.01270 dx=-0.0029 rx=-0.00039 drx=-0.00011 y=0.00650 dy=-0.0002 ry=0.00080 dry=-0.00019 z=-0.0209 dz=-0.0006 rz=-0.00114 drz=0.00007 s=0.00195 ds=0.00001 t_epoch=1988.0");
    tolerance = 0.000100000000;
    ci.xyzt.x = 3370658.378000000000000;
    ci.xyzt.y = 711877.314000000010000;
    ci.xyzt.z = 5349787.086000000100000;
    ci.xyzt.t = 2017.000000000000000;
    ce.xyzt.x = 3370658.188900000000000;
    ce.xyzt.y = 711877.423700000040000;
    ce.xyzt.z = 5349787.124300000300000;
    ce.xyzt.t = 2017.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=helmert ellps=GRS80 transpose x=0.01270 dx=-0.0029 rx=-0.00039 drx=-0.00011 y=0.00650 dy=-0.0002 ry=0.00080 dry=-0.00019 z=-0.0209 dz=-0.0006 rz=-0.00114 drz=0.00007 s=0.00195 ds=0.00001 t_epoch=1988.0", ci, ce, co);

    ci.xyzt.x = 3370658.378000000000000;
    ci.xyzt.y = 711877.314000000010000;
    ci.xyzt.z = 5349787.086000000100000;
    ci.xyzt.t = 2018.000000000000000;
    ce.xyzt.x = 3370658.180869999800000;
    ce.xyzt.y = 711877.427499999990000;
    ce.xyzt.z = 5349787.126480000100000;
    ce.xyzt.t = 2018.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=helmert ellps=GRS80 transpose x=0.01270 dx=-0.0029 rx=-0.00039 drx=-0.00011 y=0.00650 dy=-0.0002 ry=0.00080 dry=-0.00019 z=-0.0209 dz=-0.0006 rz=-0.00114 drz=0.00007 s=0.00195 ds=0.00001 t_epoch=1988.0", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=geoc ellps=GRS80");
    tolerance = 1e-9;
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.209439510239320;
    ce.xyzt.y = 0.956771576793152;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "proj=geoc ellps=GRS80", ci, ce, co);

    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 1.570796326794897;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.209439510239320;
    ce.xyzt.y = 1.570796326794897;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "proj=geoc ellps=GRS80", ci, ce, co);

    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = -1.570796326794897;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.209439510239320;
    ce.xyzt.y = -1.570796326794897;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "proj=geoc ellps=GRS80", ci, ce, co);

    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 1.570796326794722;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 0.209439510239320;
    ce.xyzt.y = 1.570796326794722;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_lpz_dist (P, co.lpz, ce.lpz);
    test_update (d, tolerance, "proj=geoc ellps=GRS80", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=utm ellps=GRS80 zone=32 to_meter=0");
    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=utm ellps=GRS80 zone=32 to_meter=10");
    ci.xyzt.x = 0.209439510239320;
    ci.xyzt.y = 0.959931088596881;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 69187.563200000004000;
    ce.xyzt.y = 609890.782499999970000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=utm ellps=GRS80 zone=32 to_meter=10", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=pipeline step init=epsg:25832 inv step init=epsg:25833 step init=epsg:25833 inv step init=epsg:25832");
    ci.xyzt.x = 691875.632140000000000;
    ci.xyzt.y = 6098907.825009999800000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 691875.632140000000000;
    ce.xyzt.y = 6098907.825009999800000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=pipeline step init=epsg:25832 inv step init=epsg:25833 step init=epsg:25833 inv step init=epsg:25832", ci, ce, co);

    direction = -1;
    ci.xyzt.x = 12.000000000000000;
    ci.xyzt.y = 55.000000000000000;
    ci.xyzt.z = 0.000000000000000;
    ci.xyzt.t = 0.000000000000000;
    ce.xyzt.x = 12.000000000000000;
    ce.xyzt.y = 55.000000000000000;
    ce.xyzt.z = 0.000000000000000;
    ce.xyzt.t = 0.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=pipeline step init=epsg:25832 inv step init=epsg:25833 step init=epsg:25833 inv step init=epsg:25832", ci, ce, co);

    direction = 1;
    tolerance = 0.000500000000;
    (void) P;
    proj_destroy (P);


    P = proj_create (0, "proj=helmert ellps=GRS80 transpose x=0.0127 dx=-0.0029 rx=-0.00039 drx=-0.00011 y=0.0065 dy=-0.0002 ry=0.00080 dry=-0.00019 z=-0.0209 dz=-0.0006 rz=-0.00114 drz=0.00007 s=0.00195 ds=0.00001 t_epoch=1988.0");
    tolerance = 0.000030000000;
    ci.xyzt.x = 3370658.378000000000000;
    ci.xyzt.y = 711877.314000000010000;
    ci.xyzt.z = 5349787.086000000100000;
    ci.xyzt.t = 2017.000000000000000;
    ce.xyzt.x = 3370658.188900000000000;
    ce.xyzt.y = 711877.423700000040000;
    ce.xyzt.z = 5349787.124300000300000;
    ce.xyzt.t = 2017.000000000000000;
    co = proj_trans (P, direction, ci);
    d = proj_xyz_dist (co.xyz, ce.xyz);
    test_update (d, tolerance, "proj=helmert ellps=GRS80 transpose x=0.0127 dx=-0.0029 rx=-0.00039 drx=-0.00011 y=0.0065 dy=-0.0002 ry=0.00080 dry=-0.00019 z=-0.0209 dz=-0.0006 rz=-0.00114 drz=0.00007 s=0.00195 ds=0.00001 t_epoch=1988.0", ci, ce, co);

    printf ("OK: %d,  KO: %d\nOL: %d\n", OK, KO, OK+KO);
    return 0;
}