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

#include "cwonder.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>

#include "cwonder_config.h"

using std::exception;
using std::ifstream;
using std::ios_base;
using std::list;
using std::ostringstream;
using std::string;
using std::vector;

#define PLAYNOW 0

Cwonder::Cwonder() {
    renderStream = new OSCStream("render");
    visualStream = new OSCStream("visual");
    timerStream  = new OSCStream("timer");

    returnString = "";

    // create the project and the scenario
    project  = new Project(cwonderConf->maxNoSources);
    scenario = nullptr;

    // these must be set externally !
    dtdPath     = "";
    projectPath = "";
}

Cwonder::~Cwonder() {
    delete renderStream;
    delete visualStream;
    delete timerStream;
    delete project;
}

int Cwonder::createProject(string path) {
    // add fileextension ".xml" if it was not passed in path
    string::size_type i = path.find(".xml");

    if (i == path.npos) { path.append(".xml"); }

    // check if an absolute filepath or just a filename were sent
    // and construct projectname and -path accordingly
    if (path.find('/') != 0) {  // just a filename
        projectPath     = cwonderConf->projectPath;
        projectFileName = path;
    } else {  // absolute filepath
        projectPath     = pathsplit(path, 0);
        projectFileName = pathsplit(path, 1);
    }

    // construct absolute filepath
    string filePath = join(projectPath, projectFileName);

    // Check if the filpath is not empty, if not return
    if (filePath.empty()) {
        returnString = "no filename specified.";
        return 1;  // name is empty
    }

    // Check if the project file already exists, if so return
    ifstream fin;
    fin.open(filePath.c_str(), ios_base::in);

    if (fin.is_open()) {
        fin.close();
        returnString = "project=" + filePath + " already exists.";
        return 2;  // file does already exist
    }

    // now create the project
    try {
        project->name = projectFileName.substr(0, projectFileName.find(".xml"));
        project->setDtdPath(dtdPath);
        project->createProject();
    }
    catch (const exception& ex) {
        returnString = "project=" + filePath + " error: " + ex.what();
        return 3;  // create error
    }

    // get the new scenario from the project

    // send information about the whole project to the visual stream
    // TODO: this may exceed the sizelimit of an OSC message, slice it up somehow
    string p = project->show();

    for (streamIter = visualStream->begin(); streamIter != visualStream->end();
         ++streamIter) {
        lo_send(streamIter->address, "/WONDER/project/xmlDump", "is", p.empty(),
                p.c_str());
    }

    scenario = project->getScenario();

    sendScenario();

    returnString = "create project=" + filePath + " ok. ";

    return 0;
}

int Cwonder::loadProject(string path) {
    // add fileextension ".xml" if it was not passed in path
    string::size_type i = path.find(".xml");

    if (i == path.npos) { path.append(".xml"); }

    // check if an absolute filepath of just a filename were sent
    // and construct projectname and -path accordingly
    if (path.find('/') != 0) {  // just a filename
        projectPath     = cwonderConf->projectPath;
        projectFileName = path;
    } else {  // absolute filepath
        projectPath     = pathsplit(path, 0);
        projectFileName = pathsplit(path, 1);
    }

    // construct absolute filepath
    string filePath = join(projectPath, projectFileName);

    // check if the project file exists, if not return
    ifstream fin;
    fin.open(filePath.c_str(), ios_base::in);

    if (!fin.is_open()) {
        returnString = "project=" + filePath + " does not exist.";
        return 1;  // file does not exist
    } else {
        fin.close();
    }

    // now read the project from the file
    try {
        project->setDtdPath(dtdPath);
        project->readFromFile(filePath.c_str());
    }
    catch (const xmlpp::validity_error& ex) {
        returnString = "Validation error! project=" + path + " is not a valid project.";
        return 3;  // error reading the dom representation
    }
    catch (const Scenario::tooManySources& ex) {
        returnString = "project=" + path
                       + (" load error. Project contains too many sources for the "
                          "current system setup.");
        return 2;  // error parsing the file
    }
    catch (const exception& ex) {
        returnString = "project=" + path + " parse error: " + ex.what();
        return 2;  // error parsing the file
    }

    // send information about the whole project to the visual stream
    // TODO: this may exceed the sizelimit of an OSC message, slice it up somehow
    string p = project->show();

    for (streamIter = visualStream->begin(); streamIter != visualStream->end();
         ++streamIter) {
        lo_send(streamIter->address, "/WONDER/project/xmlDump", "is", p.empty(),
                p.c_str());
    }

    // get the scenario of the loaded project and communicate it via OSC
    scenario = project->snapshots.front();

    sendScenario();

    returnString = "loading project=" + path + " ok. ";

    return 0;
}

int Cwonder::saveProject() {
    if (!checkScenario()) { return 5; }

    // construct absolute filepath
    string filePath = join(projectPath, projectFileName);

    // now write the project to the file
    try {
        // NOTE: save does not create missing directories
        project->writeToFile(filePath);
    }
    catch (const exception& ex) {
        returnString = "project=" + filePath + " write error: " + ex.what();
        return 2;  // save error
    }

    returnString = "save project=" + filePath + " ok. ";

    return 0;
}

int Cwonder::saveProjectAs(string path) {
    if (!checkScenario()) { return 5; }

    // add fileextension ".xml" if it was not passed in path
    string::size_type i = path.find(".xml");

    if (i == path.npos) { path.append(".xml"); }

    // check if an absolute filepath of just a filename were sent
    // and construct projectname and -path accordingly
    if (path.find('/') != 0) {  // just a filename
        projectPath     = cwonderConf->projectPath;
        projectFileName = path;
    } else {  // absolute filepath
        projectPath     = pathsplit(path, 0);
        projectFileName = pathsplit(path, 1);
    }

    // construct absolute filepath
    string filePath = join(projectPath, projectFileName);

    // set the new project name
    project->name = projectFileName.substr(0, projectFileName.find(".xml"));

    // now write the project to the file
    try {
        // NOTE: save does not create missing directories
        project->writeToFile(filePath);
    }
    catch (const exception& ex) {
        returnString = "project=" + filePath + " write error: " + ex.what();
        return 2;  // save error
    }

    returnString = "save project as =" + filePath + " ok. ";

    return 0;
}

