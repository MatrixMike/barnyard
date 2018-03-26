/* pack.c: Illustrate an efficient "information packing" function.
 *
 * 	By Terry R. McConnell
 *
 * Let W = {0,1,2,...} be the set of whole numbers. A recursive function
 * C: WxW --> W is an information packing function if it is 1-1 and there
 * are recursive functions T ("tail") and H ("head") such that
 *
 *        H(C(x,y)) = x and T(C(x,y)) = y.
 *
 * Such sets of functions are of interest in recursion theory because they
 * allow functions of many variables to be treated as functions of one
 * variable. 
 *
 * One example of an information packing function is C(x,y) = [(x+y)^2 +
 * (x+y) + 2x]/2. (Note that the expression in brackets is always even.) This
 * example is particularly efficient because it maps WxW onto W. This
 * program illustrates this example by implementing the head and tail functions
 * as recursive, or primitive recursive functions. The head and tail are
 * documented below with their implementations. 
 *
 * To see that C(x,y) is a bijection, it suffices to show 2C(x,y) is a 
 * bijection from WxW onto the even integers. Introduce the auxiliary
 * function v(n) = n^2 + n, which is clearly strictly increasing onto
 * a certain subset of the even whole numbers. It is easy to see that
 * with n = x+y,
 *
 * 2C(x,y) = v(n) + 2x < v(n+1), 0 <= x <= n, n = 0,1,...
 *
 * and over the stated range, (n,x) covers the set of all even
 * whole numbers exactly once. The desired result follows, since (x,y) ->
 * (x+y,x) is 1-1. 
 *
 * The function C, and variants of it, appear in a number of texts on the theory
 * of computation. See, e.g., problem 10.3-4 in Marvin Minsky, Computation: 
 * finite and infinite machines, Prentice Hall, Englewood Cliffs, N.J., 1967.
 *
 * Run with -h option to see usage information.
 *
 */

/* compile: cc -o pack pack.c

      Use -D_SHORT_STRINGS if your compiler does not support multiline
          string constants.

      These are very slow and are included only to prove a point:

      Use -DPRIMITIVE_RECURSIVE to implement all functions as primitive 
      recursive functions.

      Use -DRECURSIVE to implement all functions as recursive functions.

*/

/* Set these appropriately for your own platform: */
#define _GNU_SOURCE
#define USE_LONG_LONG   /* or USE_LONG or USE_INT */

#include<stdio.h>
#include<stdlib.h>
#include<limits.h>

#ifdef USE_LONG_LONG
#define CONVERT atoll
#define TYPEMAX ULONG_LONG_MAX
#define PRINT_AS "llu"  /* conversion after % in printf */
typedef unsigned long long whole;  
#endif
#ifdef USE_LONG
#define CONVERT atol
#define TYPEMAX ULONG_MAX
#define PRINT_AS "lu"
typedef unsigned long whole;
#endif
#ifdef USE_INT
#define CONVERT atoi
#define TYPEMAX UINT_MAX
#define PRINT_AS "u"
typedef unsigned int whole;
#endif

#define VERSION "1.0"
#define USAGE "pack [ -1 <z> -h -v] [x y]"
#ifndef _SHORT_STRINGS
#define HELP "\n\npack [ -1 <z> -h -v ] [x y]\n\n\
Pack information in whole numbers x and y into a single whole number z and \n\
print the result. \n\n\
-1: Unpack the information in the whole number z and print result as x y.\n\
-v: Print version number and exit. \n\
-h: Print this helpful information. \n\n"
#else
#define HELP USAGE
#endif

/* The implementation of the packing function, C(x,y).  */

whole pack( whole x, whole y)
{
	return ((x+y)*(x+y) + (x+y) + 2*x)/2;
	/* Well, that was pretty easy, wasn't it? */
}

/* The auxiliary function V */

whole V(whole z){

	return z*z + z;
}


/* U, an integer valued "inverse" of V. I.e., U(x) is the unique whole
 * number n that satisfies
 *
 * (*) V(n) <= x < V(n+1). 
 *
 * */

