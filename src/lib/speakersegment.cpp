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

#include "speakersegment.h"

#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>

#include "wonder_path.h"

using std::exception;
using std::ifstream;
using std::istringstream;
using std::ostringstream;
using std::string;
using std::vector;

//-----------------------------------Segment---------------------------------//

Segment::Segment(xmlpp::Node* n) {
    node        = n;
    id          = 0;
    noSpeakers  = 0;
    windowWidth = 0.0;
    readFromXML();
}

void Segment::readFromXML() {
    if (const xmlpp::Element* nodeElement = dynamic_cast<const xmlpp::Element*>(node)) {
        for (auto const& attribute : nodeElement->get_attributes()) {
            nodeName = attribute->get_name();
            istringstream is(attribute->get_value());

            if (nodeName == "id") {
                is >> id;
            } else if (nodeName == "numspeak") {
                is >> noSpeakers;
            } else if (nodeName == "winwidth") {
                is >> windowWidth;
            } else if (nodeName == "startx") {
                is >> start[0];
            } else if (nodeName == "starty") {
                is >> start[1];
            } else if (nodeName == "startz") {
                is >> start[2];
            } else if (nodeName == "endx") {
                is >> end[0];
            } else if (nodeName == "endy") {
                is >> end[1];
            } else if (nodeName == "endz") {
                is >> end[2];
            } else if (nodeName == "normalx") {
                is >> normal[0];
            } else if (nodeName == "normaly") {
                is >> normal[1];
            } else if (nodeName == "normalz") {
                is >> normal[2];
            }
        }
    }
}

void Segment::syncToXML() {
    if (xmlpp::Element* nodeElement = dynamic_cast<xmlpp::Element*>(node)) {
        for (auto& attribute : nodeElement->get_attributes()) {
            nodeName = attribute->get_name();
            ostringstream os;

            if (nodeName == "id") {
                os << id;
            } else if (nodeName == "numspeak") {
                os << noSpeakers;
            } else if (nodeName == "winwidth") {
                os << windowWidth;
            } else if (nodeName == "startx") {
                os << start[0];
            } else if (nodeName == "starty") {
                os << start[1];
            } else if (nodeName == "startz") {
                os << start[2];
            } else if (nodeName == "endx") {
                os << end[0];
            } else if (nodeName == "endy") {
                os << end[1];
            } else if (nodeName == "endz") {
                os << end[2];
            } else if (nodeName == "normalx") {
                os << normal[0];
            } else if (nodeName == "normaly") {
                os << normal[1];
            } else if (nodeName == "normalz") {
                os << normal[2];
            }

            const Glib::ustring out = os.str();
            dynamic_cast<xmlpp::AttributeNode*>(attribute)->set_value(out);
        }
    }
}

//-------------------------------end of Segment------------------------------//

//---------------------------------SegmentArray------------------------------//

SegmentArray::SegmentArray(string fileName) {
    int ret = readFromFile(fileName);

    if (ret != 0) { throw exception(); }
}

SegmentArray::~SegmentArray() {
    vector<Segment*>::iterator it;

    for (it = segments.begin(); it != segments.end(); ++it) { delete (*it); }
}

void SegmentArray::getSegments(xmlpp::Node* node, const Glib::ustring& xpath) {
    for (auto& i : node->find(xpath)) {
        Segment* t = new Segment(i);
        segments.push_back(t);
    }
}

int SegmentArray::readFromFile(string fileName) {
    // check if the file exist
    ifstream fin(fileName.c_str(), std::ios_base::in);

    if (!fin.is_open()) {
        // cout << endl <<"[E-wonderconfig] file=" << fileName << " does not exist." <<
        // endl;
        return 1;  // file does not exist
    }

    fin.close();

    // Read the Dom representation from a xml-file
    try {
        // Create the parser object
        // Set for checking of the dtd correctness
        // Parse the xml-file and construct the Dom represantation
        // Get the root node of the tree
        // parser.set_validate();
        xmlpp::DomParser parser;
        parser.set_substitute_entities();
        parser.parse_file(fileName);

        if (parser) {
            xmlpp::Node* root = parser.get_document()->get_root_node();

            if (root) {
                // validate the current dom representation, but first find the dtd
                string dtdPath;
                dtdPath = join(INSTALL_PREFIX, "configs/dtd/twonder_speakerarray.dtd");
                fin.open(dtdPath.c_str(), std::ios_base::in);

                if (!fin.is_open()) {
                    //// cout << "[E-wonderconfig] dtd file does not exist." << endl;
                    return 2;  // dtd file does not exist
                }

                try {
                    xmlpp::DtdValidator validator(dtdPath.c_str());
                    validator.validate(parser.get_document());
                    //// cout << "[V-wonderconfig] Validation successfull" << endl <<
                    /// endl;
                }
                catch (const xmlpp::validity_error& ex) {
                    //// cout << "[V-wonderconfig] Error validating the document"<< endl
                    //// << ex.what() << endl << endl;
                    return 3;  /// dtd error
                }

                // - Construct the Array and search in the Dom for Segments
                // - The objects for the segments gets constructed and the array object
                //   holds pointers to those elements
                getSegments(root, "/speakerarray/segment");
            }
        }
    }
    catch (const exception& ex) {
        //// cout << "Exception caught: " << ex.what() << endl;
        return 4;  // xml error
    }

    return 0;
}

//-----------------------------end of SegmentArray---------------------------//

//-------------------------------SegmentArrayIter----------------------------//

Segment* SegmentArrayIter::operator*() {
    // Segment *tmp = new Segment( xmlpp::NodeSet::iterator::operator*() );
    // return tmp;
    return new Segment(xmlpp::Node::NodeSet::iterator::operator*());
}

//---------------------------end of SegmentArrayIter-------------------------//
