/*
 * state_machine.c
 *
 *  Created on: 24 Aug 2021
 *      Author: nigil
 *
 */

//Declare a global variable for event buffer
//Extern or Get and Set array funcs to access the array

//To initialise this buffer, we can use memset()

#include "statemachine.h"
#include "main.h"


typedef enum {STATE_TRANSITION, STATE_HANDLED, STATE_IGNORED,  INIT_STATUS}status;

static status ON_State_PC13(StateMachine *me, Event *e)
{
	me->state=OFF;
	return STATE_TRANSITION;
}
static status ON_State_Entry(StateMachine *me, Event *e)
{
	/*
	 Can add any entry behaviour here
	 */
	HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
	HAL_Delay(250);
	return STATE_HANDLED ;

}
static status ON_State_Continue(StateMachine *me, Event *e)
{
	HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
	HAL_Delay(250);
	return STATE_HANDLED ; //No change is state, hence State_Handled

}
static status ON_State_Exit(StateMachine *me, Event *e)
{

	HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
	HAL_Delay(250);
	me->state=OFF;
	return STATE_TRANSITION ;

}
static status OFF_State_PA0(StateMachine *me, Event *e)
{
	me->state=OFF;
	return STATE_TRANSITION;
}
static status OFF_State_Entry(StateMachine *me, Event *e)
{
	/*
		 Can add any entry behaviour here
	 */

	HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
	HAL_Delay(1000);
	return STATE_HANDLED;

}
static status OFF_State_Continue(StateMachine *me, Event *e)
{

	HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
	HAL_Delay(1000);

	return STATE_HANDLED;

}
static status OFF_State_Exit(StateMachine *me, Event *e)
{
	HAL_GPIO_TogglePin(LED_GPIO_Port,LED_Pin);
	HAL_Delay(1000);
	me->state=ON;
	return STATE_TRANSITION;

}

static status DEFAULT_State_PC13(StateMachine *me, Event *e)
{
	me->state=OFF;
	return STATE_TRANSITION;
}


static status DEFAULT_State(StateMachine *me,Event *e)
{
	  me->state=DEFAULT;
	  HAL_GPIO_WritePin(LED_GPIO_Port,LED_Pin,1);
	  return INIT_STATUS;
}

static status Ignore_Event(StateMachine *me,Event *e)
{
	return STATE_IGNORED;
}



typedef status (*StateMachineAction)(StateMachine *me, Event *e);

/* > Add an event called timeout, it is used very freq
 * > GETICK for HAL STM32 for return current tick.
 * > Have some interface functions
 * > The HAL func changes frm uC to uC, we need to adapt  for it.
 * >
 * */
StateMachineAction const Statemachine_table[MAXSTATES][MAXEVENTS] ={
	               /* No Event*/ 	  		/*STARTUP*/ 	 					 /*Entry*/								/*Exit*/		  			     /*PA0*/         			/* PC13*/
/* On State */ 	 {&ON_State_Continue, 	 &Ignore_Event,      					 &ON_State_Entry,                     ON_State_Exit,					 &Ignore_Event,    			&ON_State_PC13  },
/*Off state */	 {&OFF_State_Continue,	 &Ignore_Event,      					 &OFF_State_Entry/*offstatePA0*/,     &OFF_State_Exit,                   &OFF_State_PA0,    		&Ignore_Event  },
/*Default state*/{&Ignore_Event, 		 &DEFAULT_State/*Default start up*/,     &Ignore_Event,    				      &Ignore_Event,					 &Ignore_Event,				&DEFAULT_State_PC13   },
};

/*pop the event
	 * then execute below
	 * Before calling pop() we can check the buffer length
	 * Or if the event received is no event, don't call state machine table
*/
/*
 * If a state requires you to evaluate a guard condition and then move to another
 * state depending on the guard evaluation, insert the guard in the state in question
 * and avoid having an exit action (or an exit function) this would simplify the code and
 * logic further
 */

void StateMachine_Dispatch(StateMachine * me, Event *e)
{
	status stat;
	state_typedef previous_state= me->state;
	e->event=Pop_Event_From_Buffer(me);
	stat= (*Statemachine_table[me->state][e->event])(me,e);

	 if (stat==STATE_TRANSITION)
	 {
		 (*Statemachine_table[previous_state][EXIT])(me,(Event *)0);
		 (*Statemachine_table[me->state][ENTRY])(me,(Event *)0);
	 }
	 else  if (stat == INIT_STATUS){
		 (*Statemachine_table[me->state][e->event])(me,e);
	 }
	return;
}




void initstatemachine(StateMachine *me,int size_of_buffer)
{
	//event_typedef temp[size_of_buffer];
	me->state=2;
	me->size_of_buffer = size_of_buffer;
	//me->event_buffer=temp;
	me->event_buffer= (event_typedef*)malloc(size_of_buffer*sizeof(event_typedef));

	//memset(me->event_buffer,NOEVENT,size_of_buffer*sizeof(event_typedef));   //seg fault  or memory is not allocated

	me->event_buffer[0] = STARTUP;  //Load the first event as start up
	me->current_buffer_length = 1;
	me->read_index = 0;
	me->write_index = 1;

	me->ptr_Pop_Event_From_Buffer=Pop_Event_From_Buffer;
	me->ptr_Add_Event_To_Buffer=Add_Event_To_Buffer;

	//free(temp);
	return;
}

void initevent(Event *me)
{
	me->event=STARTUP;
	return;
}

/*
 * 	It's better to use static allocation instead of dy. allc
 * 	memset func to set an array, memcopy to copy the memory
 *
 * 	*/
//void init_buffer(StateMachine *me,int size_of_buffer){
//	me->size_of_buffer = size_of_buffer;
//	memset(me->event_buffer,0,size_of_buffer*sizeof(event_typedef));
//	me->event_buffer[0] = STARTUP;  //Load the first event as start up
//	me->current_buffer_length = 1;
//	me->read_index = 0;
//	me->write_index = 1;
//
//	me->ptr_Pop_Event_From_Buffer=Pop_Event_From_Buffer;
//	me->ptr_Add_Event_To_Buffer=Add_Event_To_Buffer;
//	return;
//
//}


void Add_Event_To_Buffer(StateMachine *me, event_typedef event)
{
	if(me->current_buffer_length == me->size_of_buffer){


	}
	else{
		me->event_buffer[me->write_index] = event; //Adding event that occurred  to buffer
		me->current_buffer_length++;
		me->write_index++;

	}
	return ;
}




event_typedef Pop_Event_From_Buffer(StateMachine *me)
{

	if( me->current_buffer_length == 0){
		return NOEVENT;
		//If there are no events to process, we return the NOEVENT event_typedef, the no_event is considerent an event condition that does what is needs to do.
	}
	else{

		event_typedef e;
		e = me->event_buffer[me->read_index];
		me->current_buffer_length--;
		me->read_index++;
		if(me->read_index == me->size_of_buffer){
			me->read_index = 0;
			me->write_index=0;
		}
	return e ; //Return the event
	}

}



