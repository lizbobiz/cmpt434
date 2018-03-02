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
int window_sz;                      /* Maximum sliding window size */
int sn_cur;                         /* Current sequence number */
int sn_max;                         /* Maximum sequence number */
int timeout;                        /* Retransmission timeout value (seconds) */

struct recvinfo recvs[MAXRECV];     /* Receiver info array */
int numrecv = 0;                    /* Number of receivers */

struct queue *msg_q;                /* Queue of pending outgoing messages */

int quit = 0;                       /* Quit program flag */

/* Program entry point */
int main(int argc, char *argv[]) {
    FILE* fp;
    char buf[MSGSZ];
    char *token;
    struct addrinfo hints, *res, *p;
    struct sockaddr_storage recvaddr;
    socklen_t recvaddrlen;
    struct timeval readtimeout;
    fd_set readfds;
    int flags;
    int bytes;
    int rv;
    int i, j;
    
    /* Ensure five command-line arguments were given */
    if (argc != 5) {
        fprintf(stderr, "usage: %s <window size> <initial sequence number> "
            "<retransmission timeout value> <receiver info filename>\n",
            argv[0]);
        exit(1);
    }
    
    /* Get maximum sliding window size */
    if (strtoint(argv[1], &window_sz) != 0) {
        fprintf(stderr, "sender: window size must be an integer\n");
        exit(1);
    } else if (window_sz <= 0) {
        fprintf(stderr, "sender: window size must be greater than 0\n");
        exit(1);
    } else {
        sn_max = 2*window_sz - 1;
    }
    
    /* Get initial sequence number */
    if (strtoint(argv[2], &sn_cur) != 0) {
        fprintf(stderr, "sender: initial sequence number must be an integer\n");
        exit(1);
    } else if (sn_cur < 0) {
        fprintf(stderr, "sender: initial sequence number must be "
            "non-negative\n");
        exit(1);
    }
    
    /* Get timeout value */
    if (strtoint(argv[3], &timeout) != 0) {
        fprintf(stderr, "sender: timeout must be an integer\n");
        exit(1);
    } else if (timeout < 0) {
        fprintf(stderr, "sender: timeout must be non-negative\n");
        exit(1);
    }
    
    /* Open receiver address info file */
    if ((fp = fopen(argv[4], "r")) == NULL) {
        perror("sender: fopen");
        exit(1);
    }
    
    /* Iterate over file lines */
    while (fgets(buf, sizeof(buf), fp) != NULL) {
        int portnum;
        
        /* If file contains too many receivers, exit on error */
        if (numrecv >= MAXRECV) {
            fprintf(stderr, "sender: too many receivers\n");
            exit(1);
        }
        
        /* Get hostname */
        token = strtok(buf, " \t");
        recvs[numrecv].hostname = (char *) malloc(strlen(token)+1);
        strncpy(recvs[numrecv].hostname, token, strlen(token)+1);
        
        /* Get port number */
        token = strtok(NULL, " \t\n");
        if (strtoint(token, &portnum) != 0) {
            fprintf(stderr, "sender: port number must be an integer\n");
            exit(1);
        } else if ((portnum < 30000) || (portnum > 40000)) {
            fprintf(stderr, "sender: port number must be between 30000 and "
                "40000 inclusive\n");
            exit(1);
        }
        recvs[numrecv].port = (char *) malloc(strlen(token)+1);
        strncpy(recvs[numrecv].port, token, strlen(token)+1);
        
        numrecv++;
    }
    fclose(fp);
    
    /* Fill in hints for UDP */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    
    /* Set up connection for each receiver */
    for (i=0; i < numrecv; i++) {
        /* Get addrinfo for client */
        if ((rv = getaddrinfo(recvs[i].hostname, recvs[i].port, &hints, &res))
            != 0) {
            fprintf(stderr, "sender: getaddrinfo: %s\n", gai_strerror(rv));
            exit(1);
        }
        
        /* Iterate over results */
        for (p = res; p != NULL; p = p->ai_next) {
            /* Create socket */
            if ((recvs[i].fd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
                perror("sender: socket");
                continue;
            }
            
            /* Set file descriptor control so that recvfrom is non-blocking */
            flags = fcntl(recvs[i].fd, F_GETFL, 0);
            fcntl(recvs[i].fd, F_SETFL, flags | O_NONBLOCK);
            
            break;
        }
        
        /* If socket creation failed, exit on error */
        if (p == NULL) {
            fprintf(stderr, "sender: failed to bind socket\n");
            exit(1);
        }
        
        /* Upon successful socket creation, save address and address length */
        memcpy(&recvs[i].addr, p->ai_addr, sizeof(recvs[i].addr));
        recvs[i].addrlen = p->ai_addrlen;
        
        /* Free getaddrinfo results */
        freeaddrinfo(res);
        
        /* Initialize receiver sliding window */
        recvs[i].sn = sn_cur;
        
        /* Send initial sequence number to receiver */
        snprintf(buf, sizeof(buf), "%i %s", sn_cur, INITSTR);
        if (sendto(recvs[i].fd, buf, strlen(buf), 0,
            (struct sockaddr *) &recvs[i].addr, recvs[i].addrlen) == -1) {
            perror("sender: sendto");
            exit(1);
        }
    }
    
    /* Zero out select file descriptor set */
    FD_ZERO(&readfds);
    
    /* Specify select timeout to be zero */
    readtimeout.tv_sec = 0;
    readtimeout.tv_usec = 0;
    
    /* Create message queue */
    msg_q = q_create();
    
    /* Begin receiving messages */
    while (1) {
        /* Get any pending acks from receivers */
        for (i=0; i < numrecv; i++) {
            recvaddrlen = sizeof(struct sockaddr_storage);
            rv = recvfrom(recvs[i].fd, buf, sizeof(buf)-1, 0,
                (struct sockaddr *) &recvaddr, &recvaddrlen);
            if ((rv == -1) && (errno != EAGAIN && errno != EWOULDBLOCK)) {
                perror("sender: recvfrom");
                exit(1);
            } else if (rv != -1) {
                /* Find corresponding message in queue */
                for (j=0; j < msg_q->sz; j++) {
                    if (recvs[i].sn == msg_q->msgs[j]->sn) {
                        /* Increment receiver sequence number */
                        if (++recvs[i].sn > sn_max) {
                            recvs[i].sn = 0;
                        }
                        
                        /* Increment acks and potentially remove from queue */
                        if (++msg_q->msgs[j]->numacks == numrecv) {
                            printf("Received ack from receiver %i\n", i);
                            
                            q_pop(msg_q);
                            q_print(msg_q);
                        }
                        
                        break;
                    }
                }
            }
        }
        
        /* Reset select file descriptor set and check stdin for input */
        if (!quit) {
            FD_SET(STDIN_FILENO, &readfds);
            if (select(1, &readfds, NULL, NULL, &readtimeout)) {
                /* Write sequence number and message to buffer */
                bytes = sprintf(buf, "%i ", sn_cur);
                fgets(&buf[bytes], MSGSZ-bytes, stdin);
                
                /* Trim newline from message */
                if (buf[strlen(buf)-1] == '\n') {
                    buf[strlen(buf)-1] = '\0';
                }
                
                /* Check for QUIT message */
                if (strncmp(&buf[bytes], QUITSTR, strlen(QUITSTR)) == 0) {
                    quit = 1;
                }
                
                if (!quit) {
                    /* Add message to outgoing queue */
                    q_push(msg_q, buf);
                    q_print(msg_q);
                    
                    /* Slide to next sequence number */
                    if (++sn_cur > sn_max) {
                        sn_cur = 0;
                    }
                    
                    printf("Message \"%s\" added to outgoing queue\n", buf);
                }
            }
        }
        
        /* If quit message was given and queue is empty, exit */
        if ((quit) && (msg_q->sz <= 0)) {
            break;
        }
        
        /* If queue is not empty, iterate over receivers */
        for (i=0; i < numrecv; i++) {
            /* Iterate over messages */
            for (j=0; j < msg_q->sz; j++) {
                /* If receiver is waiting on message, send it */
                if (msg_q->msgs[j]->sn == recvs[i].sn) {
                    printf("Sending message with sequence number %i to "
                        "receiver %i\n", msg_q->msgs[j]->sn, i);
                    
                    if (sendto(recvs[i].fd,
                        msg_q->msgs[j]->buf, strlen(msg_q->msgs[j]->buf), 0,
                        (struct sockaddr *) &recvs[i].addr, recvs[i].addrlen)
                        == -1) {
                        perror("sender: sendto");
                        exit(1);
                    }
                    
                    break;
                }
            }
        }
        
        /* Wait for given timeout before next iteration */
        sleep(timeout);
    }
    
    /* Close all socket file descriptors */
    for (i=0; i < numrecv; i++) {
        close(recvs[i].fd);
    }
    
    exit(0);
}
