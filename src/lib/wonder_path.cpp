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

#include <fstream>

#include "wonder_path.h"

using std::string;
using std::ifstream;


bool isabs(string s) {
    return s.at(0) == '/';
}


string join(string a, string b) {
    string path = a;

    if(path == "" || path.at((path.length() - 1)) == '/') {
        path += b;
    } else {
        path += (string) "/" += b;
    }

    return path;
}


string pathsplit(string name, int part) {
    int n = name.find_last_of("/");

    if(part == 0) { // head
        // if name ends with "/" truncate that and find again
        if(n == (int)(name.length() - 1)) {
            name = name.substr(0, n);
            n = name.find_last_of("/");
            return name.substr(0, n + 1);
        } else {
            return name.substr(0, n + 1);
        }
    } else if(part == 1) { // tail
        return name.substr(n + 1);
    }

    return name;
}


bool pathexists(string name) {
    ifstream fin;
    fin.open(name.c_str(), std::ios_base::in);

    if(!fin.is_open()) {
        return false;
    }

    fin.close();

    return true;
}


int makedirs(string name, mode_t mode = 0777) {
    string head, tail;

    head = pathsplit(name, 0);
    tail = pathsplit(name, 1);

    if(tail.empty()) {
        head = pathsplit(head, 0);
        tail = pathsplit(head, 1);
    }

    if(! tail.empty() && ! head.empty() && ! pathexists(head)) {
        makedirs(head, mode);

        if(tail == ".") {
            return 0;
        }
    }

#ifdef WIN32
    mkdir(name.c_str());
#else
    mkdir(name.c_str(), mode);
#endif

    return 0;
}
