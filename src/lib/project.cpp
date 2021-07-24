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

#include "project.h"

#include <exception>
#include <iostream>
#include <sstream>

#include "wonder_path.h"

using std::cerr;
using std::cout;
using std::endl;
using std::exception;
using std::istringstream;
using std::list;
using std::ostringstream;
using std::string;

using xmlpp::Attribute;
// using xmlpp::Node::NodeSet;
using xmlpp::Document;
using xmlpp::DtdValidator;
using xmlpp::Element;
using xmlpp::Node;

//---------------------------------Project------------------------------------//

Project::Project(int maxNoSources) {
    fromFile = true;

    rootNode = nullptr;
    doc      = nullptr;
    scenario = nullptr;
    dtdFile  = "cwonder_project.dtd";
    dtdPath  = "";

    name = "";

    this->maxNoSources = maxNoSources;
}

Project::~Project() {
    if (!fromFile) {
        if (doc != nullptr) {
            delete doc;
            doc = nullptr;
        }
    }

    clearSnaphots();
}

void Project::createProject() {
    if (fromFile) {
        doc      = new Document;
        fromFile = false;  // flag for "must be deleted at the end"
    }

    clearSnaphots();

    ostringstream os;
    doc->set_internal_subset("cwonder_project", "", dtdFile);
    rootNode = doc->create_root_node("cwonder_project");
    rootNode->set_attribute("name", name);

    createScenario();
}

int Project::createScenario() {
    // this can only be done once, so if data in snapshots is
    // there, this call is not allowed
    if (!snapshots.empty()) { return -1; }

    Scenario* newSnapshot = new Scenario(maxNoSources);
    newSnapshot->id       = 0;  // 0 is default, but just to make sure

    if (rootNode != nullptr) {
        newSnapshot->node =
            dynamic_cast<xmlpp::Element*>(rootNode)->add_child_element("scenario");

        // Validate and add snapshot to the list of snapshots
        validate();
        snapshots.push_back(newSnapshot);
        scenario = newSnapshot;

        return 0;
    }
    delete newSnapshot;

    return 1;  // cannot get root node
}

Scenario* Project::getScenario() {
    if (!snapshots.empty()) { return snapshots.front(); }
    return nullptr;
}

void Project::writeProjectToDOM() {
    try {
        Element::AttributeList::const_iterator attribIter;

        if (Element* nodeElement = dynamic_cast<Element*>(rootNode)) {
            // write header
            for (auto& attribute : nodeElement->get_attributes()) {
                attribName = attribute->get_name();
                ostringstream os;

                if (attribName == "name") { os << name; }

                const Glib::ustring out = os.str();
                dynamic_cast<xmlpp::AttributeNode*>(attribute)->set_value(out);
            }

            // write snapshots
            writeSnapshotsToDOM();
        }
    }
    catch (const exception& ex) {
        cout << "Exception caught: " << ex.what() << endl;
    }
}

void Project::readProjectFromDOM() {
    name = "";

    if (rootNode != nullptr) {
        const Element::AttributeList& attributes = rootNode->get_attributes();

        Element::AttributeList::const_iterator attribIter;

        for (attribIter = attributes.begin(); attribIter != attributes.end();
             ++attribIter) {
            const Attribute* projectAttribute = *attribIter;
            Glib::ustring attribName          = projectAttribute->get_name();

            if (attribName == "name") { name = projectAttribute->get_value(); }
        }

        try {
            readSnapshotsFromDOM(rootNode, "/cwonder_project/scenario");
            readSnapshotsFromDOM(rootNode, "/cwonder_project/snapshot");
        }
        catch (exception& ex) {
            // because readSnapshotsFromDOM allocates memory we have to free
            // that first and then rethrow this exception
            clearSnaphots();

            if (doc != nullptr) {
                doc      = nullptr;
                rootNode = nullptr;
            }

            throw;
        }
    }

    scenario = snapshots.front();
}

void Project::clearSnaphots() {
    // this clears the scenario as well
    while (!snapshots.empty()) {
        delete snapshots.back();
        snapshots.pop_back();
    }
}

