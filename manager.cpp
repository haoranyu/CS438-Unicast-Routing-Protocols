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
using namespace std;
#define MAXNODE 20

short int nodelist[MAXNODE+1];
long int topology[MAXNODE+1][MAXNODE+1];

struct msg{
	int source;
	int dest;
	string message;
};

vector<msg> msg_queue; //need to gonna freed

void init_topology_empty(){
	int i, j;
	for(i = 0; i <= MAXNODE; i++){
		for(j = 0; j <= MAXNODE; j++){
			topology[i][j] = -1;
		}
		nodelist[i] = 0;
	}
}

void init_topology(char * filename){
    ifstream topologyfile(filename);
	if(topologyfile.is_open()){
		string line;
		while (!topologyfile.eof()){
			int node_a, node_b, cost;
			getline(topologyfile, line, '\n');
			istringstream topoin(line);
			topoin>>node_a;
			topoin>>node_b;
			topoin>>cost;
			nodelist[node_a] = nodelist[node_b] = 1;
			topology[node_a][node_b] = cost;
			topology[node_b][node_a] = cost;
		}
	}
}

void print_topology(){
	int i, j;
	for(i = 1; i <= MAXNODE; i++){
		for(j = 1; j <= MAXNODE; j++){
			cout<<topology[i][j]<<'\t';
		}
		cout<<endl;
	}

	for(i = 1; i <= MAXNODE; i++){
		cout<<nodelist[i]<<endl;
	}
}

void update_topology(){
    string line;
    while(getline(cin, line, '\n')){
    	int node_a, node_b, cost;
    	istringstream topoin(line);
		topoin>>node_a;
		topoin>>node_b;
		topoin>>cost;
		nodelist[node_a] = nodelist[node_b] = 1;
		topology[node_a][node_b] = cost;
		topology[node_b][node_a] = cost;
		//print_topology();
    }
}

void init_msg(char * filename){
    ifstream msgfile(filename);
	if(msgfile.is_open()){
		string line;
		while (!msgfile.eof()){
			msg temp;// = new msg;
			getline(msgfile, line, '\n');
			if(line != ""){
				temp.source = atoi(line.substr(0,line.find(" ")).c_str());
				line = line.substr(line.find(" ")+1);
				temp.dest = atoi(line.substr(0,line.find(" ")).c_str());
				temp.message = line.substr(line.find(" ")+1);
				
				msg_queue.push_back(temp);
			}
		}
	}
}


int main(int argc, char* argv[]){
	if (argc != 3){
        printf("Use format: ./manager topologyfile messagefile\n");
    	exit(1);
    }
    init_topology_empty();
    init_topology(argv[1]);
    init_msg(argv[2]);
    
    int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
	struct addrinfo hints, *servinfo, *p;
	struct sockaddr_storage their_addr; // connector's address information
	socklen_t sin_size;
	struct sigaction sa;
	int yes=1;
	char s[INET6_ADDRSTRLEN];
	int rv;

	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; // use my IP

    //msg temp = msg_queue.at(0);
    //cout<<msg_queue.size();
    print_topology();
    update_topology();
}