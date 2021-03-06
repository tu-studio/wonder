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

#include "oscinctrl.h"

#include <iostream>
#include <sstream>

#include "cwonder.h"
#include "cwonder_config.h"
#include "liblo_extended.h"
#include "oscstream.h"

// XXX:these have to be set at runtime
Cwonder* OSCControl::cwonder        = NULL;
lo_address OSCControl::timerAddress = NULL;

OSCControl::OSCControl(const char* port) : OSCServer(port) { timerAddress = NULL; }

OSCControl::~OSCControl() {
    if (timerAddress) { lo_address_free(timerAddress); }
}

void OSCControl::addReply(Cwonder* cwonder) {
    // We must pass the this pointer to be able to access
    // member functions/variables inside this callback function
    OSCControl::cwonder = cwonder;

    addMethod("/WONDER/reply", "sis", replyHandler, this);
}

void OSCControl::addMethods() {
    addMethod("/WONDER/jfwonder/frametime", "i", timerFrameHandler, this);
    addMethod("/WONDER/jfwonder/connect", "", timerConnectHandler, this);
    addMethod("/WONDER/jfwonder/error", "s", timerErrorHandler, this);

    // stream commands
    addMethod("/WONDER/stream/render/connect", "ss", renderStreamConnectHandler, this);
    addMethod("/WONDER/stream/render/connect", "s", renderStreamConnectHandler, this);
    addMethod("/WONDER/stream/render/connect", "", renderStreamConnectHandler, this);
    addMethod("/WONDER/stream/render/pong", "i", pongHandler, this);
    addMethod("/WONDER/stream/render/send", NULL, renderSendHandler, this);

    addMethod("/WONDER/stream/visual/connect", "ss", visualStreamConnectHandler, this);
    addMethod("/WONDER/stream/visual/connect", "s", visualStreamConnectHandler, this);
    addMethod("/WONDER/stream/visual/connect", "", visualStreamConnectHandler, this);
    addMethod("/WONDER/stream/visual/pong", "i", pongHandler, this);
    addMethod("/WONDER/stream/visual/send", NULL, visualSendHandler, this);

    addMethod("/WONDER/stream/timer/connect", "ss", timerStreamConnectHandler, this);
    addMethod("/WONDER/stream/timer/connect", "s", timerStreamConnectHandler, this);
    addMethod("/WONDER/stream/timer/connect", "", timerStreamConnectHandler, this);
    addMethod("/WONDER/stream/timer/pong", "i", pongHandler, this);

    // depending on mode ( basic or normal ) assign the appropriate handlers for incoming
    // OSC messages
    if (cwonderConf->basicMode) {
        addMethod("/WONDER/source/activate", "i", forwardToRenderStreamHandler, this);
        addMethod("/WONDER/source/deactivate", "i", forwardToRenderStreamHandler, this);
        addMethod("/WONDER/source/type", "ii", forwardToRenderStreamHandler, this);
        addMethod("/WONDER/source/type", "iif", forwardToRenderStreamHandler, this);
        addMethod("/WONDER/source/position", "iff", forwardToRenderStreamHandler, this);
        addMethod("/WONDER/source/position", "ifff", forwardToRenderStreamHandler, this);
        addMethod("/WONDER/source/position", "iffff", forwardToRenderStreamHandler, this);
        // xxx legacy support for deprecated z coordinate, time and duration are also
        // flipped. xxx // addMethod( "/WONDER/source/position",      "ifffff",
        // forwardToRenderStreamHandler,  this );
        // xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        // //
        addMethod("/WONDER/source/angle", "if", forwardToRenderStreamHandler, this);
        addMethod("/WONDER/source/angle", "iff", forwardToRenderStreamHandler, this);
        addMethod("/WONDER/source/angle", "ifff", forwardToRenderStreamHandler, this);
        addMethod("/WONDER/source/dopplerEffect", "ii", forwardToRenderStreamHandler,
                  this);
        addMethod("/WONDER/listener/position", "iff", forwardToRenderStreamHandler, this);

        addMethod("/WONDER/source/position3D", "ifff", forwardToRenderStreamHandler,
                  this);
        addMethod("/WONDER/source/position3D", "iffff", forwardToRenderStreamHandler,
                  this);
        addMethod("/WONDER/source/position3D", "ifffff", forwardToRenderStreamHandler,
                  this);
    } else {  // normal mode ( not basic )
        // realtime commmands
        addMethod("/WONDER/source/activate", "i", sourceActivateHandler, this);
        addMethod("/WONDER/source/deactivate", "i", sourceDeactivateHandler, this);
        addMethod("/WONDER/source/name", "is", sourceNameHandler, this);
        addMethod("/WONDER/source/type", "ii", sourceTypeHandler, this);
        addMethod("/WONDER/source/type", "iif", sourceTypeHandler, this);
        addMethod("/WONDER/source/angle", "if", sourceAngleHandler, this);
        addMethod("/WONDER/source/angle", "iff", sourceAngleHandler, this);
        addMethod("/WONDER/source/angle", "ifff", sourceAngleHandler, this);
        addMethod("/WONDER/source/position", "iff", sourcePositionHandler, this);
        addMethod("/WONDER/source/position", "ifff", sourcePositionHandler, this);
        addMethod("/WONDER/source/position", "iffff", sourcePositionHandler, this);
        // xxx legacy support for deprecated z coordinate, time and duration are also
        // flipped. xxx // addMethod( "/WONDER/source/position",          "ifffff",
        // sourcePositionHandler,          this );
        // xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
        // //
        addMethod("/WONDER/source/color", "iiii", sourceColorHandler, this);
        addMethod("/WONDER/source/groupID", "ii", sourceGroupIDHandler, this);
        addMethod("/WONDER/source/rotationDirection", "ii",
                  sourceRotationDirectionHandler, this);
        addMethod("/WONDER/source/scalingDirection", "ii", sourceScalingDirectionHandler,
                  this);
        addMethod("/WONDER/source/dopplerEffect", "ii", sourceDopplerEffectHandler, this);

        addMethod("/WONDER/source/position3D", "ifff", sourcePosition3DHandler, this);
        addMethod("/WONDER/source/position3D", "iffff", sourcePosition3DHandler, this);
        addMethod("/WONDER/source/position3D", "ifffff", sourcePosition3DHandler, this);

        addMethod("/WONDER/listener/position", "iff", listenerPositionHandler, this);

        addMethod("/WONDER/group/activate", "i", groupActivateHandler, this);
        addMethod("/WONDER/group/deactivate", "i", groupDeactivateHandler, this);
        addMethod("/WONDER/group/position", "iff", groupPositionHandler, this);
        addMethod("/WONDER/group/color", "iiii", groupColorHandler, this);

        // project commands
        addMethod("/WONDER/project/createWithoutScore", "s",
                  projectCreateWithoutScoreHandler, this);
        addMethod("/WONDER/project/load", "s", projectLoadHandler, this);
        addMethod("/WONDER/project/save", "", projectSaveHandler, this);
        addMethod("/WONDER/project/save", "s", projectSaveAsHandler, this);

        // snapshot commands
        addMethod("/WONDER/snapshot/take", "i", snapshotTakeHandler, this);
        addMethod("/WONDER/snapshot/take", "is", snapshotTakeHandler, this);
        addMethod("/WONDER/snapshot/recall", "if", snapshotRecallHandler, this);
        addMethod("/WONDER/snapshot/delete", "i", snapshotDeleteHandler, this);
        addMethod("/WONDER/snapshot/rename", "is", snapshotRenameHandler, this);
        addMethod("/WONDER/snapshot/copy", "ii", snapshotCopyHandler, this);
    }

    // The generic handler must be added last.
    // Otherwise it would be called instead of the handlers.
    addMethod(NULL, NULL, genericHandler);
}

