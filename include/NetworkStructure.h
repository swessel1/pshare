/*******************************************************************************
 * File        : NetworkStructure.h
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

#ifndef NETWORKSTRUCTURE_H
#define NETWORKSTRUCTURE_H

#include <vector>
#include <list>
#include <forward_list>
#include <mutex>
#include <netinet/in.h>
#include "Node.h"
#include "TcpListener.h"

/**
 * @class   NetworkStructure
 *
 * @brief   Represents the structure of the network.
 *
 * @details This class represents the network structure with respect to the
 *          owning node (i.e. the current process).
 */
class NetworkStructure : public EventRegistrar {

    public:
        /**
         * @brief Constructor for root node.
         *
         * Constructs a network structure with no defined parents.
         *
         * @param[in]   queue   The event queue for this registrar.
         */
        NetworkStructure(BlockingQueue<Event> &queue);

        /**
         * @brief Constructor for child node.
         *
         * Constructs a network structure with a parent node.
         *
         * @param[in]   parent_addr     Parent node address.
         * @param[in]   term            Indicates whether this node is a
         *                              terminal or not. A terminal node is
         *                              one that does not accept and child nodes
         *                              (i.e. receive only). Terminal nodes
         *                              are not considered as candidates during
         *                              parent change.
         * @param[in]   queue           The event queue for this registrar.
         *
         */
        NetworkStructure(struct sockaddr_in parent_addr,
                         bool term,
                         BlockingQueue<Event> &queue);

        /**
         * @brief Destructor.
         *
         * Releases all system resources. Frees all Node objects in `ancestry`,
         * `siblings` and `children` lists. Closes TcpListener (if open).
         */
        ~NetworkStructure();

        /**
         * @brief Starts the networking.
         *
         * Attempts to connect with the parent node (if any). If the node is not
         * a terminator, a TcpListener will be started to listen for incoming
         * connections.
         */
        bool start();

        /**
         * @brief Indicates whether node is the parent of current network.
         *
         * @param[in]   node    The node that will be compared with the parent
         *                      node of this network structure.
         *
         * @return `true` if `node` is a parent of this network structure,
         *         `false` otherwise.
         */
        bool is_parent(Node *node);

        /**
         * @brief Constructs a list of nodes to relay messages to.
         *
         * Determines which nodes (from parent and children) to relay the
         * messages to, excluding the originator. If the originator is `NULL`
         * pointer, the resulting linked-list will contain all children plus
         * the parent.
         *
         * @param[in]   originator  The originator of the initial message. This
         *                          node will be excluded from the resulting
         *                          list if not `NULL`.
         *
         * @return A list of nodes to relay the messages to.
         */
        std::forward_list<Node *> send_list(Node *originator);

        /**
         * @brief Gets the network queue.
         *
         * The network queue is handled by this network structure.
         *
         * @return Reference to a `BlockingQueue<Event>` object that holds the
         *         queue of events for this network.
         */
        BlockingQueue<Event>& get_network_queue();

        /**
         * @brief Sets the port for the TCP listener.
         *
         * @param[in]   port    The port for the TCP listener.
         */
        void set_tcp_port(unsigned short port);

        /**
         * @brief Sets maximum number of connections accepted by this node.
         *
         * @param[in]   max     Maximum number of connections.
         */
        void set_max_conn(unsigned short max);

        /**
         * @brief Sets the key required for the network.
         *
         * @param[in]   key     The key that is required to connect to the
         *                      network. The key is set by the root node.
         */
        void set_key(std::string key);

        /**
         * @brief Sets the directory to share.
         *
         * @param[in]   dir     The directory the node is sharing.
         */
        void set_dir(std::string dir);

        /**
         * @brief Retrieves the next available sibling number.
         *
         * @return The next sibling number.
         */
        unsigned short get_next_sibling_number();

    private:
        /**
         * @brief Handles event queue for the network.
         *
         * This function will block execution of the thread.
         */
        void control();

        /**
         * @brief Initiates handshake with a parent.
         *
         * Connection to parent is opened by this function.
         *
         * @returns `true` if handshake was successful, `false` otherwise.
         */
        bool handshake();

        /**
         * @brief Executes a parent change.
         *
         * Function should be called only if connection to parent is
         * severed.
         *
         * @returns `true` if parent change was successful, `false` otherwise.
         */
        bool parent_change();

        /**
         * @brief Clears selected lists.
         *
         * Frees any Node pointers remaining in the lists.
         *
         * @param[in]   a   Clears ancestry list.
         * @param[in]   s   Clears sibling list.
         * @param[in]   c   Clears children list.
         */
        void clear_lists(bool a, bool s, bool c);

        /**
         * @brief Sends a topological information to children
         */
        void send_topology_to_children();
        
        /// The event queue that is handled by the network structure.
        BlockingQueue<Event> network_queue;
    
        /// The generation identifier of this network node.
        unsigned short generation = 0;

        /// The sibling number of this network node.
        unsigned short sibling_number;

        /// Keeps track of sibling numbers that were 
        unsigned short next_sibling_number = 1;

        /// Locks the instance for thread safety.
        std::mutex thread_safety;
        
        /// Indicates whether current node is a terminator or not.
        bool terminal;

        /// An array of ancestors. 0th index is always parent.
        std::vector<Node *> ancestry;

        /// A list of siblings.
        std::list<Node *> siblings;

        /// A list of children.
        std::list<Node *> children;

        /// The TcpListener that was initiated (or not) by this network.
        TcpListener *tcp_listener;

        /// The port that the TcpListener will bind to.
        unsigned short tcp_port = 26005;

        /// The number of current child connections.
        unsigned short num_conn = 0;

        /// Maximum number of connections.
        unsigned short max_conn = 10;

        /// The directory we're sharing.
        std::string dir;

        /// The key that is used to communicate between nodes.
        std::string key;

};

#endif
