/*******************************************************************************
 * File        : Node.h
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

#ifndef NODE_H
#define NODE_H

#include <string>
#include <mutex>
#include <netinet/in.h>
#include "EventRegistrar.h"
#include "NetworkMessage.h"

/**
 * @class   Node
 *
 * @brief   Represents a node in the network.
 *
 * @details This class represents a single node in the network without any
 *          information on the relationship between the nodes.
 */
class Node : public EventRegistrar {

    public:
        /**
         * @brief Unknown node constructor.
         *
         * Constructs a node for which the branch coordinates (generation and
         * sibling number) are unknown.
         *
         * @param[in]   sd      The socket descriptor for the node. Set to zero
         *                      if connection is not already open.
         * @param[in]   addr    The address structure of the node.
         * @param[in]   queue   The event queue for this registrar.
         *
         */
        Node(int sd, struct sockaddr_in addr, BlockingQueue<Event> &queue);

        /**
         * @brief Known node constructor.
         *
         * Constructs a node for which both branch coordinates and connection
         * information are known.
         *
         * @param[in]   generation      The generation identifier of the node.
         *                              This is the distance of the node from
         *                              the network root node.
         * @param[in]   sibling_number  The sibling number of the node which
         *                              indicates the seniority of this node
         *                              among the siblings.
         * @param[in]   sd              The socket descriptor for the node. Set
         *                              to zero if connection is not already
         *                              open.
         * @param[in]   addr            The address structure of the node.
         * @param[in]   queue           The event queue for this registrar.
         */
        Node(unsigned short generation,
             unsigned short sibling_number,
             int sd,
             struct sockaddr_in addr,
             BlockingQueue<Event> &queue);

        /**
         * @brief Destructor.
         *
         * Cleans up and closes the socket on the node.
         */
        ~Node();

        /**
         * @brief Opens a connection to the node.
         *
         * @return Returns `true` if connection is opened successfully, `false`
         *         otherwise.
         */
        bool open();

        /**
         * @brief Closes the connection to the node.
         *
         * Ends the connection with the node by closing the socket descriptor.
         */
        void close();

        /**
         * @brief Sends a message to the node.
         *
         * @param[in]   msg     Reference to the message that will be sent.
         *
         * @return Returns `true` if the message was sent successfully, `false`
         *         otherwise.
         */
        bool send(NetworkMessage& msg);

        /**
         * @brief Starts listening for incoming messages from the node.
         *
         * This function will return if the socket descriptor `sd` value is `0`.
         * Unless started on a new thread, this function will block execution of
         * the current thread. If listen fails (e.g. remote disconnect) the
         * socket descriptor will set to `0` and an event will be registered.
         */
        void listen();

        /**
         * @brief Sets the generation identifier of the node.
         *
         * @param[in]   generation      The generation identifier of the node.
         *
         * @return Reference to this instance of the node (to enable cascading).
         */
        Node& set_generation(unsigned short generation);

        /**
         * @brief Sets the sibling number of the node.
         *
         * @param[in]   sibling_number  The sibling number of the node.
         *
         * @return Reference to this instance of the node (to enable cascading).
         */
        Node& set_sibling_number(unsigned short sibling_number);

        /**
         * @brief Gets the generation identifier of the node.
         *
         * @return The generation identifier of the node as `unsigned short`.
         */
        unsigned short get_generation() const;

        /**
         * @brief Gets the sibing number of the node.
         *
         * @return The sibling number of the node as `unsigned short`.
         */
        unsigned short get_sibling_number() const;

        /**
         * @brief Gets the address structure of this node.
         *
         * @return Reference to the address structure of this node of type
         *         `struct sockaddr_in`.
         */
        struct sockaddr_in& get_addr();

        /**
         * @brief Gets the internet address of the node.
         *
         * @return A human-readable representation of this node's internet
         *         address as `std::string`.
         */
        std::string get_ineta() const;

        /**
         * @brief Gets the socket descriptor of the node.
         *
         * @return Socket descriptor as `int`.
         */
        int get_sd() const;

        unsigned short get_tcp_port() const;

        bool is_terminal() const;

        void set_terminal(bool terminality);

        void set_tcp_port(unsigned short port);

    private:
        /// The generation of the node, essentially the distance from root node.
        unsigned short generation;

        /// The rank among siblings.
        unsigned short sibling_number;

        /// Socket descriptor for the node.
        int sd;

        /// Address structure of the node.
        struct sockaddr_in addr;

        /// Indicates terminality of the node.
        bool terminal;

        /// If this is not a terminal node, the TCP port is listens to.
        unsigned short tcp_port;
};

#endif
