#ifndef NEIGHBOR_H
#define NEIGHBOR_H

class Neighbor {
public:
	bool online; 
	int cost;
	int port;
	string ip;
	struct sockaddr_in addr;

	bool operator==(const Neighbor& other)
	{
    	return other.ip == ip && other.port == port;
	}
};

typedef map<int, Neighbor> Neighbors;

typedef struct sockaddr_in Sockaddr;

ssize_t sendTo(int sockfd, string& str, Sockaddr& addr) {
	return sendto(sockfd,str.c_str(),str.size(),0,(struct sockaddr *)&addr,sizeof(addr)); 
}

/*void createSockaddr(Sockaddr& addr,string& ip, int port) {
   createSockaddr(addr, ip.c_str(), port);
}*/
Sockaddr createSockaddr(const char *ip, int port) {
	Sockaddr addr;
	bzero(&addr,sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = inet_addr(ip);
	addr.sin_port = htons(port);
	return addr;
}

#endif