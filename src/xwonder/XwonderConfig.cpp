/*
 * (c) Copyright 2006-7 -- Hans-Joachim Mond
 *
 * sWONDER:
 * Wave field synthesis Of New Dimensions of Electronic music in Realtime
 * http://swonder.sourceforge.net
 *
 * created at the Technische Universitaet Berlin, Germany
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.

 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "XwonderConfig.h"

#include <getopt.h>

#include <QIntValidator>
#include <QRegExp>
#include <iostream>

using std::cerr;
using std::cout;
using std::endl;
using std::string;

XwonderConfig* xwConf = nullptr;

//--------------------------------constructors--------------------------------//

XwonderConfig::XwonderConfig(int argc, char* argv[], QObject* parent) : QObject(parent) {
    // set default values
    cwonderPort       = "58100";
    xwonderPort       = "58400";
    cwonderHost       = "127.0.0.1";
    verbose           = false;
    runWithoutCwonder = false;
    projectOnlyMode   = true;  // this is project dependent and will vary at runtime

    name = "xwonder";

    cwonderPingTimeout = 0;

    // this will be set at runtime by cwonder via OSC
    maxNoSources = 0;
    roomName     = "20m x 20m Demoroom";

    parseArgs(argc, argv);
    cwonderAddr = lo_address_new(cwonderHost.c_str(), cwonderPort.c_str());

    // in demomode maxNoSources will not be set by cwonder
    if (runWithoutCwonder) { maxNoSources = 15; }
}

//----------------------------end of constructors-----------------------------//

void XwonderConfig::parseArgs(int argc, char* argv[]) {
    int c;

    while (1) {
        int option_index = 0;

        static struct option long_options[] = {
            {"cwonderhost", required_argument, nullptr, 'i'},
            {"cwonderport", required_argument, nullptr, 'p'},
            {"listeningport", required_argument, nullptr, 'o'},
            {"timeout", required_argument, nullptr, 't'},
            {"name", required_argument, nullptr, 'n'},
            {"demomode", no_argument, nullptr, 'd'},
            {"verbose", no_argument, nullptr, 'v'},
            {"help", no_argument, nullptr, 'h'},
            {nullptr, 0, nullptr, 0}};

        c = getopt_long(argc, argv, "i:p:o:t:n:dvh", long_options, &option_index);

        if (c == -1) { break; }

        switch (c) {
        case 'i':
            cwonderHost = optarg;
            break;

        case 'p':
            cwonderPort = optarg;
            break;

        case 'o':
            xwonderPort = optarg;
            break;

        case 't':
            cwonderPingTimeout = atoi(optarg);
            break;

        case 'n':
            name = optarg;
            break;

        case 'd':
            runWithoutCwonder = true;
            break;

        case 'v':
            verbose = true;
            break;

        case 'h':
            std::cout << std::endl
                      << "Xwonder arguments:" << std::endl
                      << "-i x (x = host of cwonder, default is 127.0.0.1)" << std::endl
                      << "-p x (x = port of cwonder, default is 58100)" << std::endl
                      << "-o x (x = port of xwonder, default is 58000)" << std::endl
                      << "-t x (x = timeout in ms for connection to cwonder, by default "
                         "it is turned off and uses 0ms)"
                      << std::endl
                      << "-n x (x = name under which to connect to cwonder)" << std::endl
                      << "-d   (demomode, run without connecting to cwonder)" << std::endl
                      << "-v   (verbose mode, prints received OSC messages to console)"
                      << std::endl
                      << std::endl;
            std::exit(EXIT_FAILURE);

        case '?':
            break;

        default:
            printf("?? getopt returned character code 0%o ??\n", c);
        }
    }

    if (optind < argc) {
        printf("non-option ARGV-elements: ");

        while (optind < argc) { printf("%s ", argv[optind++]); }

        printf("\n");

        exit(EXIT_FAILURE);
    }
}

void XwonderConfig::abort(QString message) {
    std::cerr << message.toStdString() << std::endl << "Use -h for help" << std::endl;
    std::exit(1);
}
