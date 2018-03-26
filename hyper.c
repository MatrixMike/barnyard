/* Hyper.c:
 
   Compute the hypergeometric function of real parameters and a real variable. 
   (Maybe implement complex case later?)

   The hypergeometric function, F(a,b,c;x), is really a 3 parameter (a,b,c) 
   family of functions of x. It is analytic in its parameters except for poles
   in c a 0, -1, -2, ... . For fixed (a,b,c) it is analytic in x in the slit
   complex plane with cut from 1 to infinity along the real axis. In the unit
   disk, it has an elegant expansion

  	F(a,b,c;x) = 1 + abx/c + ... + [(a)_j(b)_j/(j!(c)_j)]x^j + ...

   where (a)_j = a(a+1)(a+2)... (j factors.)	

   The hypergeometric function satisfies many identities, some of which allow
   the series definition to be extended beyond the disk. Of particular interest
   is the identity

  	F(a,b,c;x) = (1-x)^(-a) F(a,c-b,c,x/(x-1)).
   
   Since the transformation x -> x/(x-1) maps the half plane Re(x) < 1/2 onto
   the unit disc, mapping 0 to 0, the cases below allow us to calculate F for 
   all x from minus infinity to 1 by ultimately summing a series:

  	case: x <= 0: Apply x -> x/(x-1) and then sum.
        case: x > 0:  Sum	

   For background on the hypergeometric function, including proofs of all the
   above statements, see, e.g., Special Functions and Their Applications, by
   N.N. Lebedev and Richard R. Silverman, Dover, NY, 1972, pp. 238-250.

   Compile: cc -o hyper hyper.c -lm

   For usage information, give the command hyper -h or see USAGE defined
   below.

   API usage:

	extern double hyper(double a, double a, double c, double x, 
		double derror);

   Compile this file with cc -c -DNO_MAIN and link your program with hyper.o

   Bugs:
		- We only support real number computations.

		- The number of displayed digits in the answer doesn't depend 
                  on the precision.

		- We don't yet support the confluent or generalized (Barnes)
 		  versions of the hypergeometric function.

		- The interactive dialog implemented here is a clunky terminal
                  version. Obviously one could replace it with a jazzier
                  windowing version.

   Author: Terry R. McConnell
	   trmcconn@syr.edu

	   In programs that compute functions we try to meet the following
	   design goals:

	   	- Have a standalone version of the function that can be linked
		  and called externally.

		- Have a wrapper main function that processes arguments and
		  allows the function to be computed from the command line.

		- Have my standard version and help options.

		- Have an interactive (-i) option that will allow the program
		  to be run from an interactive dialogue.
		
		- Have an option to generate xml output
		  that uses a style sheet named function_table.css. 

		- Fully document the source with comments, especially 
		  providing references or full details to key mathematical
		  ideas. 

		- The normal output should be one or more tables of values.

*/


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<float.h>
#include<math.h>

#define VERSION "1.1"
#define PROGRAMNAME "Hyper"

#define USAGE "hyper [-hvi -p <n> -a|b|c|x <n> -dx|a|b|c -n <n> -xml ]"   

#define HELP "\n"USAGE"\n\n\
-h:   print this helpful message.\n\
-v:   print version number and exit.\n\
-i:   interactive mode. Supply values at prompts.\n\
-p:   obtain values to accuracy of  < n (default .0000001.)\n\
-a:   specify starting value of parameter a to be n (default 1.0.)\n\
-b:   specify starting value of parameter b (default 1.0.)\n\
-c:   specify starting value of parameter c (default 1.0.)\n\
-x:   specify starting value of variable (default 0.0.)\n\
-dx:  specify increment in variable  to be n (default = 0.1.)\n\
-da:  specify increment in a (default = 0.0.)\n\
-db:  specify increment in b (default = 0.0.)\n\
-dc:  specify increment in c (default = 0.0.)\n\
-n:   specify number of times to increment, i.e. rows - 1 (default = 0.)\n\
-xml: generate xml output for function_table stylesheet.\n\n\
Tabulate the hypergeometric function F(a,b,c;x).\n\n"

