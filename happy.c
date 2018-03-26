/* happy.c: 

A nonzero integer n is called happy if the sequence f(n), f(f(n)), ...,
where f is computed by summing the squares of the decimal digits of n,
eventually terminates in 1,1,1, ... (It is easy to see that the sequence must
fall into some cycle: clearly  f(n) < 81*log(n). Since f(n) > n then entails
n^81 > 10^n we have f(n) <= n for all but finitely many n. 

It possible to prove a much better bound: f(n) <= n as soon as n >= b^2,
where b is the base.

A nonzero number is called ecstatic to base b [my terminology] if it
is happy in all bases 2...b.

The program tests single numbers for these properties or lists
all happy/ecstatic numbers in a specified range. The base may be changed also. 

	Author: Terry R. McConnell
*/


/* Compile: cc -o happy happy.c */

#include <stdlib.h>
#include <stdio.h>

#define VERSION "1.0"
#define USAGE "Usage: happy [-l -e -r <integer>] <integer>\n"
#define HELP  "Usage: happy [ -l -e -r <integer>] <integer>\n\
w/o options prints whether or not integer is happy to default radix(10).\n\
-r: change the default radix to the given integer.\n\
-l: print instead all happy integers <= the given one.\n\
-e: substitute ecstatic for happy. Ecstatic means happy to all radices \n\
below and equal to the default one.\n\n"

#define TRUE 1
#define FALSE 0
#define DELTA_LOOPS 10


/* This function returns the sum of squares of the digits */

int
f(int n , int rdx)
{
	int k = 0,m;

	while(n){
		m = n % rdx;  /* m is least significant digit */ 
		n = n / rdx;
		k += m*m;
	}
	return k;
}

/* This function returns TRUE if the number n is happy in the given radix,
   FALSE if not */

int
is_happy(int n, int rdx)
{
	int j,old_n,loops=10;	

	/* Recall that n is happy if and only if iterating the function f
           on n eventually produces a cycle of length one. We detect a
           cycle by periodically storing the current value of n and then
           comparing it with new values of n generated in the inner loop
           below. Each time through the outer loop the size of the inner
           loop is grown. When the size exceeds the length of the cycle
           the latter is sure to be detected. */

	while(TRUE){
		old_n = n;
		for(j=loops;j>0;j--)
			if((n = f(n,rdx))== old_n)break;
		if(n==old_n)break;
		loops += DELTA_LOOPS;
	}
	return n == 1 ? TRUE : FALSE;
}
		

int
main(int argc, char **argv)
{

	int num,i=1,r,ok;
	int radix = 10; 
	int lflag = FALSE;
	int eflag = FALSE;

	/* Process command line options */
	
	while(((i+1)<=argc)&&(argv[i][0] == '-')){
		if(argv[i][1] == 'h'){
			printf(HELP);
			return 0;
		}
		if(argv[i][1] == 'v'){
			printf("%s\n",VERSION);
			return 0;
		}
		if(argv[i][1]=='r'){
			/* make sure there is an arg */
			if((i+1) >= argc){
				fprintf(stderr,USAGE);
				return 1;
			}
			radix = atoi(argv[i+1]);
			/* Do sanity test on radix */
			if(radix < 2){
				fprintf(stderr,"Bad base %d\n",radix);
				return 1;
			}
			i += 2;
			continue;
		}
		if(argv[i][1]=='l'){
			lflag = TRUE;
			i++;
			continue;
		}
		if(argv[i][1]=='e'){
			eflag = TRUE;
			i++;
			continue;
		}
		/* Unknown option if we got to here */
		fprintf(stderr,USAGE);
		return 1;
	}

	/* Make sure there is one arg left */

	if((i+1) != argc){ 
		fprintf(stderr,USAGE);
		return 1;
	}

	num = atoi(argv[i]);
	num = num < 0 ? -num : num;   /* Normalize so that num is >= 0 */
	if(!num){
		fprintf(stderr,"Invalid number argument.\n");
		return 1;
	}

	if(lflag){
				/* Print heading */

		if(eflag) printf("Ecstatic numbers (radix <= %d) <= %d:\n",
						radix,num);
		else printf("Happy numbers (radix = %d) <= %d:\n",
						radix,num);

		for(i=1;i<=num;i++)
			if(eflag){ /* must be happy in all bases <= radix */
				ok = TRUE;
				for(r=2;r<=radix;r++)
					if(!is_happy(i,r))ok=FALSE;
				if(ok)
					printf("%d\n",i);
			}
			else
				if(is_happy(i,radix))
					printf("%d\n",i);
		return 0;
	}

	/* Normal operation: testing only num */

	if(is_happy(num,radix))
		printf("%d is happy.\n",num);

	else 
		printf("%d is not happy.\n",num);
	return 0;
}

