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

#include "oscping.h"

#include <iostream>

#include "cwonder_config.h"
#include "liblo_extended.h"
#include "oscstream.h"

using std::list;
using std::string;
using std::vector;

OSCPing::OSCPing(lo_address address, string path, int pingSpan, int allowedLostPongs,
                 string name)
    : address(address)
    , name(name)
    , pingCount(pingSpan)
    , pingSpan(pingSpan)
    , allowedLostPongs(allowedLostPongs)
    , path(path) {}

bool OSCPing::ping() {
    // check the responses
    if (pingCount == 0) {
        // if more then the allowed percentage of pings remains unanswered, remove client
        // from stream
        if ((100 - (100.0 * pongCount / pingSpan)) > allowedLostPongs) {
            // wonderlog->print(LOG_INFO, "[V-ping] path=" + path + " host=" + (string)
            // lo_address_get_hostname(address) + " port=" + (string)
            // lo_address_get_port(address) + " is down.");

            return false;  // host is down
        }

        pongCount = 0;
    }

    // send ping
    lo_send(address, path.c_str(), "i", pingCount);

    // decrement the ping counter and reset if necessary
    if (pingCount > 0) {
        pingCount -= 1;
    } else {
        pingCount = pingSpan;
    }

    return true;
}

void OSCPing::pong() { ++pongCount; }

ListOSCPing::ListOSCPing(string path) : path(path) {}

ListOSCPing::~ListOSCPing() {
    for (clients = begin(); clients != end(); ++clients) { delete (*clients); }

    clear();
}

int ListOSCPing::add(lo_address a, string name, int pingSpan, int allowedLostPongs) {
    // check whether client is already on the list
    for (clients = begin(); clients != end(); ++clients)
        if (issame((*clients)->address, a)) { return 1; }

    // add new client to list
    push_back(new OSCPing(a, path, pingSpan, allowedLostPongs, name));

    return 0;
}

int ListOSCPing::remove(lo_address b) {
    for (clients = begin(); clients != end(); ++clients) {
        if (issame((*clients)->address, b)) {
            delete (*clients);
            erase(clients);
            return 0;
        }
    }

    return 1;
}

int ListOSCPing::pong(int seq, lo_address from) {
    for (clients = begin(); clients != end(); ++clients) {
        if (issame((*clients)->address, from)) {
            (*clients)->pong();
            return 0;
        }
    }

    return 1;
}

void ListOSCPing::ping(list<OSCStreamClient>& deadClients) {
    std::list<OSCPing*>::iterator next;

    for (clients = begin(); clients != end(); ++clients) {
        bool clientAlive = (*clients)->ping();

        if (!clientAlive) {
            deadClients.push_back(
                OSCStreamClient(lo_address_get_hostname((*clients)->address),
                                lo_address_get_port((*clients)->address),
                                (*clients)->name, (*clients)->address));
            delete (*clients);
            clients = --(erase(clients));
        }
    }
}