#define PROMPT ": "
#define MAXLINE 256
#define XML_HEADER "<?xml version=\"1.0\" standalone=\"yes\"?>\n\
<!DOCTYPE function_table \n\
[\n\
]>\n\
<?xml-stylesheet type=\"text/css\" href=\"function_table.css\"?>\n\
<function_table>\n\
<title> Hypergeometric Function F(a,b,c;x) </title>\n"
#define XML_FOOTER "</function_table>\n"


/* Types for my_data fields */

#define INTEGER 0
#define DOUBLE 1
#define STRING 2

union my_data {
	int i;
	double d;
	char *s;
};

/* Forward declarations: all are implemented in this file. */

int getuser(union my_data *data, int type, char *message);
double hypersum(double a, double b, double c, double x, double derror);
double hyper(double a, double b, double c, double x, double derror);
int handle_error(void);

#define DERROR .0000001


#ifndef NO_MAIN
int
main(int argc, char **argv)
{
	int n = 1,i=1,j;
	int xml=0;
	double a = 1.0, b = 1.0, c=1.0, x = 0.0, dx = 0.1, derror = DERROR;
	double da = 0.0, db = 0.0, dc = 0.0,t;
	int interactive = 0;
	union my_data user_data;


	/* Process command line options */

	while((i < argc) && (argv[i][0] == '-')){
		  if(strcmp(argv[i],"-h")==0){
			printf("%s\n", HELP);
			exit(0);
		  }
		  if(strcmp(argv[i],"-v")==0){
			printf("%s\n",VERSION);
			exit(0);
		  }
		  if(strcmp(argv[i],"-a")==0){
			a = atof(argv[i+1]);
			if(derror <= DBL_EPSILON)
			        fprintf(stderr,"Warning: requested precision may exceed implementation limit.\n");
			i += 2;
			continue;
		  }
		  if(strcmp(argv[i],"-xml")==0){
			  xml = 1;
			  i += 1;
			  continue;
		  }
		  if(strcmp(argv[i],"-b")==0){
			b = atof(argv[i+1]);
			i += 2;
			continue;
		  }
		  if(strcmp(argv[i],"-c")==0){
			c = atof(argv[i+1]);
			i += 2;
			continue;
		  }
		  if(strcmp(argv[i],"-x")==0){
			 x = atof(argv[i+1]);
			i += 2;
			continue;
		  }
		  if(strcmp(argv[i],"-dx")==0){
			dx = atof(argv[i+1]);
			i += 2;
			continue;
		  }
		  if(strcmp(argv[i],"-da")==0){
			da = atof(argv[i+1]);
			i += 2;
			continue;
		  }
		  if(strcmp(argv[i],"-db")==0){
			db = atof(argv[i+1]);
			i += 2;
			continue;
		  }
		  if(strcmp(argv[i],"-dc")==0){
			dc = atof(argv[i+1]);
			i += 2;
			continue;
		  }
		  if(strcmp(argv[i],"-p")==0){
			derror = atof(argv[i+1]);
			i += 2;
			continue;
		  }
		  if(strcmp(argv[i],"-n")==0){
			n = atoi(argv[i+1]);
			i += 2;
			continue;
		  }
		  if(strcmp(argv[i],"-i")==0){

                  /* Dialogue with user to set up parameters. We print
		   * to standard error to allow for redirection of output. */

			fprintf(stderr,"\n\n\t\t%s Version %s Interactive Mode\n\n",
					PROGRAMNAME,VERSION);

			fprintf(stderr,"\nGenerate a table of values of the hypergeometric function F(a,b,c;x).\n");
			fprintf(stderr,"Please enter requested data at the prompts.\n\n");
			while(getuser(&user_data,DOUBLE,"Parameter a=?") && handle_error());
			a = user_data.d;
			while(getuser(&user_data,DOUBLE,"Parameter b=?") && handle_error());
			b = user_data.d;
			while(getuser(&user_data,DOUBLE,"Parameter c=?") && handle_error())
				;
			c = user_data.d;
			while(getuser(&user_data,DOUBLE,"Initial x value?") && handle_error())
				;
			x = user_data.d;
			while(getuser(&user_data,INTEGER,"Number, n, of table rows to create?") && handle_error())
				;
			n = user_data.i;
			while(getuser(&user_data,DOUBLE,"Increment, dx, in x between successive rows?") && handle_error())
				;
			dx = user_data.d;
			while(getuser(&user_data,DOUBLE,"Increment, da, in a?") && handle_error())
				;
			da = user_data.d;
			while(getuser(&user_data,DOUBLE,"Increment, db, in b?") && handle_error())
				;
			db = user_data.d;
			while(getuser(&user_data,DOUBLE,"Increment, dc, in c?") && handle_error())
				;
			dc = user_data.d;
			fprintf(stderr,"\n\n");
			i+=1;
		  	continue;
		  }
		  fprintf(stderr, "hyper: Unknown or unimplemented option %s\n", argv[i]);
		  fprintf(stderr, "%s\n",USAGE);
		  return 1;
		}

	/* do sanity checks here. Warn the user when there is a problem
	 * but blunder onward anyway. */

	if(x + dx*(double)n >= 1.0)
		fprintf(stderr,"Warning: x range includes values 1.0 or larger. F is singular there.\n\n" );

	i=0,t=c;
	while(i++<n){
		if((t <= 0.0)&&((-t)-floor(-t)==0))
		fprintf(stderr,"Warning: c parameter can be 0 or negative integer.\n\n");
		t += dc;
	}

	if(xml){
		printf(XML_HEADER);
		printf("<header>\n\
	<cell width=\"5\">x</cell>\n\
	<cell width=\"5\">a</cell>\n\
	<cell width=\"5\">b</cell>\n\
	<cell width=\"5\">c</cell>\n\
	<cell width=\"11\">F(a,b,c;x)</cell>\n\
</header> \n");
	}
	else {
		printf("               The Hypergeometric Function\n\n");
		printf("     x        a        b        c             F(a,b,c;x)  \n",
			a,b,c);
		printf("--------------------------------------------------------\n");

	}
	i = 1;
	do {
		if(xml){
			printf("<row>\n\
	<cell width=\"5\">%.3f</cell>\n\
	<cell width=\"5\">%.3f</cell>\n\
	<cell width=\"5\">%.3f</cell>\n\
	<cell width=\"5\">%.3f</cell>\n\
	<cell width=\"11\">%.7f</cell>\n\
</row>\n",x,a,b,c,hyper(a,b,c,x,derror));
		}
		else
		       printf("%8.3f %8.3f %8.3f %8.3f %20.8f\n",x,a,b,c,hyper(a,b,c,x,derror));
		x += dx;
		a += da;
		b += db;
		c += dc;
	} while(i++ < n);
	
	if(xml)printf(XML_FOOTER);
	return 0;
}
#endif /* NO_MAIN */


