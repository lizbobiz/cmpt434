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
#include <string.h>

/* Structure representing a queue node containing a UDP message */
struct node {
    char data[MSGSZ];               /* Node data (i.e. UDP message) */
    struct node *next;              /* Reference to next node in queue */
};

/* Structure representing a queue of UDP messages */
struct queue {
    struct node *head;              /* Queue head */
    struct node *tail;              /* Queue tail */
};

/* Function prototypes */
struct queue*       q_create(void);
int                 q_is_empty(struct queue*);
int                 q_enqueue(struct queue*, char*);
char*               q_dequeue(struct queue*);
void                q_print(struct queue*);

#endif
