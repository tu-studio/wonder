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

#include <iosfwd>
#include <libxml++/libxml++.h>
#include <list>
#include <string>
#include <vector>

#include "vector3d.h"

namespace xmlpp {
    class Document;
    class Node;
    class Element;
}



// A sound source belonging to a wonder scenario
class Source {

public:
    Source();
    ~Source();

    // do the sources have the same id
    bool operator == (Source const& other) const {
        return (id == other.id);
    }

    // assignement copies only data, not the node
    Source& operator = (Source const& other);

    // unique identifier of the source ( starting at 0 )
    int id;

    // type of the source: 0 = plane wave, 1 = point source
    int type;

    // name of the source
    std::string name;

    // if the source is active ( audible and written to the DOM representation )
    bool active;

    // x/y/z-coordinates of the source
    Vector3D pos;

    // angle of the source, when source is a plane wave
    float angle;

    // id of group the source belongs to ( 0 if no group )
    int groupID;

    // color in RGB ( values from 0 to 255 )
    int color[ 3 ];

    // simulate doppler effect
    bool dopplerEffect;

    // needed for xwonder
    // if source is part of a group, the direction of rotation and scaling can be inverted
    bool invertedRotationDirection;
    bool invertedScalingDirection;

    // node in the DOM representation
    xmlpp::Node* node;

private:
    Glib::ustring nodename;
    Glib::ustring attribName;

}; // class Source



// A groupobject for sources belonging to a wonder scenario
class SourceGroup {

public:
    SourceGroup();
    ~SourceGroup();

    // do the sources have the same id
    bool operator == (SourceGroup const& other) const {
        return (id == other.id);
    }

    // assignement copies only data, not the node
    SourceGroup& operator = (SourceGroup const& other);

    // unique identifier of the group ( starting at 1 )
    int id;

    // if the group is active
    bool active;

    // x/y/z-coordinates of the group
    Vector3D pos;

    // color in RGB (values from 0 to 255)
    int color[ 3 ];

    // node in the DOM representation
    xmlpp::Node* node;

private:
    Glib::ustring nodeName;
    Glib::ustring attribName;

}; // class SourceGroup



// A scenario ( static configuration of sound sources )
class Scenario {

public:
    Scenario(int maxNoSources);

    Scenario(xmlpp::Node* node, int maxNoSources);

    Scenario(const Scenario& other, xmlpp::Node* node = NULL);

    ~Scenario();

    // (de)activate the source (make it audible and let it be written to the DOM representation)
    void activateSource(int id);
    void deactivateSource(int id);

    // (de)activate the group
    void activateGroup(int id);
    void deactivateGroup(int id);

    // returns a string containing the source info
    std::string show();

    // do the scenarios have the same id
    bool operator==(Scenario const& other) const {
        return (id == other.id);
    }

    // write the data representation back to the DOM representation
    void writeToDOM();

    // unique identifier
    int id;

    // name of the scenario
    std::string name;

    // node in the DOM representation
    xmlpp::Node* node;

    // maximum number of sources
    int maxNoSources;

    // CAREFUL: This design is inherently unsafe! Every user of class Scenario must check
    //          index into sourcesVector!
    // the array of sources belonging to this scenario
    std::vector< Source > sourcesVector;

    // CAREFUL: This design is inherently unsafe! Every user of class Scenario must check
    //          index into sourceGroupVector!
    // the array of sources belonging to this scenario
    std::vector< SourceGroup > sourceGroupsVector;

    // internal exception class for the case when too many sources should be read from a projectfile
    class tooManySources {};

private:
    // read the scenario from the DOM representation
    void readFromDOM();

    // just for parsing purposes
    Glib::ustring elementName;
    Glib::ustring attribName;

}; // class Scenario



// A wonder project that can be saved/loaded to/from a xml file
class Project {

public:
    Project(int maxNoSources);
    ~Project();

    // take a snapshot of the current scenario
    //  0 = success, 1 =  cannot get root node , -1 = no scenario present or invalid id
    int takeSnapshot(int snapshotID, std::string name = "");

    // recall a previously taken snapshot
    // 0 = success, 1 = cannot get root node, -1 snapshot does not exist
    int recallSnapshot(int snapshotID);


    // delete snapshot from the project
    // 0 = success, 1 = snapshot does not exist, 2 = error on validation
    int deleteSnapshot(int snapshotID);

    // deep copy a snapshot
    // 0 = success, 1 = source snapshot does not exist, 2 = target snapshot already exist
    int copySnapshot(int fromID, int toID);

    // rename an existing snapshot
    // 0 = success, 1 = snapshot does not exist, 2 = error validation
    int renameSnapshot(int snapshotID, std::string name);


    // set the path to where the DTD can be found against which to valdiate projects
    void setDtdPath(std::string dtdPath) {
        this->dtdPath = dtdPath;
    }

    // creates a new project DOM representation with the appropriate fileheader.
    //  calls createScenario()
    void createProject();

    // returns access to the scenario
    Scenario* getScenario();

    // parse a xml file and create the DOM repressentation
    // this parsing will remove all textnodes below rootnode, because otherwise
    // the linebreaks will be messed up in the xml file which will be written by
    // Document::write_to_file_formatted().
    // calls readProjectFromDOM()
    void readFromFile(Glib::ustring);

    // write the DOM representation to a xml file
    void writeToFile(std::string fileName);

    // write class data back to the DOM representation, set project header and
    //  calls writeSnapshotsToDOM().
    void writeProjectToDOM();

    // get class data from the DOM representation.
    // calls readSnapshotsFromDOM() to get the snapshot data of the project.
    void readProjectFromDOM();

    // returns a string containing the scenario in xml representation
    //  additionaly contained in the return string are the shnaphot headers
    //  without the source data.
    //  ( this is somewhat a quickfix to prevent exceedance of the OSC
    //    package size (~32kB), this may still happen with large projects with
    //    lots of snapshots )
    std::string show();

private:
    // creates a clean scenario for a new project
    int createScenario();

    // remove all the snapshots from the list
    //  (this also clears the scenario, i.e. snapshot #0)
    void clearSnaphots();

    // write snapshots data back to the DOM representation
    void writeSnapshotsToDOM();

    // get scenario/snapshot specific data from the DOM representation.
    //  this allocates memory for new Scenarios and calls their constructor which
    //  reads data (through Scenario::readFromDOM()) from the given node.
    void readSnapshotsFromDOM(const xmlpp::Node* node, const Glib::ustring& xpath);

    // validates the DOM representation against a DTD
    int validate();

public:
    // name of the project
    std::string name;

    // path to where the DTD can be found
    std::string dtdPath;

    int maxNoSources;

    // scenario is snapshot with id 0
    std::list<Scenario*> snapshots;

private:
    // pointer to the scenario, i.e. snapshots.front()
    Scenario* scenario;

    xmlpp::Document* doc;
    xmlpp::Element*  rootNode;
    xmlpp::DomParser parser;
    std::string      dtdFile;
    bool             fromFile;

    // just for parsing purposes
    Glib::ustring attribName;

}; // class Project
