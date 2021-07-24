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

#include "osc.h"

#include <iostream>

using std::cerr;
using std::endl;
using std::exception;

OSCServer::OSCServer(const char* port) {
    serverThread = lo_server_thread_new(port, nullptr);

    if (serverThread == nullptr) {
        cerr << "Warning !!! Could not create OSC Server thread " << endl;
        throw exception();
    }
}

OSCServer::~OSCServer() { lo_server_thread_free(serverThread); }

void OSCServer::start() { lo_server_thread_start(serverThread); }

void OSCServer::stop() { lo_server_thread_stop(serverThread); }

void OSCServer::addMethod(const char* path, const char* typespec, lo_method_handler h,
                          void* user_data) {
    lo_server_thread_add_method(serverThread, path, typespec, h, user_data);
}
