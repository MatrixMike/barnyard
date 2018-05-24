/* Euclid.c: Implements the Euclidean algorithm for finding the
 * greatest common divisor of integers a and b: gcd(a,b).
 *
 *  By Terry R. McConnell
 *
 *                              Theory
 *
 * The greatest common divisor is defined for all pairs of nonzero integers
 * a and b. Let us define gcd(a,b) to be a natural number g
 * such that g|a and g|b and such that d|a and d|b --> d|g. There can
 * be only one such positive number since any two would have to divide each
 * other. The existence of at least one such number follows from the
 * Euclidean algorithm, discussed in the next paragraph. (We show below that 
 * g has the familiar interpretation as * the greatest common divisor of 
 * a and b.) 
 *
 * The Euclidean algorithm for gcd(a,b) works as follows: assume 0 < a <= b and 
 * apply the division algorithm to get
 *
 * 	b = a*q + r,  where 0 <= r < a
 *
 * If r = 0, then the gcd is a. Otherwise, gcd(a,b) = gcd(r,a). 
 * The first case is obvious. In the second case, let g = gcd(a,b)
 * and c = gcd(r,a). Since g|a and g|b we have g|r. Hence g|c. 
 * Since c|r and c|a, we have c|b, hence c|g. Since g and c divide each
 * other, they are equal.
 *
 * Notably, gcd(a,b), when it exists, can be expressed as an integer
 * combination of a and b, i.e., gcd(a,b) = c*a + d*b for certain integers
 * c and d. Here is a proof: Let A = {n: n is a natural number of the form
 * c*a + d*b for integers c and d}. Clearly A is nonempty, so it has a least
 * element, say m. Let g = gcd(a,b). Clearly g|m since m belongs to A. It
 * suffices to show m|g. Divide a by m: a = mq + r, with 0 <= r < m. Since
 * r = a - mq, r would a smaller element of A unless r = 0. Since m is
 * the smallest, r = 0. Thus m|a. By a similar argument, m|b. Thus m|g as
 * required.
 *
 * Now we can show that gcd(a,b), as defined above, is also the greatest
 * common divisor of a and b. Since 1 always divides a and b, and any
 * common divisor cannot exceed min(|a|,|b|), the set of common divisors is
 * a nonempty finite set of natural numbers. It therefore has a largest element
 * which we denote as k. Let g = gcd(a,b). Since k divides both a and b, it
 * divides any integer combination of a and b, hence k|g. Thus k <= g, since
 * both are positive. But g <= k, since both are common divisors and k is the
 * largest such.
 *
 * Our implementation is recursive and also yields the coefficients c and d
 * such that gcd(a,b) = c*a + d*b.
 *
*/


/* compile: cc -o euclid  euclid.c

      Use -D_SHORT_STRINGS if your compiler does not support multiline
          string constants.

   Run euclid -h for usage information.

*/


#include<stdio.h>
#include<stdlib.h>

#define VERSION "1.1"
#define USAGE "euclid [ -h -v -- ] a b"
#ifndef _SHORT_STRINGS
#define HELP "\neuclid [ -h -v --] a b\n\n\
Find the greatest common divisor of a and b. Express as c*a + d*b. \n\n\
--: Signal end of options so that negative a and or b can be input.\n\
-v: Print version number and exit. \n\
-h: Print this helpful information. \n\n"
#else
#define HELP USAGE
#endif

/* my_gcd: return the greatest common divisor of a and b, or -1 if it
 * is not defined. Return through the pointer arguments the integers such
 * that gcd(a,b) = c*a + b*d.
 */

int
my_gcd (int a, int b, int *c, int *d)
{
  int q, r, C, D, rval;
  int sgn_a = 1, sgn_b = 1, swap = 0;

  /* Normalize so that 0 < a <= b */
  if ((a == 0) || (b == 0))
    return -1;
  if (a < 0)
    {
      a = -a;
      sgn_a = -1;
    }
  if (b < 0)
    {
      b = -b;
      sgn_b = -1;
    }
  if (b < a)
    {
      int t = b;
      b = a;
      a = t;
      swap = 1;
    }

  /* Now a <= b and both >= 1. */

  q = b / a;
  r = b - a * q;
  if (r == 0)
    {
      if (swap)
	{
	  *d = 1;
	  *c = 0;
	}
      else
	{
	  *c = 1;
	  *d = 0;
	}
      *c = sgn_a * (*c);
      *d = sgn_b * (*d);
      return a;
    }

  rval = my_gcd (a, r, &C, &D);
  if (swap)
    {
      *d = (C - D * q);
      *c = D;
    }
  else
    {
      *d = D;
      *c = (C - D * q);
    }
  *c = sgn_a * (*c);
  *d = sgn_b * (*d);
  return rval;
}


int
main (int argc, char **argv)
{
  int g, a, b, c, d;
  int j = 0;

  /* Process command line */
  while (++j < argc)
    {
      if (argv[j][0] == '-')
	switch (argv[j][1])
	  {
	  case '-':
	    ++j;
	    break;
	  case 'v':
	  case 'V':
	    printf ("%s\n", VERSION);
	    exit (0);
	  case '?':
	  case 'h':
	  case 'H':
	    printf ("%s\n", HELP);
	    exit (0);
	  default:
	    fprintf (stderr, "euclid: unkown option %s\n", argv[j]);
	    fprintf (stderr, "%s\n", USAGE);
	    exit (1);
	  }
      break;
    }

  if (j >= argc)
    {
      fprintf (stderr, "euclid: usage error.\n");
      fprintf (stderr, "%s\n", USAGE);
      return 1;
    }
  a = atoi (argv[j++]);
  if (j >= argc)
    {
      fprintf (stderr, "euclid: usage error.\n");
      fprintf (stderr, "%s\n", USAGE);
      return 1;
    }
  b = atoi (argv[j++]);

  g = my_gcd (a, b, &c, &d);
  if (g == -1)
    {
      fprintf (stderr, "euclid: gcd(%d,%d) is not defined.\n", a, b);
      return 1;
    }
  printf ("gcd(%d,%d) = %d = (%d)*(%d)+(%d)*(%d).\n", a, b, g, c, a, d, b);
  return 0;

}
