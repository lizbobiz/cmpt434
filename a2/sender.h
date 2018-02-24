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

/* Library includes */
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <strconvert.h>
#include <string.h>
/*#include <sys/types.h>
#include <sys/socket.h>*/
#include <unistd.h>

/* Global constant definitions */
#define MAXRECV         10          /* Maximum number of receivers */
#define HOSTNAMESZ      32          /* Maximum length of hostname string */
#define PORTSZ          5           /* Maximum length of port string */
#define SERVERPORT      "30000"     /* Port to be used by server */

/* Structure representing receiver address info */
struct recv {
    char hostname[HOSTNAMESZ+1];
    char port[PORTSZ+1];
    struct sockaddr addr;
    size_t addrlen;
    int fd;
};

/* Global variable declarations */
extern int windowsz;
extern int initseq;
extern int timeout;
extern char *recvinfofilename;
extern struct recv recvs[MAXRECV];
extern int numrecv;

#endif
