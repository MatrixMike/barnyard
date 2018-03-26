/*******************************************************/
/* leapyear.c: Ansi C source for leap year program     */
/* By Terry R. McConnell                               */
/*******************************************************/

/* Usage: leapyear year

        correctly prints: year is a leap year

		<or>
		       	year is not a leap year  */


/* The following background information is taken from the calendar FAQ 
   maintained by Claus Tondering.

The Julian calendar has 1 leap year every 4 years:

        Every year divisible by 4 is a leap year.

However, this rule was not followed in the first years after the
introduction of the Julian calendar in 45 BC. Due to a counting error,
every 3rd year was a leap year in the first years of this calendar's
existence. The leap years were:

        45 BC, 42 BC, 39 BC, 36 BC, 33 BC, 30 BC,
        27 BC, 24 BC, 21 BC, 18 BC, 15 BC, 12 BC, 9 BC,
        AD 8, AD 12, and every 4th year from then on.

There were no leap years between 9 BC and AD 8. This period without
leap years was decreed by emperor Augustus and earned him a place in
the calendar, as the 8th month was named after him.

It is a curious fact that although the method of reckoning years after
the (official) birthyear of Christ was not introduced until the 6th
century, by some stroke of luck the Julian leap years coincide with
years of our Lord that are divisible by 4.

The Gregorian calendar has 97 leap years every 400 years:

        Every year divisible by 4 is a leap year.
        However, every year divisible by 100 is not a leap year.
        However, every year divisible by 400 is a leap year after all.

*/

/* Use negative numbers for years B.C. The year must be in the range
 -sizeof(int)  < year <= sizeof(int) */

#include<stdio.h>
#include<stdlib.h>

#define TRUE 1
#define FALSE 0

int GLeapYearQ(int );  /* Gregorian rule. */
int JLeapYearQ(int );   /* Julian rule. They return 1 or 0 */

int
main(int argc, char **argv)
{
	int year;
	
	if(argc <= 1){
		fprintf(stderr,"Usage: leapyear year\n");
		return 1;
	}
	
	year = atoi(*++argv);

	if( year >= 1582 )    /* Use Gregorian Rule */
		if( GLeapYearQ(year) == TRUE ) {
			printf("%s is a leap year.\n",*argv);
			return 0;
		}
		else {
			printf("%s is not a leap year.\n",*argv);
			return 0;
		}

/*      Else use Julian Rule */

	if(JLeapYearQ(year) == TRUE) {   /* Here year < 1582 */
		printf("%s is a leap year.\n",*argv);
		return 0;
	}
	if( year == 0 ){
		printf("There is no year 0.\n");
		return 0;
	}
	printf("%s is not a leap year.\n",*argv);
	return 0;
}

/* Returns TRUE or FALSE according as y is a leap year or not in
    the Gregorian calendar -- valid on and after 1582. Bear in mind
    that the Gregorian calendar was not adopted everywhere at the same time. 
    So the folks in Turkey in 1800 might have considered that a leap year,
    even though this program says they are wrong. I suspect they will not
    mind :-)  */

int GLeapYearQ(int y)
{
	if( (y % 4) != 0 ) return FALSE;
	if( ( y % 100 ) != 0 ) return TRUE;
	if( ( y % 400) != 0 ) return FALSE;
	return TRUE;
}

/* Returns TRUE or FALSE according as y is a leap year or not in
	the Julian calendar -- valid before 1582. The Julian Calendar
	was implemented in the year -45 (45 BC) , which was a leap year. 
	zero. There are no leap years before -45 since the concept was not
        yet invented. */

int JLeapYearQ(int y)
{
	if( y >= 8 ) return y % 4 == 0 ? TRUE : FALSE;
	if( y <= -46) return FALSE;
	if( y > -9 ) return FALSE;
	return (-y) % 3 == 0 ? TRUE : FALSE;
}
	
		
