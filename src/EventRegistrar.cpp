/*******************************************************************************
 * File        : EventRegistrar.cpp
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

#include "EventRegistrar.h"
#include "out.h"

EventRegistrar::EventRegistrar(BlockingQueue<Event> &event_queue) :
    event_queue(event_queue), suppress_events(false) { }

void EventRegistrar::register_event(Event &event) {

    if (!suppress_events)
        event_queue.push(event);
}
