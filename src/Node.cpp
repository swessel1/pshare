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
#include <arpa/inet.h>
#include <cstdint>
#include <cstdio>
#include <thread>
#include <unistd.h>
#include "Node.h"

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

    return true;
}

void Node::close() {

    ::close(sd);
}

void Node::listen() {

    while (true) {

        /* construct empty message and let it receive from socket */
        NetworkMessage *msg = new NetworkMessage();
        
        if (!msg->recv(sd))
            break; // oops!

        /* register an event indicating new message */
        Event event(*this, msg, Event::NODE_MSG_RECEIVED);
        register_event(event);
    }

    Event event(*this, this, Event::NODE_DISCONNECT);
    register_event(event);

    ::close(sd);
}

bool Node::send(NetworkMessage &msg) {

    return msg.send(sd);
}

struct sockaddr_in& Node::get_addr() {

    return addr;
}

std::string Node::get_ineta() const {

    return inet_ntoa(addr.sin_addr);
}
