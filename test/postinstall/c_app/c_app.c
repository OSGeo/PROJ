#include <stdio.h>
#include <proj.h>

int test_transform() {
    PJ *P;
    PJ_COORD a, b;
    P = proj_create_crs_to_crs(
        PJ_DEFAULT_CTX,
        "EPSG:4326",
        "+proj=utm +zone=32 +datum=WGS84", /* or EPSG:32632 */
        NULL);
    if (0 == P) {
        fprintf(stderr, "Oops\n");
        return 1;
    }
    /* Copenhagen: 55d N, 12d E */
    a = proj_coord(55, 12, 0, 0);
    b = proj_trans(P, PJ_FWD, a);
    printf("easting: %.2f, northing: %.2f, ", b.enu.e, b.enu.n);
    b = proj_trans(P, PJ_INV, b);
    printf("latitude: %.2f, longitude: %.2f\n", b.lp.lam, b.lp.phi);
    proj_destroy(P);
    return 0;
}

int main(int argc, char *argv[]) {
    PJ_INFO info;
    info = proj_info();
    if(argc == 2 && argv[1][0] == '-') {
        switch(argv[1][1]) {
            case 't':
                return(test_transform());
            case 's':
                printf("%s\n", info.searchpath);
                return(0);
            case 'v':
                printf("%d.%d.%d\n", info.major, info.minor, info.patch);
                return(0);
        }
    }
    fprintf(stderr, "Use option -t, -s or -v\n");
    return(1);
}
