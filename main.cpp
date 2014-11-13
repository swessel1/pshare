#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <getopt.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <netdb.h>
#include "out.h" /* for extern bool verbose */
#include "NetworkStructure.h"

int main(int argc, char **argv) {

    if (argc < 4) {

        std::cout <<
            "+++ usage: " << argv[0] << " [options] <mode> <args...>"
        << std::endl;

        return -1;
    }

    bool terminal = false;
    int  tcp_port = 26005;
    int  max_conn = 10;
    int  c;

    while (true) {

        static struct option long_options[] = {

            {"verbose",             no_argument, 0, 'v'},
            {"terminal",            no_argument, 0, 't'},
            {"tcp-port",            required_argument, 0, 'p'},
            {"max-connections",     required_argument, 0, 'm'}
            
        };

        int opt_index = 0;

        c = getopt_long(argc, argv, "vtp:m:", long_options, &opt_index);

        if (c == -1)
            break;

        switch (c) {

            case 0:
                break;

            /* verbose */
            case 'v':
                std::cout << "verbose: true" << std::endl;
                verbose = 1;
                break;

            /* tcp port */
            case 'p':
                std::cout << "port: " << optarg << std::endl;
                tcp_port = atoi(optarg);
                if (tcp_port < 0 || tcp_port > 65535) {
                    
                    std::cout <<
                        "+++ invalid port: " << optarg << " +++"
                    << std::endl;
                    return -1;
                }
                break;

            /* max number of connections to this node */
            case 'm':
                std::cout << "maxconn: " << optarg << std::endl;
                max_conn = atoi(optarg);
                if (max_conn < 0 || max_conn > 65535) {
                    
                    std::cout <<
                        "+++ max connections must be between 0 and 65535 +++"
                    << std::endl;
                    return -1;
                }
                if (max_conn == 0)
                    terminal = true;
                break;

            /* terminal */
            case 't':
                std::cout << "terminal: true" << std::endl;
                terminal = true;
                break;
                
            case '?':
                break;
                
            default:
                exit(-1);
        }
    }

    NetworkStructure *network;
    BlockingQueue<Event> event_queue;

    /* parse rest of the commands */
    if (optind < argc) {

        /* node running in host mode (root node). min. 2 args: dir, key */
        if (strcmp("host", argv[optind]) == 0) {

            if (argc - optind - 1 < 2) {
                
                std::cout <<
                    "+++ missing args: host <dir> <key> +++"
                << std::endl;

                return -1;
            }

            network = new NetworkStructure(event_queue);
            network->set_key(argv[optind + 2]);
            network->set_dir(argv[optind + 1]);
        }

        /* node connecting to a parent. min. 3 args: ip, port, key */
        else if (strcmp("connect", argv[optind]) == 0) {

            if (argc - optind - 1 < 3) {
                
                std::cout <<
                    "+++ missing args: connect <ip|hostname> <port> <key> +++"
                << std::endl;

                return -1;
            }

            /* validate port */
            int port = atoi(argv[optind + 2]);
            if (port < 0 || port > 65535) {

                std::cout <<
                    "+++ invalid arg: port must be between 0 and 65535 +++"
                << std::endl;

                return -1;
            }

            /* validate ip address or hostname */
            struct addrinfo *a_info;
            if (getaddrinfo(argv[optind + 1], NULL, NULL, &a_info) != 0) {

                std::cout <<
                    "+++ invalid ip or hostname +++"
                << std::endl;

                return -1;
            }

            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_port   = htons(port);
            addr.sin_addr   = ((struct sockaddr_in *)a_info->ai_addr)->sin_addr;

            network = new NetworkStructure(addr, terminal, event_queue);
            network->set_key(argv[optind + 3]);
        }

        /* lolwut */
        else {

            std::cout << "+++ mode not indicated +++" << std::endl;
            return -1;
        }

        optind++;
    }

    /* set max connections, tcp port and then start network */
    network->set_max_conn(max_conn);
    network->set_tcp_port(tcp_port);
    
    if (!network->start()) {

        std::cout << "unable to establish network structure" << std::endl;
        return -2;
    }

    while (true) {
        event_queue.front();
    }
    
    return 0;
}
