/*
*  arithprb.c: 
*
*  Prints a page of randomly generated arithmetic problems. Good for combatting
*  Alzheimer's. The page is followed by another containing the solutions.
*
*	By Terry R. McConnell
*
*  Compile: cc -o arithprb arithprb.c
*
*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 16184
#endif

/* This needs to be changed in strings below too */
#define NPROBS 12

#define VERSION "1.1"
#define PROGNAME "arithprb"
#define USAGE "arithprb [-n <nn> -s <nn> -c <nn> -t -vh]"

#define HELP "-h: print this helpful message\n\
-v: print version number and exit\n\
-n: generate nn problems  (default is 12) \n\
-s: use nn as RNG seed (default is current system time if available.)\n\
-c: print problems in nn columns (default is 3.)\n\
-t: write output as a TeX source file (latex)\n\n\
Prints page of arithmetic problems to stdout, followed by page of answers. \n\n"

/* In its default setup, this program writes to stdout a description of
a page of arithmetic problems. It can write either an ascii text "prettyprint"
version of the arithmetic problems, or a LaTeX source file, which must 
subsequently be typeset to see the problems. 

In either version, the program views the page as a table of 3 columns and
4 rows (in the default setup). Each table entry is a conceptual box containing
a problem. 

In the LaTeX version the table is created with a \table command sequence, and
every entry (problem) in the table is in turn a \table containing 4+ lines
typeset to represent the problem, with or without answer. 

The default configuration can be changed easily by changing defines below,
but it is not so easy to change the default TeX setup. 

*/

/* output types */

#define ASCII 0
#define TEX 1

/* Call these depending on type of output */

static int make_prob_ascii(int row, int col, int type, int ans); 
static int make_prob_tex(int row, int col, int type, int ans); 
static int(*make_prob)(int,int,int,int);

/* Stuff for the ascii problem generator */

#define BANNER "\n\nDo the following arithmetic problems:\n\
(Answers on next page.)\n\n\n\n\n\n\n"
static char page_buf[BUFFER_SIZE];
static int cspace = 8; /* blank cols to leave between problems */
static int rspace = 8; /* blank rows to leave between problems. Should be
                          more than double the length of maxnum below */
static int cols = 3; /* number of problem columns */

/* These settings will produce 4 digit multiplication problems and division
   problems with 8 digit dividends and 4 digit divisors. Think carefully
   before you change them */

static int probcols = 13; /* this should be at least one more than thrice as 
                             long as the length of maxnum below */
static int maxnum = 9999; /* length of the longest generated addend. A 
                             dividend can be up to twice this long. */
static int underbarlen = 4; /* should match the number of digits of maxnum */

/* A macro for indexing into the page_buf */

#define BUF_POSITION(row,col,rowoffset) page_buf+((col)-1)*(probcols+cspace)+\
 (((row)-1)*(3+rspace)+(rowoffset))*(cols*(probcols+cspace)+1)

/* Stuff for the TeX problem generator */

/* This gets printed at the top of the output document. Edit it to implement
font and size changes, etc. The default defines needed to create problems which
have a particular look and feel are given below near the tex problem routine */

#define TEX_DOC_HEADER "\\documentclass{report}\n\
\\pagestyle{empty}\n\
\\setlength{\\oddsidemargin}{0in}\n\
\\setlength{\\textwidth}{18 cm}\n\
\\setlength{\\textheight}{22 cm}\n\
\\begin{document}\n\\large\n"
#define TEX_DOC_FOOTER "\\end{document}\n\\bye\n"

/* Make 3 columns of problems. There are 5 columns shown because two columns 
contain blank problems for filling purposes. */

#define TEX_PAGE_HEADER "\\begin{tabular}{rrrrr}\n"
#define TEX_PAGE_FOOTER "\\end{tabular}\n"



#ifdef _NO_RANDOM
#define RANDOM rand
#define SRANDOM srand
#else
#define RANDOM random
#define SRANDOM srandom
#endif

/* Default values */
#define INITIAL_SEED 3445
#ifndef _MAX_RAND
#define _MAX_RAND RAND_MAX
#endif

