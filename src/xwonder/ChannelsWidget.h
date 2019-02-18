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

#ifndef CHANNELSWIDGET_H
#define CHANNELSWIDGET_H

//----------------------------------includes----------------------------------//

#include <QWidget>
#include <QList>
#include <QMap>

#include "Colors.h"

class QVBoxLayout;
class QContextMenuEvent;

class Source;
class SourceWidget;

//----------------------------------------------------------------------------//

//Doxygen description
/*!
 *      \brief
 *      Containerwidget for SourceWidgets
 *
 *      \details
 *      This is a containerwidget for the SourceWidgets. It dispatches the messages to and from the SourceWidgets.
 *      Furthermore it prevents the assignment of sourceIDentical ID-numbers to different SourceWidgets.
 *
 *      \author
 *      Hans-Joachim Mond
 *
 *      \date
 *      20.07.2008
 *
 *      \bug
 *      Flicker when a sourcewidget is expandend, added, or deleted. May be solved with double buffering.
 */

class ChannelsWidget : public QWidget {
    Q_OBJECT
public:
    ChannelsWidget(QWidget* parent = 0);

    void reset();

    void  setMode(bool projectOnlyMode);
    QSize sizeHint();
    void  setChannel(Source& source);
    int   getNoSources() const;
    int   sourceIDIsActive(int sourceID);
    void  resetIDAfterIDChange(unsigned int xID, int oldID);
    void  swapAfterIDChange(unsigned int xID, int oldID, int newID);

public slots:
    void init();
    int  activateChannel(int sourceID = -1, bool external = false);
    void activateChannels();
    void deactivateChannel(int sourceID);
    void deactivateChannel(unsigned int xID);
    void deactivateAllChannels();
    void changeType(int sourceID, bool planewave);
    void changeType(unsigned int xID, bool planewave);
    void changeName(int sourceID, QString name);
    void changeSourceRecordMode(unsigned int xID, bool recordEnabled);
    void changeSourceRecordMode(int sourceID, bool recordEnabled, bool externalSet);
    void changeSourceReadMode(unsigned int xID, bool readEnabled);
    void changeSourceReadMode(int sourceID, bool readEnabled, bool externalSet);
    void changeColor(unsigned int xID, const QColor& newColor);
    void changeColor(int sourceID, QColor newColor);
    void changeDopplerEffect(unsigned int xID, bool dopplerOn);
    void changeDopplerEffect(int sourceID, bool dopplerOn);
    void changeRotationDirection(unsigned int xID, bool inverted);
    void changeRotationDirection(int sourceID, bool inverted);
    void changeScalingDirection(unsigned int xID, bool inverted);
    void changeScalingDirection(int sourceID, bool inverted);

    void updateWrapped(int notUsed);

signals:
    void sourceActivated(int sourceID);
    void sourceDeactivated(int sourceID);
    void colorChanged(unsigned int xID, const QColor& newColor);
    void visibleChanged(unsigned int xID);
    void typeChanged(unsigned int xID, bool planewave);
    void nameChanged(unsigned int xID, const QString& name);
    void sourceIDChanged(unsigned int xID, int oldID, int newID);
    void dopplerEffectChanged(unsigned int xID, bool dopplerOn);
    void rotationDirectionChanged(unsigned int xID, bool inverted);
    void scalingDirectionChanged(unsigned int xID, bool inverted);
    void sourceRecordModeChanged(unsigned int xID, bool recEnabled);
    void sourceReadModeChanged(unsigned int xID, bool readEnabled);
    void sourceSelected(unsigned int xID);

    // for internal use, i.e. no OSC communication is triggered by these
    void channelActivated(unsigned int xID, int sourceID, bool external = false);
    void channelDeactivated(unsigned int xID, bool external = false);
    void modified();

    // to dispatch use of source related keys
    void viewRelatedKeyPressed(QKeyEvent* event);

protected:
    void contextMenuEvent(QContextMenuEvent* event);
    void keyPressEvent(QKeyEvent* event);

private slots:
    //void changeID( unsigned int xID, int oldID, int sourceID );
    void modified(unsigned int, int, int);
    void modified(unsigned int, bool);
    void modified(unsigned int, const QColor&);
    void modified(unsigned int, const QString&);

private:
    bool projectOnlyMode;
    QMap< unsigned int, SourceWidget* > sourceWidgets; // keys are xIDs
    QVBoxLayout* vLayout;
};
#endif //CHANNELSWIDGET_H
