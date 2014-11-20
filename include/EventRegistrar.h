/*******************************************************************************
 * File        : EventRegistrar.h
 * Description : An interface for classes that can register events in an event
 *               queue.
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

#ifndef EVENTREGISTRAR_H
#define EVENTREGISTRAR_H

#include <atomic>
#include "BlockingQueue.h"
#include "Event.h"

/**
 * @class   EventRegistrar
 *
 * @brief   An interface for classes that can register events in an event queue.
 *
 * @details Classes that register events to an event queue must inherit this
 *          class in order to ensure consistency.
 */
class EventRegistrar {

    public:
        /**
         * @brief Constructor.
         *
         * @param[in]   event_queue     Reference to a BlockingQueue<Event> that
         *                              the EventRegistrar will register events
         *                              to.
         */
        EventRegistrar(BlockingQueue<Event> &event_queue);

    protected:
        /**
         * @brief Registers an event to the queue.
         *
         * @param[in]   event   Reference to the event that will be registered
         *                      into the queue.
         */
        void register_event(Event &event);
        
        /// Reference to the queue that this EventRegistrar registers to.
        BlockingQueue<Event>& event_queue;

        /// Blocks this EventRegistrar from registering events.
        std::atomic<bool> suppress_events;
    
};

#endif
