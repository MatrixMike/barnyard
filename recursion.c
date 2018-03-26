/* recursion.c: Implement recursive/primitive recursive functions in C 

 	This file declares a number of functions that build up to the
  	world's slowest implementation of the power function (x^y). The main
        program is just a simple test platform for the power function. If
        you wish, you could build other recursive functions and a different
        test platform once you understand the pattern set out below. 

        compile: cc -o recursion -DPROFILE recursion.c

        You could leave off -DPROFILE, but that includes variables that track
        how many times each building block routine is called. You can then
        have your main platform report these values. Otherwise, you just have
        a very slow implementation of simple arithmetic functions.
        
*/

/* By Terry R. McConnell */

#include <stdio.h>
#include <stdlib.h>

/* 
   The idea is that, after defining the base functions and rules, the
   code for the remaining functions represents the proof that they are
   primitive recursive (recursive).

   A reference to the theory of recursive functions, which uses the notation
   we use below, is "Introduction to Mathematical Logic, 3rd Ed," Elliot
   Mendelson, Wadsworth&Brooks/Cole, 1987, pp 132-149.

   Except for some of the base routines, all functions have the declaration
   
		int foo(int n, int* args)

   The pointer argument indicates the first in a variable length list of 
   integer args. Argument n is the length of the list. Increment args in the
   body of foo to access each succesive variable. The function returns the
   value it computes.

*/


/* The following are used for profiling */
#ifdef PROFILE
int Z_calls;
int N_calls;
int substitute_calls;
int project_calls;
int recurse_calls;
int mu_calls;
int add_calls;
int mult_calls;
int power_calls;
int one_calls;
#endif

/* Base functions */

/* Z: the zero function */

int Z( int n, int *x){
#ifdef PROFILE
	Z_calls++;
#endif
	return 0;       /* This is an easy one */
}

/* N: the successor function */

int N( int n, int *x){
#ifdef PROFILE
	N_calls++;
#endif
	return (*x) + 1;
}

/* The projection functions. Since there are infinitely many of these, we
   must cheat a bit by including an extra variable in the declaration to
   select which projection. Thus, strictly speaking the following is really
   a schema, or rule, for constructing projection functions. */

int project(int n, int i, int *x){

#ifdef PROFILE
	project_calls++;
#endif
	return x[i-1];
}


/* The composition  rule: we are allowed to form
 
	f(h_1(x_1...,x_n), h_2(...), ... , h_n(...))

from recursive functions f and h_1, ..., h_n. */

int substitute(int n, int *x, int(*f)(int, int *), int(**h)(int,int *)){

	int *hvals, rval, i;
#ifdef PROFILE
	substitute_calls++;
#endif
	/* Allocate and fill array of return values to pass to f */

	hvals = (int *)malloc(n*sizeof(int));
	for(i=0;i<n;i++)
		hvals[i] = h[i](n,x);

	rval = f(n, hvals);
	
	/* clean up */
	free(hvals);
	return rval;
}

/* The recursion rule: given recursive functions g and h (data), and
   parameters x_1...x_n, we are allowed to recursively define a function
   f(x_1,...,x_n,y) of n+1 variables as follows:

	f(x_1,...,x_n,0) = g(x_1,...,x_n), and
	f(x_1,...,x_n,y+1) = h(x_1,...,x_n,y,f(x_1,...,x_n,y)).

    The case of no parameters (n = 0) is allowed. 
*/

int recurse(int n, int *x, int y, int(*g)(int,int*), int(*h)(int, int *)){

	int *xyf, rval, i;

#ifdef PROFILE
	recurse_calls++;
#endif
	if(y==0)
		return g(n,x);
	else {
		/* build argument for h */
		xyf = (int *)malloc((n+2)*sizeof(int));
		for(i=0;i<n;i++)xyf[i] = x[i];
		xyf[i++] = y;
		xyf[i] = recurse(n,x,y-1,g,h);  /* here's the recursion! */
		rval = h(n+2,xyf);
		free(xyf);  /* clean up */
		return rval;
	}
}

/* The mu operator: searches (possibly unboundedly,) for the smallest value
   y such that f(x_1,...,x_n,y) = 0. This value must exist.  It defines a
   function of x_1,...,x_n. A function calling this is recursive, but not 
   primitive recursive unless there is another way to build it without using mu.
*/