void Project::readFromFile(Glib::ustring filepath) {
    clearSnaphots();

    // Read the Dom representation from an xml-file
    parser.set_substitute_entities(false);
    parser.parse_file(filepath);

    if (parser) {
        rootNode = parser.get_document()->get_root_node();

        if (rootNode != nullptr) {
            if (!fromFile) { delete doc; }

            fromFile = true;
            doc      = parser.get_document();
            validate();

            // remove all textnodes from the complete xml tree to get
            // nice linebreaks when writing to file with write_to_file_formatted()
            Node::NodeList snapshotNodeList = rootNode->get_children();
            Node::NodeList::iterator it;

            for (it = snapshotNodeList.begin(); it != snapshotNodeList.end(); ++it) {
                Node* snapshotNode = *it;

                if (snapshotNode->get_name() == "text") {
                    xmlpp::Element::remove_node(snapshotNode);
                } else {
                    Node::NodeList childNodeList = snapshotNode->get_children();
                    Node::NodeList::iterator it;

                    for (it = childNodeList.begin(); it != childNodeList.end(); ++it) {
                        Node* sourceOrGroupNode = *it;

                        if (sourceOrGroupNode->get_name() == "text") {
                            xmlpp::Node::remove_node(sourceOrGroupNode);
                        }
                    }
                }
            }
        }
    }

    readProjectFromDOM();
}

void Project::writeToFile(string filename) {
    writeProjectToDOM();

    if (doc != nullptr) { doc->write_to_file_formatted(filename); }
}

string Project::show() {
    if (doc != nullptr) {
        writeProjectToDOM();

        // return complete DOM representation without snapshot source data
        Document shortDoc;
        shortDoc.create_root_node_by_import(rootNode);
        Element* shortRootNode      = shortDoc.get_root_node();
        Node::NodeList snapshotList = shortRootNode->get_children();

        for (auto& iter : snapshotList) {
            if (Element* nodeElement = dynamic_cast<Element*>(iter)) {
                if (nodeElement->get_name() == "snapshot") {
                    Node::NodeList childList;
                    childList = nodeElement->get_children();

                    for (auto& srcIt : childList) { xmlpp::Element::remove_node(srcIt); }
                }
            }
        }

        return shortDoc.write_to_string_formatted();
    }

    return "";
}

int Project::validate() {
    DtdValidator validator(join(dtdPath, dtdFile));
    validator.validate(doc);
    return 0;
}

int Project::takeSnapshot(int snapshotID, string name) {
    if (snapshots.empty()) {
        return -1;  // no scenario
    }

    if (snapshotID <= 0) {
        return -1;  // invalid snapshotID
    }

    // check if snapshot already exists if so overwrite existing snapshot
    // otherwise create a new one
    bool overwrite = false;
    list<Scenario*>::iterator iter;

    for (iter = snapshots.begin(); iter != snapshots.end(); ++iter) {
        if ((*iter)->id == snapshotID) {
            overwrite = true;
            break;
        }
    }

    // add a new snapshot
    if (!overwrite) {
        // Add snapshot to the Dom representation and list of snapshots
        if (rootNode != nullptr) {
            // construct new node for new snapshot
            Element* snapshotElement =
                dynamic_cast<xmlpp::Element*>(rootNode)->add_child_element("snapshot");
            ostringstream os;
            os.str("");
            os << snapshotID;
            snapshotElement->set_attribute("id", os.str());
            os.str("");
            os << name;
            snapshotElement->set_attribute("name", os.str());

            // copy source and group data from scenario
            Scenario* newSnapshot = new Scenario(*scenario, snapshotElement);
            newSnapshot->id       = snapshotID;
            newSnapshot->name     = name;

            // validate and add snapshot to the list of snapshots
            validate();
            snapshots.push_back(newSnapshot);
        } else {
            return 1;  // can not get root node
        }
    } else {  // overwrite the existing snapshot with data from scenario
        Scenario* snapshot = *iter;
        // set the new name
        snapshot->name = name;

        // loop through all sources in the scenario
        for (int i = 0; i < (int)scenario->sourcesVector.size(); ++i) {
            // check if source is active in the scenario
            if (scenario->sourcesVector[i].active) {
                // if the source was not already active in the old snapshot,
                // then add it to the DOM representation
                if (!snapshot->sourcesVector[i].active) { snapshot->activateSource(i); }

                // copy source data from the new snapshot
                snapshot->sourcesVector[i] = scenario->sourcesVector[i];
            } else {  // source is not active in scenario
                // if it is active in the old snaphot then deactivate it
                // and remove it from the DOM representation
                if (snapshot->sourcesVector[i].active) { snapshot->deactivateSource(i); }
            }
        }

        // loop through all groups in the scenario
        for (int i = 0; i < (int)scenario->sourceGroupsVector.size(); ++i) {
            // check if source is active in the scenario
            if (scenario->sourceGroupsVector[i].active) {
                // if the source was not already active in the old snapshot,
                // then add it to the DOM representation
                if (!snapshot->sourceGroupsVector[i].active) {
                    snapshot->activateGroup(i + 1);
                }

                // copy group data from the new snapshot
                snapshot->sourceGroupsVector[i] = scenario->sourceGroupsVector[i];
            } else {  // group is not active in scenario
                // if it is active in the old snaphot then deactivate it
                // and remove it from the DOM representation
                if (snapshot->sourceGroupsVector[i].active) {
                    snapshot->deactivateGroup(i + 1);
                }
            }
        }
    }

    return 0;
}

