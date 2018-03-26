/* cover.c: Compute the expected time to obtain ("cover") a given
              	    string of zeros and ones by a sequence of iid Bernoullis.

	compile: cc -O2 -D_POSIX_SOURCE -o e_cover cover.c

	usage: e_cover string_of_zeros_and_ones

	The only limit on the length of the argument is the one imposed by
	the operating system.

	(Another potentially useful routine, PrintAsDecimal, prints an
	arbitrarily long string of 0s and 1s as a base 10 decimal.)
*/


/* Algorithm (pointed out to me by  P.Griffin) If s is the given string of 
	length n Then E T(s) = E T(t) + 2^n, where t is the longest string
	such that s = t.*t. Perhaps surprisingly, the algorithm shows that
        the expected time is always an integer. 

	The algorithm applies much more generally, e.g., to find the expected
	time to first encounter a given sequence of states in a finite-state
	ergodic Markov chain. (In that case, 2^n must be replaced by a
	quantity depending on the transition matrix and stationary 
	probabilities.)

   Also see: 

	Example 4.20, pp 160-161 in S.M. Ross, Introduction to
	Probability Models 5th Ed., Academic Press, San Diego, 1993 

	G. Blom, On the mean number of random digits until a given sequence
	occurs, J. Appl. Prob. 19(1982), 136-143.

	P.T. Nielsen, On the expected duration of a search for a fixed pattern
	in random data, IEEE Trans. Information Theory 19(1973), 702-704.
*/

/* Program by T. McConnell, 5/17/94  */

#define USAGE "Usage: ranstring string_of_0s_and_1s"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <limits.h>

char *last_equal_first(char *strptr);
void expected_cover_time(char *,char *);
int PrintAsDecimal(char *);

int
main(int argc, char *argv[])
{
	char *time_in_binary;
	int i;
	int L;

/* Sanity Checks */

	if(argc != 2){ 
		fprintf(stderr,"%s\n",USAGE);
		return 1;
	}

	L = strlen(argv[1]);

	if(L > ARG_MAX) {
		/* O.S. will probably complain anyway */
		fprintf(stderr,"Input string too long. Max is %d\n", ARG_MAX);
		return 1;
	}

	for(i=0;i<L;i++)
		if((*(*(argv+1)+i) != '0') && (*(*(argv+1)+i) != '1')){
		 	fprintf(stderr,"Input string must consist of 0s and 1s only\n");
			return 1;
	}
	
	/* Allocate buffer to hold expected cover time in binary */

	if((time_in_binary = (char *)malloc(L+2))==NULL){
		fprintf(stderr,"Cannot allocate array for answer\n");
		return 1;
	}
	for(i=0;i<L+1;i++)time_in_binary[i]='0';
	time_in_binary[i] = '\0';

	/* Do it ! */

	printf("For string %s, ",argv[1]);
	expected_cover_time(argv[1],time_in_binary);	
	printf("expected time to cover = ");
	PrintAsDecimal(time_in_binary);
	printf("\n");
	free(time_in_binary);
	return 0;
}

/* last_equal_first(char *strptr): returns a pointer to the beginning
of the longest string at the end of passed string which also occurs at
the beginning. */

char *last_equal_first(char *strptr)
{
	char *ptr;
	int len;

	if((len=strlen(strptr)) <=1)return NULL;
	ptr = strptr + 1;
	while((strstr(strptr,ptr)!=strptr)&&(ptr - strptr < len))ptr++;
	return ptr;
}


/* Call this recursively, as explained in above description of algorithm. */

void expected_cover_time(char *target, char *time_in_binary)
{
	int N;
	int L;

	if(target==NULL)return;
	N = strlen(target);
	if(N<=0)
		return; 
	L = strlen(time_in_binary);
	time_in_binary[L-N-1] = '1'; /* Add 2^N to accum. time */
	expected_cover_time(last_equal_first(target),time_in_binary);
	return;
}

/* Stuff below is only used to print out the answer as a base 10 number */

/* Convert 5 byte array of '0's and '1's to an int. Array can be
	either <= 4 bytes + null terminator, or 5 bytes without null
	terminator.  */
