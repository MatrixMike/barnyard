/* command-line argument parsing template for C-programs:
	This allows a fairly standard Unix command line. Options
	may take any number of option arguments, but there must
        be whitespace between option flag and option arguments.
        Consecutive one letter flags will be globbed into one.
	 One option called 
	-help is reserved for a help message. Options may be strings of
	characters and may be given either separately or combined,
	although you must include returns in find_opt for all
	possible combinations. The special option -- indicates
	the end of options, useful when filenames may begin with -  */

/* Version 1.1, By Terry McConnell, February 1996 */

#include <stdio.h>
#include <stdlib.h>

#define INFO  "No information available"    /* Change this */
#define USAGE "No Usage Information available"
#define MAX_ARG_LEN 80             /* Max length of combined options */

int find_opt(char *option_string);
#include <stdarg.h>
void myerror(char *fmt, ...);
char ProgramName[256];

int main(int argc, char **argv)
{
	int c=-1;                /* numeric option code */
	char *info = INFO;
	char opsep = '-';      /* Standard for Unix */
	char argstr[MAX_ARG_LEN];

/* Program name is available in ProgramName */
	strcpy(ProgramName,argv[0]);
/* Process Command Line Options */

if(--argc > 0){  /* Anything on command line ? */
                ++argv;
                while((argc > 0)&&((*argv[0]) == opsep)){ /* loop while 
							there are flags */
                        argstr[0]=opsep;
                        argstr[1]='\0';
                        strcat(argstr,++*argv); /* glob argstr*/
                        argc--;
                        while((argc>0)&&(strlen(*argv)<3)&&
				((*(argv+1))[0]==opsep)&&((*(argv+1))[1]!=opsep)){
                                ++argv;
                                argc--;
                                strcat(argstr,++*argv);
                        }
                        ++argv; /* ok, consecutive flags now globbed into one*/
                       c = find_opt(argstr+1); /* look up flag code */
                switch(c){

/* N.B. Your code should process command line arguments if
   they are expected. Make sure to increment argv and decrement argc 
   each time. The code for handling the arguments for a given flag must
   go in the case for that flag. 
*/

			case -1: break;  /* default, e.g no options */

			case 0: printf("%s\n",info);
				return 0;
/*  YOUR CASES HERE */
                        case -2: ;  /* barf (don't use in your code) */
			default:
				fprintf(stderr,"%s: illegal option\n",argstr);
				fprintf(stderr,USAGE);
				return 1;
			} /* End flag switch */
	} /* End flag grabbing while loop */
} /* end if: any args ? */

/*   BODY OF PROGRAM GOES HERE  */
/*   argc now gives a count of remaining arguments on command line,
	and argv points at the first one */

	return 0;
}

/*  find_opt: return a unique small integer for each possible option string */
/*  There should be a case in the switch statement in main to handle each. */
/*  -2 reserved for arg not found -1 reserved for no options */

/* N.B. word doesn't contain the leading hyphen */

int find_opt(char *word)
{
	if(strcmp(word,"help")==0)return 0;

/* arg not found */
	return -2;
}

/* myerror: generic error reporting routine */

void myerror(char *fmt, ...)
{
        va_list args;

        va_start(args,fmt);
        fprintf(stderr,"%s: ",ProgramName);
        vfprintf(stderr,fmt,args);
        fprintf(stderr,"\n");
        va_end(args);
        exit(1);
}