/* Problem types */
#define ADD 0
#define SUB 1
#define MULT 2
#define DIV 3

#define NO_ANSWERS 0
#define ANSWERS 1



/* return a number chosen at random from 0,1...,k-1 */

int random_on(int k){

	double U;   /* U(0,1) random variable */
	int rval;

	U = ((double)RANDOM())/((double)_MAX_RAND);
	rval = (int)((double)k*U);
	return rval;
} 

int
main(int argc, char **argv){

	int c,i=1;
	int output_type = ASCII;
	int nprobs = NPROBS;
	int row=1,col=1;
	int seed = 0; 
	int type;
	time_t timenow;


	timenow = time(NULL); /* time stamp */

	make_prob = make_prob_ascii;

	/* Process command line */
	while((i<argc) && (argv[i][0]=='-')){
		switch(argv[i][1]){
		case 't':
			output_type = TEX;
			make_prob = make_prob_tex;
			break;
		case 'v':
			printf("%s\n",VERSION);
			return 0;
		case 'h':
			printf("\n%s\n\n",USAGE);
			printf("%s\n",HELP);
			return 0;
		case 'n':
			/* store next arg as number of problems to make */
			nprobs = atoi(argv[i+1]);
			i++;
			break;
		case 'c':
			cols = atoi(argv[i+1]);
			i++;
			break;
		case 's':
			/* store next arg as rng seed */
			seed = atoi(argv[i+1]);
			i++;
			break;
		default:
			fprintf(stderr,"%s:%s\n",PROGNAME,USAGE);
			return 1;
		}
		i++;
	}

	if(i != argc){ /* garbage on command line */
		fprintf(stderr,"%s:%s\n",PROGNAME,USAGE);
		return 1;
	}


	/* Do a sanity check on the parameters. We will write our page of
           problems into the global array page_buf, then dump that to stdout
           afterwards. We don't want to overflow that buffer */

	if(cols <= 0) {
		fprintf(stderr,"cols parameter = %d, a crazy value\n",cols);
		exit(1);
	}
	if(nprobs <= 0) {
		fprintf(stderr,"nprobs parameter = %d, a crazy value\n",nprobs);
		exit(1);
	}

	/* how many rows of problems ? */

	i = nprobs/cols + 1;    /* upper bound */

	if( i*(3+rspace)*((probcols+cspace)+1)+1 >= BUFFER_SIZE ){
		fprintf(stderr,"Internal buffer too small for problem set size\n");
		fprintf(stderr,"Tone down request, or compile with bigger buffer\n");
		exit(1);
	} 

	/* Seed the random number generator */

	/* If no seed is supplied, then use current system time */
	
	if(!seed)
		if((seed = (long)time(NULL)) == -1){
			seed = INITIAL_SEED; /* if all else fails */
			fprintf(stderr, "Warning: no seed available. Using %d\n",INITIAL_SEED);
		}
		
	SRANDOM((unsigned)seed);

	if(output_type == ASCII ){

	/* print the date and version number 
                         of problem set (i.e, the seed ) */

		printf("\n\nDate: %sVersion: %d\n",ctime(&timenow),seed);
	
		/* print the banner */

		printf("%s", BANNER);
	}
	else {

		printf(TEX_DOC_HEADER);
		printf("\\today \\ \\ (Version %d.) See attached page for answers.\n \n",
				seed);
		printf("\\nopagebreak\n\n");
		printf(TEX_PAGE_HEADER);
	}

		/* Generate the problems */

	for(i=0;i<nprobs;i++){
		row = i/cols+1;
		col = (i % cols ) + 1;

		/* pick a type of problem */
		
		type = random_on(4); 

		make_prob(row,col,type,NO_ANSWERS);
	}

	if(output_type == ASCII){

	/* print the whole thing out */

		printf("%s",page_buf);

	/* printf a formfeed, then the whole thing over, this time
                with answers */

		printf("\f");  
		printf("\n\nDate: %sVersion: %d\n",ctime(&timenow),seed);
		printf("Solutions:\n\n\n\n\n\n");
	}
	else {

		printf(TEX_PAGE_FOOTER);
		printf("\n\\pagebreak\n\n");
		printf("\\date \\ \\ (Version %d)\n \n",seed);
		printf("\\nopagebreak\n");
		printf(TEX_PAGE_HEADER);
	}

	SRANDOM((unsigned)seed); /* so we'll get the same problems again */

	for(i=0;i<nprobs;i++){
		row = i/cols+1;
		col = (i % cols ) + 1;

		/* pick a type of problem */
		
		type = random_on(4); 

		make_prob(row,col,type,ANSWERS);
	}

	if(output_type == ASCII)
		printf("%s",page_buf);
	else {
		printf(TEX_PAGE_FOOTER);
		printf(TEX_DOC_FOOTER);
	}
	return 0;	

}


