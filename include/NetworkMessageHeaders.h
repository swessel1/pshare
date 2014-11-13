/*******************************************************************************
 * File        : NetworkMessageHeaders.h
 * Description : Macro definitions for NetworkMessage headers.
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

#ifndef NETWORKMESSAGEHEADERS_H
#define NETWORKMESSAGEHEADERS_H

/* HEADER:
 *      PSHARE_CONN_REQ
 *
 * DESCRIPTION:
 *      A handshake request. Key is not a null-terminated string.
 *
 * PAYLOAD:
 *      1 byte : terminality
 *      2 bytes: tcp port (0 if terminal);
 *      4 bytes: key size, n (uint32_t)
 *      n bytes: key
 */
#define PSHARE_CONN_REQ     0x01

/* HEADER:
 *      PSHARE_CONN_REP
 *      PSHARE_NET_TOP
 *
 * DESCRIPTION:
 *      Used in two conditions: local network topology information and handshake
 *      response.
 * 
 *      A handshake response from the parent. Payload includes ancestry and
 *      siblings (for connecting node). Terminal siblings are not included in
 *      the transmitted list. Ancestry does not include the sender.
 * 
 * PAYLOAD:
 *      2 bytes: ancestry vector size, n (uint16_t)
 *      2 bytes: sibling vector size, m (uint16_t)
 *      2 bytes: parent generation (uint16_t)
 *      2 bytes: assigned sibling number (uint16_t)
 *
 *      ancestry vector (n elements):
 *          4 bytes: internet address (uint32_t)
 *          2 bytes: listening port (uint16_t)
 *
 *      sibling vector (m elements):
 *          4 bytes: internet address (uint32_t)
 *          2 bytes: listening port (uint16_t)
 *          2 bytes: sibling number (uint16_t)
 */
#define PSHARE_CONN_REP     0x02
#define PSHARE_NET_TOP      0x02

/* HEADER:
 *      PSHARE_CONN_BAD
 *
 * DESCRIPTION:
 *      A bad key was sent, connection is dropped by parent after sending this
 *      message.
 *
 * PAYLOAD:
 *      null
 */
#define PSHARE_CONN_BAD     0x03

#endif
