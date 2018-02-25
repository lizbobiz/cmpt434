/*******************************************************************************
 * CMPT 434
 * Assignment 2
 *
 * Lizzie Adams
 * ema651
 * 11139300
 ******************************************************************************/

/* Local includes */
#include "sender.h"

/* Global variable definitions */
int maxwindowsz;                    /* Maximum sliding window size */
int seqnum;                         /* Current sequence number */
int timeout;                        /* Timeout value in seconds */
char *recvinfofilename;             /* Name of file containing receiver info */

struct recv recvs[MAXRECV];         /* Receiver info array */
int numrecv = 0;                   /* Number of receivers */

struct queue *msg_q;                /* Queue of pending outgoing messages */

/* Program entry point */
int main(int argc, char *argv[]) {
    FILE* fp;
    char buf[MSGSZ-HEADERSZ];
    char msg[MSGSZ];
    char *token;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int i;
    
    /* Ensure five command-line arguments were given */
    if (argc != 5) {
        fprintf(stderr, "usage: %s <window size> <initial sequence number> "
            "<timeout value> <receiver info filename>\n", argv[0]);
        return 1;
    }
    
    /* Get maximum sliding window size */
    if (strtoint(argv[1], &maxwindowsz) != 0) {
        fprintf(stderr, "sender: window size must be an integer\n");
        return 1;
    }
    
    /* Get initial sequence number */
    if (strtoint(argv[2], &seqnum) != 0) {
        fprintf(stderr, "sender: initial sequence number must be an integer\n");
        return 1;
    }
    
    /* Get timeout value */
    if (strtoint(argv[3], &timeout) != 0) {
        fprintf(stderr, "sender: timeout must be an integer\n");
        return 1;
    }
    
    /* Get receiver address info filename */
    recvinfofilename = argv[4];
    
    /* Open receiver address info file */
    if ((fp = fopen(recvinfofilename, "r")) == NULL) {
        perror("sender: fopen");
        return 1;
    }
    
    /* Iterate over file lines */
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        /* If file contains too many receivers, exit on error */
        if (numrecv >= MAXRECV) {
            fprintf(stderr, "sender: too many receivers\n");
            return 1;
        }
        
        /* Get hostname */
        token = strtok(buf, " \t");
        if (strlen(token) >= HOSTNAMESZ) {
            fprintf(stderr, "sender: hostname %s is too long\n", token);
            return 1;
        }
        strncpy(recvs[numrecv].hostname, token, HOSTNAMESZ);
        
        /* Get port number */
        token = strtok(NULL, " \t\n");
        if (strlen(token) >= PORTSZ) {
            fprintf(stderr, "sender: port %s is too long\n", token);
            return 1;
        }
        strncpy(recvs[numrecv].port, token, PORTSZ);
        
        numrecv++;
    }
    fclose(fp);
    
    /* Fill in hints for UDP */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    
    /* Iterate over receivers */
    for (i=0; i < numrecv; i++) {
        /* Get addrinfo for client */
        if ((rv = getaddrinfo(recvs[i].hostname, recvs[i].port, &hints,
            &servinfo)) != 0) {
            fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
            return 1;
        }
        
        /* Iterate over results */
        for (p = servinfo; p != NULL; p = p->ai_next) {
            /* Attempt to create socket */
            if ((recvs[i].fd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
                perror("sender: socket");
                continue;
            }
            
            break;
        }
        
        /* If socket creation failed, exit on error */
        if (p == NULL) {
            fprintf(stderr, "sender: failed to bind socket\n");
            return 1;
        }
        
        /* Upon successful socket creation, save address and address length */
        memcpy(&recvs[i].addr, p->ai_addr, sizeof(recvs[i].addr));
        recvs[i].addrlen = p->ai_addrlen;
        
        /* Free getaddrinfo results */
        freeaddrinfo(servinfo);
        
        /* Initialize receiver sliding window */
        recvs[i].seqnum = seqnum;
        recvs[i].windowsz = 0;
        
        /* Send initial sequence number to receiver */
        snprintf(msg, sizeof(msg), "%i", seqnum);
        if (sendto(recvs[i].fd, msg, strlen(msg), 0,
            &recvs[i].addr, recvs[i].addrlen) == -1) {
            perror("sender: sendto");
            return 1;
        }
    }
    
    /* Create message queue */
    msg_q = q_create();
    
    /* Begin receiving messages */
    while (1) {
        /* Read message from stdin and trim newline */
        fgets(buf, sizeof(buf), stdin);
        buf[strlen(buf)-1] = '\0';
        
        /* Create message containing header and body and add it to the queue */
        snprintf(msg, MSGSZ, "%i %s", seqnum++, buf);
        q_enqueue(msg_q, msg);
        q_print(msg_q);
        
        /* Attempt to send message to all receivers */
        for (i=0; i < numrecv; i++) {
            if (sendto(recvs[i].fd, msg, strlen(msg), 0,
                &recvs[i].addr, recvs[i].addrlen) == -1) {
                perror("sender: sendto");
                return 1;
            }
            recvs[i].windowsz++;
        }
    }
    
    return 0;
}
