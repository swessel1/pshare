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
#include "out.h"

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

    int try_times = 3;
    int c;

    /* close current socket if open */
    this->close();
    
    if ((sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
        return false;

    while (try_times-- > 0) {

        if ((c = connect(sd, (struct sockaddr *) &addr, sizeof(addr))) >= 0)
            break;

        out() << "connection failed, trying again in 3 seconds..." << std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(3000));
    }

    if (c < 0)
        return false;

    return true;
}

void Node::close() {

    if (sd >= 0)
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

    this->close();
}

Node::~Node() {

    this->close();
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

int Node::get_sd() const {

    return sd;
}

Node& Node::set_generation(unsigned short generation) {

    this->generation = generation;
    return *this;
}

Node& Node::set_sibling_number(unsigned short sibling_number) {

    this->sibling_number = sibling_number;
    return *this;
}

unsigned short Node::get_generation() const {

    return generation;
}

unsigned short Node::get_sibling_number() const {

    return sibling_number;
}

unsigned short Node::get_tcp_port() const {

    return tcp_port;
}

bool Node::is_terminal() const {

    return terminal;
}

void Node::set_terminal(bool terminality) {

    terminal = terminality;
}

void Node::set_tcp_port(unsigned short port) {

    tcp_port = port;
}
