/* KMP.c: Implements strstr library function using the Knuth-Morris-Pratt 
   substring search algorithm.

	By Terry R. McConnell

  The entry point my_strstr is an implementation of the standard C library
  function strstr using the KMP algorithm. Extensive documentation is
  provided in the form of comments. There is also a main program invoked
  as "kmp <target string>" (or with -h option for usage information) which
  searches for the first occurence of the target in the stdin stream and
  prints information about where it was found.

  Reference: Knuth, D.E., J.H. Morris, and V.R. Pratt, "Fast pattern matching
  in strings", SIAM J. Computing, 6:2, 323-350.

  Compile: cc -o kmp kmp.c 
  Include -DMAX_SOURCE=... to change size of largest string that can be
  searched.

*/

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#ifndef MAX_SOURCE
#define MAX_SOURCE 16384
#endif

#define VERSION "1.1"
#define PROGNAME "kmp"
#define USAGE "kmp [-flnvhr] target"
#define HELP "-h: print this helpful message\n\
-v: print version number and exit\n\
-l: use library version of strstr instead. (Useful for benchmarking.)\n\
-f: list definition of the Knuth-Morris-Pratt failure function for target\n\
-r: print the shortest repeating prefix of target \n\
-n: count occurrences of target in source and exit\n\
Read stdin and find first occurence of target string in it. \n\
Print information about where it was found.\n\n"

int *f;  /* Pointer to array containing values of failure function */

/* Create the failure function for target string. The failure function
   is defined as follows, for i = 1 to m, where m is the length of
   the target string: Let the target string be b_1...b_m. Then f(i) is
   the length of the longest proper suffix of b_1...b_i which is a prefix
   of the target string. 

   If you are trying to find the first occurence of a target string in
   a stream of input characters and the i+1st character is wrong, you
   need not start over looking for the beginning of the target from the
   next received character on, because the last few characters received
   might still be a valid head start on forming the target. The failure
   function tells exactly how many characters head start you have. 

   In more colorful language, the idea behind this algorithm is: Don't
   throw the baby out with the bathwater. 

   The code below was translated into C from psuedo code given on page
   151 of Aho, Sethi, and Ullman. Unfortunately, the C-convention of
   starting array indices from 0 makes the code harder to follow than it
   should be. To make it easier, we never use f[0]. */

int make_f(const char *target){

	int t = 0; /* think of t+1 as the length of the current valid
                      prefix. */
	int s,m;

	m = strlen(target);
	f = malloc((m+1)*sizeof(int));


	f[1] = 0; /* When first character is received there is no proper
                     suffix. */

	for(s=1;s<m;s++){ /* Create f[s] */

		while((t > 0) && (target[s] != target[t])) 
			t = f[t];  /* This is the subtle part of the
                                 algorithm. The target does not begin
                                 t_0...t_t t_s. We could examine t_1...t_s,
                                 t_2...t_s,... successively to see which (if any)
                                 gives a valid prefix. But this is not
                                 necessary. Say the longest valid prefix is
                                 t_k...t_t t_s. Then t_k...t_t is also a valid
                                 prefix. But f(t) gave the length of the 
                                 longest valid prefix of the form t_k...t_t.
				 Thus no k such that t_k..t_t is longer than
                                 f(t) need be considered. Now iterate this
                                 reasoning to conclude that only candidates
                                 t_k...t_t of lengths f(t), f(f(t)),...
                                 need be considered. */ 

		if(target[t] == target[s]){ /* The next character can be
                                               added to current prefix to get
                                               a longer valid prefix. Good! */
			t++;
			f[s+1] = t;
		}
		else f[s+1] = 0; /* Clearly the while loop above must have
                                    exited due to t=0. */
	}
} 

/* Theoretical note: the assignment t = f[t] can be done at most m-1
   times, where m is the length of the target string (although the
   while loop can iterate more than once on given pass. For example,
   if the target is abababb, then f(6)=4, f(f(6))=2, and f(f(f(6)))= 0.
   The assignment is done 3 times when s = 6.) It is then easy to see that
   the running time of the routine make_f is O(m).

   Lemma. Let f be a non-negative integer valued function such that 
   f(1) = 0, and so that f is "continuous upwards", i.e., f(t+1) <= f(t) + 1.
   Then

		__
     A = 	\   [ f(s) - f(s+1) ]+   <=  (l-1),
		/__

     where a+ denotes the maximum of a and 0, and a- denotes the minimum
     of a and 0. Here and below, all sums run from 1 to l-1.

	Proof: Since a = (a+) + (a-) for any number a,  we have
				__
	-f(l) = f(1) - f(l) =	\   [ f(s) - f(s+1) ]    = 
				/__
		__
        A + 	\   [ f(s) - f(s+1) ]-    >=    A  - (l-1).
		/__

	Using f(l) >= 0 and rearranging, we obtain the desired inequality.

								QED.

   Consider now a given iteration of the 'for' loop with a given value
   of s. Let n be the number of times the assignment t = f(t) is done.
                               (n)              (n)          (n)
   Note that f(s+1) is either f (s) + 1, or 0 = f(s), where f  denotes the
   n-1 fold iterate of f. Since f(t) < t (only proper suffixes are 
   considered,) we have 

	 (n)
	f (s)  <=  f(s) - n + 1.

   Thus, in either case, f(s) - f(s+1) >= n. Now apply the lemma to obtain
   the desired bound on the sum of n.
   
*/

