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
#include <cstdio>
#include <string>
#include <netinet/in.h>
#include "out.h"
#include "NetworkMessageHeaders.h"
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
            
            /* ------------------ send handshake request ------------------- */
            out() << "sending handshake request to parent: "
            << ancestry[0]->get_ineta() << std::endl;
            
            FILE *f         = tmpfile();
            uint32_t keylen = htonl(key.length());
            out() << "key is: " << key << std::endl;
            out() << "wrote: " << fwrite(&keylen, sizeof(uint32_t), 1, f) << std::endl;
            out() << "wrote: " << fwrite(&key[0], sizeof(char), key.length(), f) << std::endl;
            
            NetworkMessage *msg = new NetworkMessage(PSHARE_CONN_REQ, f);
            bool result = msg->send(ancestry[0]->get_sd());
            delete msg;
            
            if (!result) {

                out(4) << "unable to send handshake request" << std::endl;
                return false;
            }

            /* ------------ receive handshake response --------------------- */
            msg = new NetworkMessage();
            result = msg->recv(ancestry[0]->get_sd());

            if (!result) {

                out(4) << "lost connection to parent without handshake response"
                << std::endl;

                delete msg;
                return false;
            }

            out(0) << "received local topology from parent" << std::endl;

            // TODO: process handshake response

            delete msg;

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

        else if (e.get_flag() == Event::NODE_MSG_RECEIVED) {

            /* get sender and message */
            Node &sender = static_cast<Node &>(e.get_registrar());
            NetworkMessage *msg = static_cast<NetworkMessage *>(e.get_data());

            if (msg->get_header() == PSHARE_CONN_REQ) {

                /* get the key length */
                uint32_t keylen;
                fread(&keylen, sizeof(uint32_t), 1, msg->get_payload());
                keylen = ntohl(keylen);
                out() << "received key length: " << keylen << std::endl;

                /* get key sent by node */
                std::string r_key(keylen, 0);
                fread(&r_key[0], sizeof(char), keylen, msg->get_payload());
    
                out() << "received connection request with key: " << r_key << std::endl;
            }

            delete msg;
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

unsigned short NetworkStructure::get_next_sibling_number() {

    if (next_sibling_number == 65535)
        next_sibling_number = 1;

    return next_sibling_number++;
}