int mu(int n, int *x, int(*f)(int, int *)){

	int *xy, i, rval, y;

#ifdef PROFILE
	mu_calls++;
#endif

	/* allocate and fill args to pass to f */ 
	xy = (int *)malloc((n+1)*sizeof(int));

	for(i=0;i<n;i++)xy[i] = x[i];
	xy[n]=0; /* initial value of y */

	while(f(n+1,xy)) xy[n] += 1;  /* search */

	rval = xy[n];
	free(xy);    /* clean up */
	return rval;
}

/* From here on, all functions must use only the base functions, rules above,
   and previously defined functions in their definitions. Those that avoid use
   of mu are primitive recursive. The rules assume, and you must conform to,
   the following declaration for all derived functions foo:

	int foo( int, int *);

   Note that functions are allowed to handle the case of 0 variables by
   returning a constant value. 
*/

/* The identity function of 1 variable */

int id(int n, int *x){ /* really only depends on first var */

	return project(1,1,x);
}

/* Helpers for defining addition */
/* last: return last variable.  */

int last(int n, int *x){

	return project(n,n,x);
}

/* inc_4th: return 4th variable + 1. This will be the h function in the
   recursion for add() below. Thus it has 2 variables beyond the 2 arguments
   of add. The last (4th) variable holds the recursively passed function
   value, so that's what gets added to. */

int inc_4th(int n, int *x){

	int rval;
	int (**hs)(int, int*);

	hs = ( int(**)(int,int*) )malloc(4*sizeof(int(*)(int,int*)));
	hs[0]=hs[1]=hs[2]=hs[3]=last;  /* only the last one gets used */

	rval = substitute(4, x, N, hs);
	free(hs);   /* clean up */
	return rval;
}

/* add(2,x,y): return x + y */

int add(int n, int *x){

#ifdef PROFILE
	add_calls++;
#endif
	return recurse(2,x,x[1],id,inc_4th);
}

/* Similarly, we prepare for multiplication. h data for the multiplication
   recursion. */

int addto4th(int n, int *x){

	int rval;
	int (**hs)(int, int*);

	hs = ( int(**)(int,int*) )malloc(4*sizeof(int(*)(int,int*)));
	hs[0]= id;
	hs[1]=hs[2]=hs[3]=last;  /* only the 3rd gets used */

	rval = substitute(4, x, add, hs);
	free(hs);
	return rval;
}


int mult(int n, int *x){

#ifdef PROFILE
	mult_calls++;
#endif
	return recurse(2,x,x[1],Z,addto4th);
}


/* h data for power recursion */
int multonto4th(int n, int *x){

	int rval;
	int (**hs)(int, int*);

	hs = ( int(**)(int,int*) )malloc(4*sizeof(int(*)(int,int*)));
	hs[0]= id;
	hs[1]=hs[2]=hs[3]=last;  /* only the 3rd gets used */

	rval = substitute(4, x, mult, hs);
	free(hs);
	return rval;
}

/* The function identically one: base of power recursion */

int one(int n, int *x){

	int rval;
	int (**hs)(int, int*);

#ifdef PROFILE
	one_calls++;
#endif

	hs = ( int(**)(int,int*) )malloc(2*sizeof(int(*)(int,int*)));
	hs[0]= Z;
	hs[1]=id;  
	rval =  substitute(2,x,N,hs);
	free(hs);
	return rval;
}

/* power(2,x,y): calculate x^y */

int power(int n, int *x){

#ifdef PROFILE
	power_calls++;
#endif
	return recurse(2,x,x[1],one,multonto4th);
}


/* Test platform */

int main()
{
	int args[2] = {8,4};

	printf("8 to power 4 is %d\n", power(2,args));
#ifdef PROFILE
	printf("%s\t%s\t%s\t%s\t%s\t%s\n","Z","N","proj","subst","recurse",
	 "add");
	printf("%d\t%d\t%d\t%d\t%d\t%d\n",Z_calls, N_calls, project_calls,
		substitute_calls, recurse_calls, add_calls);
#endif
	return 0;
}
