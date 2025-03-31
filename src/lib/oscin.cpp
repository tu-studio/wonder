/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                   *
 *  WONDER - Wave field synthesis Of New Dimensions of Electronic music in Realtime  *
 *  http://swonder.sourceforge.net                                                   *
 *                                                                                   *
 *                                                                                   *
 *  Technische Universit�t Berlin, Germany                                           *
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

#include "oscin.h"

#include <iostream>
#include <sstream>

void err_handler(int num, const char *msg, const char *where){
    std::cout << "Error " << num << " while creating server in function " << where <<  ": " << msg << std::endl;
}
// multicastServerThread listens on random free port
OSCServer::OSCServer(const char* port): serverThread(port, err_handler), multicastServerThread(nullptr, nullptr) {
    if (!serverThread.is_valid()) {
        throw EServ();
    }

    if (multicastServerThread.is_valid()) {
        throw EServ();
    }

}

OSCServer::OSCServer(const char* port, const char* multicast_group,
                     const char* multicast_port): serverThread(port, err_handler), multicastServerThread(multicast_group, multicast_port, nullptr, nullptr, err_handler) {

    if (!serverThread.is_valid()) {
        throw EServ();
    }

    std::cout << "Joining Multicast group " << multicast_group << " on port "
                << multicast_port << std::endl;

    if (!multicastServerThread.is_valid()) {
        throw EServ();
    }
    
}

OSCServer::~OSCServer() {
// Threads now take care of freeing themselves
}

void OSCServer::start() {
    serverThread.start();
    if (multicastServerThread.is_valid()) multicastServerThread.start();
}

void OSCServer::stop() {
    serverThread.stop();
    if (multicastServerThread.is_valid()) multicastServerThread.stop();
}

void OSCServer::addMethod(const char* path, const char* types, lo_method_handler h,
                          void* user_data) {
    serverThread.add_method(path, types, h, user_data);
    if (multicastServerThread.is_valid())
        multicastServerThread.add_method(path, types, h, user_data);
}

void OSCServer::send(lo_address addr, const char* path, const char* types, const char* value){
    lo::Address a(addr, false);
    a.send_from(serverThread, path, types, value);
}


std::string OSCServer::getContent(const char* path, const char* types, lo_arg** argv,
                                  int argc) {
    std::ostringstream contents;

    contents << "[OSCServer::" << path << "]   ";

    for (int i = 0; i < argc; ++i) {
        contents << types[i] << "=";

        switch (types[i]) {
        case 's':
            contents << &argv[i]->s;
            break;

        case 'f':
            contents << argv[i]->f;
            break;

        case 'i':
            contents << argv[i]->i;
        }

        contents << "   ";
    }

    return contents.str();
}
