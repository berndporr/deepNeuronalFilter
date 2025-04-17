#include <stdlib.h>
#include <stdio.h>
#include "matio.h"

char datavarname[] = "data";

int main(int argc,char **argv)
{
    mat_t *matfp = Mat_Open(argv[1],MAT_ACC_RDONLY);
    if ( NULL == matfp ) {
	fprintf(stderr,"Error opening MAT file \"%s\"!\n",argv[1]);
	return EXIT_FAILURE;
    }

    long int sweepNo = 300;
    if (argc > 2) {
	sweepNo = atoi(argv[2]);
    }

    fprintf(stderr,"Loading %s...\n",argv[1]);
    matvar_t *matvar = Mat_VarRead(matfp,datavarname);

    if ( NULL == matvar ) {
	fprintf(stderr,"Variable %s not found, or error "
		"reading MAT file\n",datavarname);
	return EXIT_FAILURE;
    }

    fprintf(stderr,"dimension = %d, dims[0] = %ld, dims[1] = %ld, data type=%d\n",
	    matvar->rank,
	    matvar->dims[0],
	    matvar->dims[1],
	    matvar->data_type);

    if ( MAT_T_DOUBLE != matvar->data_type ) {
	fprintf(stderr,"Data type needs to be IEEE 754 double precision but is not. See matio.h.\n");
	return EXIT_FAILURE;
    }

    long int nSweeps = matvar->dims[0];
    long int nSamples = matvar->dims[1];

    if (sweepNo >= nSweeps) {
	fprintf(stderr,"Requested sweep number is out of range.\n");
	return EXIT_FAILURE;	
    }

    fprintf(stderr,"Extracting sweep %ld.\n",sweepNo);

    const double *yData = static_cast<const double*>(matvar->data);
    FILE* f = fopen("/tmp/d.dat","wt");
    for(int i=0; i<nSamples; ++i)
    {
	double d = yData[i*nSweeps+sweepNo];
	fprintf(f,"%f\n",d);
    }
    fclose(f);

    Mat_VarFree(matvar);
    Mat_Close(matfp);
    return EXIT_SUCCESS;
}
