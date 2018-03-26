/* ordinal.c: print the set-theoretic definition of the finite ordinal given
   on the command line. Recall that the set theoretic definition of the first
   three ordinals, starting from 0 is:

	0, {0}, {0,{0}}, ... 

   where 0 denotes the empty set. 

	By Terry R. McConnell

    compile: cc -o ordinal ordinal.c
   
*/


#include <stdio.h>
#include <stdlib.h>

#define USAGE "ordinal: usage: ordinal n"

#define MAX_N 256  /* This is wayyyyyyyyy too big! */

/* The following routine does all the work. It is fed an array of pointers 
   of length N, set up to point as follows:

   p[0] --> p[1] --> ... --> p[N-1] --> NULL
 
   Think of each --> as pointing from a set to the first of its members. 
*/


void wrap(int N,void *(*p[]))
{
	int i;

	if(!N){   /* base of recursion */
		printf("0");
		return;
	}

	printf("{");  /* wrap set brackets around */

	for(i=N;i>0;i--){ /* iterate over elements of set */
		wrap(N-i,&(p[i])); /* call recursively on element */
		if(i > 1)printf(","); /* omit comma after last el't */
	}

	printf("}");  /* close bracket opened above */
	return;
}


int main(int argc, char **argv)
{
	
	void  *(*q[MAX_N]);
	int N,n,i;

	/* sanity checks */

	if(argc != 2){
		fprintf(stderr,"%s\n",USAGE);
		return 1;
	}

	n = N = atoi(argv[1]);

	if((n<0)||(n > MAX_N)){
		fprintf(stderr,"ordinal: value %d out of range. (0-%d)\n",
			n,MAX_N);
		return 1;
	}


	/* prepare array of pointers */

	for(n=0;n<N;n++)q[n]=(void *)&(q[n+1]);	
	q[n]=(void *)NULL;

	/* do the work! */
	wrap(N,q);

	/* clean up */
	printf("\n");
	return 0;
}	
