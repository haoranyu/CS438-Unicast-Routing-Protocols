#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <string>
#include <vector>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <signal.h>
#include <pthread.h>
#include <map>
#include <ctime>
#include <climits>
#include <limits>
#include <queue>
using namespace std;

#include "node.h"

#define MAXBUFF 1024
#define PORT 8080

vector<Node> node_list;
map<int, toNode> topology;
vector<pair<int, string> > msg_queue;

int maxnode;
int node_id = 0;
int sockfd;

void 	send_message();
void 	recvMessage(string msg, sockaddr_in their_addr);
void * 	slave(void* _args);
int 	init_node_conn(int from_node, int aim_node, Node node, int max);
Node 	init_node(int cost);
Node 	update_node(sockaddr_in their_addr);
void 	update_node_next(sockaddr_in their_addr);
void 	init_topology(char * filename);
void 	init_msg(char* filename);
void 	flood_new_node(int cost, sockaddr_in addr);
void 	flood_flag(string s);

void 	send_message() {
	for (vector<pair<int, string> >::iterator it = msg_queue.begin(); it != msg_queue.end(); ++it) {
		char * msg = (char *)((it->second).c_str());
		sockaddr_in addr = node_list[it->first-1].addr;
		sendto(sockfd, msg, ((string)msg).size(), 0, (struct sockaddr *)&addr, sizeof(addr)); 
	}
}

void 	recvMessage(string status, sockaddr_in their_addr) {
	if(status == "200" && node_id < maxnode) {
		node_id++;
		stringstream buff;
		buff<<node_id;

		sendto(sockfd, buff.str().c_str(), buff.str().length(), 0,
				(struct sockaddr *)&their_addr, sizeof(their_addr));
		
		node_list.push_back(update_node(their_addr));
		update_node_next(their_addr);

		string str = "201";
		for(map<int, Node>::iterator it = topology[node_id].begin(); it != topology[node_id].end(); ++it){
			
			char buff[MAXBUFF];

			if(it->second.port == 0)
				sprintf(buff," %d %d %d", it->first, it->second.cost, it->second.port);
			else{
				sprintf(buff," %d %d %d %s", it->first, it->second.cost, it->second.port, it->second.ip.c_str());
				flood_new_node(it->second.cost, it->second.addr);
			}
			str = str + buff;			
		}
		sendto(sockfd, str.c_str(), str.size(), 0,
				(struct sockaddr *)&their_addr, sizeof(their_addr)); 

		cout << "YE" << endl;
		if(node_id == maxnode) {
			sleep(1);
			flood_flag("200");
			send_message();
		}
	}
}


void * 	slave(void* null){
    free(null);
    int node_a, node_b, cost;
    // hyu: enable adding topology 
    while(1) {
    	cin>>node_a>>node_b>>cost;
    	topology[node_a][node_b].cost = topology[node_b][node_a].cost = cost;

    	char buff[2][MAXBUFF];
    	
    	if(cost >= 0) {
    		sprintf(buff[0], "%d %d %d %s", node_b, cost, node_list[node_b-1].port, node_list[node_b-1].ip.c_str());
	    	sprintf(buff[1], "%d %d %d %s", node_a, cost, node_list[node_a-1].port, node_list[node_a-1].ip.c_str());

    	}
    	else {
			sprintf(buff[0], "%d %d", node_b, cost);
	    	sprintf(buff[1], "%d %d", node_a, cost);
	    }

		string conn = "201 ";
		string str_a = conn + buff[0];
		sendto(sockfd, str_a.c_str(), str_a.size(), 0, (struct sockaddr *)&(node_list[node_a-1].addr), sizeof(node_list[node_a-1].addr)); 
    	string str_b = conn + buff[1];
    	sendto(sockfd, str_b.c_str(), str_b.size(), 0, (struct sockaddr *)&(node_list[node_b-1].addr), sizeof(node_list[node_b-1].addr)); 
    	send_message();
    }
    return NULL;
}

int 	init_node_conn(int from_node, int aim_node, Node node, int max){
	if(from_node > max)
		max = from_node;
	if(topology.find(from_node) == topology.end()) {
		toNode to_node;
		to_node.insert(pair<int, Node>(aim_node, node));
		topology[from_node] = to_node; 
	}
	else {
		topology[from_node][aim_node] = node;
	}
	return max;
}

Node 	init_node(int cost){
	Node node;
	node.alive = false;
	node.cost = cost;
	node.port = 0;
	node.ip = "";
	return node;
}

