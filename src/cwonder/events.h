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

#include <lo/lo.h>

#include <list>
#include <string>

#include "timestamp.h"
#include "vector3d.h"

using std::list;
using std::string;

class Event
{
  public:
    Event();
    ~Event();

    bool operator==(const Event& other) const { return timestamp == other.timestamp; }

    bool operator<(const Event& other) const { return timestamp < other.timestamp; }

    bool operator<(const TimeStamp& other) const { return timestamp < other; }

    lo_address from;
    string oscpath;
    int id;
    int type;  /// 0 = plane source, 1 = point source
    string name;
    bool active;
    Vector3D pos;
    float angle;
    TimeStamp timestamp;
    float duration;
};  // class Event

class Timeline
{
  public:
    Timeline()  {}

    ~Timeline();

    void addevent(Event* event);

    Event* getevent(TimeStamp now);

    int getsize() { return timeline.size(); }

    void print();

  private:
    list<Event*> timeline;
    list<Event*>::iterator it, last;
    bool found{false};
};  // class Timeline
