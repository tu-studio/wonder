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

#include <libxml++/libxml++.h>

#include <vector>

#include "config.h"
#include "vector3d.h"

namespace xmlpp
{
class Node;
}

//-----------------------------------Segment---------------------------------//

// A segment of speakers
class Segment
{
  public:
    Segment(xmlpp::Node* node);

    void syncToXML();

    int id;
    int noSpeakers;
    float windowWidth;
    Vector3D start;
    Vector3D end;
    Vector3D normal;

  private:
    void readFromXML();

    xmlpp::Node* node;
    Glib::ustring nodeName;
};

//-------------------------------end of Segment------------------------------//

//---------------------------------SegmentArray------------------------------//

// Array of speaker segments
class SegmentArray
{
  public:
    SegmentArray(std::string fileName);
    ~SegmentArray();

    std::vector<Segment*> segments;

  private:
    void getSegments(xmlpp::Node* node, const Glib::ustring& xpath);

    int readFromFile(std::string fileName);
};

//-----------------------------end of SegmentArray---------------------------//

//-------------------------------SegmentArrayIter----------------------------//

// An iterator for the dom representation of the segments of speakers.
// This iterator is used to traverse the dom tree. If a segment is found, a new object of
// type Segment gets constructed and initialised by the attributes of the xml-element.
class SegmentArrayIter : public xmlpp::Node::NodeSet::iterator
{
  public:
    SegmentArrayIter() = default;

    SegmentArrayIter(const xmlpp::Node::NodeSet::iterator& other)
        : xmlpp::Node::NodeSet::iterator(other) {}

    ~SegmentArrayIter() = default;

    /// construct a segment when the operator* is called
    // XXX: maybe think this over... new in * is not so common...
    Segment* operator*();
};

//---------------------------end of SegmentArrayIter-------------------------//