int Cwonder::takeSnapshot(int snapshotID, string name) {
    if (!checkScenario()) { return 5; }

    if (snapshotID == 0) {
        returnString = " Snapshot ID 0 is not valid ";
        return 2;
    }

    ostringstream st;
    st << snapshotID;

    int ret = project->takeSnapshot(snapshotID, name);

    if (ret == 1) {
        returnString = "can not get rood node of project file.";
        return 1;  // can not get root node
    }

    returnString = "take snapshot " + st.str() + " ok. ";

    return 0;
}

int Cwonder::deleteSnapshot(int snapshotID) {
    if (!checkScenario()) { return 5; }

    ostringstream st;
    st << snapshotID;

    int ret = project->deleteSnapshot(snapshotID);

    if (ret == 1) {
        returnString = "snapshot " + st.str() + " does not exist.";
        return 1;  // snapshot does not exist
    } else if (ret == 2) {
        returnString = "validation of document failed.";
        return 2;
    }

    returnString = "delete snapshot " + st.str() + " ok.";

    return 0;
}

int Cwonder::recallSnapshot(int snapshotID, float duration) {
    if (!checkScenario()) { return 5; }

    ostringstream st;
    st << snapshotID;

    // temporarily store info about current scenario so
    // that only deactivated sources are activated and vice versa
    Scenario oldScenario = *scenario;

    // recall snapshot in project
    // this gets the right data into the scenario
    int ret = project->recallSnapshot(snapshotID);

    if (ret != 0) {
        returnString = "snapshot " + st.str() + " does not exist in the project.";
        return 1;
    } else {
        returnString = "snapshot " + st.str() + " does exist in the project.";
    }

    // send new scenario to streams
    // (activate/deactivate according to new and old)

    // send all source data to the streams and update the scenario
    for (int i = 0; i < (int)scenario->sourcesVector.size(); ++i) {
        // send data about active sources
        // if source wasn't active, then send activate first
        if (scenario->sourcesVector[i].active) {
            // send to stream receivers (render)
            for (streamIter = renderStream->begin(); streamIter != renderStream->end();
                 ++streamIter) {
                // activate it
                lo_send(streamIter->address, "/WONDER/source/activate", "i", i);

                // send type
                lo_send(streamIter->address, "/WONDER/source/type", "ii", i,
                        scenario->sourcesVector[i].type);

                // send position
                lo_send(streamIter->address, "/WONDER/source/position3D", "iffff", i,
                        scenario->sourcesVector[i].pos[0],
                        scenario->sourcesVector[i].pos[1],
                        scenario->sourcesVector[i].pos[2], duration);

                // send angle
                lo_send(streamIter->address, "/WONDER/source/angle", "iff", i,
                        scenario->sourcesVector[i].angle, duration);

                // send doppler effect
                lo_send(streamIter->address, "/WONDER/source/dopplerEffect", "ii", i,
                        scenario->sourcesVector[i].dopplerEffect);
            }

            // send to stream receivers (visual)
            for (streamIter = visualStream->begin(); streamIter != visualStream->end();
                 ++streamIter) {
                // activate it
                lo_send(streamIter->address, "/WONDER/source/activate", "i", i);

                // send type
                lo_send(streamIter->address, "/WONDER/source/type", "ii", i,
                        scenario->sourcesVector[i].type);

                // send position
                lo_send(streamIter->address, "/WONDER/source/position", "iff", i,
                        scenario->sourcesVector[i].pos[0],
                        scenario->sourcesVector[i].pos[1]);

                // send angle
                lo_send(streamIter->address, "/WONDER/source/angle", "if", i,
                        scenario->sourcesVector[i].angle);

                // send name
                lo_send(streamIter->address, "/WONDER/source/name", "is", i,
                        scenario->sourcesVector[i].name.c_str());

                // send groupID
                lo_send(streamIter->address, "/WONDER/source/groupID", "ii", i,
                        scenario->sourcesVector[i].groupID);

                // send color
                lo_send(streamIter->address, "/WONDER/source/color", "iiii", i,
                        scenario->sourcesVector[i].color[0],
                        scenario->sourcesVector[i].color[1],
                        scenario->sourcesVector[i].color[2]);

                // send doppler effect
                lo_send(streamIter->address, "/WONDER/source/dopplerEffect", "ii", i,
                        scenario->sourcesVector[i].dopplerEffect);

                // send rotation direction
                lo_send(streamIter->address, "/WONDER/source/rotationDirection", "ii", i,
                        scenario->sourcesVector[i].invertedRotationDirection);

                // send scaling direction
                lo_send(streamIter->address, "/WONDER/source/scalingDirection", "ii", i,
                        scenario->sourcesVector[i].invertedScalingDirection);
            }
        } else {
            // source is not active in scenario, so then
            // send deactivate source command to all streams but
            // only if source was active in scenario

            // renderStream
            for (streamIter = renderStream->begin(); streamIter != renderStream->end();
                 ++streamIter) {
                // deactivate it
                lo_send(streamIter->address, "/WONDER/source/deactivate", "i", i);
            }

            // visualStream
            for (streamIter = visualStream->begin(); streamIter != visualStream->end();
                 ++streamIter) {
                // deactivate it
                lo_send(streamIter->address, "/WONDER/source/deactivate", "i", i);
            }
        }
    }

    // send all group data to the streams and update the scenario
    for (int i = 0; i < (int)scenario->sourceGroupsVector.size(); ++i) {
        int id = i + 1;

        // send data about active groups
        // if group wasn't active, then send activate first
        if (scenario->sourceGroupsVector[i].active) {
            // send to stream receivers (visual)
            for (streamIter = visualStream->begin(); streamIter != visualStream->end();
                 ++streamIter) {
                // if source is not already active in scenario then activate it
                if (!oldScenario.sourceGroupsVector[i].active) {
                    lo_send(streamIter->address, "/WONDER/group/activate", "i", id);
                }

                // send position
                lo_send(streamIter->address, "/WONDER/group/position", "iff", id,
                        scenario->sourceGroupsVector[i].pos[0],
                        scenario->sourceGroupsVector[i].pos[1]);

                // send color
                lo_send(streamIter->address, "/WONDER/group/color", "iiii", id,
                        scenario->sourceGroupsVector[i].color[0],
                        scenario->sourceGroupsVector[i].color[1],
                        scenario->sourceGroupsVector[i].color[2]);
            }
        } else {
            // visualStream
            for (streamIter = visualStream->begin(); streamIter != visualStream->end();
                 ++streamIter) {
                // if group was active then deactivate it
                if (oldScenario.sourcesVector[i].active) {
                    lo_send(streamIter->address, "/WONDER/group/deactivate", "i", id);
                }
            }
        }
    }

    return 0;
}

