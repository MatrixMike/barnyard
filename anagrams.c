/* anagrams.c: 

   Author: Terry R. McConnell

   Print all anagrams occuring in a dictionary file, or all anagrams 
   of a word supplied on the command line. The dictionary file can be
   specified with -d option. 

   The algorithm is as suggested in column 2 of Programming Pearls, Jon
   Bentley, 2nd Ed., Addison-Wesley, Reading, 2000.

   First sort each word on letters, producing an indentifier, or tag, to be 
   attached to the word. Then sort words by tag. This brings all anagrams
   together in a block.  

	compile: cc -o anagrams [-DDICT="..."] anagrams.c

   -DDICT can be used to set default dictionary. (default DICT is
   "/usr/dict/words".)

*/

#include <stdio.h>
#include <stdlib.h>

#ifndef DICT
#define DICT "/usr/dict/words"
#endif

#define VERSION "1.0"
#define USAGE "anagrams [ -d file  -h -v] [word]"
#ifndef _SHORT_STRINGS
#define HELP "anagrams [ -d file -h -v ] [word]\n\n\
Print anagrams of given word in dictionary, or all anagrams.\n\n\
-d: Use next argument filename of dictionary (default: /usr/dict/words)\n\
-v: Print version number and exit. \n\
-h: Print this helpful information. \n\n"
#else
#define HELP USAGE
#endif


#define MAX_LINE 255    /* longest allowed line in dictionary */
#define MAX_WORD 255    /* longest word to be considered */

static char dict_name[256] = DICT;

struct DICT_ENTRY { /* entry created as we read through dict file */
	char *tag;
	char *word;
};

/* Comparison routines for sorting */

int comp_char(char *f, char *s){
	return *f - *s;
}

int comp_entries(struct DICT_ENTRY *f, struct DICT_ENTRY *s){
	return strcmp((const char *)(f->tag), (const char *)(s->tag));
} 

/* For some reason, stdlib qsort won't work for me. Using my own version. */

static void krqsort( void *v[],int left, int right, int (*comp)(void *,void *));

