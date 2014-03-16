#ifndef DISTVEC_H
#define DISTVEC_H

int                        id;
toNode                     next;
bool                       ready = false;
bool                       converge = true;
map<int, map<int, int> >   hop_table;
map<int, int>              dist;
int                        num_nodes = 0;
bool                       t_lock = false;
string                     table = "";

void        listening();
void        set_converge();
void        connect_to(istringstream& infoin);
void        update_table(istringstream& infoin);
void        set_ready();

void *      slave(void* null);
bool        next_hop();

void listening() {
   while (1) {
      char recv_str[MAXBUFF];
      int status, recv_len;
      
      recv_len = recvfrom(sockfd, recv_str, MAXBUFF, 0, NULL, NULL);
      recv_str[recv_len] = '\0';

      istringstream recvin(recv_str);
      recvin>>status;
      switch(status){
         case 202:   set_converge();break;
         case 201:   connect_to(recvin); break;
         case 205:   update_table(recvin); break;
         case 301:   send_message(recvin, id, next); break;
         default:    break;
      }
   }
}

void set_converge(){
   ready = true;  
   converge = false;
   set_ready(); 
}

void set_ready() {
   if(t_lock)
      return;
   else
      t_lock = true;

   pthread_t tid;
   pthread_attr_t attr;
   pthread_attr_init(&attr);
   pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
   pthread_create(&tid, &attr, slave, NULL);
}

void connect_to(istringstream& infoin){
   int dest, cost, port;
   string ip;
   while(!infoin.eof()) {
      infoin>>dest>>cost;

      if(num_nodes<dest) 
         num_nodes = dest;

      if(cost < 0) {
         cout << "no longer linked to node " << dest<<endl;

         hop_table.erase(dest);
         next.erase(dest);
      }
      else {
         if(next.find(dest) == next.end() || next[dest].cost != cost)
            cout << "now linked to node " << dest << " with cost " << cost<<endl;

         infoin>>port;
         next[dest].cost = cost;

         hop_table[dest][dest] = cost;
         hop_table[dest][id] = 0;
         hop[id] = id;

         if(port == 0) {
            next[dest].alive = false;
         }
         else {
            infoin>>ip;
            next[dest].alive = true;
            next[dest].ip = ip;
            next[dest].port = port;
            next[dest].addr = get_sock_addr(ip.c_str(),port);
         }
      }
   }
   next_hop();
   converge = false;
   set_ready();
   ostringstream updatemsg;
   updatemsg<<"205 "<<id;
   for(int i = 1; i <= num_nodes; i++) {
      if(dist.find(i) == dist.end()){
         updatemsg<<" -1";
      }
      else{
         updatemsg<<" "<<dist[i];
      }
   }
   for (map<int, Node>::iterator it = next.begin(); it != next.end(); it++) {
      if(it->second.alive)
         sendto(sockfd, updatemsg.str().c_str(), updatemsg.str().size(), 0,
            (struct sockaddr *)&(it->second.addr), sizeof(it->second.addr));
   }
}

void update_table(istringstream& infoin) {

   bool flag = false;
   ostringstream infoout;
   int src, base_cost, cost, c = 0;
   string temp;
   infoin>>src;
   base_cost = hop_table[src][src];

   while(!infoin.eof()) {
      c++;
      infoin>>temp;
      cost = atoi(temp.c_str());

      if(c == id)
         continue;
      else if(cost == -1) {
         hop_table[src][c] = -1;
         flag = true;
      }
      else if(hop_table[src].find(c) == hop_table[src].end()) {
         hop_table[src][c] = base_cost + cost;
         flag = true;
      }
      else if(hop_table[src][c] == -1){
         hop_table[src][c] = base_cost + cost;
         flag = true;
      }
      else {
         hop_table[src][c] = base_cost + cost;
         flag = true;
      }
   }

   if(c > num_nodes) 
      num_nodes = c;
   
   if (flag && next_hop()){
      if(converge && ready) {
      converge = false;
      set_ready();
      }
      ostringstream updatemsg;
      updatemsg<<"205 "<<id;
      for(int i = 1; i <= num_nodes; i++) {
         if(dist.find(i) == dist.end()){
            updatemsg<<" -1";
         }
         else{
            updatemsg<<" "<<dist[i];
         }
      }
      
      for (map<int, Node>::iterator it = next.begin(); it != next.end(); it++) {
         if(it->second.alive)
            sendto(sockfd, updatemsg.str().c_str(), updatemsg.str().size(), 0,
               (struct sockaddr *)&(it->second.addr), sizeof(it->second.addr));
      }
   }
}

void* slave(void* null){
   free(null);
   while(!converge) {
      converge = true;
      sleep(2);
   }
   if(ready){
      ostringstream str;

      for(int i = 1; i <= num_nodes; i++) {
         str<<i<<" "<<hop[i]<<" "<<dist[i]<<"\n";
      }
      table = str.str();
      cout<<print_table(table);
   }
   t_lock = false;
   return NULL;
}

bool next_hop() {
   map<int, int> curr_dist = dist;
   for(int i = 1; i <= num_nodes; i++) {
      int min = numeric_limits<int>::max();
      for(map<int, map<int, int> >::iterator it = hop_table.begin(); it != hop_table.end(); it++) {
         if(it->second.find(i)!=it->second.end() && it->second[i] != -1 && it->second[i]<min){
            min = it->second[i];
            hop[i] = it->first;
            dist[i] = it->second[i];
         }
      }
   }

   hop[id] = id;
   dist[id] = 0;

   if(curr_dist == dist)
      return false;
   return true;
}

#endif