/*  fsquad.c: simulate a solution to the firing squad synchronization
 *  problem.
 *
 *  This is an old problem that appears, e.g., in Marvin Minsky, Computation:
 *  Finite and Infinite Machines, Prentice Hall, Englewood Cliffs, 1967, as
 *  problem 2.7-5 on page 28. Minsky attributes the problem to J. Myhill
 *  in 1957.
 *
 *  There are N machines arranged in a row. The machines are finite state
 *  machines. All except the two on the ends have identical state 
 *  transition diagrams. One of the two end machines is designated as
 *  "the general," following a metaphor in which we are to think of the other
 *  machines as soldiers in a firing squad. Alone among the machines, the
 *  general has volition. At some discrete time unit, the general enters
 *  a special state we may think of as "fire when ready." Thereafter his
 *  behaviour is deterministic. The problem is to design the state transitions
 *  of all the machines including the general and the machine at the other
 *  end so that all machines at some point will enter a designated state
 *  ("fire") simultaneously. The difficulty of the problem stems from the
 *  requirements that the design must work for all values of N and the
 *  transitions of each machine must depend only on its nearest neighbors.
 *  In particular, the number of states in the design of the machines must
 *  be independent of N. 
 *
 *  The purpose of this program is to implement one particular solution of
 *  this problem. It is neither the fastest, nor does it use the minimum
 *  number of states. (See Minsky op. cit. for a discussion of the best
 *  possible solutions.)
 *
 *  Here is a way to design machines:
 *
 *  You fire when both you and your neighbors are "red." The idea is to have
 *  redness spread from the middle outward. So a machine must first determine
 *  whether it is the middle machine. The parity of N, of course, affects
 *  whether there is a single middle machine or two. In the rest of the
 *  discussion it is simpler to consider the case of odd N. (We do handle
 *  the even case in the code at the expense of some complications which
 *  are not particularly illuminating.) 
 *
 *  We determine the middle by an exchange of messages. A machine that 
 *  broadcasts a message asking "am I in the middle?" is in the middle if
 *  it receives an acknowledgement from the end machines simultaneously. 
 *
 *  The messaging metaphor can easily be implemented with states, e.g., the
 *  state of wanting to send a message in a given direction. Since each
 *  machine looks at its neighbors, messages can "move" by means of state
 *  transitions in adjacent machines.
 *
 *  We begin with the left general, marked active, sending a message down the
 *  line prompting machines to test themselves for "middlehood." It only gets
 *  as far as the first active soldier, who does not forward the message. He 
 *  proceeds to broadcast a MID_TEST message. This is always forwarded by
 *  soldiers, active or passive, and reaches the active and passive generals,
 *  who respond with a MID_ACK message. As explained, the testing machine
 *  determines whether it is in the middle by seeing if the ACKs arrive from
 *  both sides at once. In either case, it eats the MID_ACK messages, but
 *  sends a message back to the general to prod him into starting the next
 *  testing cycle if he turns out not to be the middle machine. In that case
 *  he becomes passive for subsequent test rounds. In the passive state he 
 *  no longer responds to MID_TEST messages, but rather forwards them. 
 *
 *  If the ACKs are simultaneous, the testing machine becomes a red active
 *  general, and serves as such for two squads, one to his left and one to
 *  his right. He begins by broadcasting a RESET message, followed by a first
 *  MID_TEST. Thus the cycle will repeat, with the middle machine(s) in both
 *  halves becoming a red active general, ... 
 *
 *  The red general is only active for one such cycle, after which he becomes
 *  passive (but still red.) This is done upon receipt of a RESET message,
 *  which a general does not forward. The RESET turns all intervening soldiers
 *  back to active for the next testing cycle. 
 *
 *  The design of the machines is implemented in the update_state routine 
 *  below. Each machine is represented by a structure called a struct
 *  machine. The main data structure is a one dimensional array of such
 *  structs. The main loop of the program (near the end of the main routine
 *  runs through the array and changes the state (i.e., various fields) of
 *  each machine struct according to the contents of its neighbors. A brief
 *  summary of the state of each machine is printed to stdout.
 */

