#include <stdlib.h>
#include <stdio.h>
#include <matio.h>
#include "matloader.h"

char datavarname[] = "data";
char outputfile[] = "/tmp/sweep.dat";

int main(int argc,char **argv)
{
    if (argc < 2) {
	fprintf(stderr,"Usage: %s matlabmatfile.mat sweepIndex\n",argv[0]);
	return -1;
    }
    
    long int sweepNo = 0;
    if (argc > 2) {
	sweepNo = atoi(argv[2]);
    }

    MatLoader matloader;
    matloader.load(argv[1], datavarname);
    matloader.saveAsTxt(outputfile, sweepNo);
    return EXIT_SUCCESS;
}
