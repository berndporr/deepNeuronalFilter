#ifndef __MATLOADER
#define __MATLOADER

#include <stdlib.h>
#include <stdio.h>
#include <cassert>
#include "matio.h"

class MatLoader {

public:

    int load(const char* filename, const char* datavarname) {
	matfp = Mat_Open(filename,MAT_ACC_RDONLY);
	if ( NULL == matfp ) {
	    fprintf(stderr,"Error opening MAT file \"%s\"!\n",filename);
	    return -1;
	}
	
	fprintf(stderr,"Loading %s...\n",filename);
	matvar = Mat_VarRead(matfp,datavarname);

	if ( NULL == matvar ) {
	    fprintf(stderr,"Variable %s not found, or error "
		    "reading MAT file\n",datavarname);
	    return -1;
	}

	fprintf(stderr,"dimension = %d, dims[0] = %ld, dims[1] = %ld, data type=%d\n",
		matvar->rank,
		matvar->dims[0],
		matvar->dims[1],
		matvar->data_type);

	if ( MAT_T_DOUBLE != matvar->data_type ) {
	    fprintf(stderr,"Data type needs to be IEEE 754 double precision but is not. See matio.h.\n");
	    return -1;
	}

	nSweeps = matvar->dims[0];
	nSamples = matvar->dims[1];

	fprintf(stderr,"nSweeps = %ld, nSamples per sweep = %ld\n",nSweeps,nSamples);

	data = static_cast<double*>(matvar->data);
	return 0;
    }

    void saveAsTxt(const char* filename, const long int sweepIndex) const {
	FILE* f = fopen(filename,"wt");
	for(int i=0; i<nSamples; ++i)
	{
	    const double d = getData(sweepIndex, i);
	    fprintf(f,"%f\n",d);
	}
	fclose(f);
    }

    inline double getData(const long int sweepIndex, const long int sampleIndex) const {
	assert(nullptr != data);
	return data[sampleIndex * nSweeps + sweepIndex];
    }

    inline long int getNSamples() const {
	return nSamples;
    }

    ~MatLoader() {
	Mat_VarFree(matvar);
	Mat_Close(matfp);
    }

private:
    double* data = nullptr;
    long int nSweeps = 0;
    long int nSamples = 0;
    mat_t *matfp = nullptr;
    matvar_t *matvar = nullptr;
};

#endif
