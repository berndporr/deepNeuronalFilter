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

    matvar_t *matvar = Mat_VarRead(matfp,datavarname);

    if ( NULL == matvar ) {
	fprintf(stderr,"Variable %s not found, or error "
		"reading MAT file\n",datavarname);
	return EXIT_FAILURE;
    }

    fprintf(stderr,"rank = %d, dims[0] = %ld, dims[1] = %ld\n",
	    matvar->rank,
	    matvar->dims[0],
	    matvar->dims[1]);

    long int nSweeps = matvar->dims[0];
    long int nSamples = matvar->dims[1];

    const double *yData = static_cast<const double*>(matvar->data);
    FILE* f = fopen("/tmp/d.dat","wt");
    for(int i=0; i<nSamples; ++i)
    {
	double d = yData[i*nSweeps];
	fprintf(f,"%f\n",d);
    }
    fclose(f);

    Mat_VarFree(matvar);
    Mat_Close(matfp);
    return EXIT_SUCCESS;
}
