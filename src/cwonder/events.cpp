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

#include "events.h"

#include <iostream>

#include "cwonder_config.h"
using std::cout;
using std::endl;

Event::~Event() {
    if (from) { lo_address_free(from); }
}

Timeline::~Timeline() {
    for (it = timeline.begin(); it != timeline.end(); ++it) { delete (*it); }
}

void Timeline::addevent(Event* event) {
    found = false;

    for (it = timeline.begin(); it != timeline.end(); ++it) {
        if (event->timestamp < (*it)->timestamp) {
            last  = timeline.insert(it, event);
            found = true;
            break;
        }
    }

    if (found == false) { timeline.push_back(event); }
}

Event* Timeline::getevent(TimeStamp now) {
    Event* ev;

    it = timeline.begin();

    if ((*it)->timestamp < now) {
        ev = (*it);
        timeline.erase(it);
    } else {
        ev = nullptr;
    }

    return ev;
}

void Timeline::print() {
    cout << endl << "[Timeline::print] On the event stack:" << endl;

    for (it = timeline.begin(); it != timeline.end(); ++it) {
        cout << "   Type: " << (*it)->oscpath;
        (*it)->timestamp.show(" ");
    }

    cout << endl;
}
