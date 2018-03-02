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
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <queue.h>
#include <stdio.h>
#include <stdlib.h>
#include <strconvert.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

/* Structure representing receiver info */
struct recvinfo {
    char *hostname;                 /* Hostname */
    char *port;                     /* Port number */
    
    struct sockaddr addr;           /* Socket address information */
    socklen_t addrlen;              /* Socket address info struct length */
    int fd;                         /* Socket file descriptor */
    
    int sn;                         /* Next expected sequence number */
};

#endif
