#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "NetworkStructure.h"

int main() {

    BlockingQueue<Event> event_queue;

    //parent node
    Node *parent = new Node(0, (struct sockaddr_in){AF_INET, htons(15555), {inet_addr("127.0.0.1")}}, event_queue);

    NetworkStructure ns(parent, false, event_queue);
    ns.start();
    
    return 0;
}
