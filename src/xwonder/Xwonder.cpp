/*
 * (c) Copyright 2006-7 -- Hans-Joachim Mond
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
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

//----------------------------------Doxygen-----------------------------------//
/*!
 *      \mainpage Xwonder
 *
 *      \section overviewSection Overview
 *      Xwonder is a graphical user interface for WONDER (Wave field synthesis of new
 * dimensions of electronic music in realtime), which is a softwarebased applicationbundle
 * for wave field synthesis consisting of several modules. The aim of xwonder is to
 * provide an interface, which facilitates the use of WONDER's functions and establishes a
 * stringent workflow. It should allow experienced users as well as those new to WONDER to
 * accomplish their tasks with a minimum training period. To facilitate control over the
 * WONDER system Xwonder should correctly represent the current sonic state of the system
 * at all times and allow free and intuitive manipulation of this state.
 *
 *      \section installSection Installation
 *      For instructions on building and installing xwonder see \ref installPage.
 *
 *      \section manualSection User's Guide
 *      For instructions on how to use xwonder see \ref manualPage.
 */
//----------------------------------------------------------------------------//

//----------------------------------Doxygen-----------------------------------//
/*!
 *      \page installPage Building and installation
 *
 *      \section dependencies Dependencies
 *      The recommended way to build Xwonder is by using the software construction tool
 * SCons.<br> A file called SConstruct (the default filename Scons is looking for when no
 * further buildfile is specified) should be provided along with wonder's sourcecode, so
 * you will only have to call "scons" with the argument "xwonder=1" in Wonder's directory.
 * For further instructions on wonder's build system please have a look at the Wonder
 * documentation.
 *
 *      In order to build Xwonder from sourcecode with SCons you will need the following:
 *      - Trolltech Qt 4.x (preferably 4.3.2 or higher)
 *      - Qt4 tool for scons (provided with Wonder's sourcecode)
 *      - liblo (lightweight OSC implementation)
 *      - Wonder library
 *      - pkg-config
 *      - and of course a c++ compiler
 *
 */
//----------------------------------------------------------------------------//

//----------------------------------Doxygen-----------------------------------//
/*!
 *      \page manualPage User's guide
 *
 *      \section manualIntro Introduction
 *      Xwonder works like a remote control for the WONDER system. Since WONDER is a
 * distributed application, it is possible that certain components may run on different
 * computers. Xwonder's only interface to the whole WONDER system is the cwonder programm.
 * If this component is not running, then no communication will be possible. If at startup
 * Xwonder cannot establish a connection to cwonder it will notify you. If at runtime the
 * connection to cwonder is lost, xwonder will shut down.
 *
 *      The workspace of Xwonder consists of a channelarea on the left side similiar to
 * common audio sequencing software (like e.g. ardour) and a graphical representation of
 * the auditory scene in the main area of Xwonder. In this main area a representation of
 * the current listening room is displayed. Soundsources are displayed at their current
 * position (pointsources as spheres, planewave sources as an arrow with a baseplate).
 *      Sources can be grouped with the use of groupobjects (displayed as cubes). A source
 * added to a group is assigned the color of the groupobject representing this group. Each
 * group of sources has a color, which can be changed by changing the color of one of the
 * sources belonging to this group.
 *
 *      \section Startup
 *      Just type "xwonder" in your commandline or use doubleclick if you use a graphical
 * filebrowser. When starting xwonder from the commandline/terminal you can provide the
 * following arguments:
 *      - -i x Where x is the IP-address of the computer that is running cwonder (default
 * is 127.0.0.1)
 *      - -p x Where x is the port on which cwonder listens (default is 58100)
 *      - -o x Where x is the port on which xwonder listens (default is 58000)
 *      - -d Run in demomode, i.e. without connecting to cwonder. Only usefull for demoing
 * purposes.
 *      - -v Start in verbose mode. Xwonder prints out the OSC messages it receives.
 *      - -h Print help on commandline arguments.
 *      Starting xwonder without arguments uses the default values (i.e. assuming cwonder
 * runs on the same computer as Xwonder)
 *
 *      \section Mouseoperation
 *      Most of Xwonder's functionality can easily be accessed by mouse or similar
 * pointing devices.<br> Here is an overview of the possible mouseoperations in the
 * central windowelement of Xwonder in which the soundsources, groupobjects as well as the
 * surrounding room are displayed.<br> (L = left mousebutton, R = right mousebutton)<br>
 *
 *      - When clicking on the background (only if view is not "locked", see "View" menu):
 *              - L + Up/down           = zoom in/out
 *              - L + CTRL + movement   = movement of the whole image
 *              - L + ALT + Up/Down     = x-axis rotation
 *              - L + ALT + Right/Left  = y-axis rotation
 *              - L + SHIFT + Up/Down   = z-axis rotation
 *              - MouseWheel            = zoom in/out
 *
 *      - When clicking on a source (and thereby selecting it):
 *              - L + movement          = move source
 *              - L + CTRL + movement   = movement only on x-axis
 *              - L + SHIFT + movement  = movement only on y-axis
 *              - L (hold) + R + Right  = rotate planewave-source clockwise
 *              - L (hold) + R + Left   = rotate planewave-source counterclockwise
 *
 *      - When clicking on a groupobject (and thereby selecting it):
 *              - L + movement              = move whole group
 *              - L + ALT + Up/Down         = rotate clock-/counterclockwise
 *              - L + ALT + SHIFT +Up/Down  = expande/shrink
 *              - L (hold) + R + movement   = move only groupobject (not the sources of
 * this group)
 *
 *      - Grouping of sources (first select a groupobject):
 *              - L + CTRL = add/remove selected source to/from group
 *
 *
 *      Important: To rotate a planewave-source you have to first select it with the left
 * mousebutton and while holding the left mousebutton additionally press the right
 * mousebutton and while holding both buttons down drag the mouse to the left or right.
 * Pushing both buttons at once does not work.
 *
 *      Stickyclick: This is a feature which facilitates movement of sources with the
 * mouse by locking your mouse onto the selected source (or group). So with stickyclick
 * activated you just leftclick on a source, release the button and move the source. To
 * unlock just click again. This works also when clicking on the background for movement
 * of the room (only when the view is not locked).
 *
 *      \section Keyboardoperation
 *      Xwonder provides comprehensive keyboard shortcuts. For actions accessible by menu
 * the shortcuts can be found in the menus. Here is a list of commands which affect the
 * presentation of the sources in Xwonder as well as commands for soundsource movements.
 *
 *      - General:
 *              - Spacebar = start/stop scoreplayer
 *
 *      - Whole display (only when view is not "locked"):
 *              - x = positive x-axis rotation
 *              - y = positive y-axis rotation
 *              - z = positive z-axis rotation
 *              - X = negative x-axis rotation
 *              - Y = negative y-axis rotation
 *              - Z = negative z-axis rotation
 *              - g = zoom in
 *              - h = zoom out
 *
 *              independent of viewlock:
 *              - b = smaller sources
 *              - B = bigger sources
 *
 *      - Selected source:
 *              - Up    = move current source "up"    (positive y-direction)
 *              - Down  = move current source "down"  (negative y-direction)
 *              - Right = move current source "right" (positive x-direction)
 *              - Left  = move current source "left"  (negative x-direction)
 *              - r     = turn record on/off
 *
 *      - Selected group:
 *              - Up    = move whole group "up"    (positive y-direction)
 *              - Down  = move whole group "down"  (negative y-direction)
 *              - Right = move whole group "right" (positive x-direction)
 *              - Left  = move whole group "left"  (negative x-direction)
 *              - r     = turn record for whole group on/off
 *
 *              - Up    + ALT = move only groupcenter "up"    (positive y-direction)
 *              - Down  + ALT = move only groupcenter "down"  (negative y-direction)
 *              - Right + ALT = move only groupcenter "right" (positive x-direction)
 *              - Left  + ALT = move only groupcenter "left"  (negative x-direction)
 */
