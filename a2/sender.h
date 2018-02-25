/*******************************************************************************
 * CMPT 434
 * Assignment 2
 * 
 * Lizzie Adams
 * ema651
 * 11139300
 ******************************************************************************/

#ifndef SENDER_H
#define SENDER_H

#define _XOPEN_SOURCE   600

/* Local includes */
#include "param.h"

/* Library includes */
#include <netdb.h>
#include <queue.h>
#include <stdio.h>
#include <stdlib.h>
#include <strconvert.h>
#include <string.h>
#include <unistd.h>

/* Structure representing receiver address info */
struct recv {
    char hostname[HOSTNAMESZ];      /* Hostname */
    char port[PORTSZ];              /* Port number */
    
    struct sockaddr addr;           /* Socket address information */
    socklen_t addrlen;              /* Socket address info struct length */
    int fd;                         /* Socket file descriptor */
    
    int seqnum;                     /* Current sequence number */
    int windowsz;                   /* Current width of sliding window */
};

/* Global variable declarations */
extern int windowsz;
extern int initseq;
extern int timeout;
extern char *recvinfofilename;
extern struct recv recvs[MAXRECV];
extern int numrecv;
extern int seqnum;

#endif
