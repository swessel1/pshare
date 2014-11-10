/*******************************************************************************
 * File        : TcpListenerEvent.h
 * Description : An event associated with TcpListener.
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
 * @brief   An event associated with TcpListener.
 *
 * @details This event is registered by an instance of the TcpListener class.
 *          Details of the event can be found in the flag.
 */
class TcpListenerEvent : public Event {

    public:
        /**
         * @brief Flags that indicate the nature of the event.
         */
        enum Flags {
            LISTEN_FAIL,
            NEW_CONNECTION
        };

        /**
         * @brief Constructor.
         *
         * @param[in]   orig    The registrar (originator) of this event.
         * @param[in]   data    Pointer to the data carried by this event.
         * @param[in]   flag    Nature of this event.
         */
        TcpListenerEvent(EventRegistrar &orig,
                         void *data,
                         TcpListenerEvent::Flags flag);

        /**
         * @brief Gets the flag for this event.
         *
         * @return The flag that indicates the nature of this event.
         */
        TcpListenerEvent::Flags get_flag() const;

        /**
         * @brief Gets the data carried by this event.
         *
         * @return Returns the data carried by the event, cast to `T`.
         */
        template <typename T>
        T* get_data() const;

    private:
        /// The data this event carries.
        void *data;
        
        /// The flag of this event.
        TcpListenerEvent::Flags flag;

};

template <typename T>
T* TcpListenerEvent::get_data() const {

    return (T*)data;
}