int main(int argc, char **argv)
{

	int i,j=0,dict_size=0,k;
	int print_all = 1;  /* flag */
	FILE *dict_file;
	char word_buf[MAX_WORD+1]="";
	char line_buf[MAX_LINE+1];  
	struct DICT_ENTRY **dict;

	/* Process command line */
	while(++j < argc){
		if(argv[j][0] == '-')
			switch(argv[j][1]){ 
				case 'd':
				case 'D':
					if(j+1 >= argc){
						fprintf(stderr,"%s\n",USAGE);
						exit(1);
					}
					strcpy(dict_name,argv[j+1]);
					j++;
					continue;	
				case 'v':
				case 'V':
					printf("%s\n",VERSION);
					exit(0);
				case '?':
				case 'h':
				case 'H':
					printf("%s\n",HELP);
					exit(0);
				default:
					fprintf(stderr,"anagrams: unkown option %s\n",
						argv[j]);
					exit(1);
			}
		else {
			if(print_all == 0){
				fprintf(stderr, "%s\n", USAGE);
				return 1;
			}
			print_all = 0;
			strcpy(word_buf,argv[j]);
			printf("Anagrams for %s in %s:\n",word_buf, dict_name);
			qsort(word_buf,strlen(word_buf),sizeof(char),(int(*)(const void *, const void *))comp_char);
		}
			
	}

	if(j > argc){
		fprintf(stderr,"%s\n",USAGE);
		exit(1);
	}

	/* Try to open the dictionary */

	if(!(dict_file = fopen(dict_name,"r"))){
		fprintf(stderr,"anagrams: cannot open %s\n",dict_name);
		return 1;
	}

	/* Make a first pass through dictionary to see how big it is */

	while(fgets(line_buf,MAX_LINE,dict_file))dict_size++;
	rewind(dict_file);

	/* allocate memory to hold dictionary entry pointers */

	dict = (struct DICT_ENTRY **)malloc(dict_size*sizeof(struct DICT_ENTRY *));
	if(!dict){
		fprintf(stderr,"anagrams: cannot allocate dictionary memory\n");
		return 1;
	} 

	/* Load in dictionary */

	i=0;
	while(fgets(line_buf,MAX_LINE,dict_file)){

		/* kill off new line at end of line_buf */
		line_buf[strlen(line_buf)-1]='\0';

		dict[i] = (struct DICT_ENTRY *)malloc(sizeof(struct DICT_ENTRY));
		if(!dict[i]){
			fprintf(stderr,"anagrams: cannot allocate entry memory\n");
			return 1;
		}
		dict[i]->tag = (char *)malloc(k=strlen(line_buf)+1);
		if(!dict[i]->tag){
			fprintf(stderr,"anagrams: cannot allocate tag memory\n");
			return 1;
		}
		dict[i]->word = (char *)malloc(k);
		if(!dict[i]->word){
			fprintf(stderr,"anagrams: cannot allocate word memory\n");
			return 1;
		}
		strcpy(dict[i]->word,line_buf);
		strcpy(dict[i]->tag,line_buf);
		qsort(dict[i]->tag,k-1,sizeof(char),(int(*)(const void *, const void *))comp_char);
		i++;
	}

	fclose(dict_file);

	/* Sort the dictionary array */

	krqsort((void **)dict,0,dict_size-1,(int(*)(void *, void *))comp_entries);

	i = 0;
	if(print_all) /* print all anagrams */
		while(i<dict_size-1){
			/* See if the next entry has the same tag, i.e., 
                           there is at least one anagram. Otherwise skip */
			strcpy(word_buf,dict[i]->tag);
			if(strcmp(word_buf,dict[i+1]->tag)==0){
				printf("\n\nAnagrams of %s:\n", dict[i]->word);
				while(strcmp(word_buf,dict[++i]->tag)==0)
					printf("%s ",dict[i]->word);
			}
			else i++;
		}
	else   /* only anagrams of word stored from command line */
		while(i<dict_size){
			j = strcmp(word_buf,dict[i]->tag);
			if(j == 0)
				printf("%s ",dict[i]->word);
			if(j < 0) break;
			i++;
		}
	
	printf("\n");
				
	/* clean up */
	for(i=0;i<dict_size;i++){
		free(dict[i]->word);
		free(dict[i]->tag);
		free(dict[i]);
	}
	free(dict);
	
	return 0;
}

/* This is the implementation of C.A.R. Hoare's quicksort algorthm
	taken from K&R. 

usage: if array[] is an array of some kind of thingies,
       and comp(thingie *a, thingie *b) returns -1, 0, or 1 according
       as a < b, a==b, or a > b in some ordering it imposes on
       thingies, then

       krqsort( array, int left, int right, &comp ) 

       sorts array in order, using initial choices left and right
       in the algorithm.

	Declare this guy in your program as

	void krqsort ( void *arrayname[], int, int,
	 int (*functionname)(void *, void *));

	see sortargs.c for an example using this function

*/

static void swap(void *v[],int i, int j)
{
	void *temp;

	temp = v[i];
	v[i] = v[j];
	v[j] = temp;
}


static void krqsort( void *v[],int left, int right, int (*comp)(void *,void *))
/*                                                ^
                                                  |
                                            pointer to a function of
                                            two generic pointers,
                                            returning int. */
{
	int i,last;
	void swap(void *v[], int, int );  /* private swap function */

	if( left >= right ) /* do nothing if array contains fewer
                               than 2 elements */	
	return;
	swap(v, left, (left + right)/2);
	last = left;
	for ( i= left +1;i <= right; i++)
		if((*comp)(v[i],v[left]) < 0)
			swap(v,++last,i);
	swap(v,left,last);
	krqsort(v, left, last -1,comp);
	krqsort(v,last+1,right,comp);
}

