#ifndef LINKSTATE_H
#define LINKSTATE_H

int                        id;
toNode                     next;
bool                       ready = false;
bool                       converge = false;
map<int, map<int, int> >   topo_table;
map<int, int>              hop,
                           dist,
                           pd;
map<int, bool>             node;
string                     table;

class cmp {
public:
   bool operator()(vector<int> x, vector<int> y) {
      return x[0]<y[0];
   }
};

void  listening();
void  set_ready();
void  send_message(istringstream& infoin);
void  update_link(istringstream& infoin);
void  update_table(istringstream& infoin);
int   gethop(int dest); 
string print_table();

bool  not_exist(int x, int y);
bool  set(int x, int y, int cost);
bool  equal(int x, int y, int cost);
void  dijkstra() ;
string dijkstra_helper(int temp2, int i);

void listening() {
   while (1) {
      char recv_str[MAXBUFF];
      int status, recv_len;
      
      recv_len = recvfrom(sockfd, recv_str, MAXBUFF, 0, NULL, NULL);
      recv_str[recv_len] = '\0';

      istringstream recvin(recv_str);
      recvin>>status;
      switch(status){
         case 200: set_ready(); break;
         case 201: update_link(recvin); break;
         case 205: update_table(recvin); break;
         case 301: send_message(recvin); break;
         default: break;
      }
   }
}

void set_ready() {
   ready = true;
   if(converge)
      cout<<print_table();
}

void send_message(istringstream& infoin) {
   ostringstream printout, sendout;
   int source, dest;
   string temp,from;
   infoin>>from>>source>>temp>>dest>>temp;
   printout<<"from "<<source<<" to "<<dest<<" hops ";
   while(1) {
      infoin>>temp;
      if(temp == "message")
         break;
      printout<<temp<<" ";
   }
   getline(infoin, temp);
   printout<<id<<" message"<<temp;
   cout<<printout.str()<<endl;
   sendout<<"301 "<<printout.str();

   if(gethop(dest) >= 0) {
      sendto(sockfd, sendout.str().c_str(), sendout.str().size(), 0,
         (struct sockaddr *)&(next[gethop(dest)].addr), sizeof(next[gethop(dest)].addr));
   }
}

void update_link(istringstream& infoin) {
   int dest, cost, port;
   string ip;

   map<int, int> broadcast_list;

   while(!infoin.eof()) {
      infoin>>dest>>cost;

      bool send_all = false;

      if(cost < 0) {
         cout<<"no longer linked to node "<<dest<<endl;
      }
      else { 
         cout<<"now linked to node "<<dest<<" with cost "<<cost<<endl;

         if(next.find(dest) != next.end() && (!next[dest].alive)) 
            send_all = true;
         
         infoin>>port;
         next[dest].cost = cost;

         if(port == 0) { 
            next[dest].alive = false;
         }
         else {
            infoin>>ip;
            next[dest].alive = true;
            next[dest].ip = ip;
            next[dest].port = port;
            next[dest].addr = get_sock_addr(ip.c_str(),port);

            if(send_all) {
               for(map<int, map<int, int> >::iterator it1 = topo_table.begin(); it1 != topo_table.end(); it1++) {
                  for(map<int,int>::iterator it2 = it1->second.begin(); it2 != it1->second.end(); it2++) {
                     ostringstream infoout;
                     infoout<<"205 "<<it1->first<<" "<<it2->first<<" "<<it2->second;
                     string status;
                     status = infoout.str();
                     sendto(sockfd, status.c_str(), status.size(), 0,
                        (struct sockaddr *)&(next[dest].addr), sizeof(next[dest].addr));
                  }
               }
            }
         }
      }

      set(id, dest, cost);
      
      broadcast_list[dest] = cost;
   }

   for(map<int,int>::iterator it1=broadcast_list.begin(); it1!=broadcast_list.end(); it1++) {
      for(toNode::iterator it2 = next.begin(); it2 !=next.end(); it2++) {
         if(((it1->first) != it2->first && id != it2->first) && it2->second.alive) {
            ostringstream infoout;
            infoout<<"205 "<<id<<" "<<it1->first<<" "<<(it1->second);
            string status;
            status = infoout.str();
            sendto(sockfd, status.c_str(), status.size(), 0,
               (struct sockaddr *)&(next[it2->first].addr), sizeof(next[it2->first].addr));
         }
      }
   }
}

