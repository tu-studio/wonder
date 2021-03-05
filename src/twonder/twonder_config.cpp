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

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <getopt.h>
#include <iostream>
#include <libxml++/libxml++.h>
#include <sstream>

#include "twonder_config.h"
#include "wonder_path.h"

TwonderConfig* twonderConf = NULL;



TwonderConfig::TwonderConfig(int argc, char* argv[]) {
    verbose = false;

    twonderConfigfile  = join(INSTALL_PREFIX, "configs/twonder_config.xml");
    speakersConfigfile = join(INSTALL_PREFIX, "configs/twonder_speakerarray.xml");

    jackName  = "twonder"; // JACK client name
    name      = "twonder"; // Client name for cwonder registration
    noSources = 0;         // Number of sources

    planeComp = 0.2; // XXX: Empirical value, might need tweaking!

    cwonderHost   = "127.0.0.1";
    cwonderPort   = "58100";
    listeningPort = "58200";

    sampleRate   = 44100; // In Hz
    soundSpeed   = 340.0; // In meters per second
    negDelayInit = 20.0;  // In meters (for focused sources)

    ioMode = IOM_NORMAL;

    reference       = 0.0;
    focusLimit      = 0.0;
    focusMargin     = 0.0;
    speakerDistance = 0.0;

    elevationY1 = 0.0;
    elevationY2 = 0.0;
    elevationZ1 = 0.0;
    elevationZ2 = 0.0;
    slope       = 0.0;

    parseArgs(argc, argv);

    // get cwonders osc address
    cwonderAddr = lo_address_new(cwonderHost, cwonderPort);
}


TwonderConfig::~TwonderConfig() {
    if(cwonderAddr) {
        lo_address_free(cwonderAddr);
        cwonderAddr = NULL;
    }
}


void TwonderConfig::parseArgs(int argc, char* argv[]) {
    int c;

    while(1) {
        int option_index = 0;
        static struct option long_options[] = {
            {"configfile",    required_argument, 0, 'c'},
            {"speakerfile",   required_argument, 0, 's'},
            {"jackname",      required_argument, 0, 'j'},
            {"name",          required_argument, 0, 'n'},
            {"listeningport", required_argument, 0, 'o'},
            {"cwonderhost",   required_argument, 0, 'i'},
            {"cwonderport",   required_argument, 0, 'p'},
            {"planecomp",     required_argument, 0, 'm'},
            {"verbose",       no_argument,       0, 'v'},
            {"negdelay",      required_argument, 0, 1  },
            {"speedofsound",  required_argument, 0, 2  },
            {"alwaysout",     no_argument,       0, 3  },
            {"alwaysin",      no_argument,       0, 4  },
            {"help",          no_argument,       0, 'h'},
            {0, 0, 0, 0}
        };

        c = getopt_long(argc, argv, "c:s:j:n:o:i:p:m:vh", long_options, &option_index);

        if(c == -1) {
            break;
        }

        switch(c) {
            case 'c':
                twonderConfigfile = strdup(optarg);
                break;

            case 's':
                speakersConfigfile = strdup(optarg);
                break;

            case 'j':
                jackName = strdup(optarg);
                break;

            case 'n':
                name = strdup(optarg);
                break;

            case 'o':
                listeningPort = strdup(optarg);
                break;

            case 'i':
                cwonderHost = strdup(optarg);
                break;

            case 'p':
                cwonderPort = strdup(optarg);
                break;

            case 'm': {
                float temp = atof(optarg);

                if(temp >= 0.0  &&  temp <= 1.0) {
                    planeComp = temp;
                } else {
                    std::cerr << "Invalid value for argument --planecomp (-m), must be [0.0, 1.0]." << std::endl;
                }
            }
            break;

            case 'v':
                verbose = 1;
                break;

            case 1:
                negDelayInit = atof(optarg);
                break;

            case 2:
                soundSpeed = atof(optarg);
                break;

            case 3:
                ioMode = IOM_ALWAYSOUT;
                break;

            case 4:
                ioMode = IOM_ALWAYSIN;
                break;

            case 'h':
                printf("\ntwonder's commandline arguments:\n"
                       "--configfile,    -c (path to twonder's config file)\n"
                       "--speakerfile,   -s (path to twonder's speaker file)\n"
                       "--jackname,      -j (name with which twonder registers with jack)\n"
                       "--name,          -n (name with which twonder registers with cwonder)\n"
                       "--listeningport, -o (port where twonder listens, default is 58200)\n"
                       "--cwonderhost,   -i (ip-address where cwonder is running)\n"
                       "--cwonderport,   -p (port where cwonder can be reached)\n"
                       "--planecomp,     -m (factor to compensate for the fact that planewaves are louder than point sources)\n"
                       "--verbose,       -v (print output to terminal)\n"
                       "--negdelay          (initital maximum negative delay in meters)\n"
                       "--speedofsound      (in meters per second)\n"
                       "--alwaysin          (sound is always rendered as focused source)\n"
                       "--alwaysout         (sound is always rendered as not focused source)\n"
                       "--help,          -h \n\n");
                exit(EXIT_SUCCESS);

            case '?':
                break;

            default:
                printf("?? getopt returned character code 0%o ??\n", c);
        }
    }

    if(optind < argc) {
        printf("non-option ARGV-elements: ");

        while(optind < argc) {
            printf("%s ", argv[optind++]);
        }

        printf("\n");

        exit(EXIT_FAILURE);
    }
}


