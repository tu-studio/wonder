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

#include "oscin.h"

class Cwonder;

// arguments of the handler functions
#define handlerArgs const char* path, const char* types, lo_arg** argv, int argc, lo_message msg, void* user_data



// The OSC Server for cwonder
class OSCControl : public OSCServer {

public:
    OSCControl(const char* port = "58100");

    ~OSCControl();

    // add the reply message /WONDER/reply to the known messages
    // pass a pointer to the cwonder object to be able to access member variables
    void addReply(Cwonder* cwonder);

    // add the cwonder specific messages to the list of known messages
    void addMethods();

private:
    static Cwonder* cwonder;

    // the address of the timer will be set when the timer requests connection
    static lo_address timerAddress;

    static int replyHandler(handlerArgs);

    // scheduler
    static int timerFrameHandler(handlerArgs);
    static int timerConnectHandler(handlerArgs);
    static int timerErrorHandler(handlerArgs);

    // source
    static int sourceActivateHandler(handlerArgs);
    static int sourceDeactivateHandler(handlerArgs);
    static int sourceNameHandler(handlerArgs);
    static int sourcePositionHandler(handlerArgs);
    static int sourcePosition3DHandler(handlerArgs);
    static int sourceTypeHandler(handlerArgs);
    static int sourceAngleHandler(handlerArgs);
    static int sourceGroupIDHandler(handlerArgs);
    static int sourceColorHandler(handlerArgs);
    static int sourceRotationDirectionHandler(handlerArgs);
    static int sourceScalingDirectionHandler(handlerArgs);
    static int sourceDopplerEffectHandler(handlerArgs);

    // listener
    static int listenerPositionHandler(handlerArgs);

    // group
    static int groupActivateHandler(handlerArgs);
    static int groupDeactivateHandler(handlerArgs);
    static int groupPositionHandler(handlerArgs);
    static int groupColorHandler(handlerArgs);

    // project
    static int projectCreateWithoutScoreHandler(handlerArgs);
    static int projectLoadHandler(handlerArgs);
    static int projectSaveHandler(handlerArgs);
    static int projectSaveAsHandler(handlerArgs);

    // snapshot
    static int snapshotTakeHandler(handlerArgs);
    static int snapshotRecallHandler(handlerArgs);
    static int snapshotDeleteHandler(handlerArgs);
    static int snapshotRenameHandler(handlerArgs);
    static int snapshotCopyHandler(handlerArgs);

    // streams
    static int renderStreamConnectHandler(handlerArgs);
    static int visualStreamConnectHandler(handlerArgs);
    static int timerStreamConnectHandler(handlerArgs);

    static int renderSendHandler(handlerArgs);
    static int visualSendHandler(handlerArgs);
    static int pongHandler(handlerArgs);

    static int forwardToRenderStreamHandler(handlerArgs);
    static int forwardToVisualStreamHandler(handlerArgs);

    // generic handler
    static int genericHandler(handlerArgs);

}; // class OSCControl