int OSCControl::replyHandler(handlerArgs) {
    std::ostringstream log;
    log << "[V-OSCServer] reply to: " << &argv[0]->s << " state=" << argv[1]->i
        << " msg=" << &argv[2]->s;

    return 0;
}

int OSCControl::timerFrameHandler(handlerArgs) {
    int ret = 0;

    lo_address from = lo_message_get_source(msg);

    ret = issame(from, timerAddress);

    if (ret == 1) { cwonder->scheduler(argv[0]->i); }

    return 0;
}

int OSCControl::timerConnectHandler(handlerArgs) {
    lo_address from = lo_message_get_source(msg);

    lo_send(from, path, "");

    // save address of the newly connected timer but delete any preexisting timer, we can
    // only have one
    if (timerAddress) {
        lo_address_free(timerAddress);
        timerAddress =
            lo_address_new(lo_address_get_hostname(from), lo_address_get_port(from));
    } else {
        timerAddress =
            lo_address_new(lo_address_get_hostname(from), lo_address_get_port(from));
    }

    return 0;
}

int OSCControl::timerErrorHandler(handlerArgs) {
    lo_address_free(timerAddress);
    timerAddress = NULL;

    return 0;
}

int OSCControl::sourceActivateHandler(handlerArgs) {
    lo_address from = lo_message_get_source(msg);

    int ret = cwonder->setSourceActive(from, argv[0]->i, true);

    lo_send(from, "/WONDER/reply", "sis", path, ret, cwonder->returnString.c_str());

    return 0;
}