int Project::recallSnapshot(int snapshotID) {
    // in scenario activate all sources and groups which are active in snapshot
    // and set their data
    // deactivate the other sources and groups

    // get the right snapshot
    Scenario* recall = nullptr;
    list<Scenario*>::iterator iter;

    for (iter = snapshots.begin(); iter != snapshots.end(); ++iter) {
        if ((*iter)->id == snapshotID) {
            recall = *iter;
            break;
        }
    }

    if (recall == nullptr) {
        return -1;  // snapshot does not exist
    }

    // loop through all sources
    for (int i = 0; i < maxNoSources; ++i) {
        if (recall->sourcesVector[i].active) {  // source active in recalled snapshot
            // if it wasn't active, then activate source in scenario
            if (!scenario->sourcesVector[i].active) { scenario->activateSource(i); }

            // copy data of this source to scenario
            scenario->sourcesVector[i] = recall->sourcesVector[i];
        } else {  // source is not active in recalled snapshot
            // if it was active, then deactivate source in scenario
            if (scenario->sourcesVector[i].active) { scenario->deactivateSource(i); }
        }
    }

    // loop through all groups
    for (int i = 0; i < maxNoSources; ++i) {
        if (recall->sourceGroupsVector[i].active) {  // group active in recalled snapshot
            // if it wasn't active, then activate group in scenario
            if (!scenario->sourceGroupsVector[i].active) {
                scenario->activateGroup(i + 1);
            }

            // copy data of this group to scenario
            scenario->sourceGroupsVector[i] = recall->sourceGroupsVector[i];
        } else {  // group is not active in recalled snapshot
            // if it was active, then deactivate group in scenario
            if (scenario->sourceGroupsVector[i].active) {
                scenario->deactivateGroup(i + 1);
            }
        }
    }

    return 0;
}

int Project::deleteSnapshot(int snapshotID) {
    // the scenario may only be deleted by ~Project
    if (snapshotID == 0) {
        return 1;  // snapshot does not exist
    }

    list<Scenario*>::iterator iter;

    for (iter = snapshots.begin(); iter != snapshots.end(); ++iter) {
        if ((*iter)->id == snapshotID) {
            // remove the sources and groups from the dom representation of the snapshot
            Node::NodeList nList = ((*iter)->node)->get_children();

            for (auto& niter : nList) { (*iter)->node->remove_node(niter); }

            // remove snapshot from the dom representation
            xmlpp::Element::remove_node((*iter)->node);

            // delete snapshot and remove it from the snapshots list
            delete *iter;
            snapshots.erase(iter);

            if (validate() == 1) {
                return 2;  // error validation
            }

            return 0;
        }
    }

    return 1;  // snapshot does not exist
}

int Project::copySnapshot(int fromID, int toID) {
    list<Scenario*>::iterator fromIter;

    // we don't need to check scenario

    for (fromIter = ++(snapshots.begin()); fromIter != snapshots.end(); ++fromIter) {
        // if snapshot which should be copied from does exist
        if ((*fromIter)->id == fromID) {
            list<Scenario*>::iterator toIter;

            for (toIter = snapshots.begin(); toIter != snapshots.end(); ++toIter) {
                if ((*toIter)->id == toID || toID == 0) {
                    return 2;  // the target snapshot already exist or tried to copy to
                               // scenario
                }
            }

            // Add snapshot to the Dom representation and list of snapshots
            if (rootNode != nullptr) {
                // construct new node for new snapshot
                Element* newSnapshotElement = nullptr;
                ostringstream os;
                newSnapshotElement =
                    dynamic_cast<xmlpp::Element*>(rootNode)->add_child_element(
                        "snapshot");
                os.str("");
                os << toID;
                newSnapshotElement->set_attribute("id", os.str());
                os.str("");
                os << (*fromIter)->name;
                newSnapshotElement->set_attribute("name", os.str());

                // copy source data from fromID to toID
                Scenario* newSnapshot = new Scenario(*(*fromIter), newSnapshotElement);
                newSnapshot->id       = toID;
                newSnapshot->name     = (*fromIter)->name;
                snapshots.push_back(newSnapshot);
            }

            return 0;
        }
    }

    return 1;  // snapshot that you want to copy from does not exit
}

int Project::renameSnapshot(int snapshotID, string name) {
    list<Scenario*>::iterator iter;

    for (iter = snapshots.begin(); iter != snapshots.end(); ++iter) {
        if ((*iter)->id == snapshotID) {
            (*iter)->name = name;

            if (validate() == 1) {
                return 2;  // error validating
            }
            return 0;
        }
    }

    return 1;  // snapshot does not exist
}

void Project::readSnapshotsFromDOM(Node* node, const Glib::ustring& xmlPath) {
    for (auto& iter : node->find(xmlPath)) {
        Scenario* t = new Scenario(iter, maxNoSources);
        snapshots.push_back(t);
    }
}

void Project::writeSnapshotsToDOM() {
    list<Scenario*>::iterator iter;

    for (iter = snapshots.begin(); iter != snapshots.end(); ++iter) {
        (*iter)->writeToDOM();
    }
}

//-----------------------------end of Project---------------------------------//

//--------------------------------Scenario-----------------------------------//

Scenario::Scenario(int maxNoSources) : maxNoSources(maxNoSources) {
    Source source;
    SourceGroup sourceGroup;

    // initialize with max number of sources and groups possible
    for (int i = 0; i < maxNoSources; i++) {
        source.id = i;
        sourcesVector.push_back(source);

        sourceGroup.id = i + 1;
        sourceGroupsVector.push_back(sourceGroup);
    }

    id   = 0;
    node = nullptr;
}

Scenario::Scenario(Node* n, int maxNoSources) : maxNoSources(maxNoSources) {
    Source source;
    SourceGroup sourceGroup;

    // initialize with max number of sources possible
    for (int i = 0; i < maxNoSources; i++) {
        source.id = i;
        sourcesVector.push_back(source);

        sourceGroup.id = i + 1;
        sourceGroupsVector.push_back(sourceGroup);
    }

    id   = 0;
    node = n;
    readFromDOM();
}

Scenario::Scenario(const Scenario& other, Node* node) {
    id           = other.id;
    name         = other.name;
    maxNoSources = other.maxNoSources;
    this->node   = node;

    Source source;
    SourceGroup sourceGroup;

    // initialize with max number of sources possible
    //  copy data for active sources and groups
    for (int i = 0; i < maxNoSources; ++i) {
        source.id = i;
        sourcesVector.push_back(source);

        if (other.sourcesVector.at(i).active) {
            this->activateSource(i);
            this->sourcesVector.at(i) = other.sourcesVector.at(i);
        }
    }

    for (int i = 0; i < maxNoSources; ++i) {
        sourceGroup.id = i + 1;
        sourceGroupsVector.push_back(sourceGroup);

        if (other.sourceGroupsVector.at(i).active) {
            this->activateGroup(i + 1);
            this->sourceGroupsVector.at(i) = other.sourceGroupsVector.at(i);
        }
    }
}

Scenario::~Scenario() {
    while (!sourcesVector.empty()) { sourcesVector.pop_back(); }

    while (!sourceGroupsVector.empty()) { sourceGroupsVector.pop_back(); }
}

void Scenario::readFromDOM() {
    if (const Element* nodeElement = dynamic_cast<const Element*>(node)) {
        // read scenario data
        for (auto const& attribute : nodeElement->get_attributes()) {
            attribName = attribute->get_name();
            istringstream is(attribute->get_value());

            if (attribName == "id") { is >> id; }

            if (attribName == "name") { name = attribute->get_value(); }
        }

        // read data of all sources
        Node::NodeList childList = node->get_children();

        for (auto& childListIter : childList) {
            if (Element* childElement = dynamic_cast<Element*>(childListIter)) {
                elementName = childElement->get_name();

                if (elementName == "source") {
                    int id = -1;
                    const Element::AttributeList& srcAttribs =
                        childElement->get_attributes();

                    for (auto* attribute : srcAttribs) {
                        attribName = attribute->get_name();
                        istringstream is(attribute->get_value());

                        if (attribName == "id") {
                            is >> id;
                            // If id is invalid as index into sourcesVector an
                            // out_of_range exception is thrown and terminates parsing.
                            sourcesVector.at(id).id = id;
                        } else if (attribName == "type") {
                            is >> sourcesVector.at(id).type;
                        } else if (attribName == "name") {
                            sourcesVector.at(id).name = attribute->get_value();
                        } else if (attribName == "posx") {
                            is >> sourcesVector.at(id).pos[0];
                        } else if (attribName == "posy") {
                            is >> sourcesVector.at(id).pos[1];
                        } else if (attribName == "posz") {
                            is >> sourcesVector.at(id).pos[2];
                        } else if (attribName == "angle") {
                            is >> sourcesVector.at(id).angle;
                        } else if (attribName == "groupId") {
                            is >> sourcesVector.at(id).groupID;
                        } else if (attribName == "colorR") {
                            is >> sourcesVector.at(id).color[0];
                        } else if (attribName == "colorG") {
                            is >> sourcesVector.at(id).color[1];
                        } else if (attribName == "colorB") {
                            is >> sourcesVector.at(id).color[2];
                        } else if (attribName == "invRotation") {
                            is >> sourcesVector.at(id).invertedRotationDirection;
                        } else if (attribName == "invScaling") {
                            is >> sourcesVector.at(id).invertedScalingDirection;
                        } else if (attribName == "doppler") {
                            is >> sourcesVector.at(id).dopplerEffect;
                        }
                    }

                    // check for missing attributes ( can only happen with those marked as
                    // implied in the DTD ) and ad them to the current element
                    ostringstream os;

                    if (childElement->find("@groupId").empty()) {
                        os << sourcesVector.at(id).groupID;
                        childElement->set_attribute("groupId", os.str());
                    }

                    if (childElement->find("@colorR").empty()) {
                        os << sourcesVector.at(id).color[0];
                        childElement->set_attribute("colorR", os.str());
                    }

                    if (childElement->find("@colorG").empty()) {
                        os << sourcesVector.at(id).color[1];
                        childElement->set_attribute("colorG", os.str());
                    }

                    if (childElement->find("@colorB").empty()) {
                        os << sourcesVector.at(id).color[2];
                        childElement->set_attribute("colorB", os.str());
                    }

                    if (childElement->find("@invRotation").empty()) {
                        os << sourcesVector.at(id).invertedRotationDirection;
                        childElement->set_attribute("invRotation", os.str());
                    }

                    if (childElement->find("@invScaling").empty()) {
                        os << sourcesVector.at(id).invertedScalingDirection;
                        childElement->set_attribute("invScaling", os.str());
                    }

                    if (childElement->find("@doppler").empty()) {
                        os << sourcesVector.at(id).dopplerEffect;
                        childElement->set_attribute("doppler", os.str());
                    }

                    if (childElement->find("@posz").empty()) {
                        os << sourcesVector.at(id).pos[2];
                        childElement->set_attribute("posz", os.str());
                    }

                    // mark source as active and set its node
                    sourcesVector.at(id).active = true;
                    sourcesVector.at(id).node   = childElement;
                } else if (elementName == "group") {
                    int id = -1;
                    const Element::AttributeList& grpAttribs =
                        childElement->get_attributes();

                    for (auto* attribute : grpAttribs) {
                        attribName = attribute->get_name();
                        istringstream is(attribute->get_value());

                        if (attribName == "id") {
                            is >> id;
                            --id;
                            // If id is invalid as index into sourceGroupsVector
                            // an out_of_range exception is thrown and terminates parsing.
                            sourceGroupsVector.at(id).id = id + 1;
                        } else if (attribName == "posx") {
                            is >> sourceGroupsVector.at(id).pos[0];
                        } else if (attribName == "posy") {
                            is >> sourceGroupsVector.at(id).pos[1];
                        } else if (attribName == "posz") {
                            is >> sourceGroupsVector.at(id).pos[2];
                        } else if (attribName == "colorR") {
                            is >> sourceGroupsVector.at(id).color[0];
                        } else if (attribName == "colorG") {
                            is >> sourceGroupsVector.at(id).color[1];
                        } else if (attribName == "colorB") {
                            is >> sourceGroupsVector.at(id).color[2];
                        }
                    }

                    // check for missing attributes ( can only happen with those marked as
                    // implied in the DTD ) and ad them to the current element
                    ostringstream os;

                    if (childElement->find("@colorR").empty()) {
                        os << sourceGroupsVector.at(id).color[0];
                        childElement->set_attribute("colorR", os.str());
                    }

                    if (childElement->find("@colorG").empty()) {
                        os << sourceGroupsVector.at(id).color[1];
                        childElement->set_attribute("colorG", os.str());
                    }

                    if (childElement->find("@colorB").empty()) {
                        os << sourceGroupsVector.at(id).color[2];
                        childElement->set_attribute("colorB", os.str());
                    }

                    if (childElement->find("@posz").empty()) {
                        os << sourceGroupsVector.at(id).pos[2];
                        childElement->set_attribute("posz", os.str());
                    }

                    // mark group as active and set its node
                    sourceGroupsVector.at(id).active = true;
                    sourceGroupsVector.at(id).node   = childElement;
                }
            }
        }
    }
}

