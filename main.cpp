#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include "NetworkStructure.h"

int main(int argc, char* argv[]) {

    NetworkStructure *network;
    BlockingQueue<Event> event_queue;

    if (argc == 3) {


        struct sockaddr_in addr;
        addr.sin_family = AF_INET;
        addr.sin_port   = htons(atoi(argv[2]));
        addr.sin_addr.s_addr   = inet_addr(argv[1]);
        
        network = new NetworkStructure(addr, true, event_queue);
    } else {

        network = new NetworkStructure(event_queue);
    }

    network->start();

    while (true) {

        event_queue.front();
        
    }
    
    return 0;
}
