//Wiktor Pilarczyk 308533
#include "header.h"


void error_handle(const char *str, char *err){
    fprintf(stderr, str, err);
    exit(EXIT_FAILURE); 
}


int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout){
    int rv = select(nfds, readfds, writefds, exceptfds, timeout);
    if(rv < 0)
        error_handle("Select error: %s\n", strerror(errno));
    return rv;
}


int Socket(int domain, int type, int protocol){
    int rv = socket(domain, type, protocol);
    if (rv < 0)
        error_handle("Socket error %s\n", strerror(errno));
    return rv;
}


int Setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen){
    int rv = setsockopt(sockfd, level, optname, optval, optlen);
    if (rv < 0)
        error_handle("Setsockopt error %s\n", strerror(errno));
    return rv;
}


int belong_to_table(int n, uint32_t ip, struct connected *conn){
    for(int g = 0; g < n; g++){
        if(conn[g].my_ip == ip)
            return g;
    }
    return -1;
}