#include "circle_fit.h"
#include <stdio.h>
#include <math.h>
#include <stdlib.h>

double frand(){
	return (double)rand() / RAND_MAX;
}

int main(){
	int i;
	unsigned int n = 10;
	double *pt;
	double c[2], r, err;
	
	pt = (double*)malloc(sizeof(double) * 2*n);
	
	printf("%%!\n72 72 scale\n4.25 5.5 translate\n0.002 setlinewidth\n");
	for(i = 0; i < n; ++i){
		double angle = 2*M_PI*(frand()-0.5);
		double r = 1 + 0.1*frand();
		pt[2*i+0] = r * cos(angle);
		pt[2*i+1] = r * sin(angle);
		printf("newpath %f %f 0.01 0 360 arc closepath fill\n", pt[2*i+0], pt[2*i+1]);
	}
	
	circle_fit(100, n, pt, 1e-10, c, &r, &err);
	printf("newpath %f %f %f 0 360 arc closepath stroke\n", c[0], c[1], r);
	printf("showpage\n");
	
	free(pt);
	return 0;
}

