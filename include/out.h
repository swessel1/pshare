/*******************************************************************************
 * File        : out.h
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

#ifndef OUT_H
#define OUT_H

#include <iostream>

/// Indicates verbose mode.
extern bool verbose;

/**
 * @brief Prints message if verbose mode is set.
 *
 * @param[in]   s       The string that will be printed.
 * @param[in]   level   0: information, 1: warning, 2: error, 3: fatal
 *
 * @return If verbose mode is on, returns a reference to cout. Otherwise,
 *         returns a reference to a null stream.
 */
std::ostream& out(unsigned char level = 0);

#endif
