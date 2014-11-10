/*******************************************************************************
 * File        : NetworkMessage.cpp
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

#include <cstdio>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/sendfile.h>
#include <unistd.h>
#include "NetworkMessage.h"

NetworkMessage::NetworkMessage() { }

NetworkMessage::NetworkMessage(uint8_t header, FILE *payload) :
    header(header), payload(payload) { }

NetworkMessage::~NetworkMessage() {

    if (payload != NULL)
        fclose(payload);
}

uint8_t NetworkMessage::get_header() const {

    return header;
}

FILE* NetworkMessage::get_payload() {

    return payload;
}

uint32_t NetworkMessage::get_message_size() const {

    uint32_t payload_size = 0;

    if (payload != NULL) {
        
        long int current_position = ftell(payload);
        payload_size = fseek(payload, 0, SEEK_END);
        fseek(payload, current_position, SEEK_SET);
    }
    
    /* header (1) + payload_size (4) + size of payload (?) */
    return 5 + payload_size;
}

bool NetworkMessage::send(int sd) {

    uint32_t payload_left = get_message_size();
    uint32_t msg_size     = htonl(payload_left);

    if (::send(sd, &header, sizeof(uint8_t), 0) < 0)
        return false;

    if (::send(sd, &msg_size, sizeof(uint32_t), 0) < 0)
        return false;

    if (payload != NULL) {

        off_t offset = 0;
        int send_len = 0;
        int fd       = fileno(payload);

        while (payload_left > 0) {

            send_len = sendfile(sd, fd, &offset, 65535);

            if (send_len < 0)
                return false;

            payload_left -= send_len;
        }
    }

    return true;
}

bool NetworkMessage::recv(int sd) {

    int recv_len;
    uint32_t payload_size;
    char buffer[BUFFER_SIZE];

    /* receive header */
    if (::recv(sd, &header, sizeof(uint8_t), 0) <= 0)
        return false;

    /* receive message size */
    if (::recv(sd, &payload_size, sizeof(uint32_t), 0) <= 0)
        return false;

    payload_size = ntohl(payload_size);

    if (payload_size > 0) {

        payload = tmpfile();

        while (payload_size > 0) {

            recv_len = ::recv(sd, buffer, BUFFER_SIZE, 0);

            if (recv_len <= 0) {

                /* connection was terminated */
                fclose(payload);
                payload = NULL;
                return false;
            }

            fwrite(buffer, 1, recv_len, payload);

            payload_size -= recv_len;
        }
    }

    return true;
}
