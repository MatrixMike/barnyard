/* sbernstein.c: Construct a bijection from injections using a constructive
   proof of the Schroder-Bernstein Theorem.

	By Terry R. McConnell

   Compile: cc -o sbernstein sbernstein.c

   For linking only the sbernstein subroutine, add -DNO_MAIN

	The Schroder-Bernstein Theorem says that if A and B are sets and
if one-one maps F:X -> Y and G:Y -> X exist, then there is a one-one and
onto map (bijection) T: X -> Y, hence X and Y have the same cardinality. The
purpose of this program is to illustrate one particular constructive proof
of the SB theorem. This proof is due to M. Hellman ( Amer. Math. Monthly,
68(1961), p. 770.) Here is a sketch with hopefully enough detail to make
the programming clear:

                                F
  X  |              ----------------------------->             |   Y
     |                                                         |
G(Y) +                                                         |
     +                                                         |
     .                                                         *
 A   .                                                         *   F(X)
     .                                                         *
     +                                                         *
     +                                                         |
     |                                                         |
     |                                                         |

	           <-------------------------------
                               G

Let f denote the composition G inverse followed by F inverse, which maps
the "inner" region, A = F(G(X)), of X (marked with ...) to X. One iterates this 
map until the resulting point leaves A (if it ever does.) Let z denote the
first point outside of A so obtained. Then T(x) is defined to be G-inverse
of x if z lands in the range of G (marked with +++). Otherwise (and this
includes the case that the iteration never leaves A) T(x) = x. 

	The theorem and the construction are not at all interesting for
finite sets, so we consider for our illustration only functions from the whole
numbers (natural numbers + 0) to the whole numbers. For external linkage,
the main routine of this file is called sbernstein. (See declaration below.)
It returns a pointer to a function of int returning int. You supply it with
pointers to implementations of F, G and their inverses. These must be functions
taking a single integer argument and returning an int. In addition, the
inverses must return -1 when given an argument not in the range of the original
functions.  A small main routine for testing is included below, so this
file can be compiled and run as is, with F(x) = 2x and G(y) = 3y.

	Note that the above sketched construction is not necessarily
EFFECTIVE, i.e, it is possible that it may not reach a decision in finite
time (e.g, if the iteration never terminates.) Accordingly, this program 
really only constructs an approximation to the desired bijection, although
it most cases it will be exact. The defined LOOP_LIMIT below
indicates how many times to iterate f before deciding that the sequence will
stay forever in A. (With the default test functions x = 0 is a fixed point
of the iteration, so LOOP_LIMIT will be exceeded when computing the 
bijection for x = 0. A notice of this fact is printed on the error stream.)
 
Open Question (well, open for me): is there a proof of SB that yields an
effective procedure in our context?

*/

#include<stdlib.h>
#include<stdio.h>
#define LOOP_LIMIT 100

/* sbernstein: 

   Returns a pointer to a bijective function from the whole numbers to 
   themselves. The functions f and g are one to one functions on the whole
   numbers with whole number values, but are not necessarily onto. The 
   function returned is built using an algorithm based upon a proof of
   the Schroder-Bernstein theorem. One must pass also pointers to 
   implementations of the inverses of f and g. These are required to return
   -1 when evaluated at a number which does not belong to their domains,
   i.e., to the ranges of f and g respectively 

   The value returned is actually just a pointer to the_function, which
   does the real work using stored pointers to the passed in functions with
   the integer arg it is passed. All sbernstein does is set up these 
   static pointers.

*/



static int (*F)(int);
static int (*G)(int);
static int (*FINV)(int);
static int (*GINV)(int);

static int the_function(int);

int(*sbernstein(int (*f)(int), int (*g)(int), int (*f_inv)(int),
 			int (*g_inv)(int)))(int) 
{
	/* store pointers to implementations for use by the_function */

	F = f;
	G = g;
	FINV = f_inv;
	GINV = g_inv; 
	return &the_function;
}

/* As explained above, this actually implements the algorithm to compute
   the bijection for the user-supplied arg. It uses stored pointers to 
   implementations of the functions F, G and their inverses. */

static int the_function(int x)
{
	int n,m,i;

	m = n = x;
	i = 0;
	while((n=FINV(GINV(n)))!=-1){  /* Loop until we leave A. See above */
		m = n;	
		if(i++ >= LOOP_LIMIT){
			fprintf(stderr,"Loop limit exceeded on arg %d. Function be only approximate.\n",x);
			break;
		}
	}

	if(i >= LOOP_LIMIT)return F(x);  /* Never left A */
	if(GINV(m)==-1)return F(x);     /* Left A outside range of G */
	return GINV(x);                /* Left A inside range of G */
}


/* test platform */

static int test_f(int);
static int test_g(int);
static int test_finv(int);
static int test_ginv(int);

/* This platform prints a table of values with this many rows: */
#define ROWS 20

#ifndef NO_MAIN
int main()
{

	int (*T)(int);
	int i=0;

	T = sbernstein(&test_f,&test_f,&test_finv,&test_ginv);

	printf("%s\t%s\t%s\t%s\n","n","f","g","bijection");
	printf("---------------------------------\n");
	while(i<ROWS){
		printf("%d\t%d\t%d\t%d\n",i,test_f(i),test_g(i),
			T(i));
		i++;
	}
	return 0;
}
#endif


/*  test function definitions here */

static int test_f(int x)
{
	return 2*x;
}

static int test_g(int x)
{
	return 3*x;
}

static int test_finv(int x)
{

	if(x%2)return -1;
	return x/2;
}

static int test_ginv(int x)
{
	if(x%3)return -1;
	return x/3;
}
	
