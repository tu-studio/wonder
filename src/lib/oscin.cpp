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

#include <sstream>

OSCServer::OSCServer(const char* port, const char* multicast_group,
                     const char* multicast_port) {
    serverThread = lo_server_thread_new(port, nullptr);

    if (serverThread == nullptr) {
        throw EServ();
    }

    if (multicast_group != nullptr && multicast_port != nullptr) {
        multicastServerThread =
            lo_server_thread_new_multicast(multicast_group, multicast_port, nullptr);
        if (multicastServerThread == nullptr) {
            throw EServ();
        }
    }
}

OSCServer::~OSCServer() {
    if (serverThread) {
        lo_server_thread_free(serverThread);
    }

    if (multicastServerThread) {
        lo_server_thread_free(multicastServerThread);
    }
}

void OSCServer::start() {
    lo_server_thread_start(serverThread);
    if (multicastServerThread) lo_server_thread_start(multicastServerThread);
}

void OSCServer::stop() {
    lo_server_thread_stop(serverThread);
    if (multicastServerThread) lo_server_thread_stop(multicastServerThread);
}

void OSCServer::addMethod(const char* path, const char* types, lo_method_handler h,
                          void* user_data) {
    lo_server_thread_add_method(serverThread, path, types, h, user_data);
    if (multicastServerThread)
        lo_server_thread_add_method(multicastServerThread, path, types, h, user_data);
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