int Cwonder::renameSnapshot(int snapshotID, string name) {
    if (!checkScenario()) { return 5; }

    ostringstream st;
    st << snapshotID;

    int ret = project->renameSnapshot(snapshotID, name);

    if (ret == 1) {
        returnString = "snapshot " + st.str() + " does not exist.";
        return 1;  // snapshot does not exist
    } else if (ret == 2) {
        returnString = "validation of document failed.";
        return 2;
    }

    returnString = "rename snapshot " + st.str() + " ok. ";

    return 0;
}

int Cwonder::copySnapshot(int fromID, int toID) {
    if (!checkScenario()) { return 5; }

    ostringstream st;

    int ret = project->copySnapshot(fromID, toID);

    if (ret == 1) {
        st.str("");
        st << "[E-copySnapshot] \"copy from\" snapshot=" << fromID << " does not exist.";
        st.str("");
        st << fromID;
        returnString = "\"copy from\" snapshot " + st.str() + " does not exist.";
        return 1;  // source snapshot does not exist
    } else if (ret == 2) {
        st.str("");
        st << "[E-copySnapshot] \"copy to\" snapshot =" << toID << " does already exist.";
        st.str("");
        st << toID;
        returnString = "\"copy to\" snapshot " + st.str() + " does already exist.";
        return 2;
    } else if (ret == 3) {
        st.str("");
        st << "[E-copySnapshot] error adding copied snapshot to the project.";
        st.str("");
        st << toID;
        returnString = "error adding copied snapshot " + st.str() + " to the project.";
        return 3;
    }

    st.str("");
    st << "copy snapshot=" << fromID << " to snapshot=" << toID << " ok.";

    returnString = "copy snapshot " + st.str() + " ok.";

    return 0;
}

int Cwonder::setSourceActive(lo_address from, int sourceID, bool active) {
    if (!checkScenario()) { return 5; }

    ostringstream st;

    st << (active ? "activated " : "deactivated ") << "source=";

    if (sourceID == -1) {
        st << "all";
    } else {
        st << sourceID;
    }

    // test with -1, this means "all sources"
    if (sourceID >= -1 && sourceID < (int)scenario->sourcesVector.size()) {
        if (active) {
            // send to stream receivers (render)
            for (streamIter = renderStream->begin(); streamIter != renderStream->end();
                 ++streamIter) {
                lo_send(streamIter->address, "/WONDER/source/activate", "i", sourceID);
            }

            // send to stream receivers (visual)
            for (streamIter = visualStream->begin(); streamIter != visualStream->end();
                 ++streamIter) {
                if (!issame(from, streamIter->address)) {
                    lo_send(streamIter->address, "/WONDER/source/activate", "i",
                            sourceID);
                }
            }

            // update scenario
            scenario->activateSource(sourceID);
        } else {  // deactivate
            // send to stream receivers (render)
            for (streamIter = renderStream->begin(); streamIter != renderStream->end();
                 ++streamIter) {
                lo_send(streamIter->address, "/WONDER/source/deactivate", "i", sourceID);
            }

            // send to stream receivers (visual)
            for (streamIter = visualStream->begin(); streamIter != visualStream->end();
                 ++streamIter) {
                if (!issame(from, streamIter->address)) {
                    lo_send(streamIter->address, "/WONDER/source/deactivate", "i",
                            sourceID);
                }
            }

            // update scenario
            scenario->deactivateSource(sourceID);
        }

        returnString = st.str() + " ok.";
    } else {
        returnString = st.str() + " not ok, invalid id.";
        return 1;  // source does not exist
    }

    return 0;
}

int Cwonder::setSourceName(lo_address from, int sourceID, string name) {
    if (!checkScenario()) { return 5; }

    ostringstream st;

    if (sourceID >= 0 && sourceID < (int)scenario->sourcesVector.size()
        && scenario->sourcesVector[sourceID].active) {
        // send to stream receivers (visual)
        for (streamIter = visualStream->begin(); streamIter != visualStream->end();
             ++streamIter) {
            if (!issame(from, streamIter->address)) {
                lo_send(streamIter->address, "/WONDER/source/name", "is", sourceID,
                        name.c_str());
            }
        }

        // update scenario
        scenario->sourcesVector[sourceID].name = name;

        st << "sourceID " << sourceID << ", name " << name << " ok.";
        returnString = st.str();
    } else {
        st << "sourceID " << sourceID << ", name " << name << " not ok. Invalid id.";
        returnString = st.str();
        return 1;  // source does not exist
    }

    return 0;
}

int Cwonder::setSourceType(lo_address from, int sourceID, int type, float timestamp) {
    if (!checkScenario()) { return 5; }

    ostringstream st;

    if (sourceID >= 0 && sourceID < (int)scenario->sourcesVector.size()
        && scenario->sourcesVector[sourceID].active) {
        if (timestamp == PLAYNOW) {
            // send to stream receivers (render)
            for (streamIter = renderStream->begin(); streamIter != renderStream->end();
                 ++streamIter) {
                lo_send(streamIter->address, "/WONDER/source/type", "ii", sourceID, type);
            }

            // send to stream receivers (visual)
            for (streamIter = visualStream->begin(); streamIter != visualStream->end();
                 ++streamIter) {
                if (!issame(from, streamIter->address)) {
                    lo_send(streamIter->address, "/WONDER/source/type", "ii", sourceID,
                            type);
                }
            }

            // update scenario
            scenario->sourcesVector[sourceID].type = type;
        } else {  // ! PLAYNOW
            // put the event on the event stack
            Event* event     = new Event();
            event->oscpath   = "/WONDER/source/type";
            event->id        = sourceID;
            event->type      = type;
            event->timestamp = nowTime;
            event->timestamp.addsec(timestamp);

            timeLine.addevent(event);
        }

        st << "sourceID " << sourceID << ", type " << type << " ok.";
        returnString = st.str();
    } else {
        st << "sourceID " << sourceID << ", type " << type << " not ok. Invalid id.";
        returnString = st.str();
        return 1;  // source does not exist
    }

    return 0;
}

