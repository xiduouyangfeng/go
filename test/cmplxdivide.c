// Copyright 2010 The Go Authors. All rights reserved.
// Use of this source code is governed by a BSD-style
// license that can be found in the LICENSE file.

// This C program generates the file cmplxdivide1.go. It uses the
// output of the operations by C99 as the reference to check
// the implementation of complex numbers in Go.
// The generated file, cmplxdivide1.go, is compiled along
// with the driver cmplxdivide.go (the names are confusing
// and unimaginative) to run the actual test. This is done by
// the usual test runner.
//
// The file cmplxdivide1.go is checked in to the repository, but
// if it needs to be regenerated, compile and run this C program
// like this:
//	gcc '-std=c99' cmplxdivide.c && a.out >cmplxdivide1.go

#include <complex.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

#define nelem(x) (sizeof(x)/sizeof((x)[0]))

double f[] = {
	0,
	1,
	-1,
	2,
	NAN,
	INFINITY,
	-INFINITY,
};

char*
fmt(double g)
{
	static char buf[10][30];
	static int n;
	char *p;
	
	p = buf[n++];
	if(n == 10)
		n = 0;
	sprintf(p, "%g", g);
	if(strcmp(p, "-0") == 0)
		strcpy(p, "negzero");
	return p;
}

int
iscnan(double complex d)
{
	return !isinf(creal(d)) && !isinf(cimag(d)) && (isnan(creal(d)) || isnan(cimag(d)));
}

double complex zero;	// attempt to hide zero division from gcc

int
main(void)
{
	int i, j, k, l;
	double complex n, d, q;
	
	printf("// skip\n");
	printf("// # generated by cmplxdivide.c\n");
	printf("\n");
	printf("package main\n");
	printf("var tests = []Test{\n");
	for(i=0; i<nelem(f); i++)
	for(j=0; j<nelem(f); j++)
	for(k=0; k<nelem(f); k++)
	for(l=0; l<nelem(f); l++) {
		n = f[i] + f[j]*I;
		d = f[k] + f[l]*I;
		q = n/d;
		
		// BUG FIX.
		// Gcc gets the wrong answer for NaN/0 unless both sides are NaN.
		// That is, it treats (NaN+NaN*I)/0 = NaN+NaN*I (a complex NaN)
		// but it then computes (1+NaN*I)/0 = Inf+NaN*I (a complex infinity).
		// Since both numerators are complex NaNs, it seems that the
		// results should agree in kind.  Override the gcc computation in this case.
		if(iscnan(n) && d == 0)
			q = (NAN+NAN*I) / zero;

		printf("\tTest{complex(%s, %s), complex(%s, %s), complex(%s, %s)},\n",
			fmt(creal(n)), fmt(cimag(n)),
			fmt(creal(d)), fmt(cimag(d)),
			fmt(creal(q)), fmt(cimag(q)));
	}
	printf("}\n");
	return 0;
}
