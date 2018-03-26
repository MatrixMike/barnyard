/* tutney.c: translate to and from tutney.

		By Terry R. McConnell (August, 2002)

*/


/* Tutney is a spelling language similar to Pig Latin. It is used as a
   simple secret code by parents so that their children can't understand
   what is being said. (Unfortunately, children break the code rather easily.) 

   The rules are simple: each letter of a word is replaced by a codeword
   which is spoken. Vowels are pronounced as is, and most consonants are
   doubled with the letter u in between. Thus, "car" becomes "cuc a rur." 
   There are 5 exceptions to this rule: h is "hash", j is "judge",
   q is "quack", w is "wac", and y is "yac". Some regional variations may exist.

   In normal operation, this program reads text from its standard input and 
   writes tutney to its standard output. Non-letters are echoed unchanged. 
   Case is preserved. Tutney syllables are separated with a character 
   (defined as PAD_CHAR below) rather than whitespace to preserve divisions
   between words.

   When used to translate FROM tutney (/i option) the separation character is
   removed. Note that if text originally contained the separation character 
   it might not be handled properly on retranslation.

   The source uses only the standard C-library and should be completely 
   portable. (Very old compilers might not handle multiple line string
   constants.)

   */

/* Compile: cc -o tutney tutney.c
   Use -DUNIX to enable usual unix option conventions.
   Use -DPAD_CHAR=[whatever] to replace default code separator.

   Install: Move executable to a directory on your search path.
*/

#ifdef UNIX
#define OPSEP '-'
#define USAGE "tutney [-i -h -v] [ < input ] [ > output ]"
#define HELP "\n\nUsage: tutney [-i -h -v] [ < input ] [ > output] \n\n\
Bracketted items are optional. With no options, read text from terminal and\n\
print tutney to terminal.\n\n\
   -h:   print this helpful information and exit.\n\
   -v:   print version number and exit.\n\
   -i:   translate tutney back to text.\n\n"
#else
#define OPSEP '/'
#define USAGE "tutney [/i /h /? /v] [ < input ] [ > output ]"
#define HELP "\n\nUsage: tutney [/i /h /? /v] [ < input ] [ > output] \n\n\
Bracketted items are optional. With no options, read text from terminal and\n\
print tutney to terminal.\n\n\
   /? or /h: print this helpful information and exit.\n\
   /v:       print version number and exit.\n\
   /i:       translate tutney back to text.\n\n"
#endif

#define VERSION "1.0"
#define PROGRAM_NAME "Tutney"

#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<stdarg.h>
#include<ctype.h>

#define TRUE 1
#define FALSE 0
#define MAX_EXPAND 5  /* q = quack */
#ifndef PAD_CHAR
#define PAD_CHAR '-'  /* letter separator within a word */
#endif
#define MAX_TOKEN 2048

int scan(FILE *,char *);
int is_regular(char);
int is_vowel(char);
int expand(char *, char *);
int contract(char *, char *);
void my_error(char *fmt, ...);

static int reverse = FALSE;  /* flag: TRUE if translating from tutney */

/* The scanner puts the next token in the buffer and returns the number of
        characters read, -1 at EOF. Tokens are defined as contiguous strings
		of letters of the alphabet. */

int scan(FILE *input_stream, char *buf){
        int c;
        static int last_char = 0;
        int count = 0;

        /* If there is a char left over from last call, echo it and go on. This
		   is done so that non-alphabetic characters are passed 
                   through in their proper position in the output. */

        if(last_char)putchar(last_char);
        last_char = 0;

        while(TRUE){ /* put stuff until first non letter in buf */

                c = fgetc(input_stream); /* fetch next character */
		if(c==EOF){ 
			if(count){ /* In case EOF terminates a
					 word. If so, this
					   routine will get
                                          called one more time. */
				buf[count]='\0';
				return count;
			}
                    	return -1; /* signal EOF to caller */
		}
				
                if(isalpha(c))
                        buf[count++]=c; /* Just accumlate letters in buf */
                else {
			if((c==PAD_CHAR)&& reverse && (count > 0)); /* Throw
                                                                     away */
                    	else last_char = c; /* retain to print on 
                                                next call */
                    	break;
                }
                                 
        }
	buf[count]='\0'; /* properly terminate string */
        return count;
       
}

/* TRUE if passed char is a vowel (upper or lower case), false otherwise. */

int is_vowel(char c){

                switch(tolower(c)){

                        case 'a':
                        case 'e':
			case 'i':
                        case 'o':
                        case 'u':
                                return TRUE;
                        default:
                                return FALSE;
                 }
}

/* True if passed char is an irregular consonant (h,j,q,w,y), upper or lower
   case.  False otherwise. */

