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
         * @brief Constructs a NetworkMessage from header and message.
         *
         * @param[in]   header      The header of this message.
         * @param[in]   message     The message.
         */
        NetworkMessage(uint8_t header, FILE* message);

        /**
         * @brief Destructor.
         *
         * This destructor *will* perform a `close()` on `message`.
         */
        ~NetworkMessage();

        /**
         * @brief Retrieves the header of the message.
         *
         * @return The header as `uint8_t`.
         */
        uint8_t get_header() const;

        /**
         * @brief Calculates the payload size of this message.
         *
         * @return The payload size (include 1 byte header + 4 bytes of message
         *         size, as `uint32_t`.
         */
        uint32_t get_payload_size() const;

        /**
         * @brief Retrieves the message.
         *
         * @return File descriptor to the message.
         */
        FILE* get_message() const;

        /**
         * @brief Retrieves the full payload.
         *
         * Full payload includes a header of 1 byte, a message size of 4 bytes
         * followed by the message.
         *
         * @return A file descriptor to a temporary file that contains the full
         *         message.
         */
        FILE* get_payload() const;

    public:
        /// The header of this message.
        uint8_t header;

        /// The message that this NetworkMessage carries.
        FILE* message;
        
};

#endif
