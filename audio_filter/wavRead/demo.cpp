#include "wavread.h"

int main(int argc, char* argv[]) {
        char input[50];
        if (argc == 1)
		strcpy(input, "demo.wav");
        else
		strncpy(input, argv[1], 50);
	
	WAVread wavread;
	wavread.open(input);
	wavread.printHeaderInfo();
	FILE* f1 = fopen("/tmp/ch1.dat","wt");
	FILE* f2 = fopen("/tmp/ch2.dat","wt");
	fprintf(f1,"; Sample Rate 48000\n; Channels 1\n");
	fprintf(f2,"; Sample Rate 48000\n; Channels 1\n");
	long n = 0;
	while (wavread.hasSample()) {
		WAVread::StereoSample s = wavread.getStereoSample();
		double t = (double)n / (double)(wavread.getFs());
		fprintf(f1,"%f %f\n",t,s.left);
		fprintf(f2,"%f %f\n",t,s.right);
	}
	fclose(f1);
	fclose(f2);
	wavread.close();
	return EXIT_SUCCESS;
}

