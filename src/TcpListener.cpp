/*******************************************************************************
 * File        : TcpListener.cpp
 * Description : Class that listens for incoming connections.
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

#include <iostream>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <thread>
#include "TcpListener.h"
#include "Node.h"
#include "out.h"

TcpListener::TcpListener(BlockingQueue<Event> &event_queue,
                         unsigned short port) :
    EventRegistrar(event_queue) {

    /* create a socket */
    sd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

    /* set up address structure */
    memset(&addr, 0, sizeof(addr));

    addr.sin_family         = AF_INET;
    addr.sin_addr.s_addr    = htonl(INADDR_ANY);
    addr.sin_port           = htons(port);
}

TcpListener::~TcpListener() {

    ::close(sd);
}

bool TcpListener::start() {

    if (this->bind() == false)
        return false;

    std::thread t(&TcpListener::listen, this);
    t.detach();

    return true;
}

void TcpListener::stop() {

    ::close(sd);
}

void TcpListener::listen() {
    
    while (::listen(sd, 0) >= 0) {
        
        struct sockaddr_in addr_recv;
        socklen_t addr_size = sizeof(addr);
        
        int sock_recv = accept(sd, (struct sockaddr *) &addr_recv, &addr_size);
        
        /* create a new node */
        Node *node = new Node(sock_recv, addr_recv, event_queue);
        Event event(*this, node, Event::TCP_INC_CONNECTION);

        register_event(event);
    }
}

bool TcpListener::bind() {

    if (::bind(sd, (struct sockaddr *) &addr, sizeof(addr)) < 0)
        return false;
        
    return true;
}
