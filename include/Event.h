/*******************************************************************************
 * File        : Event.h
 * Description : The base class for all events.
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

#ifndef EVENT_H
#define EVENT_H

#include <string>

class EventRegistrar;

/**
 * @class   Event
 *
 * @brief   The base class for all events.
 *
 * @details This class forms the basis for all events that can be registered by
 *          the EventRegistrar instances.
 */
class Event {

    public:
        /**
         * @brief Flags that indicate the nature of the event.
         */
        enum Flags {
            NODE_DISCONNECT,
            NODE_MSG_RECEIVED,
            TCP_INC_CONNECTION,
            TCP_LISTEN_FAIL
        };
        
        /**
         * @brief Constructor.
         *
         * @param[in]   registrar   The registrar (originator) of this event.
         * @param[in]   data        The data carried by the event.
         * @param[in]   flag        Flag that indicates the nature of this
         *                          event.
         */
        Event(EventRegistrar &registrar, void *data, Event::Flags flag);

        /**
         * @brief Returns the registrar (originator) of this event.
         *
         * @return Reference to the registrar of this event.
         */
        EventRegistrar& get_registrar() const;

        /**
         * @brief Gets the flag of the event.
         *
         * @return An `Event::Flag` that indicates the nature of this event.
         */
        Event::Flags get_flag() const;

        /**
         * @brief Gets the data carried by the event.
         *
         * @return `void` pointer to the data.
         */
        void* get_data();

    private:
        /// The registrar of this event.
        EventRegistrar& registrar;

        /// Data carried by the event.
        void *data;

        /// Flag that indicates the nature of the event.
        Event::Flags flag;

};

#endif
