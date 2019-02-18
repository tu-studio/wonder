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
#include <string>
#include <vector>

#include "config.h"
#include "vector3d.h"

namespace xmlpp {
    class Node;
}

enum InOutMode {
    IOM_NORMAL,
    IOM_ALWAYSOUT,
    IOM_ALWAYSIN
};



class TwonderConfig {

public:
    TwonderConfig(int argc, char** argv);

    ~TwonderConfig();

    int readConfig();

    std::string twonderConfigfile;
    std::string speakersConfigfile;

    const char*  jackName;
    std::string  name;
    bool         verbose;
    int          noSources;

    lo_address  cwonderAddr;
    const char* cwonderHost;
    const char* cwonderPort;

    const char* listeningPort;

    float planeComp; // amplitude compensation for plane waves, should be < 1
    float soundSpeed;
    float negDelayInit;

    unsigned int sampleRate;

    enum InOutMode ioMode;

    /// corner points of area in which sources will be rendered as focussed sources
    std::vector<Vector3D> renderPolygon;

    float focusLimit;
    float focusMargin;

    float reference;
    float speakerDistance;

    float elevationY1;
    float elevationY2;
    float elevationZ1;
    float elevationZ2;
    float slope;

private:
    void parseArgs(int argc, char* argv[]);

    void getFocus(xmlpp::Node* node);

    void getSpeakers(xmlpp::Node* node);
};

extern TwonderConfig* twonderConf;
