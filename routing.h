#ifndef ROUTING_H
#define ROUTING_H

#define MAXBUFF 1024
#define PORT 8080

int sockfd;
struct sockaddr_in hints;
struct sockaddr_in their_addr;

int init_node_id() {
   string status = "200";
   char recv_str[MAXBUFF];
   sendto(sockfd, status.c_str(), status.size(), 0, (struct sockaddr *)&hints, sizeof(hints));
   int recv_len = recvfrom(sockfd, recv_str, MAXBUFF, 0, NULL,NULL);
   recv_str[recv_len] = 0;
   return atoi(recv_str);
}

struct sockaddr_in get_sock_addr(const char *ip, int port) {
   struct sockaddr_in addr;
   bzero(&addr, sizeof(addr));
   addr.sin_family = AF_INET;
   addr.sin_addr.s_addr = inet_addr(ip);
   addr.sin_port = htons(port);
   return addr;
}


#endif