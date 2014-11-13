/*******************************************************************************
 * File        : NetworkMessage.h
 * Description : Class that serializes network messages.
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

#ifndef NETWORKMESSAGE_H
#define NETWORKMESSAGE_H

#define BUFFER_SIZE 1024

#include <cstdio>
#include <cstdint>

/**
 * @class   NetworkMessage
 *
 * @brief   Class that manages and serializes network messages.
 *
 * @details This class handles network message-related operations. It can
 *          serialize messages into payloads.
 */
class NetworkMessage {

    public:
        /**
         * @brief Constructs an empty NetworkMessage.
         *
         */
        NetworkMessage();
         
        /**
         * @brief Constructs a NetworkMessage from header and payload.
         *
         * @param[in]   header      The header of this message.
         * @param[in]   payload     The payload.
         */
        NetworkMessage(uint8_t header, FILE *payload);

        /**
         * @brief Destructor.
         *
         * This destructor *will* perform a `close()` on `payload`.
         */
        ~NetworkMessage();

        /**
         * @brief Retrieves the header of the message.
         *
         * @return The header as `uint8_t`.
         */
        uint8_t get_header() const;

        /**
         * @brief Calculates the payload size.
         *
         * @return The payload size as `uint32_t`.
         */
        uint32_t get_payload_size() const;

        /**
         * @brief Gets the payload.
         *
         * @return File descriptor to the payload.
         */
        FILE* get_payload();

        /**
         * @brief Sends the network message.
         *
         * @param[in]   sd      The socket descriptor which will be used to
         *                      send the message.
         */
        bool send(int sd);

        /**
         * @brief Receives a network message.
         *
         * This function will block execution on running thread until a full,
         * properly formatted message is received.
         *
         * @param[in]   sd      The socket descriptor from which the message
         *                      will be read.
         */
        bool recv(int sd);

        /**
         * @brief Inserts a 8-bit number to the payload.
         *
         * File pointer is moved forward by 1 bytes.
         *
         * @param[in]   n   A `uint8_t` that will be inserted into the payload.
         */
        void write(uint8_t n);

        /**
         * @brief Inserts a 32-bit number to the payload.
         *
         * The number is converted to network byte order. File pointer is moved
         * forward by 4 bytes.
         *
         * @param[in]   n   A `uint32_t` that will be inserted into the payload.
         */
        void write(uint32_t n);

        /**
         * @brief Inserts a 16-bit number to the payload.
         *
         * The number is converted to network byte order. File pointer is moved
         * forward by 2 bytes.
         *
         * @param[in]   n   A `uint16_t` that will be inserted into the payload.
         */
        void write(uint16_t n);

        /**
         * @brief Retrieves a 32-bit number from the payload.
         *
         * The number is converted to host byte order. File read pointer is
         * moved forward by 4 bytes.
         *
         * @return A `uint32_t` converted to host byte order.
         */
        uint32_t read_uint32();

        /**
         * @brief Retrieves a 16-bit number from the payload.
         *
         * The number is converted to host byte order. File read pointer is
         * moved forward by 2 bytes.
         *
         * @return A `uint16_t` converted to host byte order.
         */
        uint16_t read_uint16();

        /**
         * @brief Retrieves a 8-bit number from the payload.
         *
         * File read pointer is moved forward by 1 bytes.
         *
         * @return A `uint8_t` converted to host byte order.
         */
        uint8_t read_uint8();

    public:
        /// The header of this message.
        uint8_t header;

        /// The message that this NetworkMessage carries.
        FILE *payload;
        
};

#endif
