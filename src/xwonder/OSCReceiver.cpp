/*
 * (c) Copyright 2006-8 -- Hans-Joachim Mond
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
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

//----------------------------------includes----------------------------------//

#include "OSCReceiver.h"

#include <QApplication>
#include <QMessageBox>
#include <QString>
#include <QTimer>
#include <iostream>

#include "XwonderConfig.h"
using std::cerr;
using std::cout;
using std::endl;
using std::string;

//----------------------------------------------------------------------------//

//--------------------------------constructors--------------------------------//

OSCReceiver::OSCReceiver(const char* port, QObject* parent)
    : QObject(parent), OSCServer(port) {
    addReply();
    addMethods();

    // to check for regular pings from cwonder a timer is used. It is restarted with each
    // received ping. It the timer times out a signal is send to which causes xwonder to
    // quit
    startup      = true;
    cwonderTimer = new QTimer(this);

    // disable timeout functionality if timeout is set to 0
    if (xwConf->cwonderPingTimeout != 0) {
        cwonderTimer->setSingleShot(true);
        cwonderTimer->setInterval(xwConf->cwonderPingTimeout);
        // connect timer to slots
        connect(cwonderTimer, SIGNAL(timeout()), this, SLOT(emitCwonderConnectionLost()));
        connect(this, SIGNAL(startCwonderTimer()), cwonderTimer, SLOT(start()));
        connect(this, SIGNAL(stopCwonderTimer()), cwonderTimer, SLOT(stop()));
    }

    start();
}

//----------------------------end of constructors-----------------------------//

void OSCReceiver::addReply() {
    addMethod("/WONDER/reply", "sis", OSCReceiver::replyHandler, this);
}

void OSCReceiver::addMethods() {
    addMethod("/WONDER/source/activate", "i", OSCReceiver::sourceActivateHandler, this);
    addMethod("/WONDER/source/deactivate", "i", OSCReceiver::sourceDeactivateHandler,
              this);
    addMethod("/WONDER/source/position", "iff", OSCReceiver::sourcePositionHandler, this);
    addMethod("/WONDER/source/angle", "if", OSCReceiver::sourceAngleHandler, this);
    addMethod("/WONDER/source/type", "ii", OSCReceiver::sourceTypeHandler, this);
    addMethod("/WONDER/source/name", "is", OSCReceiver::sourceNameHandler, this);
    addMethod("/WONDER/source/color", "iiii", OSCReceiver::sourceColorHandler, this);
    addMethod("/WONDER/source/groupID", "ii", OSCReceiver::sourceGroupIDHandler, this);
    addMethod("/WONDER/source/rotationDirection", "ii",
              OSCReceiver::sourceRotationDirectionHandler, this);
    addMethod("/WONDER/source/scalingDirection", "ii",
              OSCReceiver::sourceScalingDirectionHandler, this);
    addMethod("/WONDER/source/dopplerEffect", "ii",
              OSCReceiver::sourceDopplerEffectHandler, this);
    addMethod("/WONDER/score/source/enableRecord", "ii",
              OSCReceiver::sourceRecordModeHandler, this);
    addMethod("/WONDER/score/source/enableRead", "ii", OSCReceiver::sourceReadModeHandler,
              this);

    addMethod("/WONDER/group/activate", "i", OSCReceiver::groupActivateHandler, this);
    addMethod("/WONDER/group/deactivate", "i", OSCReceiver::groupDeactivateHandler, this);
    addMethod("/WONDER/group/position", "iff", OSCReceiver::groupPositionHandler, this);
    addMethod("/WONDER/group/color", "iiii", OSCReceiver::groupColorHandler, this);

    addMethod("/WONDER/mtctime", "iiii", OSCReceiver::mtcTimeHandler, this);
    addMethod("/WONDER/score/stop", "", OSCReceiver::scoreplayerStoppedHandler, this);
    addMethod("/WONDER/score/play", "", OSCReceiver::scoreplayerStartedHandler, this);
    addMethod("/WONDER/score/enableRecord", "i",
              OSCReceiver::scoreplayerRecordModeHandler, this);
    addMethod("/WONDER/score/enableRead", "i", OSCReceiver::scoreplayerReadModeHandler,
              this);
    addMethod("/WONDER/score/enableMMC", "i", OSCReceiver::mmcChangedHandler, this);
    addMethod("/WONDER/score/enableMSRC", "i", OSCReceiver::msrcChangedHandler, this);
    addMethod("/WONDER/score/status", NULL, OSCReceiver::scoreplayerStatusHandler, this);

    addMethod("/WONDER/global/maxNoSources", "i", OSCReceiver::maxNoSourcesHandler, this);
    addMethod("/WONDER/global/renderPolygon", NULL, OSCReceiver::renderPolygonHandler,
              this);
    addMethod("/WONDER/project/xmlDump", "is", OSCReceiver::projectXMLDumpHandler, this);

    addMethod("/WONDER/stream/connected", "sss",
              OSCReceiver::streamclientConnectedHandler, this);
    addMethod("/WONDER/stream/disconnected", "sss",
              OSCReceiver::streamclientDisconnectedHandler, this);

    addMethod("/WONDER/stream/visual/ping", "i", OSCReceiver::visualpingHandler, this);
    addMethod(NULL, NULL, OSCReceiver::genericHandler, this);
}

void OSCReceiver::connectToCwonder() const {
    lo_send(xwConf->cwonderAddr, "/WONDER/stream/visual/connect", "s",
            xwConf->name.toStdString().c_str());

    // start timer to see if cwonder responds
    emitStartCwonderTimer();
}

void OSCReceiver::emitCwonderConnectionLost() const {
    QString aboutToQuitMessage = "";

    if (startup) {
        aboutToQuitMessage = "The connection to cwonder could not be established!\n";
        aboutToQuitMessage +=
            "Please make sure cwonder is running. If cwonder "
            "is supposed to run on a different computer than Xwonder, then please "
            "make sure that you provided Xwonder with the correct\n - IP address\n - "
            "port\n"
            "for cwonder and that your network access and configuration are correct.";
        aboutToQuitMessage += "\n\nXwonder will exit now!";
    } else {
        aboutToQuitMessage = "Connection to cwonder lost!\n";
        aboutToQuitMessage +=
            "Please check:\n - Your network connection\n - If cwonder is still running.";
        aboutToQuitMessage += "\n\nXwonder will exit now!";
    }

    QMessageBox::critical(0, "XWonder - cwonder connection error", aboutToQuitMessage,
                          QMessageBox::Ok);

    QApplication::instance()->quit();
}

//---------------------------source-related-emitter---------------------------//

void OSCReceiver::emitSourceActivated(int sourceID) const {
    emit sourceActivated(sourceID);
}

void OSCReceiver::emitSourceDeactivated(int sourceID) const {
    emit sourceDeactivated(sourceID);
}

void OSCReceiver::emitSourcePositionChanged(int sourceID, float x, float y) const {
    emit sourcePositionChanged(sourceID, x, y);
}

void OSCReceiver::emitSourceOrientationChanged(int sourceID, float orientation) const {
    emit sourceOrientationChanged(sourceID, orientation);
}

void OSCReceiver::emitSourceTypeChanged(int sourceID, int type) const {
    bool planewave = !((bool)type);

    emit sourceTypeChanged(sourceID, planewave);
}

void OSCReceiver::emitSourceNameChanged(int sourceID, const char* name) const {
    emit sourceNameChanged(sourceID, name);
}

void OSCReceiver::emitSourceColorChanged(int sourceID, int red, int green,
                                         int blue) const {
    if (red < 0 || red > 255) { red = 128; }

    if (green < 0 || green > 255) { green = 128; }

    if (blue < 0 || blue > 255) { blue = 128; }

    QColor color(red, green, blue);

    emit sourceColorChanged(sourceID, color);
}

void OSCReceiver::emitSourceGroupIDChanged(int sourceID, int groupID) const {
    emit sourceGroupIDChanged(sourceID, groupID);
}

void OSCReceiver::emitSourceRotationDirectionChanged(int sourceID, bool inverted) const {
    emit sourceRotationDirectionChanged(sourceID, inverted);
}

void OSCReceiver::emitSourceScalingDirectionChanged(int sourceID, bool inverted) const {
    emit sourceScalingDirectionChanged(sourceID, inverted);
}

void OSCReceiver::emitSourceDopplerEffectChanged(int sourceID, bool dopplerOn) const {
    emit sourceDopplerEffectChanged(sourceID, dopplerOn);
}

void OSCReceiver::emitSourceRecordModeChanged(int id, bool recordEnabled) const {
    emit sourceRecordModeChanged(id, recordEnabled, true);
}

void OSCReceiver::emitSourceReadModeChanged(int id, bool readEnabled) const {
    emit sourceReadModeChanged(id, readEnabled, true);
}

//------------------------end-of-source-related-emitter------------------------//

//----------------------------group-related-emitter----------------------------//

void OSCReceiver::emitGroupActivated(int groupID) const {
    emit groupActivated(groupID, true);
}

void OSCReceiver::emitGroupDeactivated(int groupID) const {
    emit groupDeactivated(groupID, true);
}

void OSCReceiver::emitGroupPositionChanged(int groupID, float x, float y) const {
    emit groupPositionChanged(groupID, x, y);
}

void OSCReceiver::emitGroupColorChanged(int groupID, int red, int green, int blue) const {
    if (red < 0 || red > 255) { red = 128; }

    if (green < 0 || green > 255) { green = 128; }

    if (blue < 0 || blue > 255) { blue = 128; }

    QColor color(red, green, blue);

    emit groupColorChanged(groupID, color, true);
}

//------------------------end-of-group-related-emitter-------------------------//

//------------------------scoreplayer-related-emitter--------------------------//

void OSCReceiver::emitTimeChanged(int h, int m, int s, int ms) const {
    emit timeChanged(h, m, s, ms);
}

void OSCReceiver::emitScoreplayerStopped() const { emit scoreplayerStopped(); }

void OSCReceiver::emitScoreplayerStarted() const { emit scoreplayerStarted(); }

void OSCReceiver::emitScoreplayerRecordModeChanged(int recordEnabled) const {
    emit scoreplayerRecordModeChanged((bool)recordEnabled, true);
}

void OSCReceiver::emitScoreplayerReadModeChanged(int readEnabled) const {
    emit scoreplayerReadModeChanged((bool)readEnabled, true);
}

void OSCReceiver::emitMmcChanged(bool mmcEnabled) const {
    emit mmcChanged(mmcEnabled, true);
}

void OSCReceiver::emitMsrcChanged(bool msrcEnabled) const {
    emit msrcChanged(msrcEnabled, true);
}

//--------------------end-of-scoreplayer-related-emitter-----------------------//

//---------------------------------misc-emitter--------------------------------//

void OSCReceiver::emitProjectXMLDump(const char* xmlProject) const {
    QString temp = xmlProject;
    emit projectXMLDump(temp);
}

void OSCReceiver::emitOSCErrorMessage(QString errorMsg) const {
    emit OSCErrorMessage(errorMsg);
}

void OSCReceiver::emitStartCwonderTimer() const { emit startCwonderTimer(); }

void OSCReceiver::emitStopCwonderTimer() const { emit stopCwonderTimer(); }

void OSCReceiver::emitRenderPolygonReceived() const { emit renderPolygonReceived(); }

void OSCReceiver::emitMaxNoSourcesWasSet() const { emit maxNoSourcesWasSet(); }

void OSCReceiver::emitStreamclientConnected(QString host, QString port,
                                            QString name) const {
    emit streamclientConnected(host, port, name);
}

void OSCReceiver::emitStreamclientDisconnected(QString host, QString port,
                                               QString name) const {
    emit streamclientDisconnected(host, port, name);
}

//------------------------------end-of-misc-emitter----------------------------//

//---------------------------source-related-handler---------------------------//

int OSCReceiver::sourceActivateHandler(handlerArgs) {
    if (xwConf->verbose) { print("sourceActivate", argNames); }

    oscReceiver->emitSourceActivated(argv[0]->i);
    return 0;
}

int OSCReceiver::sourceDeactivateHandler(handlerArgs) {
    if (xwConf->verbose) { print("sourceDeactivate", argNames); }

    oscReceiver->emitSourceDeactivated(argv[0]->i);
    return 0;
}

int OSCReceiver::sourcePositionHandler(handlerArgs) {
    if (xwConf->verbose) { print("sourcePosition", argNames); }

    oscReceiver->emitSourcePositionChanged(argv[0]->i, argv[1]->f, argv[2]->f);
    return 0;
}

int OSCReceiver::sourceAngleHandler(handlerArgs) {
    if (xwConf->verbose) { print("sourceAngle", argNames); }

    oscReceiver->emitSourceOrientationChanged(argv[0]->i, argv[1]->f);

    return 0;
}

int OSCReceiver::sourceTypeHandler(handlerArgs) {
    if (xwConf->verbose) { print("sourceType", argNames); }

    oscReceiver->emitSourceTypeChanged(argv[0]->i, argv[1]->i);

    return 0;
}

int OSCReceiver::sourceNameHandler(handlerArgs) {
    if (xwConf->verbose) { print("sourceName", argNames); }

    oscReceiver->emitSourceNameChanged(argv[0]->i, &argv[1]->s);
    return 0;
}

int OSCReceiver::sourceColorHandler(handlerArgs) {
    if (xwConf->verbose) { print("sourceColor", argNames); }

    oscReceiver->emitSourceColorChanged(argv[0]->i, argv[1]->i, argv[2]->i, argv[3]->i);
    return 0;
}

int OSCReceiver::sourceGroupIDHandler(handlerArgs) {
    if (xwConf->verbose) { print("sourceGroupID", argNames); }

    oscReceiver->emitSourceGroupIDChanged(argv[0]->i, (unsigned int)argv[1]->i);

    return 0;
}

int OSCReceiver::sourceRotationDirectionHandler(handlerArgs) {
    if (xwConf->verbose) { print("RotationDirection", argNames); }

    oscReceiver->emitSourceRotationDirectionChanged(argv[0]->i, argv[1]->i);

    return 0;
}

int OSCReceiver::sourceScalingDirectionHandler(handlerArgs) {
    if (xwConf->verbose) { print("ScalingDirection", argNames); }

    oscReceiver->emitSourceScalingDirectionChanged(argv[0]->i, argv[1]->i);

    return 0;
}

int OSCReceiver::sourceDopplerEffectHandler(handlerArgs) {
    if (xwConf->verbose) { print("ScalingDirection", argNames); }

    oscReceiver->emitSourceDopplerEffectChanged(argv[0]->i, argv[1]->i);

    return 0;
}

int OSCReceiver::sourceRecordModeHandler(handlerArgs) {
    if (xwConf->verbose) { print("sourceRecordMode", argNames); }

    oscReceiver->emitSourceRecordModeChanged(argv[0]->i, argv[1]->i);
    return 0;
}

int OSCReceiver::sourceReadModeHandler(handlerArgs) {
    if (xwConf->verbose) { print("sourceReadMode", argNames); }

    oscReceiver->emitSourceReadModeChanged(argv[0]->i, argv[1]->i);
    return 0;
}

//------------------------end-of-source-related-handler------------------------//

//----------------------------group-related-handler----------------------------//

int OSCReceiver::groupActivateHandler(handlerArgs) {
    if (xwConf->verbose) { print("groupActivate", argNames); }

    oscReceiver->emitGroupActivated(argv[0]->i);
    return 0;
}

int OSCReceiver::groupDeactivateHandler(handlerArgs) {
    if (xwConf->verbose) { print("groupDeactivate", argNames); }

    oscReceiver->emitGroupDeactivated(argv[0]->i);
    return 0;
}

int OSCReceiver::groupPositionHandler(handlerArgs) {
    if (xwConf->verbose) { print("groupPosition", argNames); }

    oscReceiver->emitGroupPositionChanged(argv[0]->i, argv[1]->f, argv[2]->f);
    return 0;
}

int OSCReceiver::groupColorHandler(handlerArgs) {
    if (xwConf->verbose) { print("groupColor", argNames); }

    oscReceiver->emitGroupColorChanged(argv[0]->i, argv[1]->i, argv[2]->i, argv[3]->i);
    return 0;
}

//-------------------------end-of-gourp-related-handler------------------------//

//------------------------scoreplayer-related-handler--------------------------//

int OSCReceiver::mtcTimeHandler(handlerArgs) {
    // better not verbose this, that would be way too much
    if (xwConf->verbose) { print("mtcTime", argNames); }

    oscReceiver->emitTimeChanged(argv[0]->i, argv[1]->i, argv[2]->i, argv[3]->i);

    return 0;
}

int OSCReceiver::scoreplayerStoppedHandler(handlerArgs) {
    if (xwConf->verbose) { print("scoreplayerStopped", argNames); }

    oscReceiver->emitScoreplayerStopped();
    return 0;
}

int OSCReceiver::scoreplayerStartedHandler(handlerArgs) {
    if (xwConf->verbose) { print("scoreplayerStarted", argNames); }

    oscReceiver->emitScoreplayerStarted();
    return 0;
}

int OSCReceiver::scoreplayerRecordModeHandler(handlerArgs) {
    if (xwConf->verbose) { print("scoreplayerRecordMode", argNames); }

    oscReceiver->emitScoreplayerRecordModeChanged(argv[0]->i);
    return 0;
}

int OSCReceiver::scoreplayerReadModeHandler(handlerArgs) {
    if (xwConf->verbose) { print("scoreplayerReadMode", argNames); }

    oscReceiver->emitScoreplayerReadModeChanged(argv[0]->i);
    return 0;
}

int OSCReceiver::mmcChangedHandler(handlerArgs) {
    if (xwConf->verbose) { print("mmcChanged", argNames); }

    oscReceiver->emitMmcChanged(argv[0]->i);
    return 0;
}

int OSCReceiver::msrcChangedHandler(handlerArgs) {
    if (xwConf->verbose) { print("msrcChanged", argNames); }

    oscReceiver->emitMsrcChanged(argv[0]->i);
    return 0;
}

int OSCReceiver::scoreplayerStatusHandler(handlerArgs) {
    if (xwConf->verbose) { print("scoreplayerStatus", argNames); }

    // start or stop scoreplayer
    if (argv[0]->i == 1) {
        oscReceiver->emitScoreplayerStarted();
    } else {
        oscReceiver->emitScoreplayerStopped();
    }

    // set global recordmode of scoreplayer
    oscReceiver->emitScoreplayerRecordModeChanged(argv[1]->i);

    // set global readmode of scoreplayer
    oscReceiver->emitScoreplayerReadModeChanged(argv[2]->i);

    // set record and read mode of each individual source. Offset is determinded by
    // structure of the message /WONDER/score/status. Recordmodes start at fourth value.
    int messageOffset = 3;
    int id            = 0;

    for (int k = messageOffset; k < argc; k += (messageOffset - 1)) {
        oscReceiver->emitSourceRecordModeChanged(id, (bool)argv[k]->i);
        oscReceiver->emitSourceReadModeChanged(id, (bool)argv[k + 1]->i);
        ++id;
    }

    return 0;
}

//--------------------end-of-scoreplayer-related-handler-----------------------//

//---------------------------------misc-handler--------------------------------//

int OSCReceiver::maxNoSourcesHandler(handlerArgs) {
    if (xwConf->verbose) { print("maxNoSources", argNames); }

    // this handler is only allowed to be called once with a valid
    // value at startup, so ignore further messages
    static bool maxNoSourcesIsSet = false;

    if (!maxNoSourcesIsSet) {
        if (argv[0]->i > 0) {
            xwConf->maxNoSources = argv[0]->i;
            maxNoSourcesIsSet    = true;
        }
    }

    oscReceiver->emitMaxNoSourcesWasSet();

    return 0;
}

int OSCReceiver::renderPolygonHandler(handlerArgs) {
    if (xwConf->verbose) { print("renderPolygon", argNames); }

    // get name of room
    xwConf->roomName = &argv[0]->s;
    // get number of points
    int noPoints = argv[1]->i;

    // iterate over all points
    // but we only need x and y, so z can be ignored
    for (int i = 1; i <= noPoints; ++i) {
        QPointF p(argv[(i * 3) - 1]->f, argv[i * 3]->f);
        xwConf->roomPoints.append(p);
    }

    oscReceiver->emitRenderPolygonReceived();
    return 0;
}

int OSCReceiver::projectXMLDumpHandler(handlerArgs) {
    if (xwConf->verbose) { print("xmlDump", argNames); }

    if (argv[0]->i != 1) { oscReceiver->emitProjectXMLDump(&argv[1]->s); }

    return 0;
}

int OSCReceiver::streamclientConnectedHandler(handlerArgs) {
    if (xwConf->verbose) { print("streamclientConnected", argNames); }

    oscReceiver->emitStreamclientConnected(&argv[0]->s, &argv[1]->s, &argv[2]->s);

    return 0;
}

int OSCReceiver::streamclientDisconnectedHandler(handlerArgs) {
    if (xwConf->verbose) { print("streamclientDisconnected", argNames); }

    oscReceiver->emitStreamclientDisconnected(&argv[0]->s, &argv[1]->s, &argv[2]->s);

    return 0;
}

int OSCReceiver::visualpingHandler(handlerArgs) {
    // get sender of ping and reply with pong
    lo_address from = lo_message_get_source(msg);
    lo_send(from, "/WONDER/stream/visual/pong", "i", argv[0]->i);

    // reset the timer
    oscReceiver->emitStopCwonderTimer();
    oscReceiver->emitStartCwonderTimer();

    return 0;
}

int OSCReceiver::genericHandler(handlerArgs) {
    cout << endl
         << "[XWonder::OSCReceiver::genericHandler] No handler implemented for this "
            "message: "
         << ((OSCReceiver*)user_data)->getContent(path, types, argv, argc) << endl;
    return 0;
}

int OSCReceiver::replyHandler(handlerArgs) {
    if (xwConf->verbose) {
        cout << "[XWonder::OSCReceiver::replyHandler] received reply to: " << &argv[0]->s
             << " state=" << argv[1]->i << " msg=" << &argv[2]->s << endl
             << endl;
    }

    // if xwonder is in startup, then a reply signals that the connection to cwonder has
    // been established
    if (oscReceiver->startup) {
        // leave startup mode and restart timer to check for regular pings from cwonder
        oscReceiver->startup = false;
        oscReceiver->emitStopCwonderTimer();
        oscReceiver->emitStartCwonderTimer();
    }

    // check if an error occurred
    if (argc >= 3)
        if (argv[1]->i != 0) { oscReceiver->emitOSCErrorMessage(&argv[2]->s); }

    return 0;
}

//------------------------------end-of-misc-handler----------------------------//

void OSCReceiver::print(string handlerName, handlerArgs) {
    cerr << endl
         << "[XWonder::OSCReceiver::" << handlerName << "Handler]:" << endl
         << ((OSCReceiver*)user_data)->getContent(path, types, argv, argc) << endl;
}

OSCReceiver* oscReceiver = NULL;
