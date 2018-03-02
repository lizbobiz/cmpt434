/*******************************************************************************
 * CMPT 434
 * Assignment 2
 * 
 * Lizzie Adams
 * ema651
 * 11139300
 ******************************************************************************/

/* Local includes */
#include "receiver.h"

/* Global variable definitions */
int window_sz;                      /* Maximum sliding window size */
int sn;                             /* Received sequence number */
int sn_prev;                        /* Last received sequence number */
int sn_next;                        /* Next expected sequence number */
int sn_max;                         /* Maximum sequence number */
int loss_prob;                      /* Probability of ack loss */

char *port;                         /* Port number */

int init = 0;                       /* Reinitialize protocol flag */

int main(int argc, char *argv[]) {
    int portnum;
    char *token;
    char buf[MSGSZ];
    struct addrinfo hints, *res, *p;
    struct sockaddr_storage sendaddr;
    socklen_t sendaddrlen;
    int sockfd;
    int rv;
    
    /* Ensure four command-line arguments were given */
    if (argc != 4) {
        fprintf(stderr, "usage: %s <window size> <ack loss probability> "
            "<port number>\n", argv[0]);
        exit(1);
    }
    
    /* Get maximum sliding window size */
    if (strtoint(argv[1], &window_sz) != 0) {
        fprintf(stderr, "receiver: window size must be an integer\n");
        exit(1);
    } else if (window_sz <= 0) {
        fprintf(stderr, "receiver: window size must be greater than 0\n");
        exit(1);
    } else {
        sn_max = 2*window_sz - 1;
    }
    
    /* Get loss probability */
    if (strtoint(argv[2], &loss_prob) != 0) {
        fprintf(stderr, "receiver: ack loss probability must be an integer\n");
        exit(1);
    } else if ((loss_prob < 0) || (loss_prob > 100)) {
        fprintf(stderr, "receiver: ack loss probability must be between 0 and "
            "100 inclusive\n");
        exit(1);
    }
    
    /* Get port number */
    if (strtoint(argv[3], &portnum) != 0) {
        fprintf(stderr, "receiver: port number must be an integer\n");
        exit(1);
    } else if ((portnum < 30000) || (portnum > 40000)) {
        fprintf(stderr, "receiver: port number must be between 30000 and 40000 "
            "inclusive\n");
        exit(1);
    } else {
        port = argv[3];
    }
    
    /* Fill in hints for UDP */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE;
    
    /* Get addrinfo */
    if ((rv = getaddrinfo(NULL, port, &hints, &res)) != 0) {
        fprintf(stderr, "receiver: getaddrinfo: %s\n", gai_strerror(rv));
        exit(1);
    }
    
    /* Iterate over results */
    for (p = res; p != NULL; p = p->ai_next) {
        /* Create socket */
        if ((sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol))
            == -1) {
            perror("receiver: socket");
            continue;
        }
        
        /* Bind socket */
        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("receiver: bind");
            continue;
        }
        
        break;
    }
    
    /* If socket creation failed, exit on error */
    if (p == NULL) {
        fprintf(stderr, "receiver: failed to bind socket\n");
        exit(1);
    }
    
    /* Free getaddrinfo results */
    freeaddrinfo(res);
    
    /* Initialize random number generation */
    srand(time(NULL));
    
    /* Begin receiving messages */
    while (1) {
        /* Receive message from sender */
        memset(buf, 0, sizeof(buf));
        sendaddrlen = sizeof(struct sockaddr_storage);
        if (recvfrom(sockfd, buf, sizeof(buf)-1, 0,
            (struct sockaddr *) &sendaddr, &sendaddrlen) == -1) {
            perror("receiver: recvfrom");
            exit(1);
        }
        
        /* Parse sequence number */
        token = strtok(buf, " ");
        strtoint(token, &sn);
        
        /* Get rest of message */
        token = strtok(NULL, "");
        
        /* If reinitialization message, reset protocol state */
        if (strncmp(token, INITSTR, strlen(INITSTR)) == 0) {
            sn_prev = -1;
            init = 1;
            continue;
        }
        
        /* If not yet initialized, wait for next message */
        if (!init) {
            continue;
        }
        
        /* Print message to stdout */
        printf("RECEIVED:\t[%i] \"%s\"", sn, token);
        
        /* If new or retransmission, send ack (potentially) */
        if ((sn == sn_next) || (sn == sn_prev)) {
            /* Indicate retransmissions */
            if (sn == sn_prev) {
                printf("\t(RETRANSMISSION)");
            }
            
            /* Prompt for and read in y/n for whether message was corrupted */
            printf("\nWas the above message received correctly (Y/N)?\n");
            scanf("%s", buf);
            
            /* If message was not corrupted, send acknowledgement */
            if ((buf[0] == 'Y') || (buf[0] == 'y')) {
                /* Determine loss probability and send ack accordingly */
                if (rand() >= ((float) loss_prob)/100.00*(RAND_MAX + 1.0)) {
                    if (sendto(sockfd, buf, strlen(buf), 0,
                        (struct sockaddr *) &sendaddr, sendaddrlen) == -1) {
                        perror("receiver: sendto");
                        exit(1);
                    }
                    printf("Success: ack sent\n");
                } else {
                    printf("Corruption: ack not sent\n");
                }
                
                /* Slide to next sequence number */
                if (sn != sn_prev) {
                    sn_prev = sn_next;
                    if (++sn_next > sn_max) {
                        sn_next = 0;
                    }
                }
            }
        } else {
            printf("\t(OUT OF ORDER)\n");
        }
    }
    
    return 0;
}