int Cwonder::setSourcePosition(lo_address from, int sourceID, float x, float y,
                               float duration, float timestamp) {
    if (!checkScenario()) { return 5; }

    ostringstream st;
    float z_default = 2;

    if (sourceID >= 0 && sourceID < (int)scenario->sourcesVector.size()
        && scenario->sourcesVector[sourceID].active) {
        if (timestamp == PLAYNOW) {
            // play event now

            // send to stream receivers (render)
            for (streamIter = renderStream->begin(); streamIter != renderStream->end();
                 ++streamIter) {
                lo_send(streamIter->address, "/WONDER/source/position", "ifff", sourceID,
                        x, y, duration);
            }

            // send to stream receivers (visual)
            for (streamIter = visualStream->begin(); streamIter != visualStream->end();
                 ++streamIter) {
                if (!issame(from, streamIter->address)) {
                    lo_send(streamIter->address, "/WONDER/source/position", "iff",
                            sourceID, x, y);
                }
            }

            // update scenario
            scenario->sourcesVector[sourceID].pos[0] = x;
            scenario->sourcesVector[sourceID].pos[1] = y;
            scenario->sourcesVector[sourceID].pos[2] = z_default;
        } else {  // !PLAYNOW
            // put the event on the event stack
            Event* event     = new Event();
            event->oscpath   = "/WONDER/source/position";
            event->id        = sourceID;
            event->pos[0]    = x;
            event->pos[1]    = y;
            event->pos[2]    = z_default;
            event->duration  = duration;
            event->timestamp = nowTime;
            event->from =
                lo_address_new(lo_address_get_hostname(from), lo_address_get_port(from));
            event->timestamp.addsec(timestamp);

            timeLine.addevent(event);
        }

        st << "sourceID " << sourceID << ", position " << x << ", " << y << " ok.";
        returnString = st.str();
    } else {
        st << "sourceID " << sourceID << ", position " << x << ", " << y
           << " not ok. Invalid id.";
        returnString = st.str();
        return 1;  // source does not exist
    }

    return 0;
}

int Cwonder::setSourcePosition3D(lo_address from, int sourceID, float x, float y, float z,
                                 float duration, float timestamp) {
    if (!checkScenario()) { return 5; }

    ostringstream st;

    if (sourceID >= 0 && sourceID < (int)scenario->sourcesVector.size()
        && scenario->sourcesVector[sourceID].active) {
        if (timestamp == PLAYNOW) {
            // play event now

            // send to stream receivers (render)
            for (streamIter = renderStream->begin(); streamIter != renderStream->end();
                 ++streamIter) {
                lo_send(streamIter->address, "/WONDER/source/position3D", "iffff",
                        sourceID, x, y, z, duration);
            }

            // send to stream receivers (visual)
            for (streamIter = visualStream->begin(); streamIter != visualStream->end();
                 ++streamIter) {
                if (!issame(from, streamIter->address)) {
                    lo_send(streamIter->address, "/WONDER/source/position", "iff",
                            sourceID, x, y);
                }
            }

            // update scenario
            scenario->sourcesVector[sourceID].pos[0] = x;
            scenario->sourcesVector[sourceID].pos[1] = y;
            scenario->sourcesVector[sourceID].pos[2] = z;
        } else {  // !PLAYNOW
            // put the event on the event stack
            Event* event     = new Event();
            event->oscpath   = "/WONDER/source/position3D";
            event->id        = sourceID;
            event->pos[0]    = x;
            event->pos[1]    = y;
            event->pos[2]    = z;
            event->duration  = duration;
            event->timestamp = nowTime;
            event->from =
                lo_address_new(lo_address_get_hostname(from), lo_address_get_port(from));
            event->timestamp.addsec(timestamp);

            timeLine.addevent(event);
        }

        st << "sourceID " << sourceID << ", position " << x << ", " << y << ", " << z
           << " ok.";
        returnString = st.str();
    } else {
        st << "sourceID " << sourceID << ", position " << x << ", " << y << ", " << z
           << " not ok. Invalid id.";
        returnString = st.str();
        return 1;  // source does not exist
    }

    return 0;
}

int Cwonder::setSourceAngle(lo_address from, int sourceID, float angle, float duration,
                            float timestamp) {
    if (!checkScenario()) { return 5; }

    ostringstream st;

    if (sourceID >= 0 && sourceID < (int)scenario->sourcesVector.size()
        && scenario->sourcesVector[sourceID].active) {
        if (timestamp == PLAYNOW) {
            // send to stream receivers (render)
            for (streamIter = renderStream->begin(); streamIter != renderStream->end();
                 ++streamIter) {
                lo_send(streamIter->address, "/WONDER/source/angle", "iff", sourceID,
                        angle, duration);
            }

            // send to stream receivers (visual)
            for (streamIter = visualStream->begin(); streamIter != visualStream->end();
                 ++streamIter) {
                if (!issame(from, streamIter->address)) {
                    lo_send(streamIter->address, "/WONDER/source/angle", "if", sourceID,
                            angle);
                }
            }

            // update scenario
            scenario->sourcesVector[sourceID].angle = angle;
        } else {  // ! PLAYNOW
            // put the event on the event stack
            Event* event     = new Event();
            event->oscpath   = "/WONDER/source/angle";
            event->id        = sourceID;
            event->angle     = angle;
            event->duration  = duration;
            event->timestamp = nowTime;
            event->timestamp.addsec(timestamp);

            timeLine.addevent(event);
        }

        st << "sourceID " << sourceID << ", angle " << angle << " ok.";
        returnString = st.str();
    } else {
        st << "sourceID " << sourceID << ", angle " << angle << " not ok. Invalid id.";
        returnString = st.str();
        return 1;  // source does not exist
    }

    return 0;
}

int Cwonder::setSourceGroupID(lo_address from, int sourceID, int groupID) {
    if (!checkScenario()) { return 5; }

    ostringstream st;

    if (sourceID >= 0 && sourceID < (int)scenario->sourcesVector.size()
        && scenario->sourcesVector[sourceID].active) {
        // send to stream receivers (visual)
        for (streamIter = visualStream->begin(); streamIter != visualStream->end();
             ++streamIter) {
            if (!issame(from, streamIter->address)) {
                lo_send(streamIter->address, "/WONDER/source/groupID", "ii", sourceID,
                        groupID);
            }
        }

        // update scenario
        scenario->sourcesVector[sourceID].groupID = groupID;

        st << "sourceID " << sourceID << ", groupID " << groupID << " ok.";
        returnString = st.str();
    } else {
        st << "sourceID " << sourceID << ", groupID " << groupID
           << " not ok. Invalid id.";
        returnString = st.str();
        return 1;  // source does not exist
    }

    return 0;
}