void Scenario::writeToDOM() {
    Element::AttributeList::const_iterator scenarioAttrIter;
    Element::AttributeList::const_iterator srcAttrIter;
    Element::AttributeList::const_iterator grpAttrIter;

    if (Element* nodeElement = dynamic_cast<Element*>(node)) {
        // write scenario data
        for (auto& attribute : nodeElement->get_attributes()) {
            attribName = attribute->get_name();
            ostringstream os;

            if (attribName == "id") {
                os << id;
            } else if (attribName == "name") {
                os << name;
            }

            const Glib::ustring out = os.str();
            dynamic_cast<xmlpp::AttributeNode*>(attribute)->set_value(out);
        }

        // write source data
        for (auto& i : sourcesVector) {
            if (i.active) {
                if (Element* sourceElement = dynamic_cast<Element*>(i.node)) {
                    for (auto& srcAttrib : sourceElement->get_attributes()) {
                        attribName = srcAttrib->get_name();
                        ostringstream os;

                        if (attribName == "id") {
                            os << i.id;
                        } else if (attribName == "type") {
                            os << i.type;
                        } else if (attribName == "name") {
                            os << i.name;
                        } else if (attribName == "posx") {
                            os << i.pos[0];
                        } else if (attribName == "posy") {
                            os << i.pos[1];
                        } else if (attribName == "posz") {
                            os << i.pos[2];
                        } else if (attribName == "angle") {
                            os << i.angle;
                        } else if (attribName == "groupId") {
                            os << i.groupID;
                        } else if (attribName == "colorR") {
                            os << i.color[0];
                        } else if (attribName == "colorG") {
                            os << i.color[1];
                        } else if (attribName == "colorB") {
                            os << i.color[2];
                        } else if (attribName == "invRotation") {
                            os << i.invertedRotationDirection;
                        } else if (attribName == "invScaling") {
                            os << i.invertedScalingDirection;
                        } else if (attribName == "doppler") {
                            os << i.dopplerEffect;
                        }

                        const Glib::ustring out = os.str();
                        dynamic_cast<xmlpp::AttributeNode*>(srcAttrib)->set_value(out);
                    }
                }
            }
        }

        // write group data
        for (auto& i : sourceGroupsVector) {
            if (i.active) {
                if (Element* groupElement = dynamic_cast<Element*>(i.node)) {
                    for (auto& grpAttrib : groupElement->get_attributes()) {
                        attribName = grpAttrib->get_name();
                        ostringstream os;

                        if (attribName == "id") {
                            os << i.id;
                        } else if (attribName == "posx") {
                            os << i.pos[0];
                        } else if (attribName == "posy") {
                            os << i.pos[1];
                        } else if (attribName == "posz") {
                            os << i.pos[2];
                        } else if (attribName == "colorR") {
                            os << i.color[0];
                        } else if (attribName == "colorG") {
                            os << i.color[1];
                        } else if (attribName == "colorB") {
                            os << i.color[2];
                        }

                        const Glib::ustring out = os.str();
                        dynamic_cast<xmlpp::AttributeNode*>(grpAttrib)->set_value(out);
                    }
                }
            }
        }
    }
}