#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#define VERSION "1.0"
#define DEFAULT_LENGTH 8
#define MAX_LENGTH 1024

#define USAGE "fsquad [-hlv -t <n> -n <n> -d <n> ]"

#ifdef _SHORT_STRINGS
#define HELP USAGE
#else
#define HELP "\n"USAGE"\n\
-h: print this helpful message\n\
-v: print version number and exit\n\
-l: print legend showing meaning of the machine state summaries printed.\n\
-t: run the simulation for at most n time steps.\n\
-n: Simulate a firing squad of length n (Default = 9)\n\
-d: Delay d seconds between cycles. (Default is to run at full speed.)\n\
\nSimulate solution of firing squad synchronization problem.\n\n"
#endif

#define TRUE 1
#define FALSE 0

/* Color states */
#define RED 0
#define BLACK 1

/* Activity states */
#define PASSIVE 0
#define ACTIVE 1

/* Machine types */
#define GENERAL 0
#define SOLDIER 1

/* Message types */
#define NO_MSG 0
#define TEST_MSG 1
#define MID_TEST_MSG 2
#define MID_ACK_MSG 3
#define RESET_MSG 4
#define PROMOTE_MSG 5

/* Message directions */
#define LEFT 0
#define RIGHT 1
#define BROADCAST 2

struct machine {

	int activity;
	int timer;
	int color;
	int type;
        int message_direction;
	int message;	
	int testing;

}*machines[MAX_LENGTH],*machines_old[MAX_LENGTH];

int N = DEFAULT_LENGTH; 
void print_state(void);
void update_state(int j);
void legend(void);

int
main(int argc, char **argv)
{
	int i=1,j=1,t=1;
	int fire,delay = 0;
	int maxsteps = -1;

	/* Process command line options */

	while((i< argc) && (argv[i][0] == '-')){
		  if(strcmp(argv[i],"-h")==0){
			printf("%s\n", HELP);
			exit(0);
		  }
		  if(strcmp(argv[i],"-v")==0){
			printf("%s\n",VERSION);
			exit(0);
		  }
		  if(strcmp(argv[i],"-n")==0){
			N = atoi(argv[i+1]);
			i += 2;
			continue;
		  }
		  if(strcmp(argv[i],"-l")==0){
			  legend();
			  i += 1;
			  continue;
	          }
		  if(strcmp(argv[i],"-t")==0){
			  maxsteps = atoi(argv[i+1]);
			  i += 2;
			  continue;
	          }
		  if(strcmp(argv[i],"-d")==0){
			delay = atoi(argv[i+1]);
			i += 2;
			continue;
		  }
		  fprintf(stderr, "fsquad: Unknown option %s\n", argv[i]);
		  fprintf(stderr, "%s\n",USAGE);
		  return 1;
	}

	/* Sanity checks, and instantiate machines */

	if(N <= 0 || N > MAX_LENGTH){
		fprintf(stderr,"fsquad: Requested length not in supported range 1-%d\n",MAX_LENGTH);
		return 1;
	}

	for(i=0;i<N;i++){

		machines[i]=(struct machine *)malloc(sizeof(struct machine));
		machines_old[i]=(struct machine *)malloc(sizeof(struct machine));
		if(!(machines[i]&&machines_old[i])){
				fprintf(stderr,"fsquad: memory alloc failed.\n");
				return 1;
		}
	}

	/* Set up initial states of machines */

	machines[0]->type =  GENERAL;
	machines[0]->activity =  ACTIVE;
	machines[0]->color = RED;
	machines[0]->message_direction = RIGHT;
	machines[0]->timer = 0;
	machines[0]->message = NO_MSG;
	machines[0]->testing = TRUE;
	for(i=1;i<N-1;i++){

		machines[i]->type =  SOLDIER;
		machines[i]->activity =  ACTIVE;
		machines[i]->color = BLACK;
	        machines[i]->timer = 0;
		machines[i]->message_direction = BROADCAST;
		machines[i]->message = NO_MSG;
		machines[i]->testing = FALSE;
	}
	machines[N-1]->type =  GENERAL;
	machines[N-1]->activity =  PASSIVE;
	machines[N-1]->color = RED;
	machines[N-1]->timer = 0;
	machines[N-1]->message_direction = LEFT;
	machines[N-1]->message = NO_MSG;
	machines[N-1]->testing = FALSE;

	/* main loop: do one time step of the simulation */

	print_state();
	t = 1;
	while(TRUE){

		/* store current state in machines_old array */

		if(maxsteps == 0)break;

		for(j=0;j<N;j++)
			*(machines_old[j]) = *(machines[j]);

		/* Now go through and have each machine update its state
		 * based upon its own state and that of its neighbors. */

		for(j=0;j<N;j++)update_state(j);

		print_state();

		/* Test for the firing condition */

		fire = TRUE;
		for(j=0;j<N;j++)
			if(!machines[j]->color == RED){
				fire = FALSE;
				break;
			}
		if(fire == TRUE){
			printf("\n\n BANG!!! \n\n");
			break;
		}
		t++;
		sleep(delay);
		if(maxsteps > 0)maxsteps--;
	}
	printf("Length = %d. Synchronization in %d steps.\n\n",N,t);
	return 0;

}