int OSCControl::sourceDeactivateHandler(handlerArgs) {
    lo_address from = lo_message_get_source(msg);

    int ret = cwonder->setSourceActive(from, argv[0]->i, false);

    lo_send(from, "/WONDER/reply", "sis", path, ret, cwonder->returnString.c_str());

    return 0;
}

int OSCControl::sourceNameHandler(handlerArgs) {
    lo_address from = lo_message_get_source(msg);

    int ret = cwonder->setSourceName(from, argv[0]->i, &argv[1]->s);

    lo_send(from, "/WONDER/reply", "sis", path, ret, cwonder->returnString.c_str());

    return 0;
}

int OSCControl::sourcePositionHandler(handlerArgs) {
    lo_address from = lo_message_get_source(msg);

    float time     = 0.0;
    float duration = 0.0;

    if (argc == 5) {
        duration = argv[3]->f;
        time     = argv[4]->f;
    } else if (argc == 4) {
        duration = argv[3]->f;
    }

    int ret = cwonder->setSourcePosition(from, argv[0]->i, argv[1]->f, argv[2]->f,
                                         duration, time);

    lo_send(from, "/WONDER/reply", "sis", path, ret, cwonder->returnString.c_str());

    return 0;
}

int OSCControl::sourcePosition3DHandler(handlerArgs) {
    lo_address from = lo_message_get_source(msg);

    float time     = 0.0;
    float duration = 0.0;

    // legacy support for messages with z coordinate which is not used
    // note that time and duration are also flipped in the newer versions
    if (argc == 6) {
        duration = argv[4]->f;
        time     = argv[5]->f;
    } else if (argc == 5) {
        duration = argv[4]->f;
    }

    int ret = cwonder->setSourcePosition3D(from, argv[0]->i, argv[1]->f, argv[2]->f,
                                           argv[3]->f, duration, time);

    lo_send(from, "/WONDER/reply", "sis", path, ret, cwonder->returnString.c_str());

    return 0;
}

int OSCControl::sourceTypeHandler(handlerArgs) {
    lo_address from = lo_message_get_source(msg);

    float time = 0.0;

    if (argc == 3) { time = argv[2]->f; }

    int ret = cwonder->setSourceType(from, argv[0]->i, argv[1]->i, time);

    lo_send(from, "/WONDER/reply", "sis", path, ret, cwonder->returnString.c_str());

    return 0;
}

int OSCControl::sourceAngleHandler(handlerArgs) {
    lo_address from = lo_message_get_source(msg);

    float time     = 0.0;
    float duration = 0.0;

    if (argc == 4) {
        duration = argv[2]->f;
        time     = argv[3]->f;
    } else if (argc == 3) {
        duration = argv[2]->f;
    }

    int ret = cwonder->setSourceAngle(from, argv[0]->i, argv[1]->f, duration, time);

    lo_send(from, "/WONDER/reply", "sis", path, ret, cwonder->returnString.c_str());

    return 0;
}

int OSCControl::sourceGroupIDHandler(handlerArgs) {
    lo_address from = lo_message_get_source(msg);

    int ret = cwonder->setSourceGroupID(from, argv[0]->i, argv[1]->i);

    lo_send(from, "/WONDER/reply", "sis", path, ret, cwonder->returnString.c_str());

    return 0;
}

int OSCControl::sourceColorHandler(handlerArgs) {
    lo_address from = lo_message_get_source(msg);

    int ret =
        cwonder->setSourceColor(from, argv[0]->i, argv[1]->i, argv[2]->i, argv[3]->i);

    lo_send(from, "/WONDER/reply", "sis", path, ret, cwonder->returnString.c_str());

    return 0;
}

int OSCControl::sourceRotationDirectionHandler(handlerArgs) {
    lo_address from = lo_message_get_source(msg);

    int ret = cwonder->setSourceRotationDirection(from, argv[0]->i, (bool)argv[1]->i);

    lo_send(from, "/WONDER/reply", "sis", path, ret, cwonder->returnString.c_str());

    return 0;
}