/* make_prob_ascii: Does the hard work. Writes a (3+rspace)x(probcol+cspace) box 
containing a randomly generated problem of the given type. The global buffer
page_buf can be thought of as containing an array of such boxes. This 
routine writes the (i,j)-th such box, with i=row, j=col passed.

If the ans parameter is set to ANSWERS then the answer to the problem is
written too.

*/

int make_prob_ascii(int row, int col, int type, int ans) 
{

	int arg1, arg2;
	int answer;
	int xtra,xtra_save,l,i,j;
	char lbuf[128];
	char *bptr;

	/* generate random arguments */

	if(type == DIV)
		arg1 = random_on(maxnum*maxnum)+1;
	else
		arg1 = random_on(maxnum)+1;
	arg2 = random_on(maxnum)+1;

	/* we want the larger arg to come first */

	i = arg1;

	arg1 = (arg1 >= arg2 ? arg1 : arg2);	
	arg2 = (arg2 < i ? arg2 : i);

	/* calculate the answer to the problem here, if needed */ 

	if(ans == ANSWERS){
		
		switch(type){
			case ADD:
				answer = arg1 + arg2;
				break;
			case MULT:
				answer = arg1 * arg2;
				break;
			case SUB:
				answer = arg1 - arg2;
				break;
			case DIV:
				answer = arg1/arg2;
				break;
			default:
				fprintf(stderr,"make_prob: called with screwy problem type\n");
				exit(1);
		}

	}
	
	switch(type){
		case ADD: /* these have same format, except for operation sign*/
		case MULT:
		case SUB:


	/* locate initial buffer position of upper left corner of problem
                  output rectangle */

		bptr = BUF_POSITION(row,col,0); 

		/* create a right justified string for top operand */

		sprintf(lbuf,"%d",arg1);
		l = strlen(lbuf);

		xtra = probcols - l;
		xtra_save = xtra;  /* needed to line up operation sign later */

		/* pad with blanks in order to right justify */

		for(i=0;i<xtra;i++)
			lbuf[i]=' ';
		sprintf(lbuf+i,"%d",arg1);


		/* poke it in the page buffer */

		sprintf(bptr,"%s",lbuf);
		bptr += strlen(lbuf);


		/* put column separator space */

		for(i=0;i<cspace;i++)
			*(bptr++)=' ';

		/* if we are last problem in a column we must print newline */

		if(col == cols)
			*(bptr++)='\n';

/* The above operation is done many times, so we make a macro of it here */
#define FINISH_LINE for(i=0;i<cspace;i++)*(bptr++)=' ';\
if(col==cols)*(bptr++)='\n';


		/* position for next row */

		bptr = BUF_POSITION(row,col,1); 

		/* repeat with second arg */

		/* create a right justified string for bottom summand */

		sprintf(lbuf,"%d",arg2);
		l = strlen(lbuf);

		xtra = probcols - l;

		/* figure out how many spaces to print before the operation
                   sign */

		if(xtra == xtra_save)
			for(i=0;i<xtra-1;i++)
				lbuf[i]=' ';
		else  
			for(i=0;i<xtra_save;i++)
				lbuf[i]=' ';

		/* put in the operation sign */

		switch(type){

			case ADD:
				lbuf[i++]='+';
				break;
			case SUB:
				lbuf[i++]='-';
				break;
			case MULT:
				lbuf[i++]='x';
				break;
			default:
				fprintf(stderr,"unknown problem type. No symbol.\n");
				exit(1);

		}

		/* put in any needed spaces after operation sign */

		for(j=0;j<xtra-xtra_save-1;j++)
			lbuf[i++]=' ';

		/* put in the number */

		sprintf(lbuf+i,"%d",arg2);

		/* poke it in the output */

		sprintf(bptr,"%s",lbuf);
		bptr += strlen(lbuf);

		FINISH_LINE 

		/* position for underbar */

		bptr = BUF_POSITION(row,col,2); 

		/* now draw the underbar */

		for(i=0;i<probcols-underbarlen;i++)
			*(bptr++)=' ';
		while(i<probcols){
			*(bptr++)='-';
			i++;
		}

		FINISH_LINE

		if(ans == ANSWERS){  /* draw answer below underbar */

			bptr = BUF_POSITION(row,col,3); 

			sprintf(lbuf,"%d",answer);
			l = strlen(lbuf);

			xtra = probcols - l;

			/* pad with blanks in order to right justify */

			for(i=0;i<xtra;i++)
				lbuf[i]=' ';
			sprintf(lbuf+i,"%d",answer);


			/* poke it in the output */

			sprintf(bptr,"%s",lbuf);
			bptr += strlen(lbuf);

			FINISH_LINE

		} /* end answer case of MULT,ADD,SUB */
			break;

		case DIV:

			bptr = BUF_POSITION(row,col,0);

			/* answer goes on line 0 of box, else blank line  */

			if(ans == ANSWERS){
				sprintf(lbuf,"%d",answer);
				l = strlen(lbuf);
				xtra = probcols - l;
			}
			else xtra = probcols;
			for(i=0;i<xtra;i++)
				*(bptr++)=' ';

			/* done, i.e blank line, unless answers */

			if(ans == ANSWERS){
				sprintf(bptr,"%d",answer);
				bptr += l;
			}

			FINISH_LINE	

			/* print the overbar */

			bptr = BUF_POSITION(row,col,1);

			sprintf(lbuf,"%d",arg1);
			l = strlen(lbuf);  

			xtra = probcols - l;

			for(i=0;i<xtra;i++)
				*(bptr++)=' ';

			while(i++<probcols)
				*(bptr++)='_';

			FINISH_LINE

			/* print the main problem line */

			bptr = BUF_POSITION(row,col,2);

			sprintf(lbuf,"%d|%d",arg2,arg1);
			l = strlen(lbuf);

			xtra = probcols - l;
			for(i=0;i<xtra;i++)
				*(bptr++)=' ';
	
			sprintf(bptr,"%s",lbuf);
			bptr += l;
			
			FINISH_LINE

			break;

		default:  /* unknown problem type */
			fprintf(stderr,"make_prob: unknown problem type\n");
			exit(1);

	}  /* end main switch on problem type 
		
	/* write rows of whitespace between problems */
	if( (ans == ANSWERS) && (type != DIV) ) i=1;
	else i = 0;
	for(;i<rspace;i++){

			bptr = BUF_POSITION(row,col,(3+i)); 

			for(j=0;j<probcols+cspace;j++)
				*(bptr++) = ' ';

			if(col == cols)
				*(bptr++)='\n';
	}
		
	return 0;
}

