#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cstdlib>
#include <string>
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

long int topology[MAXNODE+1][MAXNODE+1];

void init_topology_empty(){
	int i, j;
	for(i = 0; i <= MAXNODE; i++){
		for(j = 0; j <= MAXNODE; j++){
			topology[i][j] = -1;
		}
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
}

int main(int argc, char* argv[]){
	if (argc != 3){
        printf("Use format: ./manager topologyfile messagefile\n");
    	exit(1);
    }
    init_topology_empty();
    init_topology(argv[1]);
    print_topology();
}