void print_machine(int i){

	int f;

	printf("|");
	f=machines[i]->type;
	switch(f){
		case GENERAL:
			if(machines[i]->activity==ACTIVE)
				printf("G");
			else printf("g");
			break;
		case SOLDIER:
			if(machines[i]->activity==ACTIVE)
				printf("S");
			else printf("s");
			break;
		default:
			printf("?");
	}
	f=machines[i]->color;
	switch(f){
		case RED:
			printf("R");
			break;
		case BLACK:
			break;
		default:
			printf("?");
	}
	if(machines[i]->testing)printf("t[%d]",machines[i]->timer);
	f=machines[i]->message_direction;
	switch(f){
		case LEFT: printf("<-");
			break;
		case RIGHT:
			printf("->");
			break;
		case BROADCAST:
			printf("<->");
			break;
		default:
			printf("?");
	}
	f=machines[i]->message;
	switch(f){
		case  NO_MSG:
			break;
		case  TEST_MSG:
			printf("!");
			break;
		case MID_TEST_MSG:
			printf("m?");
			break;
		case MID_ACK_MSG:
			printf("m!");
			break;
		case RESET_MSG:
			printf("0!");
			break; 
		case PROMOTE_MSG:
			printf("^");
			break;
		default: printf("??");
	}
	printf("| ");
}

void print_state(void){
	int i;

	printf("\n");
	for(i=0;i<N;i++)print_machine(i);
	printf("\n\n");
}

/* Update the state of machine number j based upon its own state and
 * those of its nearest neighbors */