/* make_prob_tex: Does the hard work of writing pages of TeX source 

If the ans parameter is set to ANSWERS then the answer to the problem is
written too.

*/


/* reserve a big enough column size for problems */
#define COLUMN_HEADER "\\ \\ \\ \\ \\ \\ \\ \\ \\ \\ \\ \\ \\ \\\\\n"

/* Begin and end a Multiplication/additition/subtraction problem. */
#define PROBM_HEADER "\\begin{tabular}{r}\n"
#define PROBM_FOOTER "\\end{tabular}"

/* Begin and end a Division Problem */
#define PROBD_HEADER "\\begin{tabular}{rl}\n"
#define DIV_DIV_LINE "%d & \\multicolumn{1}{|l}{%d}\n"
#define PROBD_FOOTER PROBM_FOOTER


/* you have to read through the source for other things that affect 
formatting */

int make_prob_tex(int row, int col, int type, int ans) 
{

	int arg1, arg2;
	int answer;
	int xtra,xtra_save,l,i,j;
	char lbuf[128];
	char *bptr;

	/* generate random arguments */

	if(type == DIV)
		arg1 = random_on(maxnum*maxnum)+1;
	else
		arg1 = random_on(maxnum)+1;
	arg2 = random_on(maxnum)+1;

	/* we want the larger arg to come first */

	i = arg1;

	arg1 = (arg1 >= arg2 ? arg1 : arg2);	
	arg2 = (arg2 < i ? arg2 : i);

	/* calculate the answer to the problem here, if needed */ 

	if(ans == ANSWERS){
		
		switch(type){
			case ADD:
				answer = arg1 + arg2;
				break;
			case MULT:
				answer = arg1 * arg2;
				break;
			case SUB:
				answer = arg1 - arg2;
				break;
			case DIV:
				answer = arg1/arg2;
				break;
			default:
				fprintf(stderr,"make_prob: called with screwy problem type\n");
				exit(1);
		}

	}

	/* Multiplication, subtraction and addition have a similar format. We
           handle division separately below. */
	
	if((type == ADD)||(type==SUB)||(type==MULT)){

		/* print a placeholder line of the right column width */

		printf(PROBM_HEADER);
		printf(COLUMN_HEADER);
		printf("%d\\\\\n",arg1);

		switch(type){
			case ADD: /* these have same format, except for operation sign*/
				printf("+\\ ");
				break;
			case SUB:
				printf("-\\ ");
				break;
			case MULT:
				printf("$\\times$\\ ");
				break;
		}
		printf("%d\\\\ \n",arg2);
		if(ans == ANSWERS)
			printf("\\hline\n%d\n",answer);
		else
			printf("\\hline\n\\  \n");
		printf(PROBM_FOOTER);
		printf("\n");
	}
	else if(type == DIV){
		printf(PROBD_HEADER);
		if(ans == ANSWERS){

			/* The answer comes on top in a division problem. */
			/* Leave 4 spaces room above the divisor. This crude
                           and should be fixed. It assumes the divisor is 4
                           digits, but about 10% of the time it is less */
			printf("\\ \\ \\ \\ &");

			/* Determine how many digits in the answer, then put
                           it enough hard spaces so it will be roughly right.
			   justified. 
                           Again, this needs to be fixed */
			sprintf(lbuf,"%d",answer);
			xtra = 9 - strlen(lbuf);
			for(i=0;i<xtra;i++)
				printf("\\ ");
			/* print the answer and the overbar */
			printf("%d",answer);
			printf(" \\\\ \\cline{2-2}\n");
		}
		else  /* if no answer, print a big blank line with the problem
                         overbar under the last 8 spaces. It will end up
                         over the dividend. */
			printf("\\ \\ \\ \\ &\\ \\ \\ \\ \\ \\ \\ \\ \\\\ \\cline{2-2}\n");

		/* make the divisor|dividend line */
		printf(DIV_DIV_LINE,arg2,arg1);
		printf(PROBD_FOOTER);
		printf("\n");
	}
	else {  /* of course, this should never happen */

		fprintf(stderr,"make_prb_tex: unknown problem type \n");
		exit(1);
	}

	/* Depending on whether or not this problem is the last in a row,
 	   either round off an outside table row leaving desired row space,
           or print a blank problem to go between problems in the current
           row. Note that it is here that we handle placement of alignment
           tabs for the big table. */

	if(col == cols)
		printf("\\\\ [1.3 in]");
	else { /* print blank problem as separator  */
		printf(" & ");        /* tab for big table */
		printf(PROBM_HEADER);
		printf(COLUMN_HEADER);
		printf("\\ \\\\ \n \\ \\\\ \n \\ \\\\ \n \\\\ \n");
		printf(PROBM_FOOTER);
		printf(" & \n");    /* next tag for big table */
	}
	printf("\n");
	return 0;
}