Node 	update_node(sockaddr_in their_addr){
	Node node;
	node.alive = true;
	node.cost = 0;
	node.port = ntohs((((struct sockaddr_in*)&their_addr)->sin_port));
	char ip[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(their_addr.sin_addr), ip, INET_ADDRSTRLEN);
	node.ip = (string)ip;
	node.addr = their_addr;
	return node;
}

void 	update_node_next(sockaddr_in their_addr){
	for(map<int, toNode>::iterator it = topology.begin(); it != topology.end(); it++){
		if(it->second.find(node_id) != it->second.end()) {
			it->second[node_id].alive = true;
			//it->second[node_id].cost = 0;
			it->second[node_id].port = ntohs((((struct sockaddr_in*)&their_addr)->sin_port));
			char ip[INET_ADDRSTRLEN];
			inet_ntop(AF_INET, &(their_addr.sin_addr), ip, INET_ADDRSTRLEN);
			it->second[node_id].ip = (string)ip;
			it->second[node_id].addr = their_addr;
		}
	}
}

void 	init_topology(char * filename){
	string sLine = "";
	ifstream topologyfile(filename);
	int max = 0;

	if(topologyfile.is_open()){
		string line;
		while (!topologyfile.eof()){
			int node_a, node_b, cost;
			getline(topologyfile, line, '\n');
			if(line != ""){
				istringstream topoin(line);
				topoin>>node_a;
				topoin>>node_b;
				topoin>>cost;
				Node node = init_node(cost);
				max = init_node_conn(node_a, node_b, node, max);
				max = init_node_conn(node_b, node_a, node, max);
			}
		}
	}
	maxnode = max;
}

void 	init_msg(char* filename) {
	ifstream msgfile(filename);
	if(msgfile.is_open()){
		string line;
		while (!msgfile.eof()){
			int source,
				dest;
			string message;
			getline(msgfile, line, '\n');
			if(line != ""){
				istringstream msgin(line);
				msgin>>source;
				msgin>>dest;
				getline(msgin, message);

				ostringstream msgout;
				msgout << "301 from " << source << " to " << dest << " hops  message" << message;
				msg_queue.push_back(pair<int,string>(source, msgout.str()));
			}
		}
	}
	msgfile.close();
}

void 	flood_new_node(int cost, sockaddr_in addr) {
	char temp[MAXBUFF];
	sprintf(temp,"201 %d %d %d %s", node_id, cost, node_list[node_id-1].port, (node_list[node_id-1].ip.c_str()));
	sendto(sockfd, temp, ((string)temp).size(), 0,(struct sockaddr *)&addr, sizeof(addr)); 
}

void 	flood_flag(string str) {
	for(int i = 0; i < node_list.size(); i++) {
		sendto(sockfd, str.c_str(), str.size(), 0,(struct sockaddr *)&(node_list[i].addr),sizeof(node_list[i].addr)); 
	}
}

int main(int argc, char**argv) {
	if (argc != 3){
        printf("Use format: ./manager topologyfile messagefile\n");
    	exit(1);
    }
	init_topology(argv[1]);
	init_msg(argv[2]);

	char *port = "8080";
    // int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct sockaddr_in hints;
	struct sockaddr_in their_addr;
	socklen_t sin_size;
	// struct sigaction sa;
	// int yes=1;
	// char s[INET6_ADDRSTRLEN];
	// int rv;

	if ((sockfd = socket(AF_INET,SOCK_DGRAM,0)) == -1){
		perror("server: socket");
	}

	memset(&hints, 0, sizeof hints);
	hints.sin_family = AF_INET;
	hints.sin_addr.s_addr = htonl(INADDR_ANY);
	hints.sin_port = htons(PORT);
	if (bind(sockfd,(struct sockaddr *)&hints,sizeof(hints)) == -1){
		close(sockfd);
		perror("setsockopt");
		exit(1);
	}

	pthread_t tid;
	pthread_create(&tid, NULL, slave, NULL);

	while(1) {  // main accept() loop
		char 	status[MAXBUFF];
		int 	status_len;
		sin_size = sizeof their_addr;
		if ((status_len = recvfrom(sockfd, status, MAXBUFF, 0, (struct sockaddr *)&their_addr, &sin_size)) == -1){
		    perror("recv");
		    exit(1);
		}
		else {
			status[status_len] = '\0';
		}
		recvMessage((string)status, their_addr);
	}
}