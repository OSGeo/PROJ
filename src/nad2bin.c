/* Convert bivariate ASCII NAD27 to NAD83 tables to NTv2 binary structure */
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define PJ_LIB__
#include <projects.h>
#define U_SEC_TO_RAD 4.848136811095359935899141023e-12
	static char
*usage = "<ASCII_dist_table local_bin_table";

/************************************************************************/
/*                             swap_words()                             */
/*                                                                      */
/*      Convert the byte order of the given word(s) in place.           */
/************************************************************************/

static int  byte_order_test = 1;
#define IS_LSB	(((unsigned char *) (&byte_order_test))[0] == 1)

static void swap_words( unsigned char *data, int word_size, int word_count )

{
    int	word;

    for( word = 0; word < word_count; word++ )
    {
        int	i;
        
        for( i = 0; i < word_size/2; i++ )
        {
            int	t;
            
            t = data[i];
            data[i] = data[word_size-i-1];
            data[word_size-i-1] = t;
        }
        
        data += word_size;
    }
}

/************************************************************************/
/*                                main()                                */
/************************************************************************/
int main(int argc, char **argv) {
    struct CTABLE ct;
    FLP *p, t;
    size_t tsize;
    int i, j, ichk;
    long lam, laml, phi, phil;
    FILE *fp;

    const char *GS_TYPE  = "SECONDS";
    const char *VERSION  = "";
    const char *SYSTEM_F = "NAD27";
    const char *SYSTEM_T = "NAD83";
    const char *SUB_NAME = "";
    const char *CREATED  = "";
    const char *UPDATED  = "";

    if (argc != 2) {
        fprintf(stderr,"usage: %s %s\n", argv[0], usage);
        exit(1);
    }

/* ==================================================================== */
/*      Read the ASCII Table                                            */
/* ==================================================================== */

    fgets(ct.id, MAX_TAB_ID, stdin);
    scanf("%d %d %*d %lf %lf %lf %lf", &ct.lim.lam, &ct.lim.phi,
          &ct.ll.lam, &ct.del.lam, &ct.ll.phi, &ct.del.phi);
    if (!(ct.cvs = (FLP *)malloc(tsize = ct.lim.lam * ct.lim.phi *
                                 sizeof(FLP)))) {
        perror("mem. alloc");
        exit(1);
    }
    ct.ll.lam *= DEG_TO_RAD;
    ct.ll.phi *= DEG_TO_RAD;
    ct.del.lam *= DEG_TO_RAD;
    ct.del.phi *= DEG_TO_RAD;
    /* load table */
    for (p = ct.cvs, i = 0; i < ct.lim.phi; ++i) {
        scanf("%d:%ld %ld", &ichk, &laml, &phil);
        if (ichk != i) {
            fprintf(stderr,"format check on row\n");
            exit(1);
        }
        t.lam = laml * U_SEC_TO_RAD;
        t.phi = phil * U_SEC_TO_RAD;
        *p++ = t;
        for (j = 1; j < ct.lim.lam; ++j) {
            scanf("%ld %ld", &lam, &phi);
            t.lam = (laml += lam) * U_SEC_TO_RAD;
            t.phi = (phil += phi) * U_SEC_TO_RAD;
            *p++ = t;
        }
    }
    if (feof(stdin)) {
        fprintf(stderr, "premature EOF\n");
        exit(1);
    }

/* ==================================================================== */
/*      Write out the NTv2 format grid shift file.                      */
/* ==================================================================== */
    if (!(fp = fopen(argv[1], "wb"))) 
    {
        perror(argv[1]);
        exit(2);
    }
        
/* -------------------------------------------------------------------- */
/*      Write the file header.                                          */
/* -------------------------------------------------------------------- */
    {    
        char achHeader[11*16];

        memset( achHeader, 0, sizeof(achHeader) );
        
        memcpy( achHeader +  0*16, "NUM_OREC", 8 );
        achHeader[ 0*16 + 8] = 0xb;

        memcpy( achHeader +  1*16, "NUM_SREC", 8 );
        achHeader[ 1*16 + 8] = 0xb;

        memcpy( achHeader +  2*16, "NUM_FILE", 8 );
        achHeader[ 2*16 + 8] = 0x1;

        memcpy( achHeader +  3*16, "GS_TYPE         ", 16 );
        memcpy( achHeader +  3*16+8, GS_TYPE, MIN(16,strlen(GS_TYPE)) );

        memcpy( achHeader +  4*16, "VERSION         ", 16 );
        memcpy( achHeader +  4*16+8, VERSION, MIN(16,strlen(VERSION)) );

        memcpy( achHeader +  5*16, "SYSTEM_F        ", 16 );
        memcpy( achHeader +  5*16+8, SYSTEM_F, MIN(16,strlen(SYSTEM_F)) );

        memcpy( achHeader +  6*16, "SYSTEM_T        ", 16 );
        memcpy( achHeader +  6*16+8, SYSTEM_T, MIN(16,strlen(SYSTEM_T)) );

        memcpy( achHeader +  7*16, "MAJOR_F ", 8);
        memcpy( achHeader +  8*16, "MINOR_F ", 8 );
        memcpy( achHeader +  9*16, "MAJOR_T ", 8 );
        memcpy( achHeader + 10*16, "MINOR_T ", 8 );

        fwrite( achHeader, 1, sizeof(achHeader), fp );
    }
        
/* -------------------------------------------------------------------- */
/*      Write the grid header.                                          */
/* -------------------------------------------------------------------- */
    {
        unsigned char achHeader[11*16];
        double dfValue;
        int nGSCount = ct.lim.lam * ct.lim.phi;
        LP ur;

        ur.lam = ct.ll.lam + (ct.lim.lam-1) * ct.del.lam;
        ur.phi = ct.ll.phi + (ct.lim.phi-1) * ct.del.phi;

        assert( sizeof(nGSCount) == 4 );

        memset( achHeader, 0, sizeof(achHeader) );

        memcpy( achHeader +  0*16, "SUB_NAME        ", 16 );
        memcpy( achHeader +  0*16+8, SUB_NAME, MIN(16,strlen(SUB_NAME)) );
    
        memcpy( achHeader +  1*16, "PARENT          ", 16 );
        memcpy( achHeader +  1*16+8, "NONE", MIN(16,strlen("NONE")) );
    
        memcpy( achHeader +  2*16, "CREATED         ", 16 );
        memcpy( achHeader +  2*16+8, CREATED, MIN(16,strlen(CREATED)) );
    
        memcpy( achHeader +  3*16, "UPDATED         ", 16 );
        memcpy( achHeader +  3*16+8, UPDATED, MIN(16,strlen(UPDATED)) );

        memcpy( achHeader +  4*16, "S_LAT   ", 8 );
        dfValue = ct.ll.phi * 3600.0 / DEG_TO_RAD;
        memcpy( achHeader +  4*16 + 8, &dfValue, 8 );

        memcpy( achHeader +  5*16, "N_LAT   ", 8 );
        dfValue = ur.phi * 3600.0 / DEG_TO_RAD;
        memcpy( achHeader +  5*16 + 8, &dfValue, 8 );

        memcpy( achHeader +  6*16, "E_LONG  ", 8 );
        dfValue = -1 * ur.lam * 3600.0 / DEG_TO_RAD;
        memcpy( achHeader +  6*16 + 8, &dfValue, 8 );

        memcpy( achHeader +  7*16, "W_LONG  ", 8 );
        dfValue = -1 * ct.ll.lam * 3600.0 / DEG_TO_RAD;
        memcpy( achHeader +  7*16 + 8, &dfValue, 8 );

        memcpy( achHeader +  8*16, "LAT_INC ", 8 );
        dfValue = ct.del.phi * 3600.0 / DEG_TO_RAD;
        memcpy( achHeader +  8*16 + 8, &dfValue, 8 );
    
        memcpy( achHeader +  9*16, "LONG_INC", 8 );
        dfValue = ct.del.lam * 3600.0 / DEG_TO_RAD;
        memcpy( achHeader +  9*16 + 8, &dfValue, 8 );
    
        memcpy( achHeader + 10*16, "GS_COUNT", 8 );
        memcpy( achHeader + 10*16+8, &nGSCount, 4 );
    
        if( !IS_LSB ) 
        {
            swap_words( achHeader +  4*16 + 8, 8, 1 );
            swap_words( achHeader +  5*16 + 8, 8, 1 );
            swap_words( achHeader +  6*16 + 8, 8, 1 );
            swap_words( achHeader +  7*16 + 8, 8, 1 );
            swap_words( achHeader +  8*16 + 8, 8, 1 );
            swap_words( achHeader +  9*16 + 8, 8, 1 );
            swap_words( achHeader + 10*16 + 8, 4, 1 );
        }

        fwrite( achHeader, 1, sizeof(achHeader), fp );
    }

/* -------------------------------------------------------------------- */
/*      Write the actual grid cells.                                    */
/* -------------------------------------------------------------------- */
    {
        float *row_buf;
        int row;

        row_buf = (float *) pj_malloc(ct.lim.lam * sizeof(float) * 4);
        memset( row_buf, 0, sizeof(float)*4 );

        for( row = 0; row < ct.lim.phi; row++ )
        {
            int	    i;

            for( i = 0; i < ct.lim.lam; i++ )
            {
                FLP *cvs = ct.cvs + (row) * ct.lim.lam
                    + (ct.lim.lam - i - 1);

                /* convert radians to seconds */
                row_buf[i*4+0] = cvs->phi * (3600.0 / (PI/180.0));
                row_buf[i*4+1] = cvs->lam * (3600.0 / (PI/180.0));

                /* We leave the accuracy values as zero */
            }

            if( !IS_LSB )
                swap_words( (unsigned char *) row_buf, 4, ct.lim.lam * 4 );

            if( fwrite( row_buf, sizeof(float), ct.lim.lam*4, fp ) 
                != 4 * ct.lim.lam )
            {
                perror( "write()" );
                exit( 2 );
            }
        }
    }

    fclose( fp );
    exit(0); /* normal completion */
}