void update_state(int j){ 

	int ld, lm=NO_MSG, rd, rm=NO_MSG; /* left/right direction, message */
	int act = PASSIVE,msg = NO_MSG, dir;

	if(machines_old[j]->type == GENERAL) {
			
			/* If we're active, we stay active by default */

			if(machines_old[j]->activity == ACTIVE)

				act = ACTIVE;

			/* See if there is a message */

			lm = NO_MSG;
			if(j)if((machines_old[j-1]->message_direction == RIGHT)||
					(machines_old[j-1]->message_direction == BROADCAST))
						lm = machines_old[j-1]->message;
			rm = NO_MSG;
			if(j<N-1)if((machines_old[j+1]->message_direction == LEFT)||
					(machines_old[j+1]->message_direction == BROADCAST))
						rm = machines_old[j+1]->message;

			/* If the message is RESET then we change to passive */

			if((rm == RESET_MSG)||(lm == RESET_MSG)){
				machines[j]->activity = PASSIVE;
				return;
		        }

			if((rm == MID_TEST_MSG)&&(lm == MID_TEST_MSG)){
				msg = MID_ACK_MSG;
				dir = BROADCAST;
			}
			else
			if(rm == MID_TEST_MSG){
				msg = MID_ACK_MSG;
				dir = RIGHT;
			}
			else if(lm == MID_TEST_MSG){
				msg = MID_ACK_MSG;
				dir = LEFT;
			}
			else
			if(machines_old[j]->activity == ACTIVE){

				if(machines_old[j]->testing == TRUE){
					msg = TEST_MSG;
					machines[j]->testing = FALSE;
					dir = BROADCAST;
					act = ACTIVE;
			        }
				else
				if(((lm == TEST_MSG)&&(machines[j-1]->type ==
							SOLDIER))||
						((rm == TEST_MSG) &&( machines[j+1]->type == SOLDIER)))
					machines[j]->testing = TRUE;
			}
				
	} /* End General */

	else {  /* SOLDIER */

		/* Record any messages from neighbors */

			lm = NO_MSG;
			ld = machines_old[j-1]->message_direction;
			if((ld == RIGHT)||(ld == BROADCAST))
				lm = machines_old[j-1]->message;
			rm = NO_MSG;
			rd = machines_old[j+1]->message_direction;
			if((rd == LEFT)||(rd == BROADCAST))
				rm = machines_old[j+1]->message;

			/* Handle promotion oddity here: this is one
			 * the headaches caused by parity considerations */

			if(lm == PROMOTE_MSG){
				     /* Become a left end Red Active 
				      * General */ 
				     machines[j]->type = GENERAL;
				     machines[j]->color = RED;
				     machines[j]->activity = ACTIVE;
				     machines[j]->testing = TRUE;
				     machines[j]->message = RESET_MSG;
				     machines[j]->message_direction = RIGHT;
				     return;
			 }
			 if(rm == PROMOTE_MSG){
				     /* Become a right end Red Active
				      * General */
				     machines[j]->type = GENERAL;
				     machines[j]->color = RED;
				     machines[j]->activity = ACTIVE;
				     machines[j]->testing = TRUE;
				     machines[j]->message = RESET_MSG;
				     machines[j]->message_direction = LEFT;
				     return;
		         }
			 if(machines_old[j]->message == PROMOTE_MSG){

				     /* We promoted our neighbor, so we must
				      * also become a red active general. We
				      * tell the direction from the dir the
				      * promote msg was sent. */

				     machines[j]->type = GENERAL;
				     machines[j]->color = RED;
				     machines[j]->activity = ACTIVE;
				     machines[j]->testing = TRUE;
				     machines[j]->message = RESET_MSG;
				     machines[j]->timer = 0;
				     if(machines_old[j]->message_direction ==
						     RIGHT)
				     	machines[j]->message_direction = LEFT;
				     else machines[j]->message_direction = RIGHT;
				     return; 
			     }

			/* If there is no message there is nothing to do */

			if( (rm == NO_MSG)&&(lm == NO_MSG)) {
				machines[j]->message = NO_MSG;
				if(machines_old[j]->timer)
					machines[j]->timer--;
				return;
			}

			/* Both active and passive cases handle reset the same */
			if((lm == RESET_MSG)||(rm == RESET_MSG)){

				act = ACTIVE;
				msg = RESET_MSG;
					/* we assume here a reset can come 
					 * from only one side and soldiers
					 * don't broadcast them */
				if(lm == RESET_MSG)dir=RIGHT;
				else dir = LEFT;
						
			}
			else 

			/* Now break into active/passive cases */

			if(machines_old[j]->activity == ACTIVE){

			   act = ACTIVE; /* remain so by default */

		           if((rm == TEST_MSG)||(lm==TEST_MSG)){
				      dir = BROADCAST;
				      msg = MID_TEST_MSG;
				      machines[j]->testing = TRUE;
				      act = ACTIVE;
			   } 
			   else
			   if(rm == MID_ACK_MSG){
				      if(lm == MID_ACK_MSG){
					      /* Middle of odd array */
					      machines[j]->type = GENERAL;
					      machines[j]->color = RED;
					      machines[j]->testing = TRUE;
					      msg = RESET_MSG;
					      dir = BROADCAST;
				      }
				      else if(!machines_old[j]->testing){
					      /* forward */
					      dir = LEFT;
					      msg = MID_ACK_MSG;
			              }
				      else { /* 1st ACK */
					        act = PASSIVE;
						msg = NO_MSG;
					        machines[j]->timer = 3;
					   }
			     }
			     else
			     if(lm == MID_ACK_MSG){
				      if(rm == MID_ACK_MSG){

					      machines[j]->type = GENERAL;
					      machines[j]->color = RED;
					      machines[j]->testing = TRUE;
					      msg = RESET_MSG;
					      dir = BROADCAST;
				      }
			      	      else if(!machines_old[j]->testing){
					      /* forward */
					      dir = RIGHT;
					      msg = MID_ACK_MSG;
				      }
				      else { /* 1st ACK */
					      act = PASSIVE;
					      msg = NO_MSG;
					      machines[j]->timer = 3;
				      }
		             }
			     else /* all other messages we forward */
		             if(lm != NO_MSG){
				     dir = RIGHT;
				     msg = lm;
			     }
			     else {
				     dir = LEFT;
				     msg = rm;
		             }

			} /* End Active Soldier */

			else { /* Passive soldier with some message other 
				   than a reset.  */

			     if((machines_old[j]->testing == TRUE) && 
					     (rm == MID_ACK_MSG) ) {
				        if(machines_old[j]->timer >= 1){

						/* left machine of middle
						 * pair: we must promote
						 * our partner to the right.
						 * Then we both serve as
						 * generals for our respective
						 * squads. */

						msg = PROMOTE_MSG;
						dir = RIGHT;
						machines[j]->testing = FALSE;
						machines[j]->timer = 0;
				        }
					else {
				     /* send test back to general to prod
				      * him into next test cycle */
				     	   msg = TEST_MSG;
					   dir = LEFT;
					   machines[j]->testing = FALSE;		
					}
			     }
			     else
			     if((machines_old[j]->testing == TRUE) &&
						(lm == MID_ACK_MSG)){

				        if(machines_old[j]->timer >= 1){

						/* right machine of middle
						 * pair */

						msg = PROMOTE_MSG;
						dir = LEFT;
						machines[j]->testing = FALSE;
						machines[j]->timer = 0;

					}
                                        else {
			     		   msg = TEST_MSG;
			   		   dir = RIGHT;
					   machines[j]->testing = FALSE;		
					}
			     }
			     else
			     if(lm != NO_MSG){
				  dir = RIGHT;
				  msg = lm;
			     }
			     else {
				  dir = LEFT;
				  msg = rm;
			     }
		             if(machines_old[j]->timer)
				     machines[j]->timer--;
			} /* End passive soldier */
				
	} /* End Soldier */

	/* Make the changes: color and type changes done above */

	machines[j]->activity = act;
	machines[j]->message_direction = dir;
	machines[j]->message = msg;
}


void legend(void){

	printf("\n\nMachine State Legend\n\n");
	printf("|Type [Color] [Testing state[timer]] Msg Direction [Msg]|\n\n");
	printf("Type G[g] or S[s] = General/Soldier[inactive].\n");
	printf("Color R = Red, Black if absent.\n");
	printf("Testing state = t if present. Timer value 0-3.\n");
	printf("Directions: -> right, <- left, <-> broadcast.\n");
	printf("Possible Messages:\n\n");
	printf("\t0!: RESET\n");
	printf("\t! : TEST\n");
	printf("\tm?: MIDDLE QUERY\n");
	printf("\tm!: MIDDLE ACK\n");
	printf("\t^ : PROMOTE\n");
	printf("\t??: UNKNOWN (should not happen.)\n\n");

}
