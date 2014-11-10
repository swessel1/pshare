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

#include <forward_list>
#include <vector>
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
         * @param[in]   node    Pointer to the parent node of this network
         *                      structure.
         * @param[in]   term    Indicates whether this node is a terminator
         *                      or not. A terminator node is one that does
         *                      not accept and child nodes (i.e. receive
         *                      only). Terminator nodes are not considered
         *                      as candidates during parent change.
         * @param[in]   queue   The event queue for this registrar.
         *
         */
        NetworkStructure(Node *node, bool term, BlockingQueue<Event> &queue);

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
         * Attempts to connect with the parent node (if any) and initiates a
         * handshake. As a result of the handshake, generation, sibling number,
         * ancestry and siblings are determined. All network related operations
         * are now controlled by the network structure.
         *
         * If the node is not a terminator, a TcpListener will be started to
         * listen for incoming connections.
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

    private:
        /// The event queue that is handled by the network structure.
        BlockingQueue<Event> network_queue;
    
        /// The generation identifier of this network node.
        unsigned short generation;

        /// The sibling number of this network node.
        unsigned short sibling_number;

        /// Keeps track of sibling numbers that were 
        static unsigned short next_sibling_number;

        /// Locks the instance for thread safety.
        std::mutex thread_safety;
        
        /// Indicates whether current node is a terminator or not.
        bool terminator;

        /// An array of ancestors. 0th index is always parent.
        std::vector<Node *> ancestry;

        /// A list of siblings.
        std::forward_list<Node *> siblings;

        /// A list of children.
        std::forward_list<Node *> children;

        /// The TcpListener that was initiated (or not) by this network.
        TcpListener *tcp_listener;
        
};
