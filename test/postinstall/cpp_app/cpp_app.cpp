#include <iostream>
#include <proj.h>

int test_transform() {
    PJ *P;
    PJ_COORD a, b;
    P = proj_create_crs_to_crs(
        PJ_DEFAULT_CTX,
        "EPSG:4326",
        "+proj=utm +zone=32 +datum=WGS84", // or EPSG:32632
        NULL);
    if (0 == P) {
        std::cerr << "Oops" << std::endl;
        return 1;
    }
    // Copenhagen: 55d N, 12d E
    a = proj_coord(55, 12, 0, 0);
    b = proj_trans(P, PJ_FWD, a);
    std::cout.precision(2);
    std::cout.setf( std::ios::fixed, std::ios::floatfield );
    std::cout << "easting: " << b.enu.e << ", northing: " << b.enu.n;
    b = proj_trans(P, PJ_INV, b);
    std::cout << ", latitude: " << b.lp.lam << ", longitude: " << b.lp.phi << std::endl;
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
                std::cout << info.searchpath << std::endl;
                return(0);
            case 'v':
                std::cout << info.major << '.' << info.minor << '.' << info.patch << std::endl;
                return(0);
        }
    }
    std::cerr << "Use option -t, -s or -v" << std::endl;
    return(1);
}
