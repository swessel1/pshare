/*******************************************************************************
 * File        : NodeEvent.h
 * Description : Node-related events.
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

#include "Event.h"

/**
 * @class   NodeEvent
 *
 * @brief   Node-related events.
 *
 * A node will always fire events of type NodeEvent. These events can contain
 * simple flags or messages received by the node.
 */
class NodeEvent : public Event {

    public:
        /**
         * @brief Flags that indicate the nature of the event.
         */
        enum Flags {
            MSG_RECEIVED,
            NODE_DISCONNECT
        };

        /**
         * @brief Constructor.
         *
         * @param[in]   orig    The originator of this event.
         * @param[in]   data    The data that is carried by the event.
         * @param[in]   flag    The flag that indicates the nature of this
         *                      event.
         */
        NodeEvent(EventRegistrar &orig, void *data, NodeEvent::Flags flag);

        /**
         * @brief Gets the flag for this event.
         *
         * @return A flag of type `enum Flags`.
         */
        NodeEvent::Flags get_flag() const;

        /**
         * @brief Gets the data carried by this event.
         *
         * @return Returns the data cast to type `T`.
         */
        template <typename T>
        T* get_data();

    private:
        /// The flag that indicates the nature of this event.
        NodeEvent::Flags flag;

        /// The data that is carried by this event.
        void *data;
};

template <typename T>
T* NodeEvent::get_data() {

    return (T*)data;
}
