/*
* boxtext.c Print text in a box. By Terry R. McConnell 12/97
*
* Compile: cc -o boxtext boxtext.c
*
* On a PC you may want to include -DUSE_OEM_CHARS which gives nice-looking
* boxes. The default is rather stodgy.
*/
 
#define VERSION "1.0"
  
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#define MAXLINES 23     /* Fills 1 standard terminal screen */
#define COLS 80
#define MAX_ARG_LEN 5             /* Max length of combined option letters */

/* Change, as needed */

#define BDRWIDTH 3
#ifdef USE_OEM_CHARS
#define HBAR 196
#define VBAR 179
#define NW_CORNER 218
#define NE_CORNER 191
#define SW_CORNER 192
#define SE_CORNER 217
#else
#define HBAR '-'
#define VBAR '|'
#define NW_CORNER '+'
#define NE_CORNER '+'
#define SW_CORNER '+'
#define SE_CORNER '+'
#endif

#define SPACE ' '
#define DEFAULT_OFFSET 0

#define TOP 0
#define BOTTOM 1

#define LOGO  "\n                    BOXTEXT VERSION "VERSION"\n\
                   By Terry R. McConnell\n\n"

#define INFO  "Boxtext: print text to screen in a box.\n\
Usage: boxtext [ -o <number> -t [--] <text> -help -version ]\n\
-t: Use the following as text rather than stdin (1 line only.)\n\
-o: Offset the box number of spaces from left side of screen.\n\
(Should be given before -t if both are used.)\n"    
#define USAGE "boxtext [ -o <number> -t [--] <text> -help -version ]"



char text[MAXLINES][COLS+1];
int offset = DEFAULT_OFFSET;
char ProgramName[256];

void repeat_char(int,char);
void hborder(int,int);
void puttext(char *mesg, int max_width);
int find_opt(char *option_string);
void myerror(char *fmt, ...);

int
main (int argc, char **argv)
{
	int width,i;
	int line = 0;
	int use_stdin = 1;
	int max_width = 0;
	int c=-1;                /* Used in option processing */
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
                        ++argv; /* ok, consecutive flags now combined into one*/
                       c = find_opt(argstr+1); /* look up flag code */
                switch(c){

			case -1: break;  /* default, e.g no options */

			case 0: 
				printf(LOGO);
				printf("%s\n",info);
				printf("Max chars per line: %d, Max lines: %d\n",
					COLS-2*BDRWIDTH,MAXLINES);
				return 0;
			case 1: printf("%s\n",VERSION);
				return 0;

			case 2: 
				if(argc == 0){
					myerror("%s\n",USAGE);
					return 1;
				}
				if(strcmp(*argv,"--")==0){
					++argv;
					--argc;
				}
				if(argc == 0){
					myerror("%s\n",USAGE);
					return 1;
				}
				use_stdin = 0;
	                        strncpy(text[0],*argv++,COLS+1-2*BDRWIDTH-offset);
				argc--;
				line++;
				max_width = strlen(text[0]);
				break;
			case 3: 
				if(argc == 0){
					myerror("%s\n",USAGE);
					return 1;
				}
				offset = atoi(*argv++);
				if((offset < 0)||(offset > COLS-1-2*BDRWIDTH)){
					myerror("Useful range of offset is 0 to %d\n",
						COLS-1-2*BDRWIDTH);
					if(offset < 0)offset = 0;
					if(offset > COLS-1-2*BDRWIDTH)
						offset = COLS-1-2*BDRWIDTH;
				}
				argc--;
				break;
			case 4:
				myerror("%s\n",USAGE);
				return 1;
                        case -2: ;  /* barf (don't use in your code) */
			default:
				fprintf(stderr,"%s: illegal option\n",argstr);
				fprintf(stderr,"%s\n",USAGE);
				return 1;
			} /* End flag switch */
	} /* End flag grabbing while loop */
} /* end if: any args ? */

	if(argc > 0)myerror("Extra args on command line\n");


	/* read the input lines */
	/* keep track of longest read so far */

	if(use_stdin){
	  while( fgets(text[line++],COLS+1-2*BDRWIDTH-offset,stdin)){
		width = strlen(text[line-1]);
		/* Remove newline at end, if present */
		if(text[line-1][width-1] == '\n')
			text[line-1][width-- -1] = '\0';
		max_width = width > max_width ? width : max_width;
		if(line >= MAXLINES + 1){
			myerror("Remaining lines ignored. Only %d accepted\n",
					MAXLINES);
			break;
		}
	  }	
        line--;  /* Now gives actual number of lines read. */
	}

	/* calculate lengths and positions */
	width = max_width + 2*BDRWIDTH;

	/* print a top border */
	hborder(width,TOP);

	/* print body of text */
	for(i=0;i<line;i++)
	   puttext(text[i],max_width);

	/* print a bottom border */
	hborder(width,BOTTOM);

	return EXIT_SUCCESS;
}

		
/* print a specified number of repetitions of a char. */
void
repeat_char(int n, char ch)
{ 
	while (n-- > 0) 
		putchar(ch);
}

/* print a horizontal border */
void
hborder(int w,int which_one)
{
	repeat_char(offset,SPACE);
	if(which_one == TOP)
	   putchar(NW_CORNER);
	else
	   putchar(SW_CORNER);
	repeat_char(w-2,HBAR);
	if(which_one == TOP)
	   putchar(NE_CORNER);
	else
	   putchar(SE_CORNER);
	putchar('\n');
}

/* print message text  */
void
puttext(char *mesg,int text_width)
{
	int i;

	repeat_char(offset,SPACE);

	/* print left border area */
	putchar(VBAR);
	repeat_char(BDRWIDTH-1,SPACE);

	/* print message */
	printf("%s",mesg);

	/* Pad with blanks */
	for(i=strlen(mesg);i<text_width;i++)
		printf(" ");

	/* print right border */
	repeat_char(BDRWIDTH -1,SPACE);
	putchar(VBAR);
	putchar('\n');
}


/*  find_opt: return a unique small integer for each possible option string */
/*  There should be a case in the switch statement in main to handle each. */
/*  -2 reserved for arg not found -1 reserved for no options */

/* N.B. word doesn't contain the leading hyphen */

int find_opt(char *word)
{
	if(strcmp(word,"help")==0)return 0;
	if(strcmp(word,"version")==0)return 1;
	if(strcmp(word,"t")==0)return 2;
	if(strcmp(word,"o")==0)return 3;

/* User left out an arg  */
	if(strcmp(word,"to")==0)return 4;
	if(strcmp(word,"ot")==0)return 4;

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
	return;
}

