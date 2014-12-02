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
    Node *node = new Node(-1, parent_addr, network_queue);
    ancestry.push_back(node);
}

NetworkStructure::~NetworkStructure() {

    clear_lists(true, true, true);
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

bool NetworkStructure::handshake() {

    /* if connected, send handshake request and start listening */
    if (ancestry[0]->open()) {
        
        /* ------------------ send handshake request ------------------- */
        out() << "sending handshake request to parent: "
        << ancestry[0]->get_ineta() << std::endl;

        FILE *f  = tmpfile();
        
        NetworkMessage *msg = new NetworkMessage(PSHARE_CONN_REQ, f);

        /* send terminality */
        msg->write((uint8_t)(terminal ? 1 : 0));
        msg->write((uint16_t)tcp_port);
        msg->write((uint32_t)key.length());
        fwrite(&key[0], sizeof(char), key.length(), f);
        
        bool result = msg->send(ancestry[0]->get_sd());
        delete msg;
        
        if (!result) {

            out(3) << "unable to send handshake request" << std::endl;
            return false;
        }

        /* ------------ receive handshake response --------------------- */
        msg = new NetworkMessage();
        result = msg->recv(ancestry[0]->get_sd());

        if (!result) {

            out(3) << "lost connection to parent without handshake response"
            << std::endl;

            delete msg;
            return false;
        }

        /* if we sent a bad key */
        if (msg->get_header() == PSHARE_CONN_BAD) {

            out(3) << "parent refused connection: bad key" << std::endl;
            return false;
        }

        /* good key, received reply */
        else if (msg->get_header() == PSHARE_CONN_REP) {

            uint16_t ancestry_size = msg->read_uint16();
            uint16_t sibling_size  = msg->read_uint16();
            out() << "received " << ancestry_size << " ancestors and "
            << sibling_size << " siblings from parents" << std::endl;

            /* set parent generation, our generation is parent + 1 */
            uint16_t parent_generation = msg->read_uint16();
            ancestry[0]->set_generation(parent_generation);
            generation     = parent_generation + 1;
            sibling_number = msg->read_uint16();

            /* read ancestry and insert into vector */
            for (uint16_t i = 0; i < ancestry_size; i++) {

                struct sockaddr_in addr;
                addr.sin_family = AF_INET;
                addr.sin_addr   = {msg->read_uint32()};
                addr.sin_port   = htons(msg->read_uint16());
                
                Node *node = new Node(-1, addr, network_queue);
                ancestry.push_back(node);
                node->set_tcp_port(ntohs(addr.sin_port));

                out() << "  +ancestor " << node->get_ineta() << ":" <<
                ntohs(addr.sin_port) << std::endl;
            }

            /* read siblings and insert into the list */
            for (uint16_t i = 0; i < sibling_size; i++) {

                struct sockaddr_in addr;
                addr.sin_family = AF_INET;
                addr.sin_addr   = {msg->read_uint32()};

                uint16_t p      = msg->read_uint16();
                addr.sin_port   = htons(p);

                Node *node = new Node(-1, addr, network_queue);
                node->set_sibling_number(msg->read_uint16());
                node->set_tcp_port(p);
                siblings.push_back(node);
                
                out() << "  +sibling " << node->get_sibling_number() << " at "
                << node->get_ineta() << ":" << ntohs(addr.sin_port)
                << std::endl;
            }
        }

        /* parent not paying attention, unexpected header */
        else {

            out(3) << "parent sent unexpected message" << std::endl;
            return false;
        }

        out() << "received local topology from parent" << std::endl;

        delete msg;

        /* start listening for incoming messages on separate thread */
        std::thread t(&Node::listen, ancestry[0]);
        t.detach();
    }
    
    /* if failed, return false */
    else {

        out(3) << "failed to connect to parent node" << std::endl;
        return false;
    }

    return true;
}

bool NetworkStructure::start() {

    /* if there are any parent nodes, connect to it */
    if (ancestry.size() > 0)
        if (!handshake())
            return false;

    /* start tcp listener if not terminator node */
    if (!terminal) {
        
        tcp_listener = new TcpListener(network_queue, tcp_port);

        if (!tcp_listener->start()) {

            out(3) << "unable to bind port " << tcp_port << std::endl;
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

            out() << "node at " << node->get_ineta()
            << " connected" << std::endl;

            std::thread t(&Node::listen, node);
            t.detach();

            num_conn++;
        }

        /* a node disconnected */
        else if (e.get_flag() == Event::NODE_DISCONNECT) {

            Node *node = static_cast<Node *>(e.get_data());

            /* if this is the parent node, commence a parent change. */
            if (ancestry.size() > 0 && node == ancestry[0]) {

                // TODO: parent change
                if (!parent_change()) {

                    out(3) << "parent change failed!" << std::endl;
                    exit(-1);
                }
            }

            /* if this is child, remove from list */
            else {

                children.remove(node);

                /* notify children that they lost a sibling */
                if (!node->is_terminal()) {
                    
                    NetworkMessage m(PSHARE_SIBLING_RMV, tmpfile());
                    m.write((uint16_t)node->get_sibling_number());

                    std::list<Node *>::iterator i;
                    for (i = children.begin(); i != children.end(); ++i)
                        m.send((*i)->get_sd());
                }

                out() << "node at " << node->get_ineta()
                << " disconnected" << std::endl;

                delete node;
            }
        }

        else if (e.get_flag() == Event::NODE_MSG_RECEIVED) {

            /* get sender and message */
            Node *sender = static_cast<Node *>(&e.get_registrar());
            NetworkMessage *msg = static_cast<NetworkMessage *>(e.get_data());

            if (msg->get_header() == PSHARE_CONN_REQ) {

                /* get terminality and tcp listening port */
                sender->set_terminal(msg->read_uint8() ? true : false);
                sender->set_tcp_port(msg->read_uint16());
                
                /* get the key length */
                uint32_t keylen = msg->read_uint32();

                /* get key sent by node */
                std::string r_key(keylen, 0);
                fread(&r_key[0], sizeof(char), keylen, msg->get_payload());

                /* if key is invalid, send a mean response and drop conn */
                if (r_key != key) {

                    NetworkMessage mean_msg(PSHARE_CONN_BAD, tmpfile());
                    mean_msg.send(sender->get_sd());
                    sender->close();

                    out() << "received bad key from " << sender->get_ineta()
                    << ", dropping connection" << std::endl;
                }

                /* key is valid, send network topology */
                else {
                    
                    NetworkMessage topology_msg(PSHARE_CONN_REP, tmpfile());

                    topology_msg.write((uint16_t)ancestry.size());
                    topology_msg.write((uint16_t)children.size());
                    topology_msg.write((uint16_t)generation);

                    sender->set_sibling_number(get_next_sibling_number());
                    topology_msg.write((uint16_t)sender->get_sibling_number());

                    for (uint16_t i = 0; i < ancestry.size(); i++) {

                        struct sockaddr_in addr = ancestry[i]->get_addr();

                        topology_msg.write((uint32_t)addr.sin_addr.s_addr);
                        topology_msg.write((uint16_t)ntohs(addr.sin_port));
                    }

                    std::list<Node *>::iterator i;
                    for (i = children.begin(); i != children.end(); ++i) {

                        struct sockaddr_in addr = (*i)->get_addr();

                        topology_msg.write((uint32_t)addr.sin_addr.s_addr);
                        topology_msg.write((uint16_t)((*i)->get_tcp_port()));
                        topology_msg.write(
                            (uint16_t)((*i)->get_sibling_number())
                        );
                    }

                    /* if msg is sent successfully */
                    if (topology_msg.send(sender->get_sd())) {

                        /* topology was sent, report change to children and
                         * add this node as a new child */
                        if (!sender->is_terminal()) {

                            NetworkMessage s_msg(PSHARE_SIBLING_ADD, tmpfile());
                            s_msg.write((uint32_t)sender->get_addr().sin_addr.s_addr);
                            s_msg.write((uint16_t)sender->get_tcp_port());
                            s_msg.write((uint16_t)sender->get_sibling_number());

                            std::list<Node *>::iterator i;
                            for (i = children.begin(); i != children.end(); ++i)
                                s_msg.send((*i)->get_sd());
                        }

                        children.push_back(sender);
                    }
                }
            }

            else if (msg->get_header() == PSHARE_SIBLING_ADD) {

                /* new sibling in network, add it */
                struct sockaddr_in addr;
                addr.sin_family = AF_INET;
                addr.sin_addr   = {msg->read_uint32()};

                uint16_t p      = msg->read_uint16();
                addr.sin_port   = htons(p);
                
                Node *node = new Node(-1, addr, network_queue);
                node->set_sibling_number(msg->read_uint16());
                node->set_tcp_port(p);
                siblings.push_back(node);

                out() << "  +sibling " << node->get_sibling_number() << " at "
                << node->get_ineta() << ":" << ntohs(addr.sin_port)
                << std::endl;
            }

            else if (msg->get_header() == PSHARE_SIBLING_RMV) {
                out() << "sibling remove received" << std::endl;
                uint16_t sibling = msg->read_uint16();

                std::list<Node *>::iterator i;
                for (i = siblings.begin(); i != siblings.end(); ++i) {
                    
                    if ((*i)->get_sibling_number() == sibling) {

                        out() << "  -sibling " << (*i)->get_sibling_number()
                        << " at " << (*i)->get_ineta() << ":"
                        << (*i)->get_tcp_port() << std::endl;

                        i = siblings.erase(i);
                        break;
                    }
                }
            }

            else if (msg->get_header() == PSHARE_NET_TOP) {

                clear_lists(false, true, false);

                /* remove all except the position 0 (parent) */
                while (ancestry.size() > 1) {

                    delete ancestry[ancestry.size() - 1];
                    ancestry.pop_back();
                }

                uint16_t ancestry_size = msg->read_uint16();
                uint16_t sibling_size  = msg->read_uint16();
                out() << "received " << ancestry_size << " ancestors and "
                << sibling_size << " siblings from parents" << std::endl;

                /* set parent generation, our generation is parent + 1 */
                uint16_t parent_generation = msg->read_uint16();
                ancestry[0]->set_generation(parent_generation);
                generation     = parent_generation + 1;
                sibling_number = msg->read_uint16();

                /* read ancestry and insert into vector */
                for (uint16_t i = 0; i < ancestry_size; i++) {

                    struct sockaddr_in addr;
                    addr.sin_family = AF_INET;
                    addr.sin_addr   = {msg->read_uint32()};
                    addr.sin_port   = htons(msg->read_uint16());
                    
                    Node *node = new Node(-1, addr, network_queue);
                    ancestry.push_back(node);

                    out() << "  +ancestor " << node->get_ineta() << ":" <<
                    ntohs(addr.sin_port) << std::endl;
                }

                /* read siblings and insert into the list */
                for (uint16_t i = 0; i < sibling_size; i++) {

                    struct sockaddr_in addr;
                    addr.sin_family = AF_INET;
                    addr.sin_addr   = {msg->read_uint32()};

                    uint16_t p      = msg->read_uint16();
                    addr.sin_port   = htons(p);

                    Node *node = new Node(-1, addr, network_queue);
                    node->set_sibling_number(msg->read_uint16());
                    node->set_tcp_port(p);
                    siblings.push_back(node);
                    
                    out() << "  +sibling " << node->get_sibling_number() << " at "
                    << node->get_ineta() << ":" << ntohs(addr.sin_port)
                    << std::endl;
                }

                // relay info to children
                send_topology_to_children();
            }

            delete msg;
        }
    }
}

bool NetworkStructure::parent_change() {

    out() << "initiating parent change" << std::endl;

    Node *np         = nullptr; // new parent
    bool this_parent = false; // indicates that this process is parent

    /* go through siblings to determine the sibling with the lowest sibling
     * number */

    out() << "looking for potential parents" << std::endl;
    std::list<Node *>::iterator i;
    for (i = siblings.begin(); i != siblings.end(); ++i) {

        unsigned short sibnum = (*i)->get_sibling_number();

        /* 1 is the lowest possible sibling number */
        if (sibnum == 1) {

            np = (*i);
            break;
        }

        /* if new parent is null, set it initially or if current sibling
         * has a lower number, set it as new candidate parent*/
        else if (np == nullptr || sibnum < np->get_sibling_number()) {

            np = (*i);
        }
    }
    
    /* check if this node is lower than other siblings */
    if (np == nullptr || sibling_number < np->get_sibling_number()) {

        out() << "i have the lowest sibling number" << std::endl;
        this_parent = true;
        np = nullptr;
    }

    /* this is the only sibling, connect to an ancestor */
    if (this_parent) {

        std::vector<Node *> temp = ancestry;
        ancestry.clear();

        std::list<Node *>::iterator i;
        for (i = siblings.begin(); i != siblings.end(); ++i)
            if (*i != np)
                delete *i;
        siblings.clear();

        for (unsigned int i = 1; i < temp.size(); i++) {

            ancestry.push_back(temp[i]);

            if (!handshake()) {
                
                ancestry.pop_back();
                //delete temp[i];
                //temp.pop_back();
            }

            else {

                /*while (temp.size() > 0) {

                    delete temp[temp.size() - 1];
                    temp.pop_back();
                }*/
                
                break;
            }
        }

        if (ancestry.size() == 0) {
            
            out() << "could not connect to any ancestor" << std::endl;

            if (terminal) {
                
                out(4) << "I cannot act as root: terminal node" << std::endl;
                return false;
            }

            out() << "i am the new root!" << std::endl;

            generation = 0;
            sibling_number = 0;
        }
    }

    /* if this is not the new parent, connect to the new parent */
    else {

        out() << "a new parent was found" << std::endl;

        /* clear siblings except new parent */
        std::list<Node *>::iterator i;
        for (i = siblings.begin(); i != siblings.end(); ++i)
            if (*i != np)
                delete *i;
        siblings.clear();

        /* clear ancestors except new parent */
        while (ancestry.size() > 0) {

            if (ancestry[ancestry.size() - 1] != np)
                delete ancestry[ancestry.size() - 1];
            ancestry.pop_back();
        }

        /* add the new parent */
        ancestry.push_back(np);

        if (!handshake()) {

            out(3) << "could not connect to new parent" << std::endl;
            return false;
        }

        out() << "connected to new parent" << std::endl;
    }

    /* relay new topology to children */

    out() << "sibling count: " << siblings.size() << std::endl;
    send_topology_to_children();
    
    out() << "parent change succeeded!" << std::endl;

    return true;
}

void NetworkStructure::send_topology_to_children() {

    out() << "relaying topology to children" << std::endl;
    
    std::list<Node *>::iterator i;
    for (i = children.begin(); i != children.end(); ++i) {

        NetworkMessage topology_msg(PSHARE_NET_TOP, tmpfile());

        topology_msg.write((uint16_t)ancestry.size());
        if (children.size() > 0)
            topology_msg.write((uint16_t)(children.size() - 1));
        else
            topology_msg.write((uint16_t)0);
        topology_msg.write((uint16_t)generation);

        topology_msg.write((uint16_t)(*i)->get_sibling_number());

        for (uint16_t m = 0; m < ancestry.size(); m++) {

            struct sockaddr_in addr = ancestry[m]->get_addr();

            topology_msg.write((uint32_t)addr.sin_addr.s_addr);
            topology_msg.write((uint16_t)ntohs(addr.sin_port));
        }

        std::list<Node *>::iterator j;
        for (j = children.begin(); j != children.end(); ++j) {

            if (*i == *j)
                continue; /* skip children self */

            struct sockaddr_in addr = (*j)->get_addr();

            topology_msg.write((uint32_t)addr.sin_addr.s_addr);
            topology_msg.write((uint16_t)((*j)->get_tcp_port()));
            topology_msg.write(
                (uint16_t)((*j)->get_sibling_number())
            );
        }

        topology_msg.send((*i)->get_sd());
    }
}

void NetworkStructure::clear_lists(bool a, bool s, bool c) {
    
    out() << "clearing all siblings" << std::endl;
    if (s) {

        std::list<Node *>::iterator i;
        for (i = siblings.begin(); i != siblings.end(); ++i)
            delete *i;
        siblings.clear();
    }
    
    out() << "clearing all children" << std::endl;
    if (c) {

        std::list<Node *>::iterator i;
        for (i = children.begin(); i != children.end(); ++i)
            delete *i;
        children.clear();
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
