/*******************************************************************************
 * File        : Event.cpp
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

#include "Event.h"

Event::Event(EventRegistrar &registrar, void *data, Event::Flags flag) :
    registrar(registrar), data(data), flag(flag) { }

EventRegistrar& Event::get_registrar() const {

    return registrar;
}

void* Event::get_data() {

    return data;
}

Event::Flags Event::get_flag() const {

    return flag;
}
