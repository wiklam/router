//Wiktor Pilarczyk 308533

void setinfviaconn(int g, struct connected *conn, struct entrance **enter);
void get_connections(struct connected **conn, int *n);
void ip_to_str(uint32_t ip, char *str);
void process_table(int n, struct connected *conn, struct entrance **enter, uint32_t new_ip, uint8_t new_mask, uint32_t new_dist, uint32_t sender_ip);
void update_table(int n, struct connected *conn, struct entrance **enter);
void print_table(int n, struct connected *conn, struct entrance *enter); 
