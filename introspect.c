/* Introspect.c: print your own listing to stdout. This can be viewed as
 * a very simple, source code level, computer virus. It is allowed not to
 * replicate comments or readability whitespace. 
 *
 * 	By Terry R. McConnell
 *
 * Compile: cc -o introspect introspect.c
 *
 * You could then do 
 *
 * 	introspect > foo.c
 * 	cc foo.c
 * 	a.out > foo.c
 *
 * and continue as many times as desired. */ 

/* Think of this as the "genetic material": */

static char dna[] = "#include<stdlib.h>\n#include<stdio.h>\n#include<string.h>\n char *quote(char *,char *); int main(){ char buffer[2048]; printf(\"static char dna[]=\\\"%s\\\";\\n%s\",quote(dna,buffer),dna);return 0;}char *quote(char *in,char *out){int i=0,j=0;for(i=0;i<strlen(in);i++){switch(in[i]){case \'\\\\\':case \'\\n\': case \'\\\"\': case \'\\\'\': out[j++]=\'\\\\\';break;default:;}if(in[i]==\'\\n\')out[j++]=\'n\';else out[j++]=in[i];}out[j]=\'\\0\';return out;}";

#include<stdlib.h>
#include<stdio.h>
#include<string.h>

char *quote(char *,char *);

int main(){
	char buffer[2048];
	printf("static char dna[] = \"%s\";\n%s",quote(dna,buffer),dna);
	return 0;
}

/* quote: stick a \ in front of every special char that appears in the passed
 * string. The special chars are \, newline, " and '. Newline has to
 * be handled slightly differently because of the need for preprocessor
 * commands to appear on separate lines.  */

char *quote(char *in,char *out){
	int i=0,j=0;
	for(i=0;i<strlen(in);i++){
		switch(in[i]){
			case '\\':
			case '\n':
			case '\"':
			case '\'':
				out[j++]='\\';
				break;
			default:
				;
		}
		if(in[i]=='\n')out[j++]='n';
		else
			out[j++]=in[i];
	}
	out[j]='\0';
	return out;
}