int Cwonder::setSourceColor(lo_address from, int sourceID, int red, int green, int blue) {
    if (!checkScenario()) { return 5; }

    ostringstream st;

    if (sourceID >= 0 && sourceID < (int)scenario->sourcesVector.size()
        && scenario->sourcesVector[sourceID].active) {
        // send to stream receivers (visual)
        for (streamIter = visualStream->begin(); streamIter != visualStream->end();
             ++streamIter) {
            if (!issame(from, streamIter->address)) {
                lo_send(streamIter->address, "/WONDER/source/color", "iiii", sourceID,
                        red, green, blue);
            }
        }

        // update scenario
        scenario->sourcesVector[sourceID].color[0] = red;
        scenario->sourcesVector[sourceID].color[1] = green;
        scenario->sourcesVector[sourceID].color[2] = blue;

        st << "sourceID " << sourceID << ", color " << red << ", " << green << ", "
           << blue << " ok.";
        returnString = st.str();
    } else {
        st << "sourceID " << sourceID << ", color " << red << ", " << green << ", "
           << blue << " not ok. Invalid id.";
        returnString = st.str();
        return 1;  // source does not exist
    }

    return 0;
}

int Cwonder::setSourceRotationDirection(lo_address from, int sourceID, bool invert) {
    if (!checkScenario()) { return 5; }

    ostringstream st;

    if (sourceID >= 0 && sourceID < (int)scenario->sourcesVector.size()
        && scenario->sourcesVector[sourceID].active) {
        // send to stream receivers (visual)
        for (streamIter = visualStream->begin(); streamIter != visualStream->end();
             ++streamIter) {
            if (!issame(from, streamIter->address)) {
                lo_send(streamIter->address, "/WONDER/source/rotationDirection", "ii",
                        sourceID, invert);
            }
        }

        // update scenario
        scenario->sourcesVector[sourceID].invertedRotationDirection = invert;

        st << "sourceID " << sourceID << ", invertedRotationDirection " << invert
           << " ok.";
        returnString = st.str();
    } else {
        st << "sourceID " << sourceID << ", invertedRotationDirection " << invert
           << " not ok. Invalid id.";
        returnString = st.str();
        return 1;  // source does not exist
    }

    return 0;
}

int Cwonder::setSourceScalingDirection(lo_address from, int sourceID, bool invert) {
    if (!checkScenario()) { return 5; }

    ostringstream st;

    if (sourceID >= 0 && sourceID < (int)scenario->sourcesVector.size()
        && scenario->sourcesVector[sourceID].active) {
        // send to stream receivers (visual)
        for (streamIter = visualStream->begin(); streamIter != visualStream->end();
             ++streamIter) {
            if (!issame(from, streamIter->address)) {
                lo_send(streamIter->address, "/WONDER/source/scalingDirection", "ii",
                        sourceID, invert);
            }
        }

        // update scenario
        scenario->sourcesVector[sourceID].invertedScalingDirection = invert;
        st << "sourceID " << sourceID << ", invertedScalingDirection " << invert
           << " ok.";
        returnString = st.str();
    } else {
        st << "sourceID " << sourceID << ", invertedScalingDirection " << invert
           << " not ok. Invalid id.";
        returnString = st.str();
        return 1;  // source does not exist
    }

    return 0;
}

int Cwonder::setSourceDopplerEffect(lo_address from, int sourceID, bool dopplerOn) {
    if (!checkScenario()) { return 5; }

    ostringstream st;

    if (sourceID >= 0 && sourceID < (int)scenario->sourcesVector.size()
        && scenario->sourcesVector[sourceID].active) {
        // send to stream receivers (render)
        for (streamIter = renderStream->begin(); streamIter != renderStream->end();
             ++streamIter) {
            lo_send(streamIter->address, "/WONDER/source/dopplerEffect", "ii", sourceID,
                    dopplerOn);
        }

        // send to stream receivers (visual)
        for (streamIter = visualStream->begin(); streamIter != visualStream->end();
             ++streamIter) {
            if (!issame(from, streamIter->address)) {
                lo_send(streamIter->address, "/WONDER/source/dopplerEffect", "ii",
                        sourceID, dopplerOn);
            }
        }

        // update scenario
        scenario->sourcesVector[sourceID].dopplerEffect = dopplerOn;
        st << "sourceID " << sourceID << ", dopplerEffect " << dopplerOn << " ok.";
        returnString = st.str();
    } else {
        st << "sourceID " << sourceID << ", dopplerEffect " << dopplerOn
           << " not ok. Invalid id.";
        returnString = st.str();
        return 1;  // source does not exist
    }

    return 0;
}

int Cwonder::setListenerPosition(lo_address from, int listenerID, float x, float y) {
    if (!checkScenario()) { return 5; }

    ostringstream st;

    if (listenerID >= 0) {
        // send to stream receivers (render)
        for (streamIter = renderStream->begin(); streamIter != renderStream->end();
             ++streamIter) {
            lo_send(streamIter->address, "/WONDER/listener/position", "iff", listenerID,
                    x, y);
        }

        // send to stream receivers (visual)
        for (streamIter = visualStream->begin(); streamIter != visualStream->end();
             ++streamIter) {
            if (!issame(from, streamIter->address)) {
                lo_send(streamIter->address, "/WONDER/listener/position", "iff",
                        listenerID, x, y);
            }
        }

        st << "listenerID " << listenerID << ", position " << x << ", " << y << " ok.";
        returnString = st.str();
    } else {
        st << "listenerID " << listenerID << ", position " << x << ", " << y
           << " not ok. Invalid id.";
        returnString = st.str();
        return 1;  // listener does not exist
    }

    return 0;
}

int Cwonder::setGroupActive(lo_address from, int groupID, bool active) {
    int id = groupID - 1;

    if (!checkScenario()) { return 5; }

    ostringstream st;

    if (id >= 0 && id < (int)scenario->sourceGroupsVector.size()) {
        if (active) {
            // send to stream receivers (visual)
            for (streamIter = visualStream->begin(); streamIter != visualStream->end();
                 ++streamIter) {
                if (!issame(from, streamIter->address)) {
                    lo_send(streamIter->address, "/WONDER/group/activate", "i", groupID);
                }
            }

            // update scenario
            scenario->activateGroup(groupID);

            st << "groupID " << groupID << " activate ok.";
            returnString = st.str();
        } else {
            // send to stream receivers (visual)
            for (streamIter = visualStream->begin(); streamIter != visualStream->end();
                 ++streamIter) {
                if (!issame(from, streamIter->address)) {
                    lo_send(streamIter->address, "/WONDER/group/deactivate", "i",
                            groupID);
                }
            }

            // update scenario
            scenario->deactivateGroup(groupID);

            st << "groupID " << groupID << " deactivate ok.";
            returnString = st.str();
        }
    } else {
        if (active) {
            st << "groupID " << groupID << " activate not ok, invalid id.";
        } else {
            st << "groupID " << groupID << " deactivate not ok, invalid id.";
        }

        returnString = st.str();
        return 1;  // group does not exist
    }

    return 0;
}

