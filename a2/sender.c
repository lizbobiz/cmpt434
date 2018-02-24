/*******************************************************************************
 * CMPT 434
 * Assignment 2
 * 
 * Lizzie Adams
 * ema651
 * 11139300
 ******************************************************************************/

#include "sender.h"

/* Global variable definitions */
int windowsz;
int initseq;
int timeout;
char *recvinfofilename;
struct recv recvs[MAXRECV];
int numrecv = 0;

int main(int argc, char *argv[]) {
    FILE* fp;
    char buf[128];
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
    
    /* Get sliding window size */
    if (strtoint(argv[1], &windowsz) != 0) {
        fprintf(stderr, "sender: window size must be an integer\n");
        return 1;
    }
    
    /* Get initial sequence number */
    if (strtoint(argv[2], &initseq) != 0) {
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
        if (strlen(token) > HOSTNAMESZ) {
            fprintf(stderr, "sender: hostname %s is too long\n", token);
            return 1;
        }
        strncpy(recvs[numrecv].hostname, token, HOSTNAMESZ);
        
        /* Get port number */
        token = strtok(NULL, " \t\n");
        if (strlen(token) > PORTSZ) {
            fprintf(stderr, "sender: port %s is too long\n", token);
            return 1;
        }
        strncpy(recvs[numrecv].port, token, PORTSZ);
        
        numrecv++;
    }
    fclose(fp);
    
    /* Fill in hints for UDP client using IPv6 */
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_DGRAM;
    
    /* Establish a connection with each receiver */
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
            
            /* Attempt to bind socket */
            if (bind(recvs[i].fd, p->ai_addr, p->ai_addrlen) == -1) {
                close(recvs[i].fd);
                perror("sender: bind");
                continue;
            }
            
            /* Upon successful binding, save address and address length */
            memcpy(&recvs[i].addr, p->ai_addr, sizeof(recvs[i].addr));
            recvs[i].addrlen = p->ai_addrlen;
            break;
        }
        
        /* If failed to bind socket, exit on error */
        if (p == NULL) {
            fprintf(stderr, "sender: failed to bind socket\n");
            return 1;
        }
        
        /* Free getaddrinfo results */
        freeaddrinfo(servinfo);
        
        /* Send initial sequence number to receiver */
        sprintf(buf, "Initial sequence value: %i\n", initseq);
        if (sendto(recvs[i].fd, buf, strlen(buf), 0,
            &recvs[i].addr, recvs[i].addrlen) == -1) {
            perror("sender: sendto");
            return 1;
        }
    }
    
    return 0;
}
