/*******************************************************************************
 * CMPT 434
 * Assignment 2
 * 
 * Lizzie Adams
 * ema651
 * 11139300
 ******************************************************************************/

#ifndef QUEUE_H
#define QUEUE_H

/* Local includes */
#include "param.h"

/* Library includes */
#include <stdio.h>
#include <stdlib.h>
#include <strconvert.h>
#include <string.h>

/* Structure representing message in queue */
struct msg {
    char buf[MSGSZ];                /* Message buffer */
    int sn;                         /* Sequence number */
    int numacks;                    /* Acks received from unique receivers */
};

/* Structure representing queue of messages */
struct queue {
    int sz;                         /* Number of elements in queue */
    struct msg *msgs[MAXPENDMSG];   /* Messages stored as array */
};

/* Function prototypes */
struct queue *      q_create(void);
int                 q_push(struct queue *, char *);
char *              q_pop(struct queue *);
void                q_print(struct queue *);

#endif