#ifdef PRIMITIVE_RECURSIVE
/* Implement U as a primitive recursive function. I claim that the
 * following is a recursion for U:
 *
 * U(x+1) = U(x) + I(U^2(x) + 3U(x) + 1 - x)
 * U(0) = 0
 *
 * where I is the indicator of the set {0} and the subtraction has
 * a - b = 0 when a < b. It suffices to prove that the function defined by
 * this recursion satisfies (*). We do this by induction on x. For x = 0 this
 * is obvious, so suppose (*) holds for n = U(x). We must show that 
 *
 * V(U(x+1)) <= x+1 < V(U(x+1)+1)   (**) 
 *
 * Case 1: U^2(x) + 3U(x) + 1 = x. Then U(x+1) = U(x) + 1, so
 * V(U(x+1)) = U^2(x) + 3U(x) + 1 = x < x+1 so the left side of (**) holds
 * with <. OTOH V(U(x+1)+1) = V(U(x)+2) = U^2(x) + 6U(x) + 4 which is clearly
 * > x+1. 
 *
 * Case 2: U^2(x) + 3U(x) + 1 > x  (This is the only alternative allowed
 * by the inductive hypothesis.) Then U(x+1) = U(x) so V(U(x+1)) =
 * V(U(x)) <= x by the inductive hypothesis. OTOH, V(U(x+1)+1) = V(U(x)+1)
 * = U^2(x) + 3U(x) + 2 > x + 1. 
 *
 * For large z this is very slow and consumes lots of stack. */

whole U(whole z){

	whole w;

	if(z == 0)return 0;
	w = U(z-1);
	if(w*w + 3*w + 1 == z)return w + 1;
	return w;
}
#elif defined( RECURSIVE )
/* We use the "mu" operator, i.e., linear search for the largest n such
 * V(n) <= z. For large z this is slow. */
whole U(whole z){

	whole i = 0;

	while(V(i+1) <= z)i++;
	return i;
}
#else 

/* Implement U `efficiently'. Since this may be passed the largest possible
 * number representable, we must be careful not to generate anything larger
 * than z during the calculation. We use a bisection method and are
 * careful about the possibililty of overflow. 
*/ 

whole U(whole z){

	whole top = z, bot = 0,m,n;

	while(1){
		/* Compute the integer average of top and bot */
		if((top%2)&&(bot%2))
			m = 1 + top/2 + bot/2;
		else m = top/2 + bot/2;

		/* Are we done ? */
		if((top==m)||(bot==m))break;

		/* Head off the possibility that V(m) will overflow. */
		/* First consider the possibility that m*m overflows. */
		n = m*m;
		if((n % m) || (n == m) || (n == 0)) { 
			top = m;
			continue;
		}
		/* OK, m*m does not overflow, but V(m) still could. */
		if(m > TYPEMAX - n){
			top = m;
			continue;
		}
		/* OK, V(m) = n + m really is what it says it is. */
		if(n + m > z)
			top = m;
		else bot = m;
	}
	return m;
}
#endif

whole head(whole z){
	return (2*z - V(U(2*z)))/2;
}
	
whole tail(whole z){
	return U(V(U(2*z))) - head(z);
}

int
main(int argc, char **argv)
{
	int j=0;
	whole x,y,z,w,t;

	/* Process command line */

	while(++j < argc){
		if(argv[j][0] == '-')
			switch(argv[j][1]){ 
				case '1':
					z = (whole)CONVERT(argv[j+1]);
					j++;
					if(z > TYPEMAX/2)fprintf(stderr,"pack: value is too big. May not unpack correctly.\n");

					printf("%"PRINT_AS" %"PRINT_AS"\n",head(z),tail(z));
					return 0;
				case 'v':
				case 'V':
					printf("%s\n",VERSION);
					exit(0);
				case '?':
				case 'h':
				case 'H':
					printf("%s\n",HELP);
#if !defined(RECURSIVE) && !defined(PRIMITIVE_RECURSIVE)
					t = U(TYPEMAX);
					w = TYPEMAX - V(t);
					if(2*t > w)t = t-1;
					printf("Maximum x + y = %"PRINT_AS".\n",t);
#endif
					printf("Maximum value handled by -1 option: z = %"PRINT_AS".\n\n",TYPEMAX/2);
					return 0;

				default:
					fprintf(stderr,"pack: unkown option %s\n",
						argv[j]);
					return 1;
			}
		else  break;
	}

	if(j < argc)
		x = (whole)CONVERT(argv[j++]);
	else {
		fprintf(stderr,"%s\n",USAGE);
		exit(1);
	}
	if(j < argc)
		y = (whole)CONVERT(argv[j++]);
	else {
		fprintf(stderr,"%s\n",USAGE);
		exit(1);
	}

	/* Check to make sure the arguments won't produce an encoding that's
	 * too big. */
	t = pack(x,y);
#if !defined(RECURSIVE) && !defined(PRIMITIVE_RECURSIVE)
	if((x != head(t))||(y != tail(t)))
		fprintf(stderr,"pack: warning: x or y is too big! Will not unpack correctly.\n");
#endif
			
	printf("%"PRINT_AS"\n",t);
	return 0;
}
