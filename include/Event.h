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
         * @brief Constructor.
         *
         * @param[in]   registrar   The registrar (originator) of this event.
         */
        Event(EventRegistrar &registrar);

        /**
         * @brief Returns the registrar (originator) of this event.
         *
         * @return Reference to the registrar of this event.
         */
        EventRegistrar& get_registrar() const;

    private:
        /// The registrar of this event.
        EventRegistrar& registrar;

};

#endif
