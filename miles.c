/* miles.c  find all combinations of laps/lanes which equal an integer number
*of miles to desired tolerance 
*
* By Terry R. McConnell (Chicken Haven Software)
*
* Compile cc -o miles miles.c
*/

#define VERSION "1.0"

/* DEFAULT DEFINITIONS: we assume a standard metric indoor track */

#define LEN 200.0           /* meters per lap in lane 1 */
#define LANEWIDTH 3.0*.3048 /* in meters, = 36 inches */
#define LANES 6

/* LIMITS */

#define MAXLAPS 1000  /* I think this is pretty safe :-) */
#define MAX_ARG_LEN 5             /* Max length of combined options */

/* GENERAL */

#define PI 3.14159265   /* Sort of a useful constant */

/* INCLUDES */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define USAGE "miles [ -l <lanes> -w <width> -lap <length> -help -version ] <tolerance>"
#define LOGO  "\n                    MILES, VERSION "VERSION"\n\
                   By Terry R. McConnell\n\n"
#define INFO  "Miles: print all lane-lap combinations that give an even number of \n\
miles to within the given tolerance. (Quits at 1000 laps.)\n\
Tolerance is measured in feet.\n\n\
USAGE: "USAGE" \n\n\
OPTIONS\n\
-l <lanes> : Use lanes as the number of lanes rather than the default (6.)\n\
-w <width> : Use width (in inches) as lane width rather than the default (36.)\n\
-lap <length>: Length of 1 lap in meters (default is 200.)\n\n\
EXAMPLES\n\
miles 50    --  Find all combinations of laps and lanes which equal an even \n\
number of miles to within 50 feet.\n\
miles -l 8 -w 48 -lap 400 50  -- Same thing for a standard outdoor track.\n\n"


int find_opt(char *option_string);
#include <stdarg.h>
void myerror(char *fmt, ...);
char ProgramName[256];

int
main (int argc, char * argv[])
{
	int lane = 1, lap = 0;
	int no_lanes = LANES;
	double lane_width = LANEWIDTH;
	double length = LEN;
	int lower;
	double tolerance, d,delta,frac;
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

			case -1: break;  /* default, e.g no options */

			case 0: 
				printf(LOGO);
				printf("%s\n",info);
				return 0;
			case 1: printf("%s\n",VERSION);
				return 0;
			case 2: 
				if(argc == 0)
					myerror("%s\n",USAGE);
				no_lanes = atoi(*argv++);
				if(no_lanes <= 0)
					myerror("%s\n","Impossible number of lanes");
				argc--;
				break;
			case 3: 
				if(argc == 0)
					myerror("%s\n",USAGE);
				lane_width = atof(*argv++);
				if(lane_width <= 0)
					myerror("%s\n","Impossible lane width");
				lane_width = (lane_width/12.0)*.3048;
				argc--;
				break;
			case 5:
				if(argc == 0)
					myerror("%s\n",USAGE);
				length = atof(*argv++);
				if(length <= 0)
					myerror("%s\n","Impossible lap length");
				argc--;
				break;
	
			case 4:
			       myerror("%s\n",USAGE);
/*  YOUR CASES HERE */
                        case -2: ;  /* barf (don't use in your code) */
			default:
				fprintf(stderr,"%s: illegal option\n",argstr);
				fprintf(stderr,"%s\n",USAGE);
				return 1;
			} /* End flag switch */
	} /* End flag grabbing while loop */
} /* end if: any args ? */

	if(argc == 0)
		myerror("%s\n",USAGE);

	tolerance = atof(argv[0])/5280;  /* in miles */

	if(tolerance < 0)
		myerror("%s\n", "Negative tolerance is crazy.\n");

	printf("lane\tlaps\tmiles\n");

	for (lane = 1; lane <= no_lanes; lane++) {
		for (lap = 1; lap <= MAXLAPS; lap++) {
			d = (length + 
				lane_width*2.0*PI*(lane - 1))*lap/1609.344;
			lower = (int) d; 
			frac = d - lower;
			delta = (frac < .5) ? frac : 1.0 - frac;
			if (delta < tolerance) 
			printf("%d\t%d\t%g\n",lane,lap,d);
		} 
	}
	return 0;
}

/*  find_opt: return a unique small integer for each possible option string */
/*  There should be a case in the switch statement in main to handle each. */
/*  -2 reserved for arg not found -1 reserved for no options */

/* N.B. word doesn't contain the leading hyphen */

int find_opt(char *word)
{
	if(strcmp(word,"help")==0)return 0;
	if(strcmp(word,"version")==0)return 1;
	if(strcmp(word,"l")==0)return 2;
	if(strcmp(word,"w")==0)return 3;
	if(strcmp(word,"lap")==0)return 5;

/* user forgot an arg */
	if(strcmp(word,"lw")==0)return 4;
	if(strcmp(word,"wl")==0)return 4;
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

