//Wiktor Pilarczyk 308533
#include "header.h"
#include "convesation.h"
#include "check.h"

static void preparemessage(uint8_t *message, uint32_t net_ip, uint8_t mask, uint32_t dist){ // function which convert ip, mask and distance to string (9*chars)
    *((uint32_t *)&message[0]) = htonl(net_ip);
    message[4] = mask;
    *((uint32_t *)&message[5]) = htonl(dist);
}


static void from_message(uint8_t *message, uint32_t *net_ip, uint8_t *mask, uint32_t *dist){ // function which get from the message ip, mask and distance
    *net_ip = ntohl(*((uint32_t *)&message[0]));
    *mask = message[4];
    *dist = ntohl(*((uint32_t *)&message[5]));
}


static void sendvector(int n, struct connected *conn, struct entrance *enter){ // function send to all directly "connected" subnetworks our distance vextor
    int sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
    int broadcastPermission = 1;
    Setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, 
                (void *)&broadcastPermission, sizeof(broadcastPermission));
    
    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(54321);
    
    uint8_t message[9];
    char ip_addr[20];
    for(int g = 0; g < n; g++){
        ip_to_str(conn[g].broad_ip, ip_addr);
        inet_pton(AF_INET, ip_addr, &server_address.sin_addr);

        ssize_t ret;
        for(int h = 0; h < n; h++){ // send all neighbours
            preparemessage(message, conn[h].net_ip, conn[h].mask, conn[h].dist);
            ret = sendto(sockfd, message,  9, MSG_DONTWAIT, 
                    (struct sockaddr*) &server_address, sizeof(server_address));
            if(ret != 9){
                if(conn[g].dist != INF_DIST)    // important cause we clean count there and it is an optimalization :)
                    setinfviaconn(g, conn, &enter);
                conn[g].dist = INF_DIST;
            }
        }
        struct entrance *help = enter;
        while(help != NULL){    // send not directly connected entrance 
            preparemessage(message, help->net_ip, help->mask, help->dist);
            ret = sendto(sockfd, message, 9, MSG_DONTWAIT,                 //duplication of code but i didn t want to create fuctions with so many arguments
                    (struct sockaddr*) &server_address, sizeof(server_address));
            if(ret != 9){
                if(conn[g].dist != INF_DIST)    // important cause we clean count there and it is an optimalization :)
                    setinfviaconn(g, conn, &help);
                conn[g].dist = INF_DIST;
            }
            help = help->next;
        }
    }
    close(sockfd);
}


static void router(int n, int sockfd, struct connected *conn, struct entrance **enter){
    sendvector(n, conn, *enter);
    
    struct timeval timeout;
    timeout.tv_sec = TURN_TIME;
    timeout.tv_usec = 0;
    fd_set descriptors;
    FD_ZERO(&descriptors);
    FD_SET(sockfd, &descriptors);
    
    while(Select(sockfd + 1, &descriptors, NULL, NULL, &timeout)){  //in LINUX Select modifies timeout http://man7.org/linux/man-pages/man2/select.2.html
        struct sockaddr_in  sender;	
        socklen_t sender_len = sizeof(sender);
        uint8_t buffer[9];

        ssize_t datagram_len;
        while((datagram_len = recvfrom(sockfd, buffer, 9, MSG_DONTWAIT, 
                                (struct sockaddr*)&sender, &sender_len)) > 0){
            if(datagram_len != 9)   // i do not want to handle datagram's with another length
                continue;
            uint32_t sender_ip = ntohl(sender.sin_addr.s_addr);
            uint32_t new_ip, new_dist;
            uint8_t new_mask;

            int index = belong_to_table(n, sender_ip, conn);
            if(index != -1){    // if it is my message i update conn to inform that the network is still available
                if(conn[index].dist == INF_DIST)
                    conn[index].dist = conn[index].start_dist;
                conn[index].count = 0;
                continue;
            }
            from_message(buffer, &new_ip, &new_mask, &new_dist);
            process_table(n, conn, enter, new_ip, new_mask, new_dist, sender_ip);
        }
        if(datagram_len < 0 && errno != EWOULDBLOCK)
            error_handle("Recvfrom error %s\n", strerror(errno));
        FD_ZERO(&descriptors);
        FD_SET(sockfd, &descriptors);
    }
}


int main(){
    struct connected *conn = NULL;
    struct entrance *enter = NULL;
    int n;
    
    get_connections(&conn, &n);
    
    int sockfd;
    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);
    
    struct sockaddr_in server_address;
    bzero(&server_address, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);

    bind(sockfd, (struct sockaddr*) &server_address, sizeof(server_address));
    
    while(1){
        router(n, sockfd, conn, &enter);
        update_table(n, conn, &enter);
        print_table(n, conn, enter); 
    }

    close(sockfd);
    free(conn);
}
