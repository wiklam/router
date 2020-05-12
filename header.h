//Wiktor Pilarczyk 308533
#include <stdio.h>              // printf, frintf
#include <stdlib.h>             // exit, free, malloc
#include <string.h>             // strigns, strerror()
#include <assert.h>             // assert in compute_icmp_checksum
#include <errno.h>              // errno
#include <stdint.h>             // int8_t etc.
#include <stdbool.h>            // bool
#include <unistd.h>             // close
#include <netinet/in.h>         // struct sockaddr_in 
#include <sys/types.h>          // according to earlier standards
#include <sys/select.h>         // select()
#include <sys/socket.h>         // socket()
#include <arpa/inet.h>          // inet_ntop/pton()


struct connected{
    uint32_t dist;          // actually distance to network
    uint32_t my_ip;         // my ip address
    uint32_t broad_ip;      // broadcast ip
    uint32_t net_ip;        // network ip
    uint32_t start_dist;    // initial/default distance to network
    uint8_t mask;           // network mask
    uint8_t count;          // time counter for how many turns we didn't get any conntact with network
};

// same as above with the difference it is a linked list and we remeber the first addres on the path (ip_via) to net_ip
struct entrance{
    struct entrance *prev;
    struct entrance *next;
    uint32_t dist;
    uint32_t net_ip;
    uint32_t ip_via;    
    uint8_t mask;
    uint8_t count;
};

#define TURN_TIME 5         // duration of turn in seconds
#define STOP_PRINT_TURNS 10 // after STOP_PRINT_TURNS we stop printing unreachable array record
#define INFINITY_TURNS 5    // number of turns after we delete not directly connected entrance in routing table 
                            // and number of turns after we decide that a subnetwork is unreachable, cause we didn't get any message from it

#define PORT 54321          // port used to send and receive messages through UDP protocol
#define MAX_DIST 40         // when the distance is higher then MAX_DIST, it means the network is unreachable
#define INF_DIST 4294967295 // 2^32-1
