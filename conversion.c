//Wiktor Pilarczyk 308533
#include "header.h"
#include "convesation.h"
#include "check.h"

static inline uint32_t make_ip(unsigned int v1, unsigned int v2, unsigned int v3, unsigned int v4){
    return ((v1 << 24) | (v2 << 16) | (v3 << 8) | v4);
}


static inline uint32_t make_broad_ip(uint32_t ip, uint8_t mask){
    return (ip | (0xFFFFFFFFU >> mask));
}


static inline uint32_t make_net_ip(uint32_t ip, uint8_t mask){
    return (ip & (~(0xFFFFFFFFU >> mask)));
}


static bool belong_to_network(uint32_t net_ip, uint8_t mask, uint32_t ip){
    return  net_ip == make_net_ip(ip, mask);
}


void ip_to_str(uint32_t ip, char *str){
    sprintf(str, "%d.%d.%d.%d", (ip >> 24), ((ip >> 16) & 0xFF), ((ip >> 8) & 0xFF), (ip & 0xFF));
}


static int get_from_table(int n, uint32_t sender_ip, struct connected *conn){
    for(int g = 0; g < n; g++){
        if(belong_to_network(conn[g].net_ip, conn[g].mask, sender_ip))
            return g;
    }
    return -1;
}


static struct entrance *get_from_list(struct entrance *enter, uint32_t ip){
    while(enter != NULL){
        if(enter->net_ip == ip)
            break;
        enter = enter->next;
    }
    return enter;
}


void get_connections(struct connected **conn, int *n){   // function initialize the connection table
    scanf("%d", n);
    *conn = malloc((*n) * sizeof(struct connected));

    for(int g = 0; g < *n; g++){
        unsigned int v1, v2, v3, v4, v5, v6;
        scanf("%u.%u.%u.%u/%u distance %u", &v1, &v2, &v3, &v4, &v5, &v6);

        (*conn)[g].dist = (*conn)[g].start_dist = v6;
        (*conn)[g].count = 0;
        (*conn)[g].my_ip = make_ip(v1, v2, v3, v4);
        (*conn)[g].mask = v5;
        (*conn)[g].broad_ip = make_broad_ip((*conn)[g].my_ip, (*conn)[g].mask);
        (*conn)[g].net_ip = make_net_ip((*conn)[g].my_ip, (*conn)[g].mask);
    }
}


static struct entrance *new_entrance(struct entrance *enter, uint32_t ip, 
                                        uint8_t mask, uint32_t dist, uint32_t via_ip){
    struct entrance *help = malloc(sizeof(struct entrance));
    if(enter != NULL)
        enter->prev = help;
    help->prev = NULL;
    help->next = enter;
    help->net_ip = ip;
    help->mask = mask;
    help->dist = dist;
    help->ip_via = via_ip;
    help->count = 0;
    return help;
}


static struct entrance *deletefromentrance(struct entrance **enter, struct entrance *delete){
    if(delete->prev == NULL)
        (*enter) = (*enter)->next;
    else
        (delete->prev)->next = delete->next;
    if(delete->next != NULL)
        (delete->next)->prev = delete->prev;

    struct entrance *help = delete->next;
    free(delete);
    return help;
}


void process_table(int n, struct connected *conn, struct entrance **enter, uint32_t new_ip,
                    uint8_t new_mask, uint32_t new_dist, uint32_t sender_ip){
    
    int send_num = get_from_table(n, sender_ip, conn);
    conn[send_num].count = 0;
    if(conn[send_num].dist == INF_DIST){   // network reconnected
        conn[send_num].dist = conn[send_num].start_dist;
        struct entrance *id = get_from_list(*enter, conn[send_num].net_ip);
        if(id != NULL)
            deletefromentrance(enter, id);
        return;
    }

    int check = get_from_table(n, new_ip, conn);
    if(check != -1){                            // if our received ip is in table (directly connected) it is the best path and we do not want to continue
        if(conn[check].dist != INF_DIST)        // else (it is unreachable) but we can connect through other network add it to the (undirectly) entrances
            return;
    }

    struct entrance *id = get_from_list(*enter, new_ip);
    if(id == NULL){                             // if we do not have this network in our list add it
        if(new_dist == INF_DIST)
            return;
        *enter = new_entrance(*enter, new_ip, new_mask, new_dist + conn[send_num].dist, sender_ip);
    }
    else{                                       // network in our (undirectly) list
        if(sender_ip == id->ip_via){            // if it is the same network which we go through update distances and count (references)
            if(new_dist == INF_DIST){
                id->dist = INF_DIST;
                return;
            }
            id->count = 0;
            id->dist = new_dist + conn[send_num].dist;
        }
        else{                                  // else if the new path is better update
            if(new_dist == INF_DIST)
                return;
            if(id->dist > new_dist + conn[send_num].dist){
                id->count = 0;
                id->dist = new_dist + conn[send_num].dist;
                id->ip_via = sender_ip;
            }
        }
    }
}


void setinfviaconn(int g, struct connected *conn, struct entrance **enter){
    struct entrance *help = *enter;
    while(help != NULL){
        if(belong_to_network(conn[g].net_ip, conn[g].mask, help->ip_via)){
            help->dist = INF_DIST;
            help->count = 0;
        }
        help = help->next;
    }
}


void update_table(int n, struct connected *conn, struct entrance **enter){
    for(int g = 0; g < n; g++){
        if(conn[g].count == STOP_PRINT_TURNS)
            continue;
        conn[g].count++;
        if(conn[g].count == INFINITY_TURNS){
            conn[g].dist = INF_DIST;
            setinfviaconn(g, conn, enter);
        }
    }
    struct entrance *help = (*enter);
    while(help != NULL){
        help->count++;
        if(help->dist == INF_DIST){
            if(help->count == INFINITY_TURNS){
                help = deletefromentrance(enter, help);
                continue;
            }
        }
        else{
            if(help->dist > MAX_DIST || help->count == INFINITY_TURNS){
                help->dist = INF_DIST;
                help->count = 0;
            }
        }
        help = help->next;
    }
}


void print_table(int n, struct connected *conn, struct entrance *enter){
    char ip[20];
    for(int g = 0; g < n; g++){
        if(conn[g].count == STOP_PRINT_TURNS)
            continue;
        ip_to_str(conn[g].net_ip, ip);
        printf("%s/%d ", ip, conn[g].mask);
        if(conn[g].dist == INF_DIST)
            printf("unreachable ");
        else
            printf("distance %d ", conn[g].dist);
        printf("connected directly\n");
    }
    while(enter != NULL){
        ip_to_str(enter->net_ip, ip);
        printf("%s/%d ", ip, enter->mask);
        if(enter->dist == INF_DIST)
            printf("unreachable ");
        else
            printf("distance %d ", enter->dist);
        ip_to_str(enter->ip_via, ip);
        printf("via %s\n", ip);
        enter = enter->next;
    }
    printf("\n");
}
