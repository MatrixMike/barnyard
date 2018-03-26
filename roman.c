/* roman.c: A program to translate Roman numerals into integers, being
*	essentially a solution of problems 2.6 and 2.10 in Aho, Sethi,
*	and Ullman.

	By Terry R. McConnell

	Compile: cc -o roman roman.c

	usage: roman

	The program expects to read lines on stdin containing XXXXX
	where XXXXX is a roman numeral.  It prints the corresponding integer
        value. If the syntax of the numeral is wrong it prints an error 
	message.  Roman numerals must be uppercase. Whitespace is not
        ignored.

	The largest roman number this program recognizes is MMMCMXCIX,
	so this program has a "Y4K problem." Some roman numeral systems
        allow D, C, L, M with bars over them for 5,000's, 10,000's, etc.

	See Number Words and Number Symbols: A Cultural History of Numbers,
	Karl Menninger (transl. by Paul Broneer), MIT Press, 1977, for
	an interesting discussion of the origins of Roman numerals.
*/

/* A part of the grammer (with syntax-directed translation actions) to
   give the idea:

fifties: XC {+90} tens | L {+50} tens  
tens: XL {+40} fives | X{+10} fives | XX{+20} fives | XXX {+30} fives  
fives:  IX {+9} | V ones {+5}
ones:   IV {+4} | I {+1} | II {+2} | III {+3} 

Note: the actions add the indicated values to a global variable called num.
(There are missing nonterminals/productions for hundreds, fivehundreds,
and thousands. That's as far as we go.)  

We can use recursive descent (predictive) parsing, since every production
begins with a distinct token. 
*/

/* Here are the tokens. The values assigned are arbitrary, except that it is
   good practise to choose values >= 256 so they don't interfere with actual
   character values.
*/

#define M  256
#define CM 257
#define D  258
#define CD 259
#define C  260
#define XC 261
#define L  262
#define XL 263
#define X  264
#define IX 265
#define V  266
#define IV 267
#define III 268
#define II 269
#define I  270

#include<stdlib.h>
#include<stdio.h>

static unsigned num;
static int lookahead; 
static int line = 1;

static void error(char *msg){
	fprintf(stderr,"Syntax error line %d. %s",line,msg);
}

static int next_token(void);   /* Forward declaration. Defined at end */

static void match(int foo) {
	if(lookahead == foo) {
		lookahead = next_token();
		return;
	}
	else error("Unexpected token\n");
}

static void fivehundreds();  /* Forward declarations. Defined after thousands */
static void hundreds();
static void fifties();
static void tens();
static void fives();
static void ones();

static void thousands(){

	int count; 

	count = 0; 
	while(lookahead == M){ 
		if(count++ >= 3) error("Too many Ms\n"); 
		num += 1000;
		match(M); 
	}
	fivehundreds();
}

static void fivehundreds(){

	if(lookahead == CM) {
		match(CM); num += 900; fifties(); return;
	}
	else if(lookahead == D){
		match(D); num += 500; 
	}
	hundreds();
}

static void hundreds() {

	int count;  

	if(lookahead == CD) {
		match(CD); num += 400; fifties(); return;
	}
	count = 0;
	while(lookahead == C){
		if(count++ >= 3) error("Too many Cs\n");
		num += 100;
		match(C); 
	}
	fifties();
}

static void fifties() {
	if(lookahead == XC) {
		match(XC); num += 90; fives(); return;
	}
	else if(lookahead == L){
		match(L); num += 50; tens(); return;
	}
	else tens();
}

static void tens() {

	int count; 

	if(lookahead == XL) {
		match(XL); num += 40; fives(); return;
	}
	count = 0;
	while(lookahead == X){
		if(count++ >= 3)error("Too many Xs\n");
		num += 10;
		match(X);
	}
	fives();
}

static void fives() {

	if( lookahead == V){
		match(V); num += 5; 
	}
	ones();
}

static void ones() {
	if(lookahead == IX){ 
		match(IX);
		num += 9; 
	}	
	else
	if(lookahead == IV){ 
		match(IV);
		num += 4; 
	}
	else
	if(lookahead == I){
		match(I);
		num += 1; 
	}
	else 
	if(lookahead == II){
		match(II);
		num +=2; 
	}
	else 
	if(lookahead == III){
		 match(III);
		 num +=3; 
	}
}


int main()
{
	while((lookahead = next_token()) !=EOF){
		num = 0;
		thousands();
		printf("%d\n",num);
		if(lookahead != '\n'){
			error("Extra characters at end of line.\n");
			while((lookahead = getchar())!= '\n');
		}
		line++;
	}
	return 0;
}

	
/* next_token: look for and return tokens M, CD, D, C, XC, L, XL, X, IX,
   IV, V, I, II, and III in the input stream. The prefix flag is set when
   a C, X, or I is read, since these may be part of a two character token.
   I, II, and III are handled as special cases. Returns EOF at end of file.
   All other characters are returned as is.
*/

int next_token(){

	int c, prefix = 0; /* flag */
	
	while((c = getchar())!= EOF)
	switch(c){
		case 'M':
			if(prefix) return CM;
			return M;
		case 'D':
			if(prefix)return CD;
			return D;
		case 'L':
			if(prefix)return XL;
			return L;
		case 'V':
			if(prefix)return IV;
			return V;

		case 'C':
			if(prefix) return XC;
			if(((c = getchar())=='D')||(c=='M')){
				prefix = 1;
				ungetc(c,stdin);
				break;
			}
			ungetc(c,stdin);
			return C;
		case 'X':
			if(prefix) return IX;
			if(((c = getchar())=='C')||(c=='L')){
				prefix = 1;
				ungetc(c,stdin);
				break;
			}
			ungetc(c,stdin);
			return X;
		case 'I':
			if(((c = getchar())=='V')||(c=='X')){
				prefix = 1;
				ungetc(c,stdin);
				break;
			}
			if(c == 'I'){
				if((c=getchar()) == 'I') return III;
				ungetc(c,stdin);
				return II;
				}
			else {
				ungetc(c,stdin);
				return I;
			}
		default:
			return c;
	}
	return EOF;
}