int OSCControl::sourceScalingDirectionHandler(handlerArgs) {
    lo_address from = lo_message_get_source(msg);

    int ret = cwonder->setSourceScalingDirection(from, argv[0]->i, (bool)argv[1]->i);

    lo_send(from, "/WONDER/reply", "sis", path, ret, cwonder->returnString.c_str());

    return 0;
}

int OSCControl::sourceDopplerEffectHandler(handlerArgs) {
    lo_address from = lo_message_get_source(msg);

    int ret = cwonder->setSourceDopplerEffect(from, argv[0]->i, (bool)argv[1]->i);

    lo_send(from, "/WONDER/reply", "sis", path, ret, cwonder->returnString.c_str());

    return 0;
}

int OSCControl::listenerPositionHandler(handlerArgs) {
    lo_address from = lo_message_get_source(msg);

    int ret = cwonder->setListenerPosition(from, argv[0]->i, argv[1]->f, argv[2]->f);

    lo_send(from, "/WONDER/reply", "sis", path, ret, cwonder->returnString.c_str());

    return 0;
}

int OSCControl::groupActivateHandler(handlerArgs) {
    lo_address from = lo_message_get_source(msg);

    int ret = cwonder->setGroupActive(from, argv[0]->i, true);

    lo_send(from, "/WONDER/reply", "sis", path, ret, cwonder->returnString.c_str());

    return 0;
}

int OSCControl::groupDeactivateHandler(handlerArgs) {
    lo_address from = lo_message_get_source(msg);

    int ret = cwonder->setGroupActive(from, argv[0]->i, false);

    lo_send(from, "/WONDER/reply", "sis", path, ret, cwonder->returnString.c_str());

    return 0;
}

int OSCControl::groupPositionHandler(handlerArgs) {
    lo_address from = lo_message_get_source(msg);

    int ret = cwonder->setGroupPosition(from, argv[0]->i, argv[1]->f, argv[2]->f);

    lo_send(from, "/WONDER/reply", "sis", path, ret, cwonder->returnString.c_str());

    return 0;
}

int OSCControl::groupColorHandler(handlerArgs) {
    lo_address from = lo_message_get_source(msg);

    int ret =
        cwonder->setGroupColor(from, argv[0]->i, argv[1]->i, argv[2]->i, argv[3]->i);

    lo_send(from, "/WONDER/reply", "sis", path, ret, cwonder->returnString.c_str());

    return 0;
}

int OSCControl::projectCreateWithoutScoreHandler(handlerArgs) {
    int ret = cwonder->createProject(&argv[0]->s);

    lo_address from = lo_message_get_source(msg);
    lo_send(from, "/WONDER/reply", "sis", path, ret, cwonder->returnString.c_str());

    return 0;
}

int OSCControl::projectLoadHandler(handlerArgs) {
    lo_address from = lo_message_get_source(msg);

    int ret = cwonder->loadProject(&argv[0]->s);

    lo_send(from, "/WONDER/reply", "sis", path, ret, cwonder->returnString.c_str());

    return 0;
}

int OSCControl::projectSaveHandler(handlerArgs) {
    int ret = cwonder->saveProject();

    lo_address from = lo_message_get_source(msg);
    lo_send(from, "/WONDER/reply", "sis", path, ret, cwonder->returnString.c_str());

    return 0;
}

int OSCControl::projectSaveAsHandler(handlerArgs) {
    int ret = cwonder->saveProjectAs(&argv[0]->s);

    lo_address from = lo_message_get_source(msg);
    lo_send(from, "/WONDER/reply", "sis", path, ret, cwonder->returnString.c_str());

    return 0;
}

int OSCControl::snapshotTakeHandler(handlerArgs) {
    int ret = 0;

    if (argc == 1) {  // unnamed snapshot
        ret = cwonder->takeSnapshot(argv[0]->i);
    } else {  // named snapshot
        ret = cwonder->takeSnapshot(argv[0]->i, &argv[1]->s);
    }

    lo_address from = lo_message_get_source(msg);
    lo_send(from, "/WONDER/reply", "sis", path, ret, cwonder->returnString.c_str());

    return 0;
}

int OSCControl::snapshotRecallHandler(handlerArgs) {
    int ret = cwonder->recallSnapshot(argv[0]->i, argv[1]->f);

    lo_address from = lo_message_get_source(msg);
    lo_send(from, "/WONDER/reply", "sis", path, ret, cwonder->returnString.c_str());

    return 0;
}

