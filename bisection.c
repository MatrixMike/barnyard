/* bisection.c: Implements the bisection method for finding roots. 
 *
 *  By Terry R. McConnell
 *
/*             

	Solves f(x) = 0, given an initial interval [a,b] such that f has
	differing signs at the endpoints. Quits when f((a+b)/2) is close
	enough to zero. Function f must be continuous. It must be implemented
	as double f(double) in this file or elsewhere. We  include an example
	implementation with f(x) = 3x^3 - x - 1. The endpoints may be
	specified with -a and -b options. They default to a = 0, b = 1.0.
	If the endpoints don't have the required property the program quits
	with an error. 

	The algorithm bisects the initial interval successively, until
	the value of f at the midpoint differs from zero by less than
	TOLERANCE, which defaults to .00000001. (It can be set on the
	command line with -t option.) The final result is the midpoint
	of the last interval. Which of the two bisected halves is retained
	at a given stage is determined by calculating f at the endpoints,
	so that the condition of different signs at the endpoints is
	preserved. 

	The program reports the current interval, and the values of f at the
	endpoints at each stage.

	The algorithm is also implemented in a user callable function.

*/

/* compile: cc -o bisection.c  -lm

      Use -D_SHORT_STRINGS if your compiler does not support multiline
          string constants.

      Use -DHAVEF and link object file containing implementation of 
      function declared as double f(double) if you want to solve a non
      default equation. In this case, also -DEQUATION= so the printout
      will give the correct heading.

      Use -DNOMAIN and -DHAVEF if you only want to link in function implemented
      below with declaration

  double bisection(double a, double b, double(*f)(double), double tolerance);

      It is up to the caller to make sure the endpoints and f are suitable.
      You're toast if they're not.

*/


#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#ifndef EQUATION
#define EQUATION "3x^3 - x - 1 = 0"
#endif
#define VERSION "1.0"
#define USAGE "bisection [ -a float -b float -t float -h -v]"
#ifndef _SHORT_STRINGS
#define HELP "\nbisection [ -a float -b float -t float -h -v ]\n\n\
Find root of linked function f using bisection method.\n\n\
-a: Use next argument as left endpoint of initial interval.\n\
-b: Use next argument as right endpoint of initial interval.\n\
-t: Use next argument as tolerance. Quit when f(midpoint) dips below this.\n\
-v: Print version number and exit. \n\
-h: Print this helpful information. \n\n"
#else
#define HELP USAGE
#endif

/* Default values */
#define TOLERANCE .00000001
#define LEFT 0.0
#define RIGHT 1.0
#define MAXPASS 256

#ifndef HAVEF
/* Here is the default function f's implementation. */

double f(double x){

	return 3.0*x*x*x - x - 1.0; 
}
#else
extern double f(double);
#endif

#ifndef NOMAIN
int
main(int argc, char **argv)
{
	double tolerance = TOLERANCE;
	double a = LEFT;
	double b = RIGHT;
	double c,d,e,mid;
	int j=0,n=1;

	/* Process command line */
	while(++j < argc){
		if(argv[j][0] == '-')
			switch(argv[j][1]){ 
				case 'a':
				case 'A':
					if(j+1 >= argc){
						fprintf(stderr,"%s\n",USAGE);
						exit(1);
					}
					a = atof(argv[j+1]);
					j++;
					continue;
				case 'b':
				case 'B':
					if(j+1 >= argc){
						fprintf(stderr,"%s\n",USAGE);
						exit(1);
					}
					b = atof(argv[j+1]);
					j++;
					continue;
				case 't':
				case 'T':
					if(j+1 >= argc){
						fprintf(stderr,"%s\n",USAGE);
						exit(1);
					}
					tolerance = atof(argv[j+1]);
					if(tolerance <= 0.0){
						fprintf(stderr,"tolerance must be positive.\n");
						exit(1);
					}
					j++;
					continue;	
				case 'v':
				case 'V':
					printf("%s\n",VERSION);
					exit(0);
				case '?':
				case 'h':
				case 'H':
					printf("%s\n",HELP);
					exit(0);
				default:
					fprintf(stderr,"bisection: unkown option %s\n",
						argv[j]);
					exit(1);
			}
	}


	/* Check necessary conditions */

	if( b < a ){
	        fprintf(stderr,"Error: initial endpoints are swapped.\n");	
		exit(1);
	}
	c = f(a); d = f(b);
	if(((c>0.0)&&(d>0.0))||((c<0.0)&&(d<0.0))){

		fprintf(stderr,"f has same sign at enpoints. Cannot continue.\n");
		exit(1);
	}	
		
	/* loop until desired tolerance */

	printf("\n\nBisection method solution of %s:\n\n",EQUATION);
	while(1){

		mid = (a+b)/2.0;
		e = f(mid);
		printf("%2d. f[%10.8f,%10.8f]  =  [%10.8f,%10.8f]\n",
				n,a,b,c,d,e);
		if(fabs(e) < tolerance){

			printf("\nSolution is x = %10.8f at tolerance %9.8f.\n\n",mid,tolerance);
			exit(0);
		}
		if( c > 0.0)
			if(e < 0.0) b = mid;
			else a = mid;
		if( c < 0.0)
			if(e > 0.0) b = mid;
			else a = mid;

	        c = f(a); d = f(b);
		n++;
		if(n>MAXPASS) exit(1); /* obviously not converging */
	}

	return 0;

}
#endif

double bisection(double a, double b, double(*f)(double),double tolerance)
{

	double mid,c,d,e;

	c = f(a); d = f(b);
	while(1){

		mid = (a+b)/2.0;
		e = f(mid);
		if(fabs(e) < tolerance)break;
		if( c > 0.0)
			if(e < 0.0) b = mid;
			else a = mid;
		if( c < 0.0)
			if(e > 0.0) b = mid;
			else a = mid;

	        c = f(a); d = f(b);
	}
	return mid;
}