int Cwonder::setGroupColor(lo_address from, int groupID, int red, int green, int blue) {
    int id = groupID - 1;

    if (!checkScenario()) { return 5; }

    ostringstream st;

    if (id >= 0 && id < (int)scenario->sourceGroupsVector.size()
        && scenario->sourceGroupsVector[id].active) {
        // send to stream receivers (visual)
        for (streamIter = visualStream->begin(); streamIter != visualStream->end();
             ++streamIter) {
            if (!issame(from, streamIter->address)) {
                lo_send(streamIter->address, "/WONDER/group/color", "iiii", groupID, red,
                        green, blue);
            }
        }

        // update scenario
        scenario->sourceGroupsVector[id].color[0] = red;
        scenario->sourceGroupsVector[id].color[1] = green;
        scenario->sourceGroupsVector[id].color[2] = blue;

        st << "groupID " << groupID << ", color " << red << ", " << green << ", " << blue
           << " ok.";
        returnString = st.str();
    } else {
        st << "groupID " << groupID << ", color " << red << ", " << green << ", " << blue
           << " not ok."
           << " Invalid id.";
        returnString = st.str();
        return 1;  // group does not exist
    }

    return 0;
}

int Cwonder::setGroupPosition(lo_address from, int groupID, float x, float y) {
    int id = groupID - 1;

    if (!checkScenario()) { return 5; }

    ostringstream st;

    if (id >= 0 && id < (int)scenario->sourceGroupsVector.size()
        && scenario->sourceGroupsVector[id].active) {
        // send to stream receivers (visual)
        for (streamIter = visualStream->begin(); streamIter != visualStream->end();
             ++streamIter) {
            if (!issame(from, streamIter->address)) {
                lo_send(streamIter->address, "/WONDER/group/position", "iff", groupID, x,
                        y);
            }
        }

        // update scenario
        scenario->sourceGroupsVector[id].pos[0] = x;
        scenario->sourceGroupsVector[id].pos[1] = y;
        st << "groupID " << groupID << ", position " << x << ", " << y << " ok.";
        returnString = st.str();
    } else {
        st << "groupID " << groupID << ", position " << x << ", " << y
           << " not ok. Invalid id.";
        returnString = st.str();
        return 1;  // group does not exist
    }

    return 0;
}

int Cwonder::renderStreamConnect(string host, string port, string name) {
    int ret                     = renderStream->connect(host, port, name);
    lo_address newClientAddress = (--renderStream->end())->address;

    // send number of allowed sources
    lo_send(newClientAddress, "/WONDER/global/maxNoSources", "i",
            cwonderConf->maxNoSources);

    // send renderpolygon
    // XXX:(CAREFUL! this might easily exceed the allowed messagesize of liblo,
    // needs future improvement like when sending /project/scenario)
    lo_message renderPolygonMessage = lo_message_new();

    lo_message_add_string(renderPolygonMessage, cwonderConf->roomName.c_str());
    lo_message_add_int32(renderPolygonMessage, cwonderConf->renderPolygonPoints.size());

    // iterate over list of renderPolygonPoints and add x,y and z coordinats to message
    vector<Vector3D>::const_iterator it;

    for (it = cwonderConf->renderPolygonPoints.begin();
         it != cwonderConf->renderPolygonPoints.end(); ++it) {
        lo_message_add_float(renderPolygonMessage, (*it)[0]);
        lo_message_add_float(renderPolygonMessage, (*it)[1]);
        lo_message_add_float(renderPolygonMessage, (*it)[2]);
    }

    lo_send_message(newClientAddress, "/WONDER/global/renderpolygon",
                    renderPolygonMessage);
    lo_message_free(renderPolygonMessage);

    // if in basic mode activate all sources for rendering and let them use their default
    // values
    if (cwonderConf->basicMode) {
        for (int i = 0; i < cwonderConf->maxNoSources; i++) {
            lo_send(newClientAddress, "/WONDER/source/activate", "i", i);
        }
    } else {
        // send the current status of the sources (even though the host is (still) on the
        // list) but only if a scenario is there (i.e. project loaded/created)
        if (scenario) {
            for (int i = 0; i < (int)scenario->sourcesVector.size(); ++i) {
                Source* source = &(scenario->sourcesVector[i]);

                if (source->active) {
                    lo_send(newClientAddress, "/WONDER/source/activate", "i", i);
                    lo_send(newClientAddress, "/WONDER/source/type", "ii", i,
                            source->type);
                    lo_send(newClientAddress, "/WONDER/source/angle", "if", i,
                            source->angle);
                    lo_send(newClientAddress, "/WONDER/source/position", "iff", i,
                            source->pos[0], source->pos[1]);
                    lo_send(newClientAddress, "/WONDER/source/position3D", "ifff", i,
                            source->pos[0], source->pos[1], source->pos[2]);
                    lo_send(newClientAddress, "/WONDER/source/dopplerEffect", "ii", i,
                            (int)source->dopplerEffect);
                } else {
                    lo_send(newClientAddress, "/WONDER/source/deactivate", "i", i);
                }
            }
        }
    }

    // send information about newly connected stream client on the visual stream
    notifyVisualStreamOfNewStreamClient(host, port, name);

    if (ret == 1) {
        returnString = "reconnected";
    } else {
        returnString = "ok";
    }

    return 0;
}