void Scenario::activateSource(int id) {
    // activate all the sources
    if (id == -1) {
        for (auto& i : sourcesVector) {
            i.active = true;

            // if this is the first time this source is activated then
            // add it to the dom representation
            // but only if a parent node does exist
            if (i.node == nullptr && node != nullptr) {
                ostringstream os;
                Element* srcElement =
                    dynamic_cast<xmlpp::Element*>(node)->add_child_element("source");
                os.str("");
                os << i.id;
                srcElement->set_attribute("id", os.str());

                os.str("");
                os << i.type;
                srcElement->set_attribute("type", os.str());

                os.str("");
                os << i.name;
                srcElement->set_attribute("name", os.str());

                os.str("");
                os << i.pos[0];
                srcElement->set_attribute("posx", os.str());

                os.str("");
                os << i.pos[1];
                srcElement->set_attribute("posy", os.str());

                os.str("");
                os << i.pos[2];
                srcElement->set_attribute("posz", os.str());

                os.str("");
                os << i.angle;
                srcElement->set_attribute("angle", os.str());

                os.str("");
                os << i.groupID;
                srcElement->set_attribute("groupId", os.str());

                os.str("");
                os << sourcesVector[id].color[0];
                srcElement->set_attribute("colorR", os.str());

                os.str("");
                os << sourcesVector[id].color[1];
                srcElement->set_attribute("colorG", os.str());

                os.str("");
                os << sourcesVector[id].color[2];
                srcElement->set_attribute("colorB", os.str());

                os.str("");
                os << sourcesVector[id].invertedRotationDirection;
                srcElement->set_attribute("invRotation", os.str());

                os.str("");
                os << sourcesVector[id].invertedScalingDirection;
                srcElement->set_attribute("invScaling", os.str());

                os.str("");
                os << sourcesVector[id].dopplerEffect;
                srcElement->set_attribute("doppler", os.str());

                i.node = srcElement;
            }
        }
    } else {  // activate just one source
        // mandatory boundschecking, operator[] of std::vector is unchecked!
        if ((id >= 0) && (id < (int)sourcesVector.size())) {
            sourcesVector[id].active = true;

            // if this is the first time this source is activated then
            // add it to the dom representation
            if (sourcesVector[id].node == nullptr && node != nullptr) {
                ostringstream os;
                Element* srcElement =
                    dynamic_cast<xmlpp::Element*>(node)->add_child_element("source");
                os.str("");
                os << sourcesVector[id].id;
                srcElement->set_attribute("id", os.str());

                os.str("");
                os << sourcesVector[id].type;
                srcElement->set_attribute("type", os.str());

                os.str("");
                os << sourcesVector[id].name;
                srcElement->set_attribute("name", os.str());

                os.str("");
                os << sourcesVector[id].pos[0];
                srcElement->set_attribute("posx", os.str());

                os.str("");
                os << sourcesVector[id].pos[1];
                srcElement->set_attribute("posy", os.str());

                os.str("");
                os << sourcesVector[id].pos[2];
                srcElement->set_attribute("posz", os.str());

                os.str("");
                os << sourcesVector[id].angle;
                srcElement->set_attribute("angle", os.str());

                os.str("");
                os << sourcesVector[id].groupID;
                srcElement->set_attribute("groupId", os.str());

                os.str("");
                os << sourcesVector[id].color[0];
                srcElement->set_attribute("colorR", os.str());

                os.str("");
                os << sourcesVector[id].color[1];
                srcElement->set_attribute("colorG", os.str());

                os.str("");
                os << sourcesVector[id].color[2];
                srcElement->set_attribute("colorB", os.str());

                os.str("");
                os << sourcesVector[id].invertedRotationDirection;
                srcElement->set_attribute("invRotation", os.str());

                os.str("");
                os << sourcesVector[id].invertedScalingDirection;
                srcElement->set_attribute("invScaling", os.str());

                os.str("");
                os << sourcesVector[id].dopplerEffect;
                srcElement->set_attribute("doppler", os.str());

                sourcesVector[id].node = srcElement;
            }
        }
    }
}

void Scenario::deactivateSource(int id) {
    // deactivate all the sources (or just one source)
    if (id == -1) {
        for (auto& i : sourcesVector) {
            // deactivate and reset to default values
            Source* temp = &i;

            temp->active = false;
            // temp->id      = 0; // id should be kept in case of reactivation
            temp->type    = 1;
            temp->name    = "";
            temp->angle   = 0.0;
            temp->groupID = 0;

            temp->pos[0] = 0.0;
            temp->pos[1] = 0.0;
            temp->pos[2] = 0.0;

            temp->color[0] = 0;
            temp->color[1] = 255;
            temp->color[2] = 0;

            temp->invertedRotationDirection = false;
            temp->invertedScalingDirection  = false;

            temp->dopplerEffect = true;

            if ((i.node != nullptr) && node != nullptr) {
                xmlpp::Node::remove_node(i.node);
            }

            i.node = nullptr;
        }
    } else {
        // mandatory boundschecking, operator[] of std::vector is unchecked!
        if ((id >= 0) && (id < (int)sourcesVector.size())) {
            // deactivate and reset to default values
            Source* temp = &(sourcesVector[id]);

            temp->active = false;
            // temp->id      = 0; // id should be kept in case of reactivation
            temp->type    = 1;
            temp->name    = "";
            temp->angle   = 0.0;
            temp->groupID = 0;

            temp->pos[0] = 0.0;
            temp->pos[1] = 0.0;
            temp->pos[2] = 0.0;

            temp->color[0] = 0;
            temp->color[1] = 255;
            temp->color[2] = 0;

            temp->invertedRotationDirection = false;
            temp->invertedScalingDirection  = false;

            temp->dopplerEffect = true;

            if ((sourcesVector[id].node != nullptr) && node != nullptr) {
                xmlpp::Node::remove_node(sourcesVector[id].node);
            }

            sourcesVector[id].node = nullptr;
        }
    }
}