int OSCControl::snapshotDeleteHandler(handlerArgs) {
    int ret = cwonder->deleteSnapshot(argv[0]->i);

    lo_address from = lo_message_get_source(msg);
    lo_send(from, "/WONDER/reply", "sis", path, ret, cwonder->returnString.c_str());

    return 0;
}

int OSCControl::snapshotRenameHandler(handlerArgs) {
    int ret = cwonder->renameSnapshot(argv[0]->i, &argv[1]->s);

    lo_address from = lo_message_get_source(msg);
    lo_send(from, "/WONDER/reply", "sis", path, ret, cwonder->returnString.c_str());

    return 0;
}

int OSCControl::snapshotCopyHandler(handlerArgs) {
    int ret = cwonder->copySnapshot(argv[0]->i, argv[1]->i);

    lo_address from = lo_message_get_source(msg);
    lo_send(from, "/WONDER/reply", "sis", path, ret, cwonder->returnString.c_str());

    return 0;
}

int OSCControl::renderStreamConnectHandler(handlerArgs) {
    int ret = 0;

    if (argc == 2) {
        ret = cwonder->renderStreamConnect(&argv[0]->s, &argv[1]->s);
    } else {
        lo_address from = lo_message_get_source(msg);

        string name = "";

        if (argc == 1) { name = &argv[0]->s; }

        ret = cwonder->renderStreamConnect(lo_address_get_hostname(from),
                                           lo_address_get_port(from), name);
    }

    lo_address from = lo_message_get_source(msg);
    lo_send(from, "/WONDER/reply", "sis", path, ret, cwonder->returnString.c_str());

    return 0;
}

int OSCControl::visualStreamConnectHandler(handlerArgs) {
    int ret = 0;

    if (argc == 2) {
        ret = cwonder->visualStreamConnect(&argv[0]->s, &argv[1]->s);
    } else {
        string name = "";

        if (argc == 1) { name = &argv[0]->s; }

        lo_address from = lo_message_get_source(msg);
        ret             = cwonder->visualStreamConnect(lo_address_get_hostname(from),
                                           lo_address_get_port(from), name);
    }

    lo_address from = lo_message_get_source(msg);
    lo_send(from, "/WONDER/reply", "sis", path, ret, cwonder->returnString.c_str());

    return 0;
}

int OSCControl::timerStreamConnectHandler(handlerArgs) {
    int ret = 0;

    lo_address from = lo_message_get_source(msg);

    if (argc == 2) {
        ret = cwonder->timerStreamConnect(&argv[0]->s, &argv[1]->s);
    } else {
        string name = "";

        if (argc == 1) { name = &argv[0]->s; }

        ret = cwonder->timerStreamConnect(lo_address_get_hostname(from),
                                          lo_address_get_port(from), name);
    }

    lo_send(from, "/WONDER/reply", "sis", path, ret, cwonder->returnString.c_str());

    return 0;
}

int OSCControl::renderSendHandler(handlerArgs) {
    cwonder->renderStream->send((char*)types, argc, argv, msg);

    return 0;
}

int OSCControl::visualSendHandler(handlerArgs) {
    cwonder->visualStream->send((char*)types, argc, argv, msg);

    return 0;
}

int OSCControl::pongHandler(handlerArgs) {
    lo_address from = lo_message_get_source(msg);

    if ((string)path == (string) "/WONDER/stream/render/pong") {
        cwonder->renderStream->pong(argv[0]->i, from);
    } else if ((string)path == (string) "/WONDER/stream/visual/pong") {
        cwonder->visualStream->pong(argv[0]->i, from);
    } else if ((string)path == (string) "/WONDER/stream/timer/pong") {
        cwonder->timerStream->pong(argv[0]->i, from);
    }

    return 0;
}

int OSCControl::forwardToRenderStreamHandler(handlerArgs) {
    cwonder->renderStream->send((char*)path, (char*)types, argc, argv, msg);

    return 0;
}

int OSCControl::forwardToVisualStreamHandler(handlerArgs) {
    cwonder->visualStream->send((char*)path, (char*)types, argc, argv, msg);

    return 0;
}

int OSCControl::genericHandler(handlerArgs) {
    lo_address from = lo_message_get_source(msg);
    lo_send(from, "/WONDER/reply", "sis", path, 1,
            "[cwonder]: No handler implemented for message");

    return 0;
}