int
bin2d( char *binary)
{
	int num = 0;
	int i = 0,j;
	
	/* Handle the case binary is a null terminated array */
	for(j=0;j<5;j++)
		if(binary[j] == '\0'){
		    while(i<j) num = 2*num + (*(binary + i++)-'0');
		    return num;
		}
	while(i<5) num = 2*num + (*(binary + i++)-'0');
	return num;
}


/* Divide a long array of zeros and ones by 10 = "1010". Return remainder as
   an int and quotient in array pointed to. Dividend arg is destroyed. This
   implements binary "long division".  NB: quotient must point to an area
   at least as large as dividend. */

int
divby10(char *dividend, char *quotient)
{
	int i,j;
	char *l;    /* pointer to leading one in dividend */

	/* Fill the quotient array with digit zeros */
	for(i=0;i<strlen(dividend);i++) quotient[i] = '0';
	quotient[i] = '\0';

	/* Find first '1' in dividend */
		l = strchr(dividend,'1');
		if(l==NULL) return 0;

	while((strlen(l)>5) || ((j=bin2d(l)) > 9)){
				    /* only done when first fails */


	/* See if l(l+1)(l+2)(l+3) > 9, equivalently,
		l(l+1)...(l+4) > 19 */

		if((strlen(l) <= 4)||(bin2d(l) > 19)){ 
			quotient[ l - dividend + 3 ] = '1';

	/* We must do the subtraction:

		l(l+1)(l+2)(l+3)
	      -	1  0    1    0
	      ___________________
		0  ?    ?   (l+3) 
	*/

			*l = '0';
			if( *(l+2) == '0' ) { /* Then we borrow from l+1, which
						 _must_ be 1 */
				*(l+1) = '0';
				*(l+2) = '1';	
			}
			else *(l+2) = '0';  /* and leave l+1 alone */
		}
		else {
			quotient[ l - dividend + 4 ] = '1';

	/* We must do the subtraction:

		l(l+1)(l+2)(l+3)(l+4)
	       -   1    0    1    0                UGLY!!!!
		--------------------
		0  ?    ?    ?   (l+4) 
	*/


			*l = '0';
			if(*(l+3)=='0'){ /* Must try to borrow from l+2 */
				*(l+3) = '1';
				if(*(l+2) == '1'){
					*(l+2) = '0';
					*(l+1) = '1'; /* Must have to borrow
                                                        from l, else we would
							not be in this case */
				}
				else { /* Must try to borrow from l+1 */
					*(l+2) = '1';
					if(*(l+1)=='1') *(l+1) = '1';
						else *(l+1) = '0';
				} 
			}
			else { /* l+3 is a 1 */
				*(l+3) = '0';
				/* l+2 is unchanged and l+1 _must_ be 0 */
				*(l+1) = '1';
			}
		}

	/* Find next '1' in dividend */
		l = strchr(dividend,'1');
		if(l==NULL) return 0;
	} /* end while */

	return j;   /* = bin2d(dividend) */
}

/* Print an arbitrarily long string of '0's and '1's, interpreted as
   a binary number, in base 10 format (Needs: divby10 and bin2d) */

int
PrintAsDecimal(char *long_num)
{
	char *quotient;
	char *dec_buf;
	char *l;
	int j, i =0;

	l = strchr(long_num,'1');
	if(l==NULL){
		printf("0");
		return 0;
	}

	quotient = (char *) malloc(strlen(long_num)+1);

	/* Buffer to hold decimal digits as they're generated. We don't need
           it this long, of course */

	dec_buf = (char *) malloc(strlen(long_num)+1);

	if((quotient == NULL) || (dec_buf == NULL)){
		fprintf(stderr,"PrintAsDecimal: Unable to malloc space \n");
		return 1;
	}
	while((strlen(l) > 5)||((j=bin2d(l)) >9)){
		dec_buf[i++] = (char)divby10(long_num,quotient);
		strcpy(long_num,quotient);
		l = strchr(long_num,'1');
	}
	dec_buf[i++]=(char)j;

/* Print dec_buf in reverse order */
	while(i--)printf("%d",dec_buf[i]);

	free(quotient);
	free(dec_buf);
	return 0;
}