int TwonderConfig::readConfig() {
    // check if file exist
    std::ifstream fin(twonderConfigfile.c_str(), std::ios_base::in);

    if(!fin.is_open()) {
        return 1; // file does not exist
    }

    fin.close();

    // Read the Dom representation from the xml-file
    try {
        xmlpp::DomParser parser;
        parser.set_substitute_entities();
        parser.parse_file(twonderConfigfile);

        if(parser) {
            xmlpp::Node* root = parser.get_document()->get_root_node();

            if(root) {
                // validate the current dom representation but first find the dtd
                std::string dtdPath;
                dtdPath = join(INSTALL_PREFIX, "configs/dtd/twonder_config.dtd");
                fin.open(dtdPath.c_str(), std::ios_base::in);

                if(!fin.is_open()) {
                    return 2;    // dtd file does not exist
                }

                try {
                    xmlpp::DtdValidator validator(dtdPath.c_str());
                    validator.validate(parser.get_document());
                    //// std::cout << "[V-wonderconfig] Validation successfull" << std::endl << std::endl;
                } catch(const xmlpp::validity_error& ex) {
                    //// std::cout << "[V-wonderconfig] Error validating the document"<< std::endl
                    //// << ex.what() << std::endl << std::endl;
                    return 3; /// dtd error
                }

                getFocus(root);
                getSpeakers(root);
            }
        }
    } catch(const std::exception& ex) {
        //// cout << "Exception caught: " << ex.what() << endl;
        return 4; // xml error
    }

    return 0;
}


void TwonderConfig::getFocus(xmlpp::Node* node) {
    xmlpp::NodeSet nset = node->find("/twonder_config/focus");

    if(nset.size() > 0) {
        if(const xmlpp::Element* nodeElement = dynamic_cast<const xmlpp::Element*>(*nset.begin())) {

            for(auto const& attribute : nodeElement->get_attributes()) {
                Glib::ustring nodeName = attribute->get_name();

                std::stringstream ss(attribute->get_value());

                if(nodeName == "limit") {
                    ss >> focusLimit;
                } else if(nodeName == "margin") {
                    ss >> focusMargin;
                }

            }
        }
    }
}


void TwonderConfig::getSpeakers(xmlpp::Node* node) {
    auto nset = node->find("/twonder_config/speakers");

    if(nset.size() > 0) {
        if(const xmlpp::Element* nodeElement = dynamic_cast<const xmlpp::Element*>(*nset.begin())) {

            for(auto const& attribute : nodeElement->get_attributes()) {
                Glib::ustring nodeName = attribute->get_name();

                std::stringstream ss(attribute->get_value());

                if(nodeName == "reference") {
                    ss >> reference;
                } else if(nodeName == "distance") {
                    ss >> speakerDistance;
                }
            }
        }
    }
}