//----------------------------------------------------------------------------//

//----------------------------------Doxygen-----------------------------------//
/*!
 *      \brief
 *      Xwonder's main function
 *
 *      \details
 *      This is the startup function for xwonder in which OpenGL support is verified,
 *      configuration data is read, the OSC-Server is started, xwonder's style is set
 *      and the connection to cwonder's visual stream is established.
 *
 *      - Order of configuration evaluation
 *              - commandline arguments
 *              - default values
 *
 *      \author
 *      Hans-Joachim Mond
 *
 *      \date
 *      09.04.2008
 */
//----------------------------------------------------------------------------//

//----------------------------------includes----------------------------------//

#include <QApplication>
#include <QGLFormat>
#include <iostream>
using std::cerr;
using std::endl;

#include "OSCReceiver.h"
#include "XwonderConfig.h"
#include "XwonderMainWindow.h"
#include "oscin.h"

//----------------------------------------------------------------------------//

int main(int argc, char* argv[]) {
    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/xwonder"));

    // check OpenGL support
    if (!QGLFormat::hasOpenGL()) {
        std::cerr << "Unable to run xwonder!" << std::endl
                  << "This system has no OpenGL support!" << std::endl;
        return 1;
    }

    // evaluate commandline arguments
    // if no arguments are given default values are used
    xwConf = new XwonderConfig(argc, argv, &app);

    // start the OSC engine
    try {
        oscReceiver = new OSCReceiver(xwConf->xwonderPort.c_str(), &app);
    }
    catch (OSCServer::EServ e) {
        std::cerr << "Unable to run xwonder!" << std::endl
                  << "OSC communication could not be established." << std::endl
                  << "Maybe the port xwonder tried to use is already in use." << std::endl
                  << "Please try another port (use: xwonder -o portnumber)" << std::endl;
        return -1;
    }

    XwonderMainWindow xwonder;
    xwonder.showMaximized();

    // connect to cwonder or init demo mode
    if (!xwConf->runWithoutCwonder) {
        oscReceiver->connectToCwonder();
    } else {
        oscReceiver->emitMaxNoSourcesWasSet();
    }

    return app.exec();

}  // main
