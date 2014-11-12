/*******************************************************************************
 * File        : NetworkStructure.cpp
 * Description : Represents the structure of the network.
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
#include <thread>
#include "out.h"
#include "NetworkStructure.h"

NetworkStructure::NetworkStructure(BlockingQueue<Event> &queue) :
    EventRegistrar(queue), terminal(false) { }

NetworkStructure::NetworkStructure(struct sockaddr_in parent_addr,
                                   bool term,
                                   BlockingQueue<Event> &queue) :
    EventRegistrar(queue), terminal(term) {

    /* create node and push into ancestry */
    Node *node = new Node(0, parent_addr, network_queue);
    ancestry.push_back(node);
}

NetworkStructure::~NetworkStructure() {

    // TODO: Complete implementation.
}

BlockingQueue<Event>& NetworkStructure::get_network_queue() {

    return network_queue;
}

void NetworkStructure::set_tcp_port(unsigned short port) {

    this->tcp_port = port;
}

void NetworkStructure::set_max_conn(unsigned short max) {

    this->max_conn = max;
}

bool NetworkStructure::start() {

    /* if there are any parent nodes, connect to it */
    if (ancestry.size() > 0) {

        /* if connected, send handshake request and start listening */
        if (ancestry[0]->open()) {
            
            // TODO: send handshake request

            // TODO: receive handshake response

            /* start listening for incoming messages on separate thread */
            std::thread t(&Node::listen, ancestry[0]);
            t.detach();
        }

        /* if failed, return false */
        else {

            out(4) << "failed to connect to parent node" << std::endl;
            return false;
        }
    }

    /* start tcp listener if not terminator node */
    if (!terminal) {
        
        tcp_listener = new TcpListener(network_queue, tcp_port);

        if (!tcp_listener->start()) {

            out(4) << "unable to bind port " << tcp_port << std::endl;
            return false;
        }

        out() << "listening for nodes on port " << tcp_port << std::endl;
    }

    std::thread control_thread(&NetworkStructure::control, this);
    control_thread.detach();

    return true;
}

void NetworkStructure::control() {

    while (true) {

        /* this will block the thread if there are no new events to process */
        Event &e = network_queue.front();

        /* after accepting a new connection, start listening for incoming
         * messages on a separate thread */
        if (e.get_flag() == Event::TCP_INC_CONNECTION) {

            Node *node = static_cast<Node *>(e.get_data());

            out() << "node at " << node->get_ineta() << " connected"
            << std::endl;

            std::thread t(&Node::listen, node);
            t.detach();

            num_conn++;
        }

        /* a node disconnected */
        else if (e.get_flag() == Event::NODE_DISCONNECT) {

            Node *node = static_cast<Node *>(e.get_data());

            out() << "node at " << node->get_ineta() << " disconnected"
            << std::endl;
        }

        network_queue.pop();
    }
}

void NetworkStructure::set_key(std::string key) {

    this->key = key;
}

void NetworkStructure::set_dir(std::string dir) {

    this->dir = dir;
}
