#ifndef NODE_H
#define NODE_H

class Node {
public:
	bool alive; 
	int cost;
	int port;
	string ip;
	struct sockaddr_in addr;

	bool operator==(const Node& right) {
    	return right.ip == ip && right.port == port;
	}
};

typedef map<int, Node> toNode;

#endif