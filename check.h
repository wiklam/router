//Wiktor Pilarczyk 308533

void error_handle(const char *str, char *err);
int belong_to_table(int n, uint32_t ip, struct connected *conn);
int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
int Socket(int domain, int type, int protocol);
int Setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen);