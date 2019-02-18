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
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#ifndef OSCRECEIVER_H
#define OSCRECEIVER_H


//----------------------------------includes----------------------------------//

#include <string>

#include <QObject>
#include <QColor>
#include "oscin.h"

class QString;
class QTimer;

//----------------------------------------------------------------------------//


//----------------------------------defines-----------------------------------//

// arguments and names of arguments of the handler functions
#define handlerArgs const char* path, const char* types, lo_arg** argv, int argc, lo_message msg, void* user_data
#define argNames path, types, argv, argc, msg, user_data

//----------------------------------------------------------------------------//


//----------------------------------Doxygen-----------------------------------//
/*!
 *      \brief
 *      Dispatcher for incoming OSC messages
 *
 *      \details
 *      Every OSC message sent to Xwonder is received by this class and dispatched to the appropriate static
 *      method, if one is registered. This static method calls a non-static method which emits a Qt signal.
 *      This multiple method approach is necessary because only static methods can be registered as handlers
 *      in the class OSCServer but only actual objects can emit signals in Qt. OSCServer encapsulates the
 *      liblo which uses funtionpointers to register callbacks, hence the need for static methods.
 *      OSCReceiver regularly checks the connection to cwonder and notifies the user if the connection
 *      is lost and tells Xwonder to exit.
 *
 *      \author
 *      Hans-Joachim Mond
 *
 *      \date
 *      06.08.2008
 */
//----------------------------------------------------------------------------//


class OSCReceiver : public QObject, OSCServer {
    Q_OBJECT

public:
    //constructors
    OSCReceiver(const char* port, QObject* parent = 0);

    // emitter
    void emitSourceActivated(int sourceID) const;
    void emitSourceDeactivated(int sourceID) const;
    void emitSourcePositionChanged(int sourceID, float x, float y) const;
    void emitSourceOrientationChanged(int sourceID, float orientation) const;
    void emitSourceTypeChanged(int sourceID, int type) const;
    void emitSourceNameChanged(int sourceID, const char* name) const;
    void emitSourceColorChanged(int sourceID, int r, int g, int b) const;
    void emitSourceGroupIDChanged(int sourceID, int groupID) const;
    void emitSourceRotationDirectionChanged(int sourceID, bool invert) const;
    void emitSourceScalingDirectionChanged(int sourceID, bool invert) const;
    void emitSourceDopplerEffectChanged(int sourceID, bool dopplerOn) const;
    void emitSourceRecordModeChanged(int sourceID, bool recordEnabled) const;
    void emitSourceReadModeChanged(int sourceID, bool readEnabled) const;

    void emitGroupActivated(int groupID) const;
    void emitGroupDeactivated(int groupID) const;
    void emitGroupPositionChanged(int groupID, float x, float y) const;
    void emitGroupColorChanged(int groupID, int r, int g, int b) const;

    void emitTimeChanged(int h, int m, int s, int ms) const;
    void emitScoreplayerStopped() const;
    void emitScoreplayerStarted() const;
    void emitScoreplayerRecordModeChanged(int recordEnabled) const;
    void emitScoreplayerReadModeChanged(int readEnabled) const;
    void emitMmcChanged(bool mmcEnabled) const;
    void emitMsrcChanged(bool msrcEnabled) const;

    void emitProjectXMLDump(const char* xmlProject) const;
    void emitOSCErrorMessage(QString errorMsg) const;
    void emitStartCwonderTimer() const;
    void emitStopCwonderTimer() const;
    void emitRenderPolygonReceived() const;
    void emitMaxNoSourcesWasSet() const;

    void emitStreamclientConnected(QString host, QString port, QString name) const;
    void emitStreamclientDisconnected(QString host, QString port, QString name) const;

    // try to establish connection to cwonder, starts "ping/pong" mechanism
    void connectToCwonder() const;

signals:
    void sourceActivated(int sourceID) const;
    void sourceDeactivated(int sourceID) const;
    void sourcePositionChanged(int sourceID, float x, float y) const;
    void sourceOrientationChanged(int sourceID, float orientation) const;
    void sourceTypeChanged(int sourceID, bool type) const;
    void sourceNameChanged(int sourceID, QString name) const;
    void sourceColorChanged(int sourceID, QColor color) const;
    void sourceGroupIDChanged(int sourceID, int groupID) const;
    void sourceRotationDirectionChanged(int sourceID, bool invert) const;
    void sourceScalingDirectionChanged(int sourceID, bool invert) const;
    void sourceDopplerEffectChanged(int sourceID, bool dopplerOn) const;
    void sourceRecordModeChanged(int sourceID, bool recordEnabled, bool externalSet) const;
    void sourceReadModeChanged(int sourceID, bool readEnabled, bool externalSet) const;

    void groupActivated(int groupID, bool externalSet) const;
    void groupDeactivated(int groupID, bool externalSet) const;
    void groupPositionChanged(int groupID, float x, float y) const;
    void groupColorChanged(int groupID, const QColor& color, bool externalSet) const;

    void timeChanged(int h, int m, int s, int ms) const;
    void scoreplayerStopped() const;
    void scoreplayerStarted() const;
    void scoreplayerRecordModeChanged(bool recordEnabled, bool externalSet) const;
    void scoreplayerReadModeChanged(bool readEnabled, bool externalSet) const;
    void mmcChanged(bool mmcEnabled, bool externalSet) const;
    void msrcChanged(bool msrcEnabled, bool externalSet) const;

    void projectXMLDump(QString xmlProject) const;
    void OSCErrorMessage(QString errorMsg) const;
    void startCwonderTimer() const;
    void stopCwonderTimer() const;
    void renderPolygonReceived() const;
    void maxNoSourcesWasSet() const;

    void streamclientConnected(QString host, QString port, QString name) const;
    void streamclientDisconnected(QString host, QString port, QString name) const;

private slots:
    void emitCwonderConnectionLost() const;

private:
    void addReply();
    void addMethods();

    // handler
    static int sourceActivateHandler(handlerArgs);
    static int sourceDeactivateHandler(handlerArgs);
    static int sourcePositionHandler(handlerArgs);
    static int sourceAngleHandler(handlerArgs);
    static int sourceTypeHandler(handlerArgs);
    static int sourceNameHandler(handlerArgs);
    static int sourceColorHandler(handlerArgs);
    static int sourceGroupIDHandler(handlerArgs);
    static int sourceRotationDirectionHandler(handlerArgs);
    static int sourceScalingDirectionHandler(handlerArgs);
    static int sourceDopplerEffectHandler(handlerArgs);
    static int sourceRecordModeHandler(handlerArgs);
    static int sourceReadModeHandler(handlerArgs);

    static int groupActivateHandler(handlerArgs);
    static int groupDeactivateHandler(handlerArgs);
    static int groupPositionHandler(handlerArgs);
    static int groupColorHandler(handlerArgs);

    static int mtcTimeHandler(handlerArgs);
    static int scoreplayerStoppedHandler(handlerArgs);
    static int scoreplayerStartedHandler(handlerArgs);
    static int scoreplayerRecordModeHandler(handlerArgs);
    static int scoreplayerReadModeHandler(handlerArgs);
    static int mmcChangedHandler(handlerArgs);
    static int msrcChangedHandler(handlerArgs);
    static int scoreplayerStatusHandler(handlerArgs);

    static int maxNoSourcesHandler(handlerArgs);
    static int renderPolygonHandler(handlerArgs);
    static int projectXMLDumpHandler(handlerArgs);

    static int streamclientConnectedHandler(handlerArgs);
    static int streamclientDisconnectedHandler(handlerArgs);

    static int visualpingHandler(handlerArgs);

    static int genericHandler(handlerArgs);
    static int replyHandler(handlerArgs);

    // for output to commandline
    static void print(std::string handlerName, handlerArgs);

    QTimer* cwonderTimer; // timer runs out when cwonder doesn't ping anymore
    bool    startup;      // flag to indicate if xwonder has just started, needed for the timer

}; //class OSCReceiver

extern OSCReceiver* oscReceiver;

#endif //OSCRECEIVER_H
