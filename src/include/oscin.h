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
#include <lo/lo_cpp.h>


#include <string>

/// A wrapper class for the liblo osc server
class OSCServer
{
  public:
    OSCServer(const char* port);
    OSCServer(const char* port, const char* multicast_group,
      const char* multicast_port);

    ~OSCServer();

    void start();
    void stop();

    void addMethod(const char* path, const char* types, lo_method_handler h,
                   void* user_data = nullptr);
    std::string getContent(const char* path, const char* types, lo_arg** argv, int argc);

    // currently only one value as content is allowed
    void send(lo_address addr, const char* path, const char* types, const char* value);
    // internal exception class
    class EServ
    {};

  private:
    lo::ServerThread  serverThread;
    lo::ServerThread multicastServerThread;
};
