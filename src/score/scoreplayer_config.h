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

#ifndef SCOREPLAYERCONFIG_H
#define SCOREPLAYERCONFIG_H

#include <string>

#include <lo/lo.h>


namespace xmlpp
{
    class Node;
}

class ScoreplayerConfig
{ 

 public:
    ScoreplayerConfig( int argc, char** argv );

    int readconfig();

    int verbose;
    int osc_verbose;
    int mtc_verbose;

    int screendump_verbose;

    const char* listeningPort;

    const char* cwonderPort;
    const char* cwonderHost;
    lo_address  cwonderAddr;

    std::string scorefilePath;

    std::string dtdPath;

    std::string scoreplayerConfigfile;

    void showValues();

    // maximum number of sound sources allowed ( default is 0, must be set by cwonder via OSC )
    int maxNoSources;

    // the name of this application, which is  used when connecting to cwonder 
    std::string name;

 private:
    void parse_args( int argc, char** argv );
    void getSettings( xmlpp::Node* node );
};

extern ScoreplayerConfig* scoreplayerConf;

#endif
