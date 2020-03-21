#include <stdio.h>
#include <proj.h>

int main(int argc, char *argv[]) {
    PJ_INFO info;
    info = proj_info();
    if(argc == 2 && argv[1][0] == '-') {
        switch(argv[1][1]) {
            case 's':
                printf("%s\n", info.searchpath);
                return(0);
            case 'v':
                printf("%d.%d.%d\n", info.major, info.minor, info.patch);
                return(0);
        }
    }
    printf("Use option -v or -s\n");
    return(1);
}
