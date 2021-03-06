/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                   *
 *  WONDER - Wave field synthesis Of New Dimensions of Electronic music in Realtime  *
 *  http://swonder.sourceforge.net                                                   *
 *                                                                                   *
 *                                                                                   *
 *  Technische Universität Berlin, Germany                                           *
 *  Audio Communication Group                                                        *
 *  www.ak.tu-berlin.de                                                              *
 *  Copyright 2006-2008                                                              *
 *                                                                                   *
 *                                                                                   *
 *  This program is free software; you can redistribute it and/or modify             *
 *  it under the terms of the GNU General Public License as published by             *
 *  the Free Software Foundation; either version 2 of the License, or                *
 *  (at your option) any later version.                                              *
 *                                                                                   *
 *  This program is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 *  GNU General Public License for more details.                                     *
 *                                                                                   *
 *  You should have received a copy of the GNU General Public License                *
 *  along with this program; if not, write to the Free Software                      *
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.       *
 *                                                                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#pragma once

#include <sys/stat.h>
#include <sys/types.h>

#include <string>

// Some global convenience functions for file path manipulations.

// check if path is an absolute path, i.e starts with a '/'
bool isabs(std::string s);

// concatenate two paths
// Insert a '/' if a does not already end with it
std::string join(std::string a, std::string b);

// split a path at the last occurrence of "/"
// part determines if the first part (head) of last part (tail)
// will be returned
// 0 = head, 1 = tail
std::string pathsplit(std::string name, int part);

bool pathexists(std::string name);

// Super-mkdir - translated from python's os.path.makedirs()
// create a leaf directory and all intermediate ones.
// Works like mkdir, except that any intermediate path segment (not
// just the rightmost) will be created if it does not exist. This is
// recursive.
// 0 = success, -1 = failed

int makedirs(std::string name, mode_t mode);