/* hyper: Compute the hypergeometric function F(a,b,c;x) to within
 * an error < derror.
 *
 * As explained in the header, we use an identity to handle x <= 0
 * and 0 < x < 1 separately. The hypersum routine does the real work.
 *
 * Return the result as a double. 
 */

double hyper(double a, double b, double c, double x, double derror)
{
	double xx=x,rval = 0;

	if(x <= 0){
		x = x/(x-1.0);
		b = c - b;
	}
	rval = hypersum(a,b,c,x,derror);
	if(xx <= 0)
		rval *= pow(1.0-xx,-a); /* use original x */

	return rval;
}

/* hypersum: return the value of F(a,b,c;x) to within error < derror.
 *
 * We compute the hypergeometric function by summing the hypergeometric
 * series. This will only converge when |x| < 1. The caller might use
 * any known transformation of argument identities to reduce computation
 * of the analytically continued function to the case |x| < 1.
 *
 * It is up to the caller to do sanity checking on the argument and
 * parameters. If fed bad values, this routine may churn away forever
 * or cause an exception.
 */

double hypersum(double a, double b, double c, double x, double derror)
{

	double rval = 1.0;
	double n = 1.0;
	double term = a*b*x/c;
	double K,C;

	/* find max(|a|,|b|,|c|) for use below */
	K = fabs(a) ? fabs(b) : fabs(a) > fabs(b);
	K = K ? fabs(c) : K > fabs(c);

	do {
		/* Decide whether we are accurate enough yet, i.e., if
		 * tail of hypergeometric series < derror. Let An be nth term
		 * of series, and Bn = (a+n)(b+n)/[(c+n)(1+n)]. Then
		 * An+1/An = Bnx. Bn --> 1, which gives radius of convergence
		 * 1. Let Cn be a monotone nonincreasing upper bound for |Bn|
		 * Then by comparison with geometric series, |An|/(1-Cnx) < 
		 * derror is sufficient to ensure accuracy. If we let K be
		 * the max of |a|,|b|,|c|, then it is easy to check that
		 * Cn = 1 + 6K/n + 2(K/n)^2 is such a monotone upper bound when
		 * n > 2K.  
		 */

		C = 1.0 + 6.0*K/n + 2.0*(K/n)*(K/n);
		if((n>2.0*K) && (C*x < 1) && (fabs(term)/(1 - C*x) < derror))break;

		rval += term;
		a += 1.0;
		b += 1.0;
		c += 1.0;
		n += 1.0;
		term = term*a*b*x/(n*c);

	} while(1);   
	return rval;
}

