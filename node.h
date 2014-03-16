#ifndef NODE_H
#define NODE_H

class Node {
public:
	bool alive; 
	int cost;
	int port;
	string ip;
	struct sockaddr_in addr;

	bool operator==(const Node& other)
	{
    	return other.ip == ip && other.port == port;
	}
};

typedef map<int, Node> toNode;

/*void createSockaddr(Sockaddr& addr,string& ip, int port) {
   createSockaddr(addr, ip.c_str(), port);
}*/

#endif