/* Return a pointer to the first occurence of target string in source
*  string, or null if the target does not occur in the source. 
*/ 

char *my_strstr(const char *src, const char *target){

	int s = 0; /* Trie state */
	int i;     /* Current index of source char */
	int m;

	make_f(target); /* Create failure function for this target */
	m = strlen(target);

	for(i=0;i<strlen(src);i++){
		while( (s>0) && (src[i] != target[s]))
			s = f[s];
		if(src[i] == target[s]) s++;
		if(s==m) return ((char *)src+i-m+1);
	}
	return NULL;
}

/* Return a pointer to the shortest repeating prefix of s. Note that
   s is destroyed in the process. 

   The shortest repeating prefix a string s is defined to be 
   the shortest string t such that s = t^k, where the exponentiation denotes
   string concatentation. The following algorithm works because the 
   length of t clearly belongs to m = strlen(s), f(m), f(f(m)), ... */

char *srepp(char *s){

	int m,u,k;
	char *buf;

	m = strlen(s);
	buf = malloc(m+1);
	buf[0]='\0';
	make_f(s);

	/* u is a trial repeating prefix */
	u = m;

	while((u=f[u])>0)

		/* If u is a repeating prefix, set m = u: m records the
                   length of the shortest repeating prefix found so far */ 
		
		if(m%u==0){     /* u must divide m */
			k = m/u;
			while(k--)
				strncat(buf,s,u); 
			if(strcmp(s,buf)==0){
				m=u; 
				s[m]='\0';
			}
			buf[0]='\0';
		}
	return s;
}

/* Print out the failure function */
void list_f(char *target){

	int i;

	printf("Failure function for %s:\n", target);
	for(i=1;i<=strlen(target);i++)
		printf("f[%d] = %d\n",i,f[i]);
}


int
main(int argc, char **argv){

	char source[MAX_SOURCE],*target,*p;
	int c,i=0;
	int f_flag = 0,n_flag = 0,r_flag=0;
	int lines = 0;
	char *ll;
        char *(*use_strstr)(const char *,const char*) = &my_strstr;

	/* Process command line */
	if(argc <= 1){
		fprintf(stderr,"%s:%s\n",PROGNAME,USAGE);
		return 1;
	}
	i=1;
	while(argv[i][0]=='-'){
		switch(argv[i][1]){
			case 'f':
				f_flag=1;
				break;
			case 'l':
				use_strstr = &strstr;
				break;
			case 'n':
				n_flag=1;
				break;
			case 'v':
				printf("%s\n",VERSION);
				return 0;
			case 'h':
				printf("%s\n",USAGE);
				printf("%s\n",HELP);
				return 0;
			case 'r':
				r_flag = 1;
				break;
			default:
				fprintf(stderr,"%s:%s\n",PROGNAME,USAGE);
				return 1;
		}
		i++;
	}

	if(i != argc-1){
		fprintf(stderr,"%s:%s\n",PROGNAME,USAGE);
		return 1;
	}

	target = argv[i];

	if(r_flag){
		printf("%s\n",srepp(target));
		return 0;
	}

	if(f_flag){
		make_f(target);
		list_f(target);
		return 0;
	}

	/* Get the source string from stdin */

	i=0;
	while((c=getchar())!=EOF){
		source[i++]=(char)c;
		if(i >= MAX_SOURCE){
			fprintf(stderr,"Source too long. Must be < %d\n",
				MAX_SOURCE);
			return 1;
		}
	}
	source[i] = '\0'; /* properly terminate */


	if(n_flag){
		make_f(target);
		c=0;
		p=source;
		while(p=use_strstr(p,target))c++,p++;
		printf("Target '%s' found %d times in source.\n",target,c);
		return 0;
	}

	/* Search for target in source */

	p = use_strstr(source,target);
	printf("target = %s\n",target);

	if(p){
		/* don't print out lines before the one containing target */
		ll = source; /* Position of start of most recent line */
		for(i=0;i<p-source;i++) 
			if(source[i]=='\n'){
				ll = source+i+1;
				lines++;
			}
		if(lines)
			printf("...line %d:\n",lines+1);

		/* print the line containing target */
		i = ll-source;
		while(i<strlen(source)){
			if(source[i]=='\n')break;
			printf("%c",source[i++]);
		}
		printf("\n");

		/* underline part of line containing target with ^ */
		for(i=0;i<p-ll;i++)
			printf(" ");
		for(i=0;i<strlen(target);i++)
			printf("^");
		printf("\n");


	}
	else printf("Not found in source\n");
	return 0;	
}