void update_table(istringstream& infoin) {
   int src, dest, cost;
   infoin>>src>>dest>>cost;

   if(set(src,dest,cost)) {
      for(toNode::iterator it = next.begin(); it !=next.end(); it++) {
         if((dest != it->first && src != it->first) && it->second.alive) {
            ostringstream infoout;
            infoout<<"205 "<<src<<" "<<dest<<" "<<cost;
            string status;
            status = infoout.str();
            sendto(sockfd, status.c_str(), status.size(), 0,
               (struct sockaddr *)&(next[it->first].addr), sizeof(next[it->first].addr));
         }
      }
   }
}

int gethop(int dest) {
   if(hop.find(dest) == hop.end())
      return -1;
   else
      return hop[dest];
}

string print_table() {
   ostringstream str;
   str<<"\n"<<table<<"\n";
   return str.str();
}

bool set(int x, int y, int cost) {
   node[x] = node[y] = true;

   if(equal(x,y,cost))
      return false;

   converge = false;
   x<y ? topo_table[x][y] = cost : topo_table[y][x] = cost;

   dist.clear();
   pd.clear();
   hop.clear();
   dijkstra();
   converge = true;

   return true;
}

bool equal(int x, int y, int cost) {
   if(not_exist(x,y))
      return false;
   return ((x<y ? topo_table[x][y] : topo_table[y][x]) == cost);
}

bool not_exist(int x, int y){
   int min = x<y?x:y;
   int max = x<y?y:x;
   return (topo_table.find(min) == topo_table.end())||(topo_table[min].find(max) == topo_table[min].end());
}

void dijkstra() {
   priority_queue<vector<int>, vector<vector<int> >, cmp> next;
   int temp1;
   vector<int> n1(2);
   vector<int> n2(2);
   n1[0] = 0; 
   n1[1] = id;
   next.push(n1);

   for(map<int, bool>::iterator it = node.begin(); it != node.end(); it++) {
      dist[it->first] = INT_MAX;
   }
   dist[id] = 0;
   pd[id] = id;
   
   while(!next.empty()) {

      n1 = next.top();
      temp1 = n1[1];
      next.pop();

      for(map<int, bool>::iterator temp2=node.begin(); temp2!=node.end(); temp2++) {
         int cost = -1;
         if(!not_exist(temp1,temp2->first))
            cost = temp1<(temp2->first) ? topo_table[temp1][temp2->first] : topo_table[temp2->first][temp1];

         if(cost >= 0 && dist[temp2->first] > dist[temp1]+cost) {
            dist[temp2->first] = dist[temp1]+cost;
            pd[temp2->first] = temp1;
            n2[0] = dist[temp2->first];
            n2[1] = temp2->first;
            next.push(n2);
         }
      }
   }

   ostringstream infoout;
   for(map<int, bool>::iterator it = node.begin(); it != node.end(); it++) {
      if(dist[it->first] != INT_MAX)
         infoout<<it->first<<" "<<dist[it->first]<<": "<<dijkstra_helper(it->first, it->first)<<endl;
   }
   if(table != infoout.str()) {
      table = infoout.str();
      if(ready) {
         cout<<print_table();
      }
   }
}

string dijkstra_helper(int temp, int i) {
   ostringstream infoout;
   if(pd[i] == id) {
      hop[temp] = i;
      infoout<<id;
      if(i != id)
         infoout<<" "<<i;
   }
   else
      infoout<<dijkstra_helper(temp,pd[i])<<" "<<i;
   return infoout.str();
}
#endif