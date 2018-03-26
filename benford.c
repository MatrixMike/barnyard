/* Benford.c: read data on stdin and compute the relative frequency 
* distribution of the leading digits. Data should be delimited by whitespace:
* spaces, tabs, and newlines, and should be in decimal (i.e, not
* scientific) format. All data should be respresentable in double precision
* on the target platform.
*
* Prints a table showing the relative frequency of each digit, and, for
* comparison, the relative frequency predicted by "Benford's Law."
*
*  Compile: cc -o benford benford.c -lm
*
*  By Terry McConnell
*
*/ 

/*
	Benford's law, a curious empirical observation, says that the
	relative frequency of the most significant digit equaling k is commonly
	distributed as log((k+1)/k). The story is that 
	the pages near the beginning of the book in old tables  
	of common logarithms were often more heavily worn than pages near the
	end.  Since people usually went to such tables to look up the logarithms
	of data they observed, this would seem to imply a nonuniform 
	distribution for the most significant digit of the mantissa. 

	There is by now a sizable literature on Benford's law. See, for
	example, T.P. Hill, "The Significant-Digit Phenomenon", Amer. Math.
	Monthly, April 1995, 322-327, and the references given there.


For example, 117 home resale prices from the Phoenix area gave the following
output:
The number of items observed was: 117

Digit   Observed        Benford
1       0.393162        0.301030
2       0.128205        0.176091
3       0.000000        0.124939
4       0.000000        0.096910
5       0.008547        0.079181
6       0.034188        0.066947
7       0.145299        0.057992
8       0.119658        0.051153
9       0.170940        0.045757

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int
main()
{
	double freq[9] = {0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0};
	char buffer[10];
	double count = 0.0,datum,i=0.0;
	int digit,j;

	while(scanf("%lf",&datum) != EOF) {
 		count += 1.0;

              /* Compute the most significant digit and 
			increment the corresp. freq */

/* The following algorithm, which seems reasonable, doesn't work in 
    all cases. For example, if datum = 8 then digit = 7 (NeXT C-library):   
		x = log10(datum);
		digit = (int) floor(pow(10,x - floor(x)));
*/

		sprintf(buffer,"%-.0e",fabs(datum));
		digit =  buffer[0] - '0'; /* Yes, I'm aware this won't work
                                           for EBCDIC, and I don't care */

		freq[digit-1] += 1.0;
	}

	/* Print out a table of results. */

	printf("The number of items observed was: %.0f\n\n",count);
	printf("Digit\tObserved\tBenford\n");
	for(j=1;j<=9;j++){
		i +=1.0;
		printf("%.0f\t%f\t%f\n",i,freq[j-1]/count,
			log10((i+1)/i));
	}
}