int Cwonder::visualStreamConnect(string host, string port, string name) {
    // in basicmode only the renderStream is active
    if (cwonderConf->basicMode) {
        returnString = "not allowed in basic mode";
        return -1;
    }

    int ret                     = visualStream->connect(host, port, name);
    lo_address newClientAddress = (--visualStream->end())->address;

    // send number of allowed sources
    lo_send(newClientAddress, "/WONDER/global/maxNoSources", "i",
            cwonderConf->maxNoSources);

    // send renderpolygon
    // XXX:(CAREFUL! this might easily exceed the allowed messagesize of liblo,
    // needs future improvement like when sending /project/scenario)
    lo_message renderPolygonMessage = lo_message_new();

    lo_message_add_string(renderPolygonMessage, cwonderConf->roomName.c_str());
    lo_message_add_int32(renderPolygonMessage, cwonderConf->renderPolygonPoints.size());

    // iterate over list of renderPolygonPoints and add x,y and z coordinats to message
    vector<Vector3D>::const_iterator it;

    for (it = cwonderConf->renderPolygonPoints.begin();
         it != cwonderConf->renderPolygonPoints.end(); ++it) {
        lo_message_add_float(renderPolygonMessage, (*it)[0]);
        lo_message_add_float(renderPolygonMessage, (*it)[1]);
        lo_message_add_float(renderPolygonMessage, (*it)[2]);
    }

    lo_send_message(newClientAddress, "/WONDER/global/renderPolygon",
                    renderPolygonMessage);
    lo_message_free(renderPolygonMessage);

    // write scenario to DOM
    project->writeProjectToDOM();

    // send information about the whole project
    // TODO: this may exceed the sizelimit of an OSC message, slice it up
    string p = project->show();
    lo_send(newClientAddress, "/WONDER/project/xmlDump", "is", p.empty(), p.c_str());

    // send information about newly connected client on the visual stream
    notifyVisualStreamOfNewStreamClient(host, port, name);

    // send information about all other connected stream clients to the newly connected
    // visual stream client
    for (streamIter = renderStream->begin(); streamIter != renderStream->end();
         ++streamIter) {
        sendStreamClientDataTo(newClientAddress, *streamIter);
    }

    for (streamIter = visualStream->begin(); streamIter != visualStream->end();
         ++streamIter) {
        if (!issame(newClientAddress, streamIter->address)) {
            sendStreamClientDataTo(newClientAddress, *streamIter);
        }
    }

    for (streamIter = timerStream->begin(); streamIter != timerStream->end();
         ++streamIter) {
        sendStreamClientDataTo(newClientAddress, *streamIter);
    }

    if (ret == 1) {
        returnString = "reconnected";
    } else {
        returnString = "ok";
    }

    return 0;
}

int Cwonder::timerStreamConnect(string host, string port, string name) {
    int ret = timerStream->connect(host, port, name);

    // send the actual frametime
    lo_send((--timerStream->end())->address, "/WONDER/frametime", "i", nowTime.getTime());

    // send information about newly connected stream client on the visual stream
    notifyVisualStreamOfNewStreamClient(host, port, name);

    if (ret == 1) {
        returnString = "reconnected";
    } else {
        returnString = "ok";
    }

    return 0;
}

void Cwonder::notifyVisualStreamOfNewStreamClient(string host, string port, string name) {
    for (streamIter = visualStream->begin(); streamIter != visualStream->end();
         ++streamIter) {
        lo_send(streamIter->address, "/WONDER/stream/connected", "sss", host.c_str(),
                port.c_str(), name.c_str());
    }
}

void Cwonder::sendStreamClientDataTo(lo_address targetAdress,
                                     OSCStreamClient& clientData) {
    lo_send(targetAdress, "/WONDER/stream/connected", "sss", clientData.host.c_str(),
            clientData.port.c_str(), clientData.name.c_str());
}

void Cwonder::notifyVisualStreamOfDeadStreamClients(
    list<OSCStreamClient>& deadStreamClients) {
    // send information about dead stream clients on the visual stream
    list<OSCStreamClient>::iterator clients;

    for (clients = deadStreamClients.begin(); clients != deadStreamClients.end();
         ++clients) {
        for (streamIter = visualStream->begin(); streamIter != visualStream->end();
             ++streamIter) {
            lo_send(streamIter->address, "/WONDER/stream/disconnected", "sss",
                    clients->host.c_str(), clients->port.c_str(), clients->name.c_str());
        }
    }
}

