/*******************************************************************************
 * File        : TcpListener.h
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

#ifndef TCPLISTENER_H
#define TCPLISTENER_H

#include <sys/socket.h>
#include "EventRegistrar.h" 

/**
 * @class   TcpListener
 * 
 * @brief   Listens for incoming connections.
 *
 * @details A TcpListener listens for incoming connections and creates
 *          Connection objects for each new connection. Finally, the TcpListener
 *          registers an event and makes the Connection object accessible.
 */
class TcpListener : public EventRegistrar {

    public:
        /**
         * @brief Constructor.
         *
         * Prepares the TcpListener but does not start listening for incoming
         * connection.
         *
         * @param[in]   port        The port that the TcpListener will bind to
         *                          for incoming connections.
         * @param[in]   event_queue Reference to an event queue.
         */
        TcpListener(BlockingQueue<Event> &event_queue, unsigned short port);

        /**
         * @brief Destructor.
         *
         * Closes the socket descriptor and frees all resources.
         */
        ~TcpListener();

        /**
         * @brief Starts the TcpListener.
         * 
         * Starts listening for incoming connections on a separate thread. A
         * Connection object is created for all new connections.
         *
         * @return Returns `true` is TcpListener was started successfully,
         *         `false` otherwise.
         */
        bool start();

        /**
         * @brief Stops the TcpListener.
         *
         * Stops listening by closing the socket.
         */
        void stop();

    private:
        /**
         * @brief Starts listening for incoming connections.
         *
         * This function will block execution of the thread unless it is
         * executed on a separate thread. New connections that are captured
         * by the listener will be available through registered events.
         */
        void listen();

        /**
         * @brief Binds to the socket.
         *
         * If binding is unsuccessful, a TcpListenerEvent is registered.
         *
         * @return Returns `true` if bind is successful, `false` otherwise.
         */
        bool bind();
    
        /// The socket descriptor of the listener.
        int sd;

        /// The address structure of the listener.
        struct sockaddr_in addr;
        
};

#endif
