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
#include "routing.h"
#include "distvec.h"

int main(int argc, char**argv) {
   if (argc != 2) {
      printf("Use format: ./distvec managerhostname\n");
      exit(1);
   }

   sockfd = socket(AF_INET,SOCK_DGRAM,0);
   hints  = get_sock_addr(argv[1], PORT);
   id     = init_node_id();

   listening();
}