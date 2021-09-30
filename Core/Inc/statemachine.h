/*
 * statemachine.h
 *
 *  Created on: 24 Aug 2021
 *      Author: revun
 */

#ifndef INC_STATEMACHINE_H_
#define INC_STATEMACHINE_H_

#include <stdlib.h>

typedef struct _Event_Queue Event_Queue;
typedef struct _StateMachine StateMachine;
typedef struct _Event Event;

typedef enum {ON, OFF, DEFAULT,MAXSTATES}state_typedef;

typedef enum {NOEVENT,STARTUP,ENTRY,EXIT,PA0,PC13,MAXEVENTS}event_typedef;

typedef struct _Event_Queue{


}Event_Queue;
typedef struct _StateMachine{

		// data related to state and data required for states

		state_typedef state;

		//Data related to event buffer

		int size_of_buffer;
		event_typedef *event_buffer;
		int read_index;
		int write_index;
		int current_buffer_length;
		void(*ptr_Add_Event_To_Buffer)(StateMachine *me, event_typedef event);
		event_typedef(*ptr_Pop_Event_From_Buffer)(StateMachine *me);


}StateMachine;

typedef struct _Event{
	event_typedef event;
}Event;



void initstatemachine(StateMachine *ptr_inst, int size);

void event_handler(StateMachine *me, Event *e);





//void init_buffer(StateMachine *me , int size_of_buffer);

void Add_Event_To_Buffer(StateMachine *me, event_typedef event);
event_typedef Pop_Event_From_Buffer(StateMachine *me);

#endif /* INC_STATEMACHINE_H_ */
