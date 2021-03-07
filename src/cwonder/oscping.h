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
#include <vector>

struct OSCStreamClient;

class OSCPing
{
  public:
    OSCPing(lo_address address, std::string path, int pingSpan = 5,
            int allowedLostPongs = 90, std::string name = "");
    ~OSCPing() = default;

    OSCPing(OSCPing&&) = default;
    OSCPing& operator=(OSCPing&&) = default;
    OSCPing(const OSCPing&)       = default;
    OSCPing& operator=(const OSCPing&) = default;

    /// evaluate responses received and send new ping
    /// if response evaluation indicates that the client is not responding
    /// return false to signal this
    bool ping();

    // gets called when a pong is received
    void pong();

  public:
    lo_address address;

    std::string name;

  private:
    int pingCount;
    int pongCount{0};
    int pingSpan;
    int allowedLostPongs;  // in percent
    std::string path;
};

// List of OSC stream clients all of which need to be pinged the check if they are still
// alive. This class is part of the stream facilities. It sends out periodic ping commands
// and evaluates the replies to check whether a host is still up or not. If a certain
// percentage of pings remains unanswered the client is considered dead and removed from
// the list.
class ListOSCPing : public std::list<OSCPing*>
{
  public:
    ListOSCPing(std::string path);
    ~ListOSCPing();

    ListOSCPing(ListOSCPing&&) = default;
    ListOSCPing& operator=(ListOSCPing&&) = default;
    ListOSCPing(const ListOSCPing&) = default;
    ListOSCPing& operator=(const ListOSCPing&) = default;

    int add(lo_address a, std::string name = "", int pingSpan = 3,
            int allowedLostPongs = 90);
    int remove(lo_address b);

    void ping(std::list<OSCStreamClient>& deadClients);
    int pong(int pingNum, lo_address from);

    std::string path;

    std::list<OSCPing*>::iterator clients;
};
