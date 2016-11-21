/*
 * messages.h
 *
 *  Created on: Apr 8, 2011
 *      Author: japoovey
 */

#ifndef MESSAGES_H_
#define MESSAGES_H_

/* These are all the message types you can use for Mreq
 * You may need to add new message types.  (Be sure to update messages.cpp if you do)
 */

typedef enum
{
    NOP = 0,

	LOAD,
	STORE,

    GETS,
    GETM,

    DATA,

    MREQ_INVALID,
	MREQ_MESSAGE_NUM	// Use this to make a Stat Array of message types
} message_t;

#endif /* MESSAGES_H_ */
