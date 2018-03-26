/* Easter.c: compute the date of easter in a given year.

	By Terry R. McConnell, with a lot of help from Claus Tondering.

	compile: cc -o easter easter.c

Exported functions: Include these declarations in your source file.

extern int daynumber(int mm,int dd, int yyyy);
extern int weekday(int mm,int dd, int yyyy);
struct MyDate {
	int mm;
	int dd;
}
extern struct MyDate *easter(int yyyy);

*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/* Undefine if you only want to compile exported functions */
#define MAIN

#define VERSION "1.0"
#define USAGE "easter [-jfnvh] [--] yyyy\n"
#define HELP "Print the date of Easter in year yyyy in mm/dd format.\n\
All options must preceed yyyy. Options cannot be combined as in -jn. Must\n\
use -j -n, etc. \n\
Supported options are the following:\n\
-j: Use Julian calendar rules instead of Gregorian calendar rules.\n\
-n: Print the day number of the year (Jan. 1 = 1) instead of mm/dd.\n\
-f: Give a more verbose report.\n\
--: Signal end of options so that yyyy can be negative (even though that's \n\
    rather silly!)\n\
-h: Print this informative message.\n\
-v: Print the version number and exit.\n"
#define PROGNAME "easter"

#define TRUE 1
#define FALSE 0

char *DayNames[] = {"Sunday","Monday","Tuesday","Wednesday",
	"Thursday","Friday","Saturday"};

/* Flags, with default values */
int JULIAN = FALSE;
int DAYNUMBER = FALSE;
int VERBOSE = FALSE;


/* The following background is taken from the Calendar FAQ maintained by
Claus Tondering. We intersperse code as appropriate. 

Copyright and disclaimer
------------------------
        This document is Copyright (C) 1997 by Claus Tondering.
        E-mail: c-t@pip.dknet.dk.
        The document may be freely distributed, provided this
        copyright notice is included and no money is charged for
        the document.

        This document is provided "as is". No warranties are made as
        to its correctness.

2.5. What day of the week was 2 August 1953?
--------------------------------------------

To calculate the day on which a particular date falls, the following
algorithm may be used (the divisions are integer divisions, in which
remainders are discarded):

a = (14 - month) / 12
y = year - a
m = month + 12*a - 2
For Julian calendar: d = (5 + day + y + y/4 + (31*m)/12) % 7
For Gregorian calendar: d = (day + y + y/4 - y/100 + y/400 + (31*m)/12) % 7

The value of d is 0 for a Sunday, 1 for a Monday, 2 for a Tuesday, etc.

*/

/* Return a number for the day of the week on which the passed date falls,
with Sun = 0 */

int
weekday(int mm, int dd, int yyyy){

	int a,y,m;

	a = (14 - mm)/12;
	y = yyyy - a;
	m = mm + 12*a - 2;

	if(JULIAN)
		return (5 + dd + y + y/4 + (31*m)/12) % 7;
	else
		return (dd + y + y/4 - y/100 + y/400 + (31*m)/12) % 7;
}

/* Return the day number of the year starting from Jan 1 = 1. Only March
and April are really needed here, but the rest are included for 
completness */

int
daynumber(int mm, int dd, int yyyy)
{

	int rval = 0;

	/* Add in prior full months: adjust for leap year later. */
	switch(mm){
		case 1: /* Jan (31 days) */
			rval = dd;
			break;
		case 2: /* Feb (28 days) */			
			rval = 31+dd;
			break;
		case 3: /* Mar (31 days) */			
			rval = 31+28+dd;
			break;
		case 4: /* Apr (30 days) */			
			rval = 31+28+31+dd;
			break;
		case 5: /* May (31 days) */			
			rval = 31+28+31+30+dd;
			break;
		case 6: /* June (30 days) */			
			rval = 31+28+31+30+31+dd;
			break;
		case 7: /* Jul (31 days) */			
			rval = 31+28+31+30+31+30+dd;
			break;
		case 8: /* Aug (31 days) */			
			rval = 31+28+31+30+31+30+31+dd;
			break;
		case 9: /* Sep (30 days) */			
			rval = 31+28+31+30+31+30+31+31+dd;
			break;
		case 10: /* Oct (31 days) */			
			rval = 31+28+31+30+31+30+31+31+30+dd;
			break;
		case 11: /* Nov (30 days) */			
			rval = 31+28+31+30+31+30+31+31+30+31+dd;
			break;
		case 12: /* Dec (31 days) */			
			rval = 31+28+31+30+31+30+31+31+30+31+30+dd;
			break;
		default:
			fprintf(stderr,"daynumber: no such month %d\n",mm);
			exit(1);
	}

	/* Correct for leap year */

	if(JULIAN){
		if(yyyy%4 == 0)
			rval++;
	}
	else
		if((yyyy%400==0)||((yyyy%4==0)&&!(yyyy%100==0)))
			rval++;
	return rval;
}

/*

2.9.2. When is Easter? (Long answer)
------------------------------------

The calculation of Easter is complicated because it is linked to (an
inaccurate version of) the Hebrew calendar.

Jesus was crucified immediately before the Jewish Passover, which is a
celebration of the Exodus from Egypt under Moses. Celebration of
Passover started on the 14th or 15th day of the (spring) month of
Nisan. Jewish months start when the moon is new, therefore the 14th or
15th day of the month must be immediately after a full moon.

It was therefore decided to make Easter Sunday the first Sunday after
the first full moon after vernal equinox. Or more precisely: Easter
Sunday is the first Sunday after the *official* full moon on or after
the *official* vernal equinox.

The official vernal equinox is always 21 March.

The official full moon may differ from the *real* full moon by one or
two days.

(Note, however, that historically, some countries have used the *real*
(astronomical) full moon instead of the official one when calculating
Easter. This was the case, for example, of the German Protestant states,
which used the astronomical full moon in the years 1700-1776. A
similar practice was used Sweden in the years 1740-1844 and in Denmark
in the 1700s.)

The full moon that precedes Easter is called the Paschal full
moon. Two concepts play an important role when calculating the Pascal
full moon: The Golden Number and the Epact. They are described in the
following sections.

The following sections give details about how to calculate the date
for Easter. Note, however, that while the Julian calendar was in use,
it was customary to use tables rather than calculations to determine
Easter. The following sections do mention how to calcuate Easter under
the Julian calendar, but the reader should be aware that this is an
attempt to express in formulas what was originally expressed in
tables. The formulas can be taken as a good indication of when Easter
was celebrated in the Western Church from approximately the 6th
century.


2.9.3. What is the Golden Number?
---------------------------------

Each year is associated with a Golden Number.

Considering that the relationship between the moon's phases and the
days of the year repeats itself every 19 years (as described in
section 1), it is natural to associate a number between 1 and 19
with each year. This number is the so-called Golden Number. It is
calculated thus:
        GoldenNumber = (year%19)+1

New moon will fall on (approximately) the same date in two years
with the same Golden Number.

*/

#define GOLDENNUMBER(x) (((x)%19)+1)

/*

2.9.4. What is the Epact?
-------------------------

Each year is associated with an Epact.

The Epact is a measure of the age of the moon (i.e. the number of days
that have passed since an "official" new moon) on a particular date.

In the Julian calendar, 8 + the Epact is the age of the moon at the
start of the year.  
In the Gregorian calendar, the Epact is the age of the moon at the
start of the year.

The Epact is linked to the Golden Number in the following manner:

Under the Julian calendar, 19 years were assumed to be exactly an
integral number of synodic months, and the following relationship
exists between the Golden Number and the Epact:

        Epact = (11 * (GoldenNumber-1)) % 30 

If this formula yields zero, the Epact is by convention frequently
designated by the symbol * and its value is said to be 30. Weird?
Maybe, but people didn't like the number zero in the old days.

Since there are only 19 possible golden numbers, the Epact can have
only 19 different values: 1, 3, 4, 6, 7, 9, 11, 12, 14, 15, 17, 18, 20,
22, 23, 25, 26, 28, and 30.


The Julian system for calculating full moons was inaccurate, and under
the Gregorian calendar, some modifications are made to the simple
relationship between the Golden Number and the Epact.

In the Gregorian calendar the Epact should be calculated thus (the
divisions are integer divisions, in which remainders are discarded):
        
1) Use the Julian formula:
      Epact = (11 * (GoldenNumber-1)) % 30

2) Adjust the Epact, taking into account the fact that 3 out of 4
   centuries have one leap year less than a Julian century:
        Epact = Epact - (3*century)/4

   (For the purpose of this calculation century=20 is used for the
   years 1900 through 1999, and similarly for other centuries,
   although this contradicts the rules in section 2.10.2.)

3) Adjust the Epact, taking into account the fact that 19 years is not
   exactly an integral number of synodic months:
        Epact = Epact + (8*century + 5)/25

   (This adds one to the epact 8 times every 2500 years.)

4) Add 8 to the Epact to make it the age of the moon on 1 January:
        Epact = Epact + 8

5) Add or subtract 30 until the Epact lies between 1 and 30.

In the Gregorian calendar, the Epact can have any value from 1 to 30.

*/

#define CENTURY(x) ((x)/100 + 1)

int
epact(int yyyy){

	int ept;

        ept = (11 * (GOLDENNUMBER(yyyy)-1)) % 30;
	if(!JULIAN) {
		ept -= (3*CENTURY(yyyy))/4;
                ept += (8*CENTURY(yyyy) + 5)/25;
		ept += 8;
	}
	while(TRUE)
		if(ept < 1)ept += 30;
		else if(ept > 30)ept -= 30;
		     else break;
	if(VERBOSE){
		printf("The Golden Number is %d\n",GOLDENNUMBER(yyyy));
		printf("The Epact is %d\n",ept);
	}
	return ept;
}

/*

2.9.5. How does one calculate Easter then?
------------------------------------------

To find Easter the following algorithm is used:

1) Calculate the Epact as described in the previous section.

2) For the Julian calendar: Add 8 to the Epact. (For the Gregorian
   calendar, this has already been done in step 5 of the calculation of
   the Epact). Subtract 30 if the sum exceeds 30.

3) Look up the Epact (as possibly modified in step 2) in this table to
   find the date for the Paschal full moon:

     Epact   Full moon     Epact   Full moon     Epact   Full moon
     -----------------     -----------------     -----------------
        1    12 April        11     2 April        21    23 March
        2    11 April        12     1 April        22    22 March
        3    10 April        13    31 March        23    21 March
        4     9 April        14    30 March        24    18 April
        5     8 April        15    29 March        25    18 or 17 April
        6     7 April        16    28 March        26    17 April
        7     6 April        17    27 March        27    16 April
        8     5 April        18    26 March        28    15 April
        9     4 April        19    25 March        29    14 April
       10     3 April        20    24 March        30    13 April

*/

struct MyDate {
	int mm;
	int dd;
} PaschalMoon[] = { 0,0, /* not used */
4,12,
4,11,
4,10,
4,9,
4,8,
4,7,
4,6,
4,5,
4,4,
4,3,
4,2,
4,1,
3,31,
3,30,
3,29,
3,28,
3,27,
3,26,
3,25,
3,24,
3,23,
3,22,
3,21,
4,18,
4,17,   /* special case */
4,17,
4,16,
4,15,
4,14,
4,13};

/*
4) Easter Sunday is the first Sunday following the above full moon
   date. If the full moon falls on a Sunday, Easter Sunday is the
   following Sunday.


An Epact of 25 requires special treatment, as it has two dates in the
above table. There are two equivalent methods for choosing the correct
full moon date:

A) Choose 18 April, unless the current century contains years with an
   epact of 24, in which case 17 April should be used.

B) If the Golden Number is > 11 choose 17 April, otherwise choose 18 April.

*/

/* Return pointer to filled in struct containing month and day of Easter in 
   year passed.  */

struct MyDate
*easter(int yyyy)
{
	struct MyDate *dptr;
	int ept,wd;

	ept = epact(yyyy);

	if(JULIAN){
		ept += 8;
		if(ept > 30) ept -= 30;
	}
	
	if((ept < 1)||(ept > 30)){ /* Should not happen! */
		fprintf(stderr,"Epact %d out of range\n",ept);
		exit(1);
	}

	if(ept == 25){  /* special case */
		if(GOLDENNUMBER(yyyy) <= 11)
			dptr = &(PaschalMoon[24]);
		else
			dptr = &(PaschalMoon[ept]);
	}
	else dptr = &(PaschalMoon[ept]);

	wd = weekday(dptr->mm,dptr->dd,yyyy);

	if(VERBOSE)
		printf("The Paschal Full Moon is on %s, %d/%d.\n",
			DayNames[wd],dptr->mm,dptr->dd);
	dptr->dd += (7-wd); /* Easter is the following Sunday */
	if(dptr->dd > 31){
		(dptr->mm)++;  /* Adjust if we crossed into April */
		dptr->dd -= 31;
	}
	return dptr;
}

/*

2.9.6. Isn't there a simpler way to calculate Easter?
-----------------------------------------------------

This is an attempt to boil down the information given in the previous
sections (the divisions are integer divisions, in which remainders are
discarded):

G = year % 19

For the Julian calendar:
    I = (19*G + 15) % 30
    J = (year + year/4 + I) % 7

For the Gregorian calendar:
    C = year/100
    H = (C - C/4 - (8*C+13)/25 + 19*G + 15) % 30
    I = H - (H/28)*(1 - (H/28)*(29/(H + 1))*((21 - G)/11))
    J = (year + year/4 + I + 2 - C + C/4) % 7

Thereafter, for both calendars:
L = I - J
EasterMonth = 3 + (L + 40)/44
EasterDay = L + 28 - 31*(EasterMonth/4)


This algorithm is based in part on the algorithm of Oudin (1940) as
quoted in "Explanatory Supplement to the Astronomical Almanac",
P. Kenneth Seidelmann, editor.

People who want to dig into the workings of this algorithm, may be
interested to know that
    G is the Golden Number-1
    H is 23-Epact (modulo 30)
    I is the number of days from 21 March to the Paschal full moon
    J is the weekday for the Paschal full moon (0=Sunday, 1=Monday,
      etc.)
    L is the number of days from 21 March to the Sunday on or before
      the Pascal full moon (a number between -6 and 28)
*/

#ifdef MAIN
int
main(int argc, char **argv){

	int i,yr;
	struct MyDate *ADate;

	/* Process command line */
	if(argc <= 1){
		fprintf(stderr,"Usage:%s\n",USAGE);
		return 1;
	}
	i=1;
	while((argv[i][0]=='-')&&(argv[i][1]!='-')){
		switch(argv[i][1]){
			case 'f':
			        VERBOSE=TRUE;	
				break;
			case 'j':
				JULIAN=TRUE;
				break;
			case 'n':
				DAYNUMBER=TRUE;
				break;
			case 'v':
				printf("%s\n",VERSION);
				return 0;
			case 'h':
				printf("\n%s\n",USAGE);
				printf("%s\n",HELP);
				return 0;
			default:
				fprintf(stderr,"Usage:%s\n",USAGE);
				return 1;
		}
		i++;
	}

	if(strcmp(argv[i],"--")==0)i++;
	if(i != argc-1){
		fprintf(stderr,"Usage:%s\n",USAGE);
		return 1;
	}

	yr = atoi(argv[i]);
	if(VERBOSE)
		printf("\nFor the year %d:\n", yr);
	ADate = easter(yr);
	if(VERBOSE)
		printf("Easter is on ");
	if(DAYNUMBER)
		printf("%d",daynumber(ADate->mm,ADate->dd,yr));
	else
		printf("%d/%d",ADate->mm,ADate->dd);
	if(VERBOSE)
		printf(".");
	printf("\n");
	return 0;
}
#endif

/* End of File */