void Scenario::activateGroup(int groupID) {
    auto& sourceGroup  = sourceGroupsVector.at(groupID - 1);
    sourceGroup.active = true;

    // if this is the first time this group is activated then
    // add it to the dom representation
    if (sourceGroup.node == nullptr && node != nullptr) {
        ostringstream os;
        Element* grpElement =
            dynamic_cast<xmlpp::Element*>(node)->add_child_element("group");
        os.str("");
        os << sourceGroup.id;
        grpElement->set_attribute("id", os.str());

        os.str("");
        os << sourceGroup.pos[0];
        grpElement->set_attribute("posx", os.str());

        os.str("");
        os << sourceGroup.pos[1];
        grpElement->set_attribute("posy", os.str());

        os.str("");
        os << sourceGroup.pos[2];
        grpElement->set_attribute("posz", os.str());

        os.str("");
        os << sourceGroup.color[0];
        grpElement->set_attribute("colorR", os.str());

        os.str("");
        os << sourceGroup.color[1];
        grpElement->set_attribute("colorG", os.str());

        os.str("");
        os << sourceGroup.color[2];
        grpElement->set_attribute("colorB", os.str());

        sourceGroup.node = grpElement;
    }
}

void Scenario::deactivateGroup(int groupID) {
    int id = groupID - 1;

    // mandatory boundschecking, operator[] of std::vector is unchecked!
    if ((id >= 0) && (id < (int)sourceGroupsVector.size())) {
        sourceGroupsVector[id].active = false;

        if ((sourceGroupsVector[id].node != nullptr) && node != nullptr) {
            xmlpp::Node::remove_node(sourceGroupsVector[id].node);
        }

        sourceGroupsVector[id].node = nullptr;
    }
}

string Scenario::show() {
    ostringstream log;
    log << "Show scenario id=" << id << endl;

    for (auto& source : sourcesVector) {
        if (source.active) {
            log << endl
                << "Source id=" << source.id << " type=" << source.type
                << " x=" << source.pos[0] << " y=" << source.pos[1]
                << " z=" << source.pos[2] << " angle=" << source.angle
                << " groupId=" << source.groupID << " colorR=" << source.color[0]
                << " colorG=" << source.color[1] << " colorB=" << source.color[2]
                << " invertedRotationDirection=" << source.invertedRotationDirection
                << " invertedScalingDirection=" << source.invertedScalingDirection
                << " doppler=" << source.dopplerEffect << endl;
        }
    }

    for (auto& sourceGroup : sourceGroupsVector) {
        if (sourceGroup.active) {
            log << endl
                << "Group id=" << sourceGroup.id << " x=" << sourceGroup.pos[0]
                << " y=" << sourceGroup.pos[1] << " z=" << sourceGroup.pos[2]
                << " colorR=" << sourceGroup.color[0]
                << " colorG=" << sourceGroup.color[1]
                << " colorB=" << sourceGroup.color[2] << endl;
        }
    }

    return log.str();
}

//----------------------------end of Scenario--------------------------------//

//---------------------------------Source------------------------------------//

Source::Source(const Source& other)
    : id(other.id)
    , type(other.type)
    , name(other.name)
    , active(other.active)
    , pos(other.pos)
    , angle(other.angle)
    , groupID(other.groupID)
    , color{other.color[0], other.color[1], other.color[2]}
    , dopplerEffect(other.dopplerEffect)
    , invertedRotationDirection(other.invertedRotationDirection)
    , invertedScalingDirection(other.invertedScalingDirection) {}

Source& Source::operator=(Source const& other) {
    id      = other.id;
    type    = other.type;
    pos     = other.pos;
    name    = other.name;
    active  = other.active;
    angle   = other.angle;
    groupID = other.groupID;

    color[0] = other.color[0];
    color[1] = other.color[1];
    color[2] = other.color[2];

    invertedRotationDirection = other.invertedRotationDirection;
    invertedScalingDirection  = other.invertedScalingDirection;
    dopplerEffect             = other.dopplerEffect;

    return *this;
}

//-----------------------------end of Source----------------------------------//

//------------------------------SourceGroup-----------------------------------//

SourceGroup& SourceGroup::operator=(SourceGroup const& other) {
    id     = other.id;
    pos    = other.pos;
    active = other.active;

    color[0] = other.color[0];
    color[1] = other.color[1];
    color[2] = other.color[2];

    return *this;
}

//---------------------------end of SourceGroup-------------------------------//
