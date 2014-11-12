/*******************************************************************************
 * File        : out.cpp
 * Description : Debug output.
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

#include <iostream>
#include "out.h"

bool verbose;
std::ostream null_stream(0);

std::ostream& out(unsigned char level) {

    if (!verbose)
        return null_stream;

    std::string prefix;

    switch (level) {

        case 1:
            prefix = "[WARNING] ";
            break;

        case 2:
            prefix = "[ERROR] ";
            break;

        case 3:
            prefix = "[FATAL] ";
            break;

        default:
            prefix = "[INFO] ";
            break;
    }

    return std::cout << prefix;
}
