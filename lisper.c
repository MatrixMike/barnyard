/* lisper.c: print a random well-formed paranthesis expression to stdout.
 * I.e., things like (()(()()))
 *
 * 	By Terry R. McConnell
 */

/* The program name comes from the unofficial reading of the lisp programming
 * language acronym: Lots of SIlly Parentheses. (Origin unknown.) 
 *
 * Pure parenthesis expressions form arguably the simplest nontrivial example
 * of a formal language grammar, and as such play an important role in the
 * foundations of mathematics and computer science. This program generates
 * random examples of such expressions. 
 *
 * It is natural to view printing "(" as a kind of push operation and ")" as a
 * matching pop. One could simply push and pop with equal probability but
 * this has the potential to generate extremely long expressions. The reason
 * is that the recursion depth in that case forms a simple symmetric random
 * walk on the nonnegative integers, and such a random walk is well known to be
 * null recurrent. (See, e.g., F. Spitzer, Principles of Random Walk, 
 * Van Nostrand, Princeton, 1964.) In particular this means that, while
 * the stack will become empty eventually (at which point the corresponding
 * parenthesis expression will be well balanced) this takes infinite expected
 * time. Therefore, we govern the behaviour of this program using two 
 * parameters. The minimum depth parameter insures that the parenthesis
 * expression will be nested to at least the desired depth. The bias parameter
 * ensures that pushes are more likely than pops until the minimum depth is
 * achieved. Thereafter pops are more likely.   
 */

/* compile: cc -o lisper lisper.c

      Use -D_NO_RANDOM if your library doesn't have random/srandom. Most do,
       	but the only truly portable RNG is rand/srand. Unfortunately it has
        very poor performance, so you should use random if possible.

      Use -D_MAX_RAND=  to set the size of the maximum value returned by
         random(). The portable RNG rand() always returns a maximum of 
         RAND_MAX (defined in stdlib.h), but some implementations of random
         do not use this value. Read the man page for random to be sure. A
	 common value is 2^31-1 = 2147483647. In so, and this is not the
         value of RAND_MAX on your system, you would compile with
         -D_MAX_RAND=214748367.

      Use -D_SHORT_STRINGS if your compiler does not support multiline
          string constants.
*/


#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<time.h>

#define VERSION "1.0"
#define USAGE "lisper [ -b <n> -d <n> -s <n> -h -v]"
#ifndef _SHORT_STRINGS
#define HELP "\n\nlisper [ -b <n> -d <n> -s <n> -h -v ]\n\n\
Print a random well-formed parenthesis expression. \n\n\
-s: Use next argument as RNG seed. (Otherwise use system time as seed.)\n\
-b: Use next argument as the bias parameter. (0 <= n <= 0.5. Default=0.1.) \n\
    Smaller values tend to produce longer expressions.\n\
-d: Use next argument as minimum depth parameter. (Default=4.) The\n\
    generated expression will be nested to at least this depth.\n\
-v: Print version number and exit. \n\
-h: Print this helpful information. \n\n"
#else
#define HELP USAGE
#endif


#ifdef _NO_RANDOM
#define RANDOM rand
#define SRANDOM srand
#else
#define RANDOM random
#define SRANDOM srandom
#endif

/* Default values */
#define BIAS .1
#define MIN_DEPTH 4
#define INITIAL_SEED 2718
#ifndef _MAX_RAND
#define _MAX_RAND RAND_MAX
#endif

static int depth = 0; /* records depth (recursive) of expression */

/* U: return a uniformly distributed random number on [0,1]. 
*/

double U(void)
{
	return ((double)RANDOM())/((double)_MAX_RAND);
} 

void push(void)
{
	printf("(");
	depth++;
}

void pop(void)
{
	printf(")");
	depth--;
}

int
main(int argc, char **argv)
{
	int j=0;
	double bias = BIAS;
	int md = MIN_DEPTH;
	long seed=0;

	/* Process command line */

	while(++j < argc){
		if(argv[j][0] == '-')
			switch(argv[j][1]){ 
				case 's':
				case 'S':
					if(j+1 >= argc){
						fprintf(stderr,"%s\n",USAGE);
						exit(1);
					}
					seed = atol(argv[j+1]);
					j++;
					continue;
				case 'b':
				case 'B':
					if(j+1 >= argc){
						fprintf(stderr,"%s\n",USAGE);
						exit(1);
					}
					bias = atof(argv[j+1]);
					j++;
					continue;	
				case 'd':
				case 'D':
					if(j+1 >= argc){
						fprintf(stderr,"%s\n",USAGE);
						exit(1);
					}
					md = atoi(argv[j+1]);
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
					fprintf(stderr,"lisper: unkown option %s\n",
						argv[j]);
					exit(1);
			}
		else {
			fprintf(stderr,"%s\n",USAGE);
			exit(1);
		}
	}

	/* Do sanity checks on parameters */

	if(md < 0){
		fprintf(stderr,"lisper: Minimum depth must be positive.\n");
		return 1;
	}

	if((bias < 0)||(bias > 0.5)){
		fprintf(stderr,"Bias must lie in range [0,0.5].\n");
		return 1;
	}
 
	/* If no seed is supplied, then use current system time */
	
	if(!seed)
		if((seed = (long)time(NULL)) == -1){
			seed = INITIAL_SEED; /* if all else fails */
			fprintf(stderr, "Using seed = %d\n",INITIAL_SEED);
		}
		
	/* Seed RNG */

	SRANDOM((int)seed);

	/* Do it! */

	while(depth < md) /* bias deeper until min depth exceeded */
		if(U() < 0.5+bias)
			push();
		else
			if(depth)
				pop();
	while(depth) /* bias shallower until we return to balance */
		if(U() > 0.5+bias)
			push();
		else
			pop();

	/* Finish */
	printf("\n");
	return 0;
}
	