int is_irregular(char c){

        switch(tolower(c)){

                case 'h':
                case 'j':
                case 'q':
                case 'w':
                case 'y':
                        return TRUE;
                default:
                        return FALSE;

        }
}


/* Expand word in source into tutney in buffer. Source should contain
   a null terminated string of letters of the alphabet only. Return number of
   letters in buffer. */

int expand(char *source, char *buffer){

 	int n,i;
	char c;
	int count=0;

	n = (int)strlen(source);
	if(!n)return 0; /* Nothing to do */

	for(i=0;i<n;i++){ /* loop over letters in source */
		c = source[i];
		if(is_vowel(c)){ /* vowels are not expanded */
			buffer[count++]=c;
			buffer[count]='\0';
		}
		else
			if(is_irregular(c)){     /* each irregular consonant
                                                  expands a different way */
				buffer[count++]=c;
				buffer[count]='\0';
				switch(tolower(c)){
					case 'h':
						strcat(buffer,"ash");
						count += 3;
						break;
					case 'j':
						strcat(buffer,"udge");
						count += 4;
						break;
					case 'q':
						strcat(buffer,"uack");
						count += 4;
						break;
					case 'w':
						strcat(buffer,"ac");
						count += 2;
						break;
					case 'y':
						strcat(buffer,"ac");
						count += 2;
						break;
					default:
						my_error("%c is not irregular consonant\n",c);
						break;
				}
			}
			else   /* regular */
				if(isalpha(c)){ /* must be regular consonant */
					buffer[count++] = c;
					buffer[count++] = 'u';
					buffer[count++] = tolower(c);
					buffer[count]='\0';
				}
				else  /* This should not happen */
					my_error("Internal: expand called with non-alphabetic %c\n",c);

                 /* all but last time through pad with letter separator */

		if(i<n-1){ /* the last letter in a word should not be
	  			 followed by the padding character */
			buffer[count++]=PAD_CHAR;
			buffer[count]='\0';
		}

	} /* end of for loop over letters in source */

                
	return count;
}

/* If source is a valid tutney code, put the corresponding letter in buf, null
   terminate. Otherwise, issue an error message and copy source to buf. Return
   the length of the string in buf. This is the inverse of the expand routine. 
*/

int contract(char *source, char *buf){
	int n;
	char c;
	int error_flag = FALSE;

	n = (int)strlen(source);

	if(!n)return 0;

	c = source[0];

	if(is_vowel(c)){
		if(n>1)error_flag = TRUE;
	}
	else
	  if(is_irregular(c)){ /* Bug: something like "bash" gets through, but
							 should not */
		if( (strcmp(source+1,"ash")!=0)&&
		    (strcmp(source+1,"ac") !=0)&&
		    (strcmp(source+1,"uack") !=0)&&
		    (strcmp(source+1,"udge") !=0))error_flag = TRUE;
	  }
	  else 
	        if((strlen(source)!=3)||(source[1] != 'u')||
                    (source[2]!=tolower(c)))
			error_flag = TRUE;

	if(error_flag){
		my_error("syntax: word %s is not valid tutney\n",source);
		strcpy(buf,source);
		return n; 
	}	

	/* Else valid tutney code. Put corresponding letter in buf and return */

	buf[0]=c;
	buf[1]='\0';
	return 1;
}


/* Buffers used only in Main */

static char token_buf[MAX_TOKEN];
static char expand_buf[MAX_TOKEN*MAX_EXPAND+MAX_EXPAND+1];

int main(int argc, char **argv){
        int n;
	int i=1;

	/* process command line */
	while((i<argc)&&(argv[i][0]==OPSEP)){
	   switch(argv[i][1]){
		
		case 'v':
			printf("%s\n",VERSION);
			return 0;
		case 'h':
		case '?':
			printf("%s\n",HELP);
			return 0;
		case 'i':
			reverse = TRUE;
			break;
		default:
			my_error("Usage: %s\n",USAGE);
			return 1;
	   }
	   i++;
	}


    while(TRUE){
            n = scan(stdin,token_buf); /* get next token */
	     if(n==-1)break; /* at eof */
		 if(n){ 
			if(reverse) /* Bug: return values of contract/expand are
					  ignored */
				contract(token_buf,expand_buf);
			else
				expand(token_buf,expand_buf);
				printf("%s",expand_buf);
		}
     }
     return 0;
}
 

/* Generic variadic error reporter */

void my_error(char *fmt, ...){
			
	va_list args;

	va_start(args,fmt);
	fprintf(stderr,"%s: error: ", PROGRAM_NAME);
	vfprintf(stderr,fmt,args);
	va_end(args);
	return;
}

/* End of file */

