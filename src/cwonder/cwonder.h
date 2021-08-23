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

#include "cwonder_config.h"
#include "events.h"
#include "oscstream.h"
#include "project.hpp"
#include "timestamp.hpp"

class Cwonder
{
  public:
    int createProject(std::string path);
    int loadProject(std::string path);
    int saveProject();
    int saveProjectAs(std::string path);

    int takeSnapshot(int snapshotID, std::string name = "");
    int deleteSnapshot(int snapshotID);
    int recallSnapshot(int snapshotID, float duration);
    int renameSnapshot(int snapshotID, std::string name);
    int copySnapshot(int fromID, int toID);

    int setSourceActive(lo_address from, int sourceID, bool active);
    int setSourceName(lo_address from, int sourceID, std::string name);
    int setSourceType(lo_address from, int sourceID, int type, float timetag);
    int setSourcePosition(lo_address from, int sourceID, float x, float y, float duration,
                          float timetag);
    int setSourcePosition3D(lo_address from, int sourceID, float x, float y, float z,
                            float duration, float timetag);
    int setSourceAngle(lo_address from, int sourceID, float angle, float duration,
                       float timetag);
    int setSourceGroupID(lo_address from, int sourceID, int groupID);
    int setSourceColor(lo_address from, int sourceID, int red, int green, int blue);
    int setSourceRotationDirection(lo_address from, int sourceID, bool invert);
    int setSourceScalingDirection(lo_address from, int sourceID, bool invert);
    int setSourceDopplerEffect(lo_address from, int sourceID, bool dopplerOn);

    int setListenerPosition(lo_address from, int listenerID, float x, float y);

    int setGroupActive(lo_address from, int groupID, bool active);
    int setGroupColor(lo_address from, int groupID, int red, int green, int blue);
    int setGroupPosition(lo_address from, int groupID, float x, float y);

    int renderStreamConnect(std::string host, std::string port, std::string name = "");
    int visualStreamConnect(std::string host, std::string port, std::string name = "");
    int timerStreamConnect(std::string host, std::string port, std::string name = "");

    void notifyVisualStreamOfNewStreamClient(std::string host, std::string port,
                                             std::string name);
    void notifyVisualStreamOfDeadStreamClients(
        std::list<OSCStreamClient>& deadStreamClients);

    void sendStreamClientDataTo(lo_address targetAdress, OSCStreamClient& clientData);

    void scheduler(int currtime);
    TimeStamp nowTime;
    TimeStamp future;

    // send data about current scenario to all streamclients
    void sendScenario();

    // security function to check if a project was loaded or created
    // returns true if everything is ok
    bool checkScenario();

    TimeStamp recordTime;

    Project project{cwonderConf->maxNoSources};
    Scenario* scenario{nullptr};

    std::string projectFileName;
    std::string projectPath;
    std::string dtdPath;

    Timeline timeLine;

    OSCStream renderStream{"render"};
    OSCStream visualStream{"visual"};
    OSCStream timerStream{"timer"};

    TimeStamp lastRenderPing;
    TimeStamp lastVisualPing;
    TimeStamp lastTimerPing;

    // clients which do not respond to pings anymore and thus can be disconnected
    std::list<OSCStreamClient> deadStreamClients;

    std::list<OSCStreamClient>::iterator streamIter;

    std::string returnString;
};  // class Cwonder
