/*******************************************************************************
 * CMPT 434
 * Assignment 2
 * 
 * Lizzie Adams
 * ema651
 * 11139300
 ******************************************************************************/

#ifndef PARAM_H
#define PARAM_H

/* Program configuration constants */
#define MAXRECV         10          /* Maximum number of receivers */
#define MAXPENDMSG      512         /* Maximum number of pending messages */

/* Buffer sizes */
#define MSGSZ           256         /* Size of buffer for UDP messages */

/* Strings used for program signaling */
#define INITSTR         "INIT"      /* String to trigger reinitialization */
#define QUITSTR         "QUIT"      /* String to trigger program termination */

#endif
