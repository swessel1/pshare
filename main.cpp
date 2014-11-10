#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include "NetworkStructure.h"

int main() {

    BlockingQueue<Event> event_queue;

    NetworkStructure ns(event_queue);
    ns.start();

    while (true) {

        event_queue.front();
    }
    
    return 0;
}
