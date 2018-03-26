/* perms.c: generate all permutations of 1...n by various methods 

	By Terry R. McConnell

   I became interested in this topic while reading Zbigniew Michalewicz and
   David B. Fogel, How to Solve It: Modern Heuristics, Springer Verlag,
   Berlin, 2000. As discussed in this text, generation of all permutations
   is necessary, e.g., in algorithms that do exhaustive searches in the
   traveling salesman problem. 

   It turns out there is a rather large literature on generation of
   permutations, and the methods illustrated here only begin to scratch
   the surface. See, e.g., R. Sedgewick, Permutation Generation Methods,
   Computing Surveys, Vol. 9, No. 2, 137-164 */

/* Compile: cc -o perms -DMETHOD -DMETHODx perms.c
                                  ^^^^^^^^
                                  see below
*/

#include <stdio.h>
#include <stdlib.h>

#ifndef METHOD
/* Define one of METHODx x=1,2,3 depending on method selected */
#define METHOD3
#endif

#define NN 128     /* maximum allowable n. This should be big enough :-) */
static int p[NN]; /* global array to hold generated permutation */

/* 

factomial: generates the "digits" in the representation of a number in
terms of factorials. Each natural number between 0 and n! - 1 has a unique
representation as a sum of terms of the form d_i*(i-1)!, where d_i lies
in the range 0..i-1, and the sum is over i=2...n. 

Stores digits from least to most significant in fact_digs array, returns
the number of digits stored.

*/

static int fac_digits[NN];

int factomial(int m, int x)
{
	int nfact=1,n=2,d0=0,d1,i;

	for(i=0;i<m;i++)fac_digits[i]=0;

	do {
		nfact *=n;
		d1 = x % nfact; /* d1 is cum sum */
		fac_digits[n-2] = n*(d1-d0)/nfact;
		n++;
		d0 = d1;
		
	} while(nfact <= x);

	return n-1;
}

/* gen_perm1: first algorithm for generating permutations. Each time it
   is called, it stores the next permutation in p array. Returns 0 if all
   is well, 1 if there is an error. Call the value of n to generate next
   perm of 1..n 

   This algorithm relies on the fact that a permutation of 1..n can be
   written uniquely as a product of transpositions of the form
   (1 s1)(2 s2)...(n sn), with 1 <= si <= i.

   Proof is by complete induction on n >= 2:

	n=2:  to swap 1 and 2 do (1,1)(2,1). To leave them fixed do (1,1)(2,2).

	Suppose n > 2. We can suppose a given permutation is cyclic, since 
	with relabelings the complete inductive hypothesis can be applied to
        each cycle in a product of nonempty disjoint cycles. For the cyclic 
        permutation of 1...n-1, if we wish to insert n in the cycle at -> i 
        -> n -> j  we can take, by induction, 
		
		[ product for case n-1 taking i->j ] (n,j) 

   We take s_i to be the ith factomial digit of the argument passed
   in. 

*/

/* Helper */
int swap(int i, int j){

	int temp;
	temp=p[i];
	p[i]=p[j];
	p[j]=temp;
	return 0;
}

int gen_perm1(int n){

	int i;

	for(i=0;i<n;i++)p[i]=i+1;
	for(i=1;i<n;i++)
		swap(i,fac_digits[i-1]);
	return 0;
}


/* gen_perm2: 2nd algorithm for generating permutations of 1 ... n. Calls
   itself recursively. When called at recursive level k-1 with parameter i,
   its job is to store k in slot i-1 of the global array p. The array should
   be initialized to all zeros. A zero entry indicates to this routine a slot
   that needs to be filled, so after filling its entry it scans the array
   for nonzero entries, then calls itself to fill those entries. Upon return
   it cleans up by marking its own entry (the ith one) again with 0.

   If no zero entry is found (this can only happen at the bottom of the 
   recursion) the permutation is printed.

   In more detail, here is what happens at the first few levels of the 
   recursion:

   Level 0: The array is all zeros. Calls itself iteratively, with parameter
            successively = 1,2,...n. The call with parameter i will ultimately
            result in all permutations having a 1 in the ith position.

   Level 1: When called with parameter i, 1 is stored at position i. Scans
            for unfilled entries (marked with 0) and for each one found
            (at postion j, say) calls itself with parameter j. This call will
            ultimately result in all permutations having a 1 in position i
            and a 2 in position j.

  The mechanism should now be clear.

   For further discussion, see R. Sedgewick,
   Algorithms, 2nd Edition, Addison-Wesley, Reading, MA, 1977. 

*/

static int lvl = -1; /* The level of recursion */
static int nn;  /* set by main. The n in 1...n above */
 
int gen_perm2(int i)
{

	int q;

	lvl++;
	p[i-1] = lvl;

	if(lvl == nn){ /* Permutation complete. Here is where you would
                          do something with the permutation in a real
                          application. */ 	
		for(q=0;q<nn;q++)printf("%d",p[q]);
		printf("\n");
	}
	for(q=1;q<=nn;q++)
		if(!p[q-1])gen_perm2(q); /* fill next unfilled slot */

	/* clean up for return to higher level */
	lvl--;
	p[i-1] = 0;
	return 0;
}

/* gen_perm3: 3rd algorithm for generating all permutations of 1...n. This
   is essentially the ACM's Algorithm 71. (R.R. Coveyou and J.G. Sullivan,
   Permutation (Algorithm 71,) Communications of the ACM, 4(1961), p. 497.)
   It is similar to gen_perm1, except that the factomial digits (see above)
   are replaced by a static array whose contents are updated on each call.
   The pattern of generation of this static array is as follows: (n=3)

	000 -> 001 -> 002 -> 010 -> 011 -> 012

   Returns 1 when the last permutation has been generated, 0 else.
   
   N.B.: there are at least 2 typos in the psuedocode description of this
   algorithm presented in the Michalewicz and Fogel book (p. 63.)

*/

static xx[NN];

int gen_perm3(int n)
{
	int i;
	static int first_call = 1;

	if(first_call){ /* initialize */

		for(i=0;i<n-1;i++)
			xx[i] = 0;
		xx[n-1] = -1;
		first_call = 0;
	}

	/* Update xx array */
	
	for(i=n-1;i>0;i--){
		if(xx[i] != i)break;
		xx[i] = 0;
	}

	if(i==0)return 1;

	xx[i] = xx[i]+1;
	p[0] = 1;
        for(i=0;i<n;i++){
		p[i]=p[i-xx[i]];  
		p[i-xx[i]] = i+1;
	}
        return 0;
}

/* Test platform */
int main(int argc, char **argv)
{
	int i,j;

#ifdef METHOD1
	for(i=0;i<120;i++){
		factomial(4,i);
		gen_perm1(5);
		for(j=0;j<4;j++)
			printf("%d",p[j]);
		printf("\t(");
		for(j=0;j<4;j++)
			printf("%d",fac_digits[j]);
		printf(")\n");
	}
#endif
#ifdef METHOD2
	nn = 4;
	gen_perm2(1);
#endif
#ifdef METHOD3
		nn=4;
		while(gen_perm3(nn)==0){
			for(i=0;i<nn;i++)
				printf("%d",p[i]);
			printf("\t(");
			for(i=0;i<nn;i++)
				printf("%d",xx[i]);
			printf(")\n");
		}
#endif
	return 0;
}
