#ifndef ROUTING_H
#define ROUTING_H

#define MAXBUFF 1024
#define PORT 8080

int               sockfd;
struct            sockaddr_in hints;
struct            sockaddr_in their_addr;
map<int, int>     hop;

int                  init_node_id() ;
struct sockaddr_in   get_sock_addr(const char *ip, int port);
int                  gethop(int dest);
string               print_table(string t);
void                 send_message(istringstream& infoin, int idx, toNode nextx);

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

int gethop(int dest) {
   if(hop.find(dest) == hop.end())
      return -1;
   else
      return hop[dest];
}

string print_table(string t) {
   ostringstream str;
   str<<"\n"<<t<<"\n";
   return str.str();
}

void send_message(istringstream& infoin, int idx, toNode nextx) {
   ostringstream infoout, inforedi;
   int src, dest;
   string temp, from;
   infoin>>from>>src>>temp>>dest>>temp;
   infoout << "from " << src << " to " << dest << " hops ";
   while(1) {
      infoin>>temp;
      if(temp == "message")
         break;
      infoout<<temp<<" ";
   }
   getline(infoin, temp);
   infoout<<idx<<" message"<<temp;
   cout<<infoout.str()<< endl;

   inforedi<<"301 "<<infoout.str();
   if(idx != dest && gethop(dest) != -1) {
      sendto(sockfd, inforedi.str().c_str(), inforedi.str().size(), 0,
         (struct sockaddr *)&(nextx[gethop(dest)].addr), sizeof(nextx[gethop(dest)].addr));
   }
}

#endif