/* Generic routine to get a line of data from the user, presumably
   in answer to a dialogue question. Response is returned though union
   pointer argument. You must tell the routine what type of data response
   is expected using the type argument. The message is printed, followed
   by a prompt.

   Returns 0 if operation was successful, 1 otherwise. */

int getuser(union my_data *data, int type, char *message){
	char buffer[MAXLINE],*p;
	int n;

	if(message && strlen(message)>0)
		fprintf(stderr,"%s",message);
	fprintf(stderr,"%s",PROMPT);
	p = fgets(buffer,MAXLINE,stdin);

	/* do some sanity checks */

	if(!p)return 1;
	n = strlen(buffer);
	if(n<=1)return 1; /* recall that buffer contains at least \n */

	switch(type){

		case INTEGER:
			data->i = atoi(buffer);
			break;
		case DOUBLE:
			data->d = atof(buffer);
			break;
		case STRING:
			if(!(p = (char *)malloc(n*sizeof(char))))
					return 1;
			strcpy(p,buffer);
			data->s = p;
			break;
		default:
			return 1;
	}
	return 0;
}
			

/* The following is called when getuser is unsuccessful. It presents the
   venerable MSDOS error message and then returns a value depending on
   how the user responds:

	abort: exit the program from here.
	retry: return 1
	ignore: return 0

   The caller needs to decide what to do based on the return value.
*/
 
int handle_error(){

	int c,d;

	fprintf(stderr,"\n   **** Bad data or other error ****\n\n");
	while(1){
		fprintf(stderr,"\nAbort[a],Retry[r],Ignore[i]?\n");
		fprintf(stderr,"Enter appropriate lower case letter and hit return:");
		c = fgetc(stdin);
		d = fgetc(stdin); /* this should be a new line */
		if(d != '\n')continue;
		fprintf(stderr,"\n");
		switch(c){
			case 'a':
				exit(1);
			case 'r':
				return 1;
			case 'i':
				return 0;
			case EOF:
				exit(1);
			default:
				fprintf(stderr,"Unrecognized response!\n");
				break;
		}
	}
}

