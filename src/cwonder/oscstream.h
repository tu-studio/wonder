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

#include <filesystem>
#include <list>
#include <string>

#include "oscping.h"

// A client of an OSC stream
struct OSCStreamClient
{
  public:
    OSCStreamClient(std::string host, std::string port, std::string name,
                    lo_address address)
        : host(host), port(port), name(name), address(address) {}

    std::string host;
    std::string port;
    std::string name;

    lo_address address;
};

class OSCStream
{
  public:
    OSCStream(std::string name);
    ~OSCStream();

    OSCStream(OSCStream&&)      = delete;
    OSCStream(const OSCStream&) = delete;
    OSCStream& operator=(OSCStream&&) = delete;
    OSCStream& operator=(const OSCStream&) = delete;

    lo_address connect(std::string host, std::string port, std::string name);
    void disconnect(lo_address b);

    void ping(std::list<OSCStreamClient>& deadClients);
    void pong(int pingNum, lo_address from);

    std::list<OSCStreamClient>::iterator begin();
    std::list<OSCStreamClient>::iterator end();
    bool hasClients();

    void send(const char* path, const char* types, int argc, lo_arg** argv,
              lo_message msg);
    void send(const char* types, int argc, lo_arg** argv, lo_message msg);

  private:
    std::list<OSCStreamClient> clients;
    std::list<OSCStreamClient>::iterator clientsIter;
    std::filesystem::path statefilename;

    std::string name;
    ListOSCPing* pingList;
};
