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

#include <systemd/sd-daemon.h>

#include <chrono>
#include <csignal>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <thread>

#include "cwonder.h"
#include "cwonder_config.h"
#include "oscinctrl.h"
#include "wonder_path.hpp"

sig_atomic_t stopFlag = 0;

void signalHandler(int signal) {
    std::cout << "[SIGNAL-HANDLER]: Received interrupt signal number " << signal
              << " - exiting now!" << std::endl;
    stopFlag = 1;
}

int main(int argc, char* argv[]) {
    cwonderConf = new CwonderConfig(argc, argv);

    int ret = cwonderConf->readConfig();

    if (ret != 0) { std::exit(EXIT_FAILURE); }

    // make default project path
    if (makedirs(cwonderConf->projectPath, mode_t(0700)) != 0) {
        std::cout << "[OSCServer][WARNING]: Could not create default project path!"
                  << std::endl;
    }

    // create the control application
    Cwonder cwonder;
    cwonder.dtdPath     = join(DATA_DIR, "dtd");
    cwonder.projectPath = cwonderConf->projectPath;

    OSCControl* oscctrl;

    try {
        oscctrl = new OSCControl(cwonderConf->listeningPort);
    }
    catch (OSCServer::EServ) {
        std::cerr << "[OSCServer][ERROR]: Could not create server! Maybe the server "
                     "(using the same port) is already running?"
                  << std::endl;
        return 0;
    }

    // add the functions to the osc server
    oscctrl->addReply(&cwonder);
    oscctrl->addMethods();

    // start the OSC receive thread
    oscctrl->start();

    // add signal handlers
#ifdef WIN32
    std::signal(SIGINT, signalHandler);
    std::signal(SIGABRT, signalHandler);
    std::signal(SIGTERM, signalHandler);
#else
    std::signal(SIGQUIT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    std::signal(SIGHUP, signalHandler);
    std::signal(SIGINT, signalHandler);
#endif

    // Notify systemd that we are ready
    sd_notify(0, "READY=1");
    // now wait for incoming OSC messages
    while (stopFlag == 0) { std::this_thread::sleep_for(std::chrono::milliseconds(1)); }

    delete oscctrl;
    delete cwonderConf;

    return EXIT_SUCCESS;
}
