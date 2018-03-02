/*******************************************************************************
 * CMPT 434
 * Assignment 2
 * 
 * Lizzie Adams
 * ema651
 * 11139300
 ******************************************************************************/

#include "queue.h"

/*******************************************************************************
 * q_create:
 * 
 * Return a reference to a new empty queue.
 ******************************************************************************/
struct queue *q_create(void) {
    struct queue *q;
    
    q = (struct queue *) malloc(sizeof(struct queue));
    q->sz = 0;
    
    return q;
}


/*******************************************************************************
 * q_push:
 * 
 * Add the given message to the tail of the given queue, returning 0 on success.
 * If the given queue pointer is null, -1 is returned and an error message is
 * printed to stderr.
 ******************************************************************************/
int q_push(struct queue *q, char *buf) {
    struct msg *m;
    
    /* If queue is null, exit on error */
    if (q == NULL) {
        fprintf(stderr, "q_push: null queue pointer\n");
        return -1;
    }
    
    /* If queue is full, exit on error */
    if (q->sz >= MAXPENDMSG) {
        fprintf(stderr, "q_push: queue is full\n");
        return -1;
    }
    
    /* Allocate memory for new message */
    m = (struct msg *) malloc(sizeof(struct msg));
    
    /* Set message properties */
    memcpy(m->buf, buf, sizeof(m->buf));
    strtoint(strtok(buf, " "), &m->sn);
    m->numacks = 0;
    
    /* Add message to queue */
    q->msgs[(q->sz)++] = m;
    
    return 0;
}

/*******************************************************************************
 * q_pop:
 * 
 * Remove the node at the head of the given queue and return its data. If the
 * given queue pointer is null or the queue is empty, -1 is returned and an
 * error message is printed to stderr.
 ******************************************************************************/
char *q_pop(struct queue *q) {
    char *buf;
    
    /* If queue is null, exit on error */
    if (q == NULL) {
        fprintf(stderr, "q_pop: null queue pointer\n");
        return NULL;
    }
    
    /* If queue is empty, exit on error */
    if (q->sz == 0) {
        fprintf(stderr, "q_pop: queue is empty\n");
        return NULL;
    }
    
    /* Save message data and free memory */
    buf = q->msgs[0]->buf;
    free(q->msgs[0]);
    
    /* Shift array elements up by one */
    memmove(&q->msgs[0], &q->msgs[1], MSGSZ*sizeof(char));
    (q->sz)--;
    
    return buf;
}

/*******************************************************************************
 * q_print:
 * 
 * Print the data contained in the given queue. If the given queue pointer is
 * null, -1 is returned and an error message is printed to stderr.
 ******************************************************************************/
void q_print(struct queue *q) {
    int i;
    
    printf("Printing queue at %p:\n", (void *) q);
    
    if (q->sz == 0) {
        printf("\t(EMPTY)\n");
        return;
    }
    
    for (i=0; i < q->sz; i++) {
        printf("\t[%i]:\t", i);
        printf("( sn = %i )\t", q->msgs[i]->sn);
        printf("( numacks = %i )\t", q->msgs[i]->numacks);
        printf("\"%s\"\n", q->msgs[i]->buf);
    }
}