void Cwonder::scheduler(int currtime) {
    // We receive an integer from jackframetime because OSC has no native support for
    // unsigned integers. We cast it back to an uint with no data lost
    nowTime.setTime((wonder_frames_t)currtime);

    Event* ev;
    float z_default = 2;

    for (int i = 0; i < (int)timeLine.getsize(); ++i) {
        ev = timeLine.getevent(nowTime);  // get event which is smaller than timestamp

        if (ev == nullptr) {
            break;  // no event found that is smaller than the timestamp
        } else {
            if (ev->oscpath == "/WONDER/source/position") {
                // send to stream receivers (render)
                for (streamIter = renderStream->begin();
                     streamIter != renderStream->end(); ++streamIter) {
                    lo_send(streamIter->address, "/WONDER/source/position", "ifff",
                            ev->id, ev->pos[0], ev->pos[1], ev->duration);
                }

                // send to stream receivers (visual)
                for (streamIter = visualStream->begin();
                     streamIter != visualStream->end(); ++streamIter) {
                    if (!issame(ev->from, streamIter->address)) {
                        lo_send(streamIter->address, "/WONDER/source/position", "iff",
                                ev->id, ev->pos[0], ev->pos[1]);
                    }
                }

                // update scenario
                scenario->sourcesVector[ev->id].pos[0] = ev->pos[0];
                scenario->sourcesVector[ev->id].pos[1] = ev->pos[1];
                scenario->sourcesVector[ev->id].pos[2] = z_default;
            } else if (ev->oscpath == "/WONDER/source/position3D") {
                // send to stream receivers (render)
                for (streamIter = renderStream->begin();
                     streamIter != renderStream->end(); ++streamIter) {
                    lo_send(streamIter->address, "/WONDER/source/position3D", "iffff",
                            ev->id, ev->pos[0], ev->pos[1], ev->pos[2], ev->duration);
                }

                // send to stream receivers (visual)
                for (streamIter = visualStream->begin();
                     streamIter != visualStream->end(); ++streamIter) {
                    if (!issame(ev->from, streamIter->address)) {
                        lo_send(streamIter->address, "/WONDER/source/position", "iff",
                                ev->id, ev->pos[0], ev->pos[1]);
                    }
                }

                // update scenario
                scenario->sourcesVector[ev->id].pos[0] = ev->pos[0];
                scenario->sourcesVector[ev->id].pos[1] = ev->pos[1];
                scenario->sourcesVector[ev->id].pos[2] = ev->pos[2];
            } else if (ev->oscpath == "/WONDER/source/type") {
                // send to stream receivers (render)
                for (streamIter = renderStream->begin();
                     streamIter != renderStream->end(); ++streamIter) {
                    lo_send(streamIter->address, "/WONDER/source/type", "ii", ev->id,
                            ev->type);
                }

                // send to stream receivers (visual)
                for (streamIter = visualStream->begin();
                     streamIter != visualStream->end(); ++streamIter) {
                    if (!issame(ev->from, streamIter->address)) {
                        lo_send(streamIter->address, "/WONDER/source/type", "ii", ev->id,
                                ev->type);
                    }
                }

                // update scenario
                scenario->sourcesVector[ev->id].type = ev->type;
            } else if (ev->oscpath == "/WONDER/source/angle") {
                // send to stream receivers (render)
                for (streamIter = renderStream->begin();
                     streamIter != renderStream->end(); ++streamIter) {
                    lo_send(streamIter->address, "/WONDER/source/angle", "iff", ev->id,
                            ev->angle, ev->duration);
                }

                // send to stream receivers (visual)
                for (streamIter = visualStream->begin();
                     streamIter != visualStream->end(); ++streamIter) {
                    if (!issame(ev->from, streamIter->address)) {
                        lo_send(streamIter->address, "/WONDER/source/angle", "if", ev->id,
                                ev->angle);
                    }
                }

                // update scenario
                scenario->sourcesVector[ev->id].angle = ev->angle;
            }

            // delete the Event sent
            delete ev;
        }
    }

    // send timer information to the timer stream
    for (streamIter = timerStream->begin(); streamIter != timerStream->end();
         ++streamIter) {
        lo_send(streamIter->address, "/WONDER/frametime", "i", (wonder_frames_t)currtime);
    }

    // make sure buffer for deletable stream clients is empty
    deadStreamClients.clear();

    // ping renderStream
    if (renderStream->hasClients()) {
        if ((nowTime - lastRenderPing) >= (wonder_frames_t)cwonderConf->pingRate) {
            renderStream->ping(deadStreamClients);

            // send information about dead clients to visual stream
            if (!deadStreamClients.empty()) {
                notifyVisualStreamOfDeadStreamClients(deadStreamClients);
            }

            deadStreamClients.clear();

            lastRenderPing.setTime(nowTime.getTime());
        }
    }

    // ping visualStream
    if (visualStream->hasClients()) {
        if ((nowTime - lastVisualPing) >= (wonder_frames_t)cwonderConf->pingRate) {
            visualStream->ping(deadStreamClients);

            // send information about dead clients to visual stream
            if (!deadStreamClients.empty()) {
                notifyVisualStreamOfDeadStreamClients(deadStreamClients);
            }

            deadStreamClients.clear();

            lastVisualPing.setTime(nowTime.getTime());
        }
    }

    // ping timerStream
    if (timerStream->hasClients()) {
        if ((nowTime - lastTimerPing) >= (wonder_frames_t)cwonderConf->pingRate) {
            timerStream->ping(deadStreamClients);

            // send information about dead clients to visual stream
            if (!deadStreamClients.empty()) {
                notifyVisualStreamOfDeadStreamClients(deadStreamClients);
            }

            deadStreamClients.clear();

            lastTimerPing.setTime(nowTime.getTime());
        }
    }
}

void Cwonder::sendScenario() {
    if (!checkScenario()) { return; }

    lo_address address = nullptr;

    for (int i = 0; i < (int)scenario->sourcesVector.size(); ++i) {
        Source* source = &(scenario->sourcesVector[i]);

        // send to stream receivers (render)
        for (streamIter = renderStream->begin(); streamIter != renderStream->end();
             ++streamIter) {
            address = streamIter->address;

            // (de)activate source
            if (source->active) {
                lo_send(address, "/WONDER/source/activate", "i", i);

                // send position
                lo_send(address, "/WONDER/source/position3D", "ifff", i, source->pos[0],
                        source->pos[1], source->pos[2]);

                // send type
                lo_send(address, "/WONDER/source/type", "ii", i, source->type);

                // send angle
                lo_send(address, "/WONDER/source/angle", "iff", i, source->angle);

                // send doppler
                lo_send(address, "/WONDER/source/dopplerEffect", "ii", i,
                        source->dopplerEffect);
            } else {
                lo_send(address, "/WONDER/source/deactivate", "i", i);
            }
        }

        // send to stream receivers (visual)
        for (streamIter = visualStream->begin(); streamIter != visualStream->end();
             ++streamIter) {
            address = streamIter->address;

            // (de)activate source
            if (source->active) {
                lo_send(address, "/WONDER/source/activate", "i", i);

                // send position
                lo_send(address, "/WONDER/source/position", "iff", i, source->pos[0],
                        source->pos[1]);

                // send type
                lo_send(address, "/WONDER/source/type", "ii", i, source->type);

                // send angle
                lo_send(address, "/WONDER/source/angle", "if", i, source->angle);

                // send name
                lo_send(address, "/WONDER/source/name", "is", i, source->name.c_str());

                // send groupID
                lo_send(address, "/WONDER/source/groupID", "ii", i, source->groupID);

                // send color
                lo_send(address, "/WONDER/source/color", "iiii", i, source->color[0],
                        source->color[1], source->color[2]);

                // send rotationDirection
                lo_send(address, "/WONDER/source/rotationDirection", "ii", i,
                        source->invertedRotationDirection);

                // send scalingDirection
                lo_send(address, "/WONDER/source/scalingDirection", "ii", i,
                        source->invertedScalingDirection);

                // send doppler
                lo_send(address, "/WONDER/source/dopplerEffect", "ii", i,
                        source->dopplerEffect);
            } else {
                lo_send(address, "/WONDER/source/deactivate", "i", i);
            }
        }
    }

    for (auto& i : scenario->sourceGroupsVector) {
        SourceGroup* group = &i;

        // send to stream receivers (visual)
        for (streamIter = visualStream->begin(); streamIter != visualStream->end();
             ++streamIter) {
            lo_address address = streamIter->address;
            int id             = group->id;

            // (de)activate group
            if (group->active) {
                // activate if
                lo_send(address, "/WONDER/group/activate", "i", id);

                // send position
                lo_send(address, "/WONDER/group/position", "iff", id, group->pos[0],
                        group->pos[1]);

                // send color
                lo_send(address, "/WONDER/group/color", "iiii", id, group->color[0],
                        group->color[1], group->color[2]);
            } else {
                lo_send(address, "/WONDER/group/deactivate", "i", id);
            }
        }
    }
}

bool Cwonder::checkScenario() {
    if (!scenario) {
        returnString = "error: no project loaded/created. ";
        return false;
    }

    return true;
}
