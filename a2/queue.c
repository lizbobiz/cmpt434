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
struct queue* q_create(void) {
    struct queue *q;
    
    q = (struct queue *) malloc(sizeof(struct queue));
    q->head = NULL;
    q->tail = NULL;
    
    return q;
}

/*******************************************************************************
 * q_is_empty:
 * 
 * Return 1 if the given queue is empty and 0 otherwise. If the given queue
 * pointer is null, -1 is returned and an error message is printed to stderr.
 ******************************************************************************/
int q_is_empty(struct queue *q) {
    /* If queue is null, exit on error */
    if (q == NULL) {
        fprintf(stderr, "q_is_empty: null queue pointer\n");
        return -1;
    }
    
    return (q->head == NULL) && (q->tail == NULL);
}

/*******************************************************************************
 * q_enqueue:
 * 
 * Add the given message to the tail of the given queue, returning 0 on success.
 * If the given queue pointer is null, -1 is returned and an error message is
 * printed to stderr.
 ******************************************************************************/
int q_enqueue(struct queue *q, char *data) {
    struct node *n;
    
    /* If queue is null, exit on error */
    if (q == NULL) {
        fprintf(stderr, "enqueue: null queue pointer\n");
        return -1;
    }
    
    /* Create new node */
    n = (struct node*) malloc(sizeof(struct node));
    memcpy(n->data, data, sizeof(n->data));
    n->next = NULL;
    
    /* Adjust next node pointer and queue head and tail */
    if (q->head == NULL) {
        q->head = n;
        q->tail = n;
    } else {
        q->tail->next = n;
        q->tail = n;
    }
    
    return 0;
}

/*******************************************************************************
 * q_dequeue:
 * 
 * Remove the node at the head of the given queue and return its data. If the
 * given queue pointer is null or the queue is empty, -1 is returned and an
 * error message is printed to stderr.
 ******************************************************************************/
char* q_dequeue(struct queue *q) {
    char *data;
    struct node *next;
    
    /* If queue is null, exit on error */
    if (q == NULL) {
        fprintf(stderr, "dequeue: null queue pointer\n");
        return NULL;
    }
    
    /* If queue is empty, exit on error */
    if (q->head == NULL) {
        fprintf(stderr, "dequeue: empty queue\n");
        return NULL;
    }
    
    /* Get data and save next node pointer of queue head before freeing it */
    data = q->head->data;
    next = q->head->next;
    free(q->head);
    
    /* Adjust next node pointer and queue head and tail */
    q->head = next;
    if (q->head == NULL) {
        q->tail = NULL;
    }
    
    return data;
}

/*******************************************************************************
 * q_print:
 * 
 * Print the data contained in the given queue. If the given queue pointer is
 * null, -1 is returned and an error message is printed to stderr.
 ******************************************************************************/
void q_print(struct queue *q) {
    struct node *n;
    
    /* If queue is null, exit on error */
    if (q == NULL) {
        fprintf(stderr, "q_print: null queue pointer\n");
        return;
    }
    
    printf("Printing queue at %p:\n", (void*) q);
    for (n=q->head; n != NULL; n=n->next) {
        /* Print node data */
        printf("\t%s", n->data);
        
        /* Indicate queue head */
        if (n == q->head) {
            printf("\t\t(head)");
        }
        
        /* Indicate queue tail */
        if (n == q->tail) {
            printf("\t\t(tail)");
        }
        
        printf("\n");
    }
}
