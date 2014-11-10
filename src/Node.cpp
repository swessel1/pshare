/*******************************************************************************
 * File        : Node.cpp
 * Description : A class that represents a node in the network.
 * Author(s)   : Tekin Ozbek <tekin@tekinozbek.net>
 *
 * Copyright (c) 2014 Tekin Ozbek, Ryan Lynar
 *
 * This file is part of pshare.
 *
 * pshare is free software: you can redistribute it and/or modify it under the
 * terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 * 
 * pshare is distributed in the hope that it will be useful, but WITHOUT ANY
 * WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR
 * A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along with
 * pshare. If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <cstdint>
#include <cstdio>
#include <thread>
#include <unistd.h>
#include "Node.h"
#include "NodeEvent.h"

Node::Node(int sd, struct sockaddr_in addr, BlockingQueue<Event> &queue) :
    EventRegistrar(queue), sd(sd), addr(addr) { }

Node::Node(unsigned short generation,
           unsigned short sibling_number,
           int sd,
           struct sockaddr_in addr,
           BlockingQueue<Event> &queue) :
    EventRegistrar(queue),
    generation(generation),
    sibling_number(sibling_number),
    sd(sd),
    addr(addr) { }

bool Node::open() {

    /* close current socket if open */
    ::close(sd);

    if ((sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        return false;

    if (connect(sd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
        return false;

    /* start listening for incoming messages on a separate thread */
    std::thread t(&Node::listen, this);
    t.detach();

    return true;
}

void Node::close() {

    ::close(sd);
}

void Node::listen() {

    ssize_t     recv_len;
    uint8_t     header;
    uint32_t    msg_size;
    char        buffer[BUFFER_SIZE];

    while (true) {

        /* receive header */
        if (recv(sd, &header, sizeof(uint8_t), 0) <= 0)
            break;

        /* receive message size */
        if (recv(sd, &msg_size, sizeof(uint32_t), 0) <= 0)
            break;

        FILE* tmp = tmpfile();
        msg_size  = ntohl(msg_size);

        while (msg_size > 0) {

            recv_len = recv(sd, buffer, BUFFER_SIZE, 0);

            if (recv_len <= 0) {

                /* connection was terminated */
                fclose(tmp);
                break;
            }

            fwrite(buffer, 1, recv_len, tmp);

            msg_size -= recv_len;
        }

        /* construct a message and register an event */
        NetworkMessage *message = new NetworkMessage(header, tmp);
        NodeEvent event(*this, message, NodeEvent::MSG_RECEIVED);
        register_event(event);
    }

    NodeEvent event(*this, this, NodeEvent::NODE_DISCONNECT);
    register_event(event);

    ::close(sd);
}
