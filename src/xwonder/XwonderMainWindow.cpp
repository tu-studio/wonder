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


//----------------------------------includes----------------------------------//

#include "XwonderMainWindow.h"
#include "lo/lo.h"

#include <QApplication>
#include <QHBoxLayout>
#include <QScrollArea>
#include <QStatusBar>
#include <QToolButton>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QToolBar>
#include <QCloseEvent>
#include <QMessageBox>
#include <QIcon>
#include <QFile>
#include <QScrollBar>
#include <QFileDialog>
#include <QtXml>

#include "Sources3DWidget.h"
#include "ChannelsWidget.h"
#include "SnapshotSelector.h"
#include "StreamClientWidget.h"
#include "TimeLCDNumber.h"
#include "FilenameDialog.h"
#include "XwonderConfig.h"
#include "SourceCoordinates.h"
#include "OSCReceiver.h"
#include "Source.h"
#include "SourceGroup.h"

#include <QDebug>
//----------------------------------------------------------------------------//


//--------------------------------constructors--------------------------------//

XwonderMainWindow::XwonderMainWindow(QWidget* parent) : QMainWindow(parent) {
    // check if cwonder runs locally
    if(QString(lo_address_get_hostname(xwConf->cwonderAddr)) != "127.0.0.1") {
        localMode = false;
    } else {
        localMode = true;
    }

    // construct and display window title
    basicWindowTitle = "Xwonder --- GUI for WONDER";

    if(xwConf->runWithoutCwonder) {
        basicWindowTitle.prepend("DEMOMODE --- ");
    }

    setWindowTitle(basicWindowTitle);

    // initialise status
    wasModified          = false;
    myProjectLoadRequest = false;

    // initialise widgets
    centralWidget      = new QWidget();
    channelsView       = new ChannelsWidget();
    sources3DView      = new Sources3DWidget();
    filenameDialog     = new FilenameDialog(this);
    snapshotSelector   = new SnapshotSelector(this);
    streamClientWidget = new StreamClientWidget(this);
    timeLCD            = new TimeLCDNumber(this);
    //timeSelector     = new TimelineWidget(this);
    connect(filenameDialog, SIGNAL(accepted()), this, SLOT(setFilename()));

    // setup the scrollarea which will contain the ChannelsWidget. The Channelswidget should
    // extend to the edge of the window so that mouseactions are accepted there
    channelsScrollArea = new QScrollArea();
    channelsScrollArea->setFocusPolicy(Qt::NoFocus);
    channelsScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    channelsScrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    channelsScrollArea->setWidgetResizable(true);
    channelsScrollArea->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);

    channelsScrollArea->setWidget(channelsView);
    channelsScrollArea->setMinimumWidth(channelsView->sizeHint().width());

    QScrollBar* scrollbar = channelsScrollArea->verticalScrollBar();
    connect(scrollbar,    SIGNAL(valueChanged(int)),
            channelsView, SLOT(updateWrapped(int)));

    // initialise layout
    hLayout = new QHBoxLayout();
    hLayout->setMargin(3);
    hLayout->setSpacing(5);
    hLayout->addWidget(channelsScrollArea);
    hLayout->addWidget(sources3DView);
    centralWidget->setLayout(hLayout);
    setCentralWidget(centralWidget);

    // initialise actions, menus, tool bar and status bar
    createActions();
    createMenus();
    createToolBars();
    theStatusBar = new QStatusBar();
    setStatusBar(theStatusBar);

    // don't allow anyting just yet, we need a new or opened project first
    centralWidget->setEnabled(false);
    disableActions();

    // init sources3DWidget and ChannelsWidget if we now the maximum number of sources allowed
    connect(oscReceiver,   SIGNAL(maxNoSourcesWasSet()),
            channelsView,  SLOT(init()));
    connect(oscReceiver,   SIGNAL(maxNoSourcesWasSet()),
            sources3DView, SLOT(init()));

    //for demo mode only
    if(xwConf->runWithoutCwonder) {
        connect(oscReceiver, SIGNAL(maxNoSourcesWasSet()),
                this,        SLOT(setDemoMode()));
    }

    // connections for all events of the Source3DWidget which shall produce an OSC message
    connect(sources3DView, SIGNAL(sourcePositionChanged(const Source&)),
            this,          SLOT(sendOSCSourcePosition(const Source&)));
    connect(sources3DView, SIGNAL(sourceOrientationChanged(const Source&)),
            this,          SLOT(sendOSCSourceOrientation(const Source&)));
    connect(sources3DView, SIGNAL(sourceColorChanged(int, const QColor&)),
            this,          SLOT(sendOSCSourceColor(int, const QColor&)));
    connect(sources3DView, SIGNAL(sourceGroupIDChanged(int, int)),
            this,          SLOT(sendOSCSourceGroupID(int, int)));
    connect(sources3DView, SIGNAL(groupActivated(int)),
            this,          SLOT(sendOSCGroupActivate(int)));
    connect(sources3DView, SIGNAL(groupDeactivated(int)),
            this,          SLOT(sendOSCGroupDeactivate(int)));
    connect(sources3DView, SIGNAL(groupPositionChanged(const SourceGroup&)),
            this,          SLOT(sendOSCGroupPosition(const SourceGroup&)));
    connect(sources3DView, SIGNAL(groupColorChanged(int, const QColor&)),
            this,          SLOT(sendOSCGroupColor(int, const QColor&)));


    // snapshotSelector connections
    connect(snapshotSelector, SIGNAL(closedMyself()),
            this,             SLOT(snapshotSelectorClosedItself()));
    connect(snapshotSelector, SIGNAL(takeSnapshotSignal(unsigned int, QString)),
            this,             SLOT(sendOSCSnapshotTake(unsigned int, QString)));
    connect(snapshotSelector, SIGNAL(deleteSnapshotSignal(unsigned int)),
            this,             SLOT(sendOSCSnapshotDelete(unsigned int)));
    connect(snapshotSelector, SIGNAL(renameSnapshotSignal(unsigned int, QString)),
            this,             SLOT(sendOSCSnapshotRename(unsigned int, QString)));
    connect(snapshotSelector, SIGNAL(copySnapshotSignal(unsigned int, unsigned int)),
            this,             SLOT(sendOSCSnapshotCopy(unsigned int, unsigned int)));
    connect(snapshotSelector, SIGNAL(recallSnapshotSignal(unsigned int, double)),
            this,             SLOT(sendOSCSnapshotRecall(unsigned int, double)));

    // streamclientwidget connections
    connect(streamClientWidget, SIGNAL(closedMyself()),
            this,               SLOT(streamClientWidgetClosedItself()));
    connect(oscReceiver,        SIGNAL(streamclientConnected(QString, QString, QString)),
            streamClientWidget, SLOT(insert(QString, QString, QString)));
    connect(oscReceiver,        SIGNAL(streamclientDisconnected(QString, QString, QString)),
            streamClientWidget, SLOT(remove(QString, QString, QString)));

    //connect( timeSelector, SIGNAL( closedMyself() ),
    // this,         SLOT  ( timeSelectorClosedItself() ) );


    //Connections for the modified-flag to save unsaved changes
    connect(snapshotSelector, SIGNAL(modified()),
            this,             SLOT(setModified()));
    //connect( sources3DView, SIGNAL( modified() ),
    // this,          SLOT  ( setModified() ) );


    // sources3DWidget connections (more are made, when the ChannelsWidet is created)
    connect(sources3DView, SIGNAL(displaySourceCoordinates(const Source&)),
            this,          SLOT(updateStatusBar(const Source&)));
    connect(sources3DView, SIGNAL(displayGroupCoordinates(const SourceGroup&)),
            this,          SLOT(updateStatusBar(const SourceGroup&)));

    //connections to the oscReceiver
    connect(oscReceiver,   SIGNAL(sourceActivated(int)),
            this,          SLOT(activateSource(int)));
    connect(oscReceiver,   SIGNAL(sourceDeactivated(int)),
            this,          SLOT(deactivateSource(int)));
    connect(oscReceiver,   SIGNAL(sourcePositionChanged(int, float, float)),
            sources3DView, SLOT(changeSourcePosition(int, float, float)));
    connect(oscReceiver,   SIGNAL(sourceOrientationChanged(int, float)),
            sources3DView, SLOT(changeSourceOrientation(int, float)));
    connect(oscReceiver,   SIGNAL(sourceTypeChanged(int, bool)),
            sources3DView, SLOT(changeSourceType(int, bool)));
    connect(oscReceiver,   SIGNAL(sourceNameChanged(int, QString)),
            sources3DView, SLOT(changeSourceName(int, QString)));
    connect(oscReceiver,   SIGNAL(sourceColorChanged(int, QColor)),
            sources3DView, SLOT(changeSourceColor(int, QColor)));
    connect(oscReceiver,   SIGNAL(sourceGroupIDChanged(int, int)),
            sources3DView, SLOT(changeSourceGroupID(int, int)));
    connect(oscReceiver,   SIGNAL(sourceRotationDirectionChanged(int, bool)),
            sources3DView, SLOT(changeSourceRotationDirection(int, bool)));
    connect(oscReceiver,   SIGNAL(sourceScalingDirectionChanged(int, bool)),
            sources3DView, SLOT(changeSourceScalingDirection(int, bool)));
    connect(oscReceiver,   SIGNAL(sourceRecordModeChanged(int, bool, bool)),
            sources3DView, SLOT(changeSourceRecordMode(int, bool, bool)));
    connect(oscReceiver,   SIGNAL(sourceReadModeChanged(int, bool, bool)),
            sources3DView, SLOT(changeSourceReadMode(int, bool, bool)));
    connect(oscReceiver,   SIGNAL(groupActivated(int, bool)),
            sources3DView, SLOT(activateGroup(int, bool)));
    connect(oscReceiver,   SIGNAL(groupDeactivated(int, bool)),
            sources3DView, SLOT(deactivateGroup(int, bool)));
    connect(oscReceiver,   SIGNAL(groupPositionChanged(int, float, float)),
            sources3DView, SLOT(changeGroupPosition(int, float, float)));
    connect(oscReceiver,   SIGNAL(groupColorChanged(int, const QColor&, bool)),
            sources3DView, SLOT(changeGroupColor(int, const QColor&, bool)));
    connect(oscReceiver,   SIGNAL(renderPolygonReceived()),
            sources3DView, SLOT(updateRoompolygon()));

    connect(oscReceiver,   SIGNAL(timeChanged(int, int, int, int)),
            timeLCD,       SLOT(setTime(int, int, int, int)));
    connect(oscReceiver,   SIGNAL(projectXMLDump(QString)),
            this,          SLOT(fetchOSCProjectXMLDump(QString)));
    connect(oscReceiver,   SIGNAL(scoreplayerStopped()),
            this,          SLOT(scoreplayerStopped()));
    connect(oscReceiver,   SIGNAL(scoreplayerStarted()),
            this,          SLOT(scoreplayerStarted()));
    connect(oscReceiver,   SIGNAL(scoreplayerRecordModeChanged(bool, bool)),
            this,          SLOT(setRecordmodeInScoreplayer(bool, bool)));
    connect(oscReceiver,   SIGNAL(scoreplayerReadModeChanged(bool, bool)),
            this,          SLOT(setReadmodeInScoreplayer(bool, bool)));
    connect(oscReceiver,   SIGNAL(mmcChanged(bool, bool)),
            this,          SLOT(enableMMC(bool, bool)));
    connect(oscReceiver,   SIGNAL(msrcChanged(bool, bool)),
            this,          SLOT(enableMSRC(bool, bool)));
    connect(oscReceiver,   SIGNAL(OSCErrorMessage(QString)),
            this,          SLOT(fetchOSCErrorMessage(QString)));
    //connect( oscReceiver, SIGNAL( timeChanged( int, int, int, int ) ),
    // timeSelector, SLOT ( setTime    ( int, int, int, int ) ) );


    // Connections between ChannelsWidget and Source3DWidget (and XwonderMainwindow)
    connect(channelsView,  SIGNAL(channelActivated(unsigned int, int, bool)),
            sources3DView, SLOT(activateSource(unsigned int, int, bool)));
    connect(channelsView,  SIGNAL(sourceActivated(int)),
            this,          SLOT(sendOSCSourceActivate(int)));
    connect(channelsView,  SIGNAL(channelDeactivated(unsigned int)),
            sources3DView, SLOT(deactivateSource(unsigned int)));
    connect(channelsView,  SIGNAL(sourceDeactivated(int)),
            this,          SLOT(sendOSCSourceDeactivate(int)));
    connect(channelsView,  SIGNAL(colorChanged(unsigned int, const QColor&)),
            sources3DView, SLOT(changeSourceColor(unsigned int, const QColor&)));
    connect(channelsView,  SIGNAL(colorChanged(unsigned int, const QColor&)),
            this,          SLOT(sendOSCSourceColor(unsigned int, const QColor&)));
    connect(sources3DView, SIGNAL(sourceTypeChanged(unsigned int, bool)),
            channelsView,  SLOT(changeType(unsigned int, bool)));
    connect(sources3DView, SIGNAL(sourceColorChanged(unsigned int, QColor)),
            channelsView,  SLOT(changeColor(unsigned int, QColor)));
    connect(sources3DView, SIGNAL(sourceRecordModeChanged(unsigned int, bool)),
            channelsView,  SLOT(changeSourceRecordMode(unsigned int, bool)));
    connect(channelsView,  SIGNAL(visibleChanged(unsigned int)),
            sources3DView, SLOT(changeSourceVisible(unsigned int)));
    connect(channelsView,  SIGNAL(rotationDirectionChanged(unsigned int, bool)),
            sources3DView, SLOT(changeSourceRotationDirection(unsigned int, bool)));
    connect(channelsView,  SIGNAL(scalingDirectionChanged(unsigned int, bool)),
            sources3DView, SLOT(changeSourceScalingDirection(unsigned int, bool)));
    connect(channelsView,  SIGNAL(typeChanged(unsigned int, bool)),
            sources3DView, SLOT(changeSourceType(unsigned int, bool)));
    connect(channelsView,  SIGNAL(sourceIDChanged(unsigned int, int, int)),
            sources3DView, SLOT(changeSourceID(unsigned int, int, int)));
    connect(channelsView,  SIGNAL(sourceIDChanged(unsigned int, int, int)),
            this,          SLOT(sendOSCNotifyIDChange(unsigned int, int, int)));
    connect(channelsView,  SIGNAL(dopplerEffectChanged(unsigned int, bool)),
            this,          SLOT(sendOSCSourceDopplerEffetChanged(unsigned int, bool)));
    connect(channelsView,  SIGNAL(nameChanged(unsigned int, const QString&)),
            sources3DView, SLOT(changeSourceName(unsigned int, const QString&)));
    connect(channelsView,  SIGNAL(channelActivated(unsigned int, int, bool)),
            this,          SLOT(toggleAddChannelActs(unsigned int, int, bool)));
    connect(channelsView,  SIGNAL(channelDeactivated(unsigned int)),
            this,          SLOT(toggleAddChannelActs(unsigned int)));
    connect(channelsView,  SIGNAL(sourceSelected(unsigned int)),
            sources3DView, SLOT(selectSource(unsigned int)));
    connect(channelsView,  SIGNAL(sourceSelected(unsigned int)),
            this,          SLOT(shiftFocusToSources3DWidget(unsigned int)));
    connect(channelsView,  SIGNAL(sourceRecordModeChanged(unsigned int, bool)),
            sources3DView, SLOT(changeSourceRecordMode(unsigned int, bool)));
    connect(channelsView,  SIGNAL(sourceReadModeChanged(unsigned int, bool)),
            sources3DView, SLOT(changeSourceReadMode(unsigned int, bool)));
    connect(channelsView, SIGNAL(viewRelatedKeyPressed(QKeyEvent*)),
            this,         SLOT(dispatchViewRelatedKeys(QKeyEvent*)));

    // incoming OSC-Messages
    connect(oscReceiver,   SIGNAL(sourceTypeChanged(int, bool)),
            channelsView,  SLOT(changeType(int, bool)));
    connect(oscReceiver,   SIGNAL(sourceNameChanged(int, QString)),
            channelsView,  SLOT(changeName(int, QString)));
    connect(oscReceiver,   SIGNAL(sourceColorChanged(int, QColor)),
            channelsView,  SLOT(changeColor(int, QColor)));
    connect(oscReceiver,   SIGNAL(sourceRecordModeChanged(int, bool, bool)),
            channelsView,  SLOT(changeSourceRecordMode(int, bool, bool)));
    connect(oscReceiver,   SIGNAL(sourceReadModeChanged(int, bool, bool)),
            channelsView,  SLOT(changeSourceReadMode(int, bool, bool)));
    connect(oscReceiver,   SIGNAL(sourceRotationDirectionChanged(int, bool)),
            channelsView,  SLOT(changeRotationDirection(int, bool)));
    connect(oscReceiver,   SIGNAL(sourceScalingDirectionChanged(int, bool)),
            channelsView,  SLOT(changeScalingDirection(int, bool)));
    connect(oscReceiver,   SIGNAL(sourceDopplerEffectChanged(int, bool)),
            channelsView,  SLOT(changeDopplerEffect(int, bool)));

    // Connections for all events of ChannelsWidget which shall produce an OSC-Message
    connect(channelsView, SIGNAL(typeChanged(unsigned int, bool)),
            this,         SLOT(sendOSCSourceType(unsigned int, bool)));
    connect(sources3DView, SIGNAL(sourceTypeChanged(unsigned int, bool)),
            this,         SLOT(sendOSCSourceType(unsigned int, bool)));
    connect(channelsView, SIGNAL(nameChanged(unsigned int, const QString&)),
            this,         SLOT(sendOSCSourceName(unsigned int, const QString&)));
    connect(channelsView, SIGNAL(sourceRecordModeChanged(unsigned int, bool)),
            this,         SLOT(sendOSCSourceRecordMode(unsigned int, bool)));
    connect(channelsView, SIGNAL(sourceReadModeChanged(unsigned int, bool)),
            this,         SLOT(sendOSCSourceReadMode(unsigned int, bool)));
    connect(channelsView, SIGNAL(rotationDirectionChanged(unsigned int, bool)),
            this,         SLOT(sendOSCSourceRotationDirection(unsigned int, bool)));
    connect(channelsView, SIGNAL(scalingDirectionChanged(unsigned int, bool)),
            this,         SLOT(sendOSCSourceScalingDirection(unsigned int, bool)));

    // connections for the modified-flag to save unsaved changes
    connect(channelsView, SIGNAL(modified()), this, SLOT(setModified()));

    // menuActions
    connect(newChannelAct,  SIGNAL(triggered()),
            channelsView,   SLOT(activateChannel()));
    connect(newChannelsAct, SIGNAL(triggered()),
            channelsView,   SLOT(activateChannels()));

    // assume scoreplayer is stopped, real status will be requested when a project is
    // created or opened.
    scoreplayerIsPlaying = false;
}

//----------------------------end of constructors-----------------------------//


QSize XwonderMainWindow::minimumSizeHint() const {
    return QSize(512, 384);
}

//-------------------------------event handlers-------------------------------//

void XwonderMainWindow::closeEvent(QCloseEvent* event) {
    if(fetchUnsavedChanges()) {
        snapshotSelector->close();
        streamClientWidget->close();
        //timeSelector->close();
        event->accept();
    } else {
        event->ignore();
    }
}


void XwonderMainWindow::keyPressEvent(QKeyEvent* event) {
    // if no project is open, then don't react
    if(projectname.isEmpty()) {
        return;
    }

    switch(event->key()) {
        case Qt::Key_Space:
            if(! projectOnlyMode) {
                if(scoreplayerIsPlaying) {
                    stopScoreplayer();
                } else {
                    startScoreplayer();
                }
            }

            break;

        case Qt::Key_W: // toogle type of source
        case Qt::Key_R: // toogle recordMode of current source or group
            if(! projectOnlyMode) {
                QApplication::sendEvent(sources3DView, event);
            }

            break;

        case Qt::Key_Delete:
        case Qt::Key_Backspace:
            channelsView->deactivateChannel(sources3DView->getCurrentXID());
            break;

        // movement commands
        case Qt::Key_X:
        case Qt::Key_Y:
        case Qt::Key_Z:
        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_Left:
        case Qt::Key_Right:
            QApplication::sendEvent(sources3DView, event);
            break;

        case Qt::Key_T:
            takeSnapshotAct->trigger();
            //TODO: maybe extend handled keyboard shortcuts, (L,K,A...)
            break;
    }
}

//---------------------------end of event handlers----------------------------//

void XwonderMainWindow::sendOSCSourceActivate(int sourceID) {
    if(! xwConf->runWithoutCwonder) {
        lo_send(xwConf->cwonderAddr, "/WONDER/source/activate", "i", sourceID);
    }
}


void XwonderMainWindow::sendOSCSourceDeactivate(int sourceID) {
    if(! xwConf->runWithoutCwonder) {
        lo_send(xwConf->cwonderAddr, "/WONDER/source/deactivate", "i", sourceID);
    }
}


void XwonderMainWindow::sendOSCSourcePosition(const Source& source) {
    SourceCoordinates coord = source.getCoordinatesRounded().mapGLCoordToWonderCoord();

    if(! xwConf->runWithoutCwonder)
        lo_send(xwConf->cwonderAddr, "/WONDER/source/position", "iff",
                source.getID(), coord.x, coord.y);

    if(! wasModified) {
        setModified();
    }
}


void XwonderMainWindow::sendOSCSourceOrientation(const Source& source) {
    SourceCoordinates coord = source.getCoordinatesRounded().mapGLCoordToWonderCoord();

    if(! xwConf->runWithoutCwonder)
        lo_send(xwConf->cwonderAddr, "/WONDER/source/angle", "if",
                source.getID(), coord.orientation);

    if(! wasModified) {
        setModified();
    }
}


void XwonderMainWindow::sendOSCSourceGroupID(int sourceID, int groupID) {
    if(! xwConf->runWithoutCwonder)
        lo_send(xwConf->cwonderAddr, "/WONDER/source/groupID", "ii",
                sourceID, groupID);

    if(! wasModified) {
        setModified();
    }
}

void XwonderMainWindow::sendOSCSourceColor(int sourceID, const QColor& newColor) {
    if(! xwConf->runWithoutCwonder)
        lo_send(xwConf->cwonderAddr, "/WONDER/source/color", "iiii",
                sourceID, newColor.red(), newColor.green(), newColor.blue());
}


void XwonderMainWindow::sendOSCSourceColor(unsigned int xID, const QColor& color) {
    const Source& source = sources3DView->getSource(xID);

    if(! xwConf->runWithoutCwonder)
        lo_send(xwConf->cwonderAddr, "/WONDER/source/color", "iiii",
                source.getID(), color.red(), color.green(), color.blue());
}


void XwonderMainWindow::sendOSCSourceType(unsigned int xID, bool planewave) {
    const Source& source     =  sources3DView->getSource(xID);

    //type point = 1, type planewave = 0
    int type = (int) ! planewave;

    if(! xwConf->runWithoutCwonder) {
        lo_send(xwConf->cwonderAddr, "/WONDER/source/type", "ii", source.getID(), type);
    }

    if(! wasModified) {
        setModified();
    }
}


void XwonderMainWindow::sendOSCSourceName(unsigned int xID, const QString& name) {
    if(! sources3DView->xIDIsUsed(xID)) {
        return;
    }

    const Source& source = sources3DView->getSource(xID);

    if(! xwConf->runWithoutCwonder)
        lo_send(xwConf->cwonderAddr, "/WONDER/source/name", "is",
                source.getID(),  name.toStdString().c_str()) ;

    if(! wasModified) {
        setModified();
    }
}


void XwonderMainWindow::sendOSCSourceRecordMode(unsigned int xID, bool recEnabled) {
    const Source& source = sources3DView->getSource(xID);

    int recordOn = (int) recEnabled;

    if(! projectOnlyMode) {
        if(! xwConf->runWithoutCwonder) {
            lo_send(xwConf->cwonderAddr, "/WONDER/score/source/enableRecord", "ii", source.getID(), recordOn);
        }
    }

    if(! wasModified) {
        setModified();
    }
}

void XwonderMainWindow::sendOSCSourceReadMode(unsigned int xID, bool readEnabled) {
    const Source& source = sources3DView->getSource(xID);

    int readOn = (int) readEnabled;

    if(! projectOnlyMode) {
        if(! xwConf->runWithoutCwonder) {
            lo_send(xwConf->cwonderAddr, "/WONDER/score/source/enableRead", "ii", source.getID(), readOn);
        }
    }

    if(! wasModified) {
        setModified();
    }
}


void XwonderMainWindow::sendOSCSourceRotationDirection(unsigned int xID, bool inverted) {
    int invertOn         = (int) inverted;
    const Source& source = sources3DView->getSource(xID);

    if(! xwConf->runWithoutCwonder) {
        lo_send(xwConf->cwonderAddr, "/WONDER/source/rotationDirection", "ii", source.getID(), invertOn);
    }

    if(! wasModified) {
        setModified();
    }
}


void XwonderMainWindow::sendOSCSourceScalingDirection(unsigned int xID, bool inverted) {
    int invertOn         = (int) inverted;
    const Source& source = sources3DView->getSource(xID);

    if(! xwConf->runWithoutCwonder) {
        lo_send(xwConf->cwonderAddr, "/WONDER/source/scalingDirection", "ii", source.getID(), invertOn);
    }

    if(! wasModified) {
        setModified();
    }
}

void XwonderMainWindow::sendOSCSourceDopplerEffetChanged(unsigned int xID, bool dopplerOn) {

    int useDoppler = (int) dopplerOn;
    const Source& source = sources3DView->getSource(xID);

    if(! xwConf->runWithoutCwonder) {
        lo_send(xwConf->cwonderAddr, "/WONDER/source/dopplerEffect", "ii", source.getID(), useDoppler);
    }

    if(! wasModified) {
        setModified();
    }
}


void XwonderMainWindow::sendOSCGroupActivate(int sourceID) {
    if(! xwConf->runWithoutCwonder) {
        lo_send(xwConf->cwonderAddr, "/WONDER/group/activate", "i", sourceID);
    }

    if(! wasModified) {
        setModified();
    }
}


void XwonderMainWindow::sendOSCGroupDeactivate(int sourceID) {
    if(! xwConf->runWithoutCwonder) {
        lo_send(xwConf->cwonderAddr, "/WONDER/group/deactivate", "i", sourceID);
    }

    if(! wasModified) {
        setModified();
    }
}



void XwonderMainWindow::sendOSCGroupColor(int groupID, const QColor& newColor) {
    if(! xwConf->runWithoutCwonder)
        lo_send(xwConf->cwonderAddr, "/WONDER/group/color", "iiii",
                groupID, newColor.red(), newColor.green(), newColor.blue());

    if(! wasModified) {
        setModified();
    }
}


void XwonderMainWindow::sendOSCGroupPosition(const SourceGroup& group) {
    SourceCoordinates coord = group.getCoordinatesRounded().mapGLCoordToWonderCoord();

    if(! xwConf->runWithoutCwonder)
        lo_send(xwConf->cwonderAddr, "/WONDER/group/position", "iff",
                group.getGroupID(), coord.x, coord.y);

    if(! wasModified) {
        setModified();
    }
}


void XwonderMainWindow::sendOSCNotifyIDChange(unsigned int xID, int oldID, int newID) {
    const Source& source     = sources3DView->getSource(xID);

    bool swap = false;

    // notify user if sources share sourceID, this may result in inconsistencies
    // between cwonder and xwonder and will be prevented.
    // ask user if this change should be undone or if the IDs should be swapped
    if(channelsView->sourceIDIsActive(source.getID()) > 1) {
        int answer = QMessageBox::warning(this, "XWonder - Source ID", "You have"
                                          " assigned the same ID to two sources!\nTo prevent unexpected"
                                          " results, you may now either:\n\n-Reset the changed ID\n or\n"
                                          "-Apply your change (this will swap the two IDs)\n\n"
                                          "What do you want to do?",
                                          QMessageBox::Apply, QMessageBox::Reset);

        if(answer == QMessageBox::Reset) {
            // do nothing an get old status back, also in Sources3DView
            channelsView-> resetIDAfterIDChange(xID, oldID);
            sources3DView->resetIDAfterIDChange(xID, oldID);
            return;
        } else { // answer == QMessageBox::Apply (Swap)
            channelsView-> swapAfterIDChange(xID, oldID, newID);
            sources3DView->swapAfterIDChange(xID, oldID, newID);
            swap = true;
        }
    } else {
        // deactivate source with the old sourceID if it is not used by anyone else
        if(! xwConf->runWithoutCwonder) {
            if(channelsView->sourceIDIsActive(oldID) == 0) {
                lo_send(xwConf->cwonderAddr, "/WONDER/source/deactivate", "i", oldID);
            }
        }
    }

    // send name, position, angle and type for xID, which has changed sourceID from oldID to newID
    if(! xwConf->runWithoutCwonder) {
        SourceCoordinates coords = source.getCoordinatesRounded().mapGLCoordToWonderCoord();
        int sourceID                   = source.getID();
        int type                 = (int) ! source.isPlanewave();

        lo_send(xwConf->cwonderAddr, "/WONDER/source/activate", "i",   newID);
        lo_send(xwConf->cwonderAddr, "/WONDER/source/name",     "is",  sourceID, (source.getName().toStdString().c_str()));
        lo_send(xwConf->cwonderAddr, "/WONDER/source/type",     "ii",  sourceID, type);
        lo_send(xwConf->cwonderAddr, "/WONDER/source/position", "iff", sourceID, coords.x, coords.y);
        lo_send(xwConf->cwonderAddr, "/WONDER/source/angle",    "if",  sourceID, coords.orientation);

        lo_send(xwConf->cwonderAddr, "/WONDER/source/groupID",            "ii", sourceID, source.getGroupID());
        lo_send(xwConf->cwonderAddr, "/WONDER/source/rotationDirection",  "ii", sourceID, (int) source.hasInvertedRotation());
        lo_send(xwConf->cwonderAddr, "/WONDER/source/scalingDirection",   "ii", sourceID, (int) source.hasInvertedScaling());
        lo_send(xwConf->cwonderAddr, "/WONDER/source/dopplerEffect",      "ii", sourceID, (int) source.hasDopplerEffect());
        lo_send(xwConf->cwonderAddr, "/WONDER/score/source/enableRecord", "ii", sourceID, (int) source.isRecordEnabled());
        lo_send(xwConf->cwonderAddr, "/WONDER/score/source/enableRead",   "ii", sourceID, (int) source.isReadEnabled());

        const GLfloat* color = source.getColor();
        lo_send(xwConf->cwonderAddr, "/WONDER/source/color", "iiii",
                sourceID, (int)(color[ 0 ] * 255), (int)(color[ 1 ] * 255), (int)(color[ 2 ] * 255));
    }

    // if it was a sourceID swap then send data for source which was changed from newID to oldID
    if(swap) {
        if(! xwConf->runWithoutCwonder) {
            // get source with newID
            Source swapped           = sources3DView->getSource(oldID);
            SourceCoordinates coords = swapped.getCoordinatesRounded().mapGLCoordToWonderCoord();
            int sourceID                   = oldID;
            int type                 = (int) ! swapped.isPlanewave();

            lo_send(xwConf->cwonderAddr, "/WONDER/source/name", "is",
                    sourceID, (swapped.getName().toStdString().c_str()));
            lo_send(xwConf->cwonderAddr, "/WONDER/source/type", "ii",
                    sourceID, type);
            lo_send(xwConf->cwonderAddr, "/WONDER/source/position", "iff",
                    sourceID, coords.x, coords.y);
            lo_send(xwConf->cwonderAddr, "/WONDER/source/angle", "if",
                    sourceID, coords.orientation);


            lo_send(xwConf->cwonderAddr, "/WONDER/source/groupID", "ii",
                    sourceID, swapped.getGroupID());
            lo_send(xwConf->cwonderAddr, "/WONDER/source/rotationDirection", "ii",
                    sourceID, (int) swapped.hasInvertedRotation());
            lo_send(xwConf->cwonderAddr, "/WONDER/source/scalingDirection", "ii",
                    sourceID, (int) swapped.hasInvertedScaling());
            lo_send(xwConf->cwonderAddr, "/WONDER/source/dopplerEffect", "ii",
                    sourceID, (int) swapped.hasDopplerEffect());
            lo_send(xwConf->cwonderAddr, "/WONDER/score/source/enableRecord", "ii",
                    sourceID, (int) swapped.isRecordEnabled());
            lo_send(xwConf->cwonderAddr, "/WONDER/score/source/enableRead", "ii",
                    sourceID, (int) swapped.isReadEnabled());

            const GLfloat*  color = swapped.getColor();
            lo_send(xwConf->cwonderAddr, "/WONDER/source/color", "iiii",
                    sourceID, (int)(color[ 0 ] * 255), (int)(color[ 1 ] * 255), (int)(color[ 2 ] * 255));
        }
    }

    if(! wasModified) {
        setModified();
    }
}


void XwonderMainWindow::setFilename() {
    projectname = filenameDialog->getFilename();
}


void XwonderMainWindow::toggleAddChannelActs(unsigned int notUsed1, int notUsed2, bool notUsed3) {
    int currNoSources = channelsView->getNoSources();

    if(currNoSources < xwConf->maxNoSources) {
        newChannelAct ->setEnabled(true);
        newChannelsAct->setEnabled(true);
    } else {
        newChannelAct ->setEnabled(false);
        newChannelsAct->setEnabled(false);
    }
}


void XwonderMainWindow::shiftFocusToSources3DWidget(unsigned int notUsed) {
    sources3DView->setFocus(Qt::OtherFocusReason);
}


void XwonderMainWindow::setNoProjectMode() {
    disableActions();
    channelMenuAct           ->setVisible(false);
    viewMenuAct              ->setVisible(false);
    snapshotSelectorMenuAct  ->setVisible(false);
    scoreplayerMenuAct       ->setVisible(false);
    scoreplayerToolBar       ->setVisible(false);

    centralWidget->setEnabled(false);
}

void XwonderMainWindow::setProjectWithoutScoreMode() {
    setMode(true);
    //showTimesliceConnectionsAct->setVisible(false);
    //showTimeTicsAct->setShortcut(Qt::Key_unknown);
    //showTimeTicsAct->setChecked(false);
    //showTimeTicsAct->setVisible(false);
    channelMenuAct           ->setVisible(true);
    viewMenuAct              ->setVisible(true);
    snapshotSelectorMenuAct  ->setVisible(true);
    scoreplayerMenuAct       ->setVisible(false);
    scoreplayerToolBar       ->setVisible(false);

    //scoreDuration = 0;
    //timegridDistance = 0;
    //sources3DView->setNoTimesclices(1,timegridDistance);
    centralWidget->setEnabled(true);
    enableActions();
}


void XwonderMainWindow::setProjectWithScoreMode() {
    setMode(false);
    //showTimesliceConnectionsAct->setVisible(true);
    //showTimeTicsAct->setShortcut(Qt::Key_T);
    //showTimeTicsAct->setChecked(true);
    //showTimeTicsAct->setVisible(true);
    channelMenuAct           ->setVisible(true);
    viewMenuAct              ->setVisible(true);
    snapshotSelectorMenuAct  ->setVisible(true);
    scoreplayerMenuAct       ->setVisible(true);
    scoreplayerToolBar       ->setVisible(true);

    //scoreDuration = 10000;
    //timegridDistance = 1000;
    //scoreDuration = 0;
    //timegridDistance = 0;
    //sources3DView->setNoTimesclices(1,timegridDistance);
    //sources3DView->setNoTimesclices(scoreDuration/timegridDistance,timegridDistance);
    //sources3DView->setNoTimesclices(8,timegridDistance);
    centralWidget->setEnabled(true);
    enableActions();

    // request current status from scoreplayer to synchronize xwonder's appearance
    if(! xwConf->runWithoutCwonder) {
        lo_send(xwConf->cwonderAddr, "/WONDER/stream/score/send", "s", "/WONDER/score/status");
    }
}

void XwonderMainWindow::setDemoMode() {
    if(xwConf->runWithoutCwonder) {
        this  ->projectOnlyMode = false ;
        xwConf->projectOnlyMode = projectOnlyMode;
        projectname             = "DemoProject";
        channelsView->setMode(projectOnlyMode);
        enableActions();

        channelMenuAct           ->setVisible(true);
        viewMenuAct              ->setVisible(true);
        snapshotSelectorMenuAct  ->setVisible(true);
        scoreplayerMenuAct       ->setVisible(true);
        scoreplayerToolBar       ->setVisible(true);

        newMenu                  ->setEnabled(false);
        newProjectWithScoreAct   ->setEnabled(false);
        newProjectWithoutScoreAct->setEnabled(false);
        openProjectAct           ->setEnabled(false);
        saveAct                  ->setEnabled(false);
        saveAsAct                ->setEnabled(false);

        centralWidget->setEnabled(true);
    }
}


void XwonderMainWindow::dispatchViewRelatedKeys(QKeyEvent* event) {
    QApplication::sendEvent(sources3DView, event);
}

void XwonderMainWindow::setMode(bool projectOnlyMode) {
    this->projectOnlyMode   = projectOnlyMode ;
    xwConf->projectOnlyMode = projectOnlyMode;

    // deactivate all sources
    channelsView->deactivateAllChannels();

    channelsView->setMode(projectOnlyMode);
}

void XwonderMainWindow::createActions() {
    newProjectWithoutScoreAct = new QAction("New Pr&oject", this);
    newProjectWithScoreAct    = new QAction("New Project with &Score", this);
    openProjectAct            = new QAction("Open Pr&oject", this);
    saveAct                   = new QAction("&Save", this);
    saveAsAct                 = new QAction("Save &As...", this);
    exitAct                   = new QAction("E&xit", this);

    newProjectWithoutScoreAct->setShortcut(QString("Ctrl+N"));
    newProjectWithoutScoreAct->setShortcutContext(Qt::ApplicationShortcut);
    newProjectWithScoreAct   ->setShortcut(QString("Ctrl+Shift+N"));
    newProjectWithScoreAct   ->setShortcutContext(Qt::ApplicationShortcut);
    openProjectAct           ->setShortcut(QString("Ctrl+O"));
    openProjectAct           ->setShortcutContext(Qt::ApplicationShortcut);
    saveAct                  ->setShortcut(QString("Ctrl+S"));
    saveAct                  ->setShortcutContext(Qt::ApplicationShortcut);
    saveAsAct                ->setShortcut(QString("Ctrl+Shift+S"));
    saveAsAct                ->setShortcutContext(Qt::ApplicationShortcut);

    saveAct  ->setEnabled(false);
    saveAsAct->setEnabled(false);

    resetViewAct = new QAction("&Reset View", this);
    resetViewAct->setShortcut(Qt::Key_K);

    lockViewAct  = new QAction("&Lock View", this);
    lockViewAct->setShortcut(Qt::Key_L);
    lockViewAct->setCheckable(true);

    lockClickAct  = new QAction("&Stickyclick", this);
    lockClickAct->setShortcut(Qt::Key_S);
    lockClickAct->setCheckable(true);

    showPerspectiveViewAct = new QAction("&Perspective View", this);
    showPerspectiveViewAct->setShortcut(Qt::Key_P);
    showPerspectiveViewAct->setCheckable(true);

    //showLightingAct = new QAction("Show &Lighting",this);
    //showLightingAct->setShortcut(Qt::Key_L);
    //showLightingAct->setCheckable(true);

    //showTimesliceConnectionsAct = new QAction("Show &Connections",this);
    //showTimesliceConnectionsAct->setShortcut(Qt::Key_C);
    //showTimesliceConnectionsAct->setCheckable(true);

    //showPointsourcesAsSpheresAct = new QAction("Show Pointsources as &Spheres",this);
    //showPointsourcesAsSpheresAct->setCheckable(true);
    //showPointsourcesAsSpheresAct->setShortcut(Qt::Key_S);

    showCoordOriginAct = new QAction("Show &origin", this);
    showCoordOriginAct->setShortcut(Qt::Key_O);
    showCoordOriginAct->setCheckable(true);

    //showTimeTicsAct = new QAction("Show &Timetics",this);
    //showTimeTicsAct->setShortcut(Qt::Key_T);
    //showTimeTicsAct->setCheckable(true);

    showNamesAct = new QAction("Show Source &names", this);
    showNamesAct->setShortcut(Qt::Key_N);
    showNamesAct->setCheckable(true);

    showCoordinatesAct = new QAction("Show Source &coordinates", this);
    showCoordinatesAct->setShortcut(Qt::Key_C);
    showCoordinatesAct->setCheckable(true);

    newChannelAct  = new QAction("Add 1 new &Channel", this);
    newChannelAct->setShortcut(Qt::Key_A);
    newChannelsAct = new QAction("Add new Channel&s", this);
    newChannelsAct->setShortcut(QString("Shift+A"));

    showSnapshotSelectorAct = new QAction("Show &Snapshot-Selector", this);
    showSnapshotSelectorAct->setCheckable(true);
    showSnapshotSelectorAct->setShortcut(Qt::Key_F5);
    showSnapshotSelectorAct->setShortcutContext(Qt::ApplicationShortcut);

    showStreamClientWidgetAct = new QAction("Show &Streamclients", this);
    showStreamClientWidgetAct->setCheckable(true);
    showStreamClientWidgetAct->setShortcut(Qt::Key_F6);
    showStreamClientWidgetAct->setShortcutContext(Qt::ApplicationShortcut);

    takeSnapshotAct = new QAction("&Take new Snapshot", this);
    takeSnapshotAct->setShortcut(Qt::Key_T);

    //showTimeSelectorAct = new QAction("Show Time-Selector",this);
    //showTimeSelectorAct->setCheckable(true);

    startScoreplayerAct           = new QAction(QIcon(":/start_icon"), "Start", this);
    stopScoreplayerAct            = new QAction(QIcon(":/stop_icon"), "Stop", this);
    setRecordmodeInScoreplayerAct = new QAction(QIcon(":/record_icon"), "Record mode", this);
    setReadmodeInScoreplayerAct   = new QAction(QIcon(":/read_icon"), "Read mode", this);
    returnToZeroInScoreplayerAct  = new QAction(QIcon(":/RTZ_icon"), "Return to zero", this);

    clearScoreplayerAct              = new QAction("Clear Score", this);
    setStartScenarioInScoreplayerAct = new QAction("Set start scenario", this);

    //undoInScoreplayerAct          = new QAction( "Undo in Scoreplayer",this );
    //redoInScoreplayerAct          = new QAction( "Redo in Scoreplayer", this );
    //loadWithScoreplayerAct        = new QAction( "(Load Score)", this );
    //saveWithScoreplayerAct        = new QAction( "(Save Score)", this );
    enableMMCAct  = new QAction("Enable MMC", this);
    enableMSRCAct = new QAction("Enable MSRC", this);

    enableMMCAct->setCheckable(true);
    enableMMCAct->setChecked(true);

    enableMSRCAct->setCheckable(true);
    enableMSRCAct->setChecked(false);

    stopScoreplayerAct->setEnabled(false);

    setRecordmodeInScoreplayerAct->setCheckable(true);
    setReadmodeInScoreplayerAct  ->setCheckable(true);

    // set checkable options according to defaults in Sources3DWidget-Constructor
    lockViewAct ->setChecked(true);
    lockClickAct->setChecked(false);
    showPerspectiveViewAct->setChecked(true);
    //showLightingAct->setChecked(true);
    //showTimesliceConnectionsAct->setChecked(true);
    //showPointsourcesAsSpheresAct->setChecked(true);
    showCoordOriginAct->setChecked(false);
    //showTimeTicsAct->setChecked(false);
    showNamesAct->setChecked(true);
    showCoordinatesAct->setChecked(false);

    //connect(newSnapshotAct, SIGNAL(triggered()), this, SLOT(newSnapshot()));
    connect(newProjectWithScoreAct,    SIGNAL(triggered()), this, SLOT(newProjectWithScore()));
    connect(newProjectWithoutScoreAct, SIGNAL(triggered()), this, SLOT(newProjectWithoutScore()));
    connect(openProjectAct,            SIGNAL(triggered()), this, SLOT(openProject()));
    connect(saveAct,                   SIGNAL(triggered()), this, SLOT(save()));
    connect(saveAsAct,                 SIGNAL(triggered()), this, SLOT(saveAs()));
    connect(exitAct,                   SIGNAL(triggered()), this, SLOT(close()));

    connect(resetViewAct,                     SIGNAL(triggered()),
            sources3DView,                    SLOT(resetView()));
    connect(lockViewAct,                      SIGNAL(toggled(bool)),
            sources3DView,                    SLOT(lockView(bool)));
    connect(lockClickAct,                     SIGNAL(toggled(bool)),
            sources3DView,                    SLOT(lockClick(bool)));
    connect(showPerspectiveViewAct,           SIGNAL(toggled(bool)),
            sources3DView,                    SLOT(showPerspectiveView(bool)));
    connect(showCoordOriginAct,               SIGNAL(toggled(bool)),
            sources3DView,                    SLOT(showOrigin(bool)));
    connect(showNamesAct,                     SIGNAL(toggled(bool)),
            sources3DView,                    SLOT(showNames(bool)));
    connect(showCoordinatesAct,               SIGNAL(toggled(bool)),
            sources3DView,                    SLOT(showCoordinates(bool)));
    connect(showSnapshotSelectorAct,          SIGNAL(toggled(bool)),
            this,                             SLOT(showSnapshotSelector(bool)));
    connect(showStreamClientWidgetAct,        SIGNAL(toggled(bool)),
            this,                             SLOT(showStreamClientWidget(bool)));
    connect(takeSnapshotAct,                  SIGNAL(triggered()),
            snapshotSelector,                 SLOT(takeSnapshot()));
    connect(enableMMCAct,                     SIGNAL(triggered(bool)),
            this,                             SLOT(enableMMC(bool)));
    connect(enableMSRCAct,                    SIGNAL(triggered(bool)),
            this,                             SLOT(enableMSRC(bool)));
    connect(startScoreplayerAct,              SIGNAL(triggered()),
            this,                             SLOT(startScoreplayer()));
    connect(stopScoreplayerAct,               SIGNAL(triggered()),
            this,                             SLOT(stopScoreplayer()));
    connect(setRecordmodeInScoreplayerAct,    SIGNAL(triggered(bool)),
            this,                             SLOT(setRecordmodeInScoreplayer(bool)));
    connect(setReadmodeInScoreplayerAct,      SIGNAL(triggered(bool)),
            this,                             SLOT(setReadmodeInScoreplayer(bool)));
    connect(returnToZeroInScoreplayerAct,     SIGNAL(triggered()),
            this,                             SLOT(returnToZeroInScoreplayer()));
    connect(clearScoreplayerAct,              SIGNAL(triggered()),
            this,                             SLOT(fetchClearScoreRequest()));
    connect(setStartScenarioInScoreplayerAct, SIGNAL(triggered()),
            this,                             SLOT(setStartScenarioInScoreplayer()));
    //connect( undoInScoreplayerAct,          SIGNAL( triggered() ),
    //         this,                          SLOT  ( undoInScoreplayer() ) );
    //connect( redoInScoreplayerAct,          SIGNAL( triggered() ),
    //         this,                          SLOT  ( redoInScoreplayer() ) );
    //connect( saveWithScoreplayerAct,        SIGNAL( triggered() ),
    //         this,                          SLOT  ( saveWithScoreplayer() ) );
    //connect( loadWithScoreplayerAct,        SIGNAL( triggered() ),
    //         this,                          SLOT  ( loadWithScoreplayer() ) );
    //connect(showTimeSelectorAct, SIGNAL(toggled(bool)),
    //this, SLOT(showTimeSelector(bool)));
    //connect(showTimeTicsAct, SIGNAL(toggled(bool)),
    //  sources3DView, SLOT(showTimeTics(bool)));
    //connect(showLightingAct, SIGNAL(toggled(bool)),
    //sources3DView, SLOT(showLighting(bool)));
    //connect(showTimesliceConnectionsAct, SIGNAL(toggled(bool)),
    //  sources3DView, SLOT(showTimesliceConnections(bool)));
    //connect(showPointsourcesAsSpheresAct, SIGNAL(toggled(bool)),
    //  sources3DView, SLOT(showPointsourcesAsSpheres(bool)));
}

void XwonderMainWindow::createMenus() {
    fileMenu = menuBar()->addMenu("&File");
    menuBar()->setContextMenuPolicy(Qt::PreventContextMenu);

    newMenu = fileMenu->addMenu("&New");
    newMenu ->addAction(newProjectWithoutScoreAct);
    newMenu ->addAction(newProjectWithScoreAct);
    fileMenu->addAction(openProjectAct);
    fileMenu->addSeparator();
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    channelMenu    = new QMenu("&Channel");
    channelMenuAct = menuBar()->addMenu(channelMenu);
    channelMenu->addAction(newChannelAct);
    channelMenu->addAction(newChannelsAct);
    channelMenuAct->setVisible(false);

    viewMenu    = new QMenu("&View");
    viewMenuAct = menuBar()->addMenu(viewMenu);
    viewMenu->addAction(resetViewAct);
    viewMenu->addAction(lockViewAct);
    viewMenu->addAction(lockClickAct);
    viewMenu->addAction(showPerspectiveViewAct);
    //viewMenu->addAction(showLightingAct);
    //viewMenu->addAction(showTimesliceConnectionsAct);
    //viewMenu->addAction(showPointsourcesAsSpheresAct);
    viewMenu->addAction(showCoordOriginAct);
    //viewMenu->addAction(showTimeTicsAct);
    viewMenu->addAction(showNamesAct);
    viewMenu->addAction(showCoordinatesAct);
    viewMenuAct->setVisible(false);

    snapshotSelectorMenu    = new QMenu("&Snapshot-Selector");
    snapshotSelectorMenuAct = menuBar()->addMenu(snapshotSelectorMenu);
    snapshotSelectorMenu->addAction(showSnapshotSelectorAct);
    snapshotSelectorMenu->addAction(takeSnapshotAct);
    snapshotSelectorMenuAct->setVisible(false);

    streamClientWidgetMenu    = new QMenu("Streamclien&ts");
    streamClientWidgetMenuAct = menuBar()->addMenu(streamClientWidgetMenu);
    streamClientWidgetMenu->addAction(showStreamClientWidgetAct);
    streamClientWidgetMenuAct->setVisible(true);

    scoreplayerMenu    = new QMenu("Score&player");
    scoreplayerMenuAct = menuBar()->addMenu(scoreplayerMenu);
    //scoreplayerMenu->addAction(showTimeSelectorAct);
    //scoreplayerMenu->addSeparator();
    scoreplayerMenu->addAction(startScoreplayerAct);
    scoreplayerMenu->addAction(stopScoreplayerAct);
    scoreplayerMenu->addAction(setRecordmodeInScoreplayerAct);
    scoreplayerMenu->addAction(setReadmodeInScoreplayerAct);
    scoreplayerMenu->addAction(returnToZeroInScoreplayerAct);
    scoreplayerMenu->addSeparator();
    scoreplayerMenu->addAction(setStartScenarioInScoreplayerAct);
    scoreplayerMenu->addSeparator();
    //scoreplayerMenu->addAction( undoInScoreplayerAct );
    //scoreplayerMenu->addAction( redoInScoreplayerAct );
    scoreplayerMenu->addAction(clearScoreplayerAct);
    scoreplayerMenu->addSeparator();
    //scoreplayerMenu->addAction( saveWithScoreplayerAct );
    //scoreplayerMenu->addAction( loadWithScoreplayerAct );
    scoreplayerMenu->addAction(enableMMCAct);
    scoreplayerMenu->addAction(enableMSRCAct);
    scoreplayerMenuAct->setVisible(false);
}


void XwonderMainWindow::createToolBars() {
    scoreplayerToolBar = addToolBar("Scoreplayer-Toolbar");
    scoreplayerToolBar->setContextMenuPolicy(Qt::PreventContextMenu);
    scoreplayerToolBar->setMovable(false);
    scoreplayerToolBar->addAction(returnToZeroInScoreplayerAct);
    scoreplayerToolBar->addAction(startScoreplayerAct);
    scoreplayerToolBar->addAction(stopScoreplayerAct);
    scoreplayerToolBar->addAction(setRecordmodeInScoreplayerAct);
    scoreplayerToolBar->addAction(setReadmodeInScoreplayerAct);

    //get access to the transport QToolButtons by name
    QList< QToolButton* > widgets = scoreplayerToolBar->findChildren< QToolButton* >();

    for(auto widget : widgets) {
        if(widget->property("text") == "Record mode") {
            globalRecButton = widget;
        } else if(widget->property("text") == "Read mode") {
            globalReadButton = widget;
            //globalReadButton->setMinimumSize( globalRecButton->sizeHint() );
        } else if(widget->property("text") == "Start") {
            startButton = widget;
        } else if(widget->property("text") == "Stop") {
            stopButton = widget;
        }
    }

    //create Palettes
    notPressedPalette = globalRecButton->palette();
    redPalette        = QPalette(Qt::red);
    greenPalette      = QPalette(Qt::green);
    bluePalette       = QPalette(Qt::blue);
    //bluePalette.setCurrentColorGroup(QPalette::Inactive);

    scoreplayerToolBar->addWidget(timeLCD);
    timeLCD->setFixedHeight(scoreplayerToolBar->iconSize().height());
    scoreplayerToolBar->setVisible(false);

    // read mode's default value in scoreplayer is "on"
    setReadmodeInScoreplayer(true, true);
}


void XwonderMainWindow::enableActions() {
    resetViewAct             ->setEnabled(true);
    showPerspectiveViewAct   ->setEnabled(true);
    showCoordOriginAct       ->setEnabled(true);
    showNamesAct             ->setEnabled(true);
    showCoordinatesAct       ->setEnabled(true);
    lockViewAct              ->setEnabled(true);
    lockClickAct             ->setEnabled(true);
    showSnapshotSelectorAct  ->setEnabled(true);

    newChannelAct  ->setEnabled(true);
    newChannelsAct ->setEnabled(true);
    takeSnapshotAct->setEnabled(true);

    startScoreplayerAct             ->setEnabled(true);
    stopScoreplayerAct              ->setEnabled(true);
    setRecordmodeInScoreplayerAct   ->setEnabled(true);
    setReadmodeInScoreplayerAct     ->setEnabled(true);
    returnToZeroInScoreplayerAct    ->setEnabled(true);
    clearScoreplayerAct             ->setEnabled(true);
    setStartScenarioInScoreplayerAct->setEnabled(true);
    enableMMCAct                    ->setEnabled(true);
    enableMSRCAct                   ->setEnabled(true);
}


void XwonderMainWindow::disableActions() {
    saveAct                  ->setEnabled(false);
    saveAsAct                ->setEnabled(false);
    resetViewAct             ->setEnabled(false);
    showPerspectiveViewAct   ->setEnabled(false);
    showCoordOriginAct       ->setEnabled(false);
    showNamesAct             ->setEnabled(false);
    showCoordinatesAct       ->setEnabled(false);
    lockViewAct              ->setEnabled(false);
    lockClickAct             ->setEnabled(false);
    showSnapshotSelectorAct  ->setEnabled(false);

    newChannelAct  ->setEnabled(false);
    newChannelsAct ->setEnabled(false);
    takeSnapshotAct->setEnabled(false);

    startScoreplayerAct             ->setEnabled(false);
    stopScoreplayerAct              ->setEnabled(false);
    setRecordmodeInScoreplayerAct   ->setEnabled(false);
    setReadmodeInScoreplayerAct     ->setEnabled(false);
    returnToZeroInScoreplayerAct    ->setEnabled(false);
    clearScoreplayerAct             ->setEnabled(false);
    setStartScenarioInScoreplayerAct->setEnabled(true);
    enableMMCAct                    ->setEnabled(false);
    enableMSRCAct                   ->setEnabled(false);
}

void XwonderMainWindow::newProjectWithScore() {
    projectOnlyMode = false;
    newProject();

}


void XwonderMainWindow::newProjectWithoutScore() {
    projectOnlyMode = true;
    newProject();
}


void XwonderMainWindow::newProject() {
    if(fetchUnsavedChanges()) {

        // get a valid filename from the user
        bool okToProceed = false;

        while(! okToProceed) {
            // check if cwonder runs on a different machine
            if(! localMode) {
                int dialogReturnCode = filenameDialog->exec();

                if(dialogReturnCode == QDialog::Rejected) {
                    return;
                }

                if(! projectname.isEmpty()) {
                    okToProceed = true;
                }
            } else {
                QMessageBox::information(this, "Xwonder", "Please select the file in which your Project"
                                         " should be stored.\nYou may also enter a new name for a "
                                         " non-existing file.");
                projectname = QFileDialog::getSaveFileName(this, "File for new Project:", ".");

                // do nothing without a filename
                if(projectname.isEmpty()) {
                    return;
                } else {
                    okToProceed = true;
                }

                // if the file already exists first delete it and make a new one,
                // because cwonder does not accept /WONDER/project/create with existing files
                QFile tempProjectfile(projectname);

                if(tempProjectfile.exists()) {
                    if(! tempProjectfile.remove()) {
                        QMessageBox::warning(this, "XWonder - Access Error", "The selected file is"
                                             "not accessible!\nMaybe you do not have permission to:"
                                             "access this file.\nPlease select another file.",
                                             QMessageBox::Ok);
                        okToProceed = false;
                    } else {
                        okToProceed = true;
                    }
                }
            }
        }

        // get rid of fileextension ".xml" if it came in
        if(projectname.endsWith(".xml")) {
            projectname = projectname.left(projectname.lastIndexOf(".xml"));
        }

        // set the xwonders mode according to what the user wants to create
        if(projectOnlyMode) {
            setProjectWithoutScoreMode();
        } else {
            setProjectWithScoreMode();
        }

        // empty the snapshotSelector
        snapshotSelector->reset();

        // reset channelsview and sources3DWidget
        channelsView->reset();
        sources3DView->reset();

        // signal that this loadrequest or in this case createrequest comes from xwonder
        myProjectLoadRequest = true;

        if(! xwConf->runWithoutCwonder) {
            if(projectOnlyMode)
                lo_send(xwConf->cwonderAddr, "/WONDER/project/createWithoutScore", "s",
                        projectname.toStdString().c_str());
            else
                lo_send(xwConf->cwonderAddr, "/WONDER/project/createWithScore", "s",
                        projectname.toStdString().c_str());
        }

        setModified();
    }
}


void XwonderMainWindow::openProject() {
    // note: the appropriate mode will be set later when the xmlDump
    // is parsed

    if(fetchUnsavedChanges()) {

        if(! localMode) {
            int dialogReturnCode = filenameDialog->exec();

            if(dialogReturnCode == QDialog::Rejected) {
                return;
            }
        } else {
            projectname = QFileDialog::getOpenFileName(this, "Open File", ".", "WONDER projectfiles (*.xml)");
        }

        if(! projectname.isEmpty()) {
            // get rid of fileextension ".xml"
            if(projectname.endsWith(".xml")) {
                projectname = projectname.left(projectname.lastIndexOf(".xml"));
            }

            // signal that this loadrequest comes from xwonder
            myProjectLoadRequest = true;

            // empty snapshotSelector
            snapshotSelector->reset();

            // reset channelsview and sources3DWidget
            channelsView->reset();
            sources3DView->reset();

            if(! xwConf->runWithoutCwonder) {
                lo_send(xwConf->cwonderAddr, "/WONDER/project/load", "s", projectname.toStdString().c_str());
            }

            setNotModified();
        }
    }
}


void XwonderMainWindow::fetchOSCProjectXMLDump(QString xmlProject) {
    if(! myProjectLoadRequest) {
        // the project was loaded previous to starting xwonder
        // or the load command for cwoder did not come from xwonder
        // notify user and ask what to do

        int answer = QMessageBox::warning(this, "XWonder - Project loaded/created", "A Project has been loaded or created!\n"
                                          "Do you want to edit this Project?",
                                          QMessageBox::Yes | QMessageBox::Default,
                                          QMessageBox::No);

        if(answer == QMessageBox::Yes) {
            //parse the information send by /WONDER/project/current
            parseLoadedProjectXMLDump(xmlProject);
        } else {
            setNoProjectMode();
        }

    } else {
        parseLoadedProjectXMLDump(xmlProject);
    }
}


void XwonderMainWindow::fetchOSCErrorMessage(QString errorMsg) {
    QMessageBox::warning(0, "Error received!", "Another part of WONDER generated an error.\n"
                         "Here is the errormessage xwonder received:\n\n" + errorMsg);
}


void XwonderMainWindow::activateSource(int sourceID) {
    // this starts a chainreaction in which a channel will be added to channelsview
    // and a source will be added to sources3DWidget
    channelsView->activateChannel(sourceID, true);
}


void XwonderMainWindow::deactivateSource(int sourceID) {
    // this tells sources3DWidget to deactivate the source as well
    channelsView-> deactivateChannel(sourceID);
}



void XwonderMainWindow::showSnapshotSelector(bool showIt) {
    if(showIt) {
        snapshotSelector->show();
    } else {
        snapshotSelector->hide();
    }

}


void XwonderMainWindow::showStreamClientWidget(bool showIt) {
    if(showIt) {
        streamClientWidget->show();
    } else {
        streamClientWidget->hide();
    }

}


void XwonderMainWindow::snapshotSelectorClosedItself() {
    showSnapshotSelectorAct->setChecked(false);
}


void XwonderMainWindow::streamClientWidgetClosedItself() {
    showStreamClientWidgetAct->setChecked(false);
}

//void XwonderMainWindow::showTimeSelector(bool showIt)
//{
//      //TEST
//      //lo_send( xwConf->cwonderAddr, "/WONDER/stream/visual/send", "siiii", "/WONDER/xwonder/mtctime",0,13,2,40);
//      if(showIt)
//              timeSelector->show();
//      else
//              timeSelector->hide();
//}

//void XwonderMainWindow::timeSelectorClosedItself()
//{
//      showTimeSelectorAct->setChecked(false);
//}


void XwonderMainWindow::startScoreplayer() {
    if(! xwConf->runWithoutCwonder) {
        lo_send(xwConf->cwonderAddr, "/WONDER/score/play", "");
    }

    startScoreplayerAct->setEnabled(false);
    stopScoreplayerAct-> setEnabled(true);
    startButton->setPalette(bluePalette);
    stopButton-> setPalette(notPressedPalette);

    scoreplayerIsPlaying = true;

}


void XwonderMainWindow::stopScoreplayer() {
    if(! xwConf->runWithoutCwonder) {
        lo_send(xwConf->cwonderAddr, "/WONDER/score/stop", "");
    }

    startScoreplayerAct->setEnabled(true);
    stopScoreplayerAct ->setEnabled(false);
    startButton->setPalette(notPressedPalette);
    stopButton-> setPalette(bluePalette);

    setRecordmodeInScoreplayer(false, true);

    scoreplayerIsPlaying = false;
}


void XwonderMainWindow::scoreplayerStopped() {
    startScoreplayerAct->setEnabled(true);
    stopScoreplayerAct ->setEnabled(false);
    startButton->setPalette(notPressedPalette);
    stopButton-> setPalette(bluePalette);

    setRecordmodeInScoreplayer(false, true);

    scoreplayerIsPlaying = false;
}


void XwonderMainWindow::scoreplayerStarted() {
    startScoreplayerAct->setEnabled(false);
    stopScoreplayerAct ->setEnabled(true);
    startButton->setPalette(bluePalette);
    stopButton->setPalette(notPressedPalette);

    scoreplayerIsPlaying = true;
}


void XwonderMainWindow::setRecordmodeInScoreplayer(bool recEnabled, bool externalSet) {
    if(recEnabled) {
        //check if recordmode was set externally, if so, do not send OSC-Messages
        globalRecButton->setPalette(redPalette);
        setRecordmodeInScoreplayerAct->setChecked(true);

        if(! xwConf->runWithoutCwonder)
            if(! externalSet) {
                lo_send(xwConf->cwonderAddr, "/WONDER/score/enableRecord", "i", 1);
            }
    } else {
        //check if recordmode was set externally, if so, do not send OSC-Messages
        globalRecButton->setPalette(notPressedPalette);
        setRecordmodeInScoreplayerAct->setChecked(false);

        if(! xwConf->runWithoutCwonder)
            if(! externalSet) {
                lo_send(xwConf->cwonderAddr, "/WONDER/score/enableRecord", "i", 0);
            }
    }

    timeLCD->setRecordMode(recEnabled);
}

void XwonderMainWindow::setReadmodeInScoreplayer(bool readEnabled, bool externalSet) {
    if(readEnabled) {
        globalReadButton->setPalette(greenPalette);
        setReadmodeInScoreplayerAct->setChecked(true);
    } else {
        globalReadButton->setPalette(notPressedPalette);
        setReadmodeInScoreplayerAct->setChecked(false);
    }

    int read = (int) readEnabled;

    sources3DView->changeReadMode(readEnabled);

    //check if readmode was set externally, if so, do not send OSC-Messages
    if(! xwConf->runWithoutCwonder)
        if(! externalSet) {
            lo_send(xwConf->cwonderAddr, "/WONDER/score/enableRead", "i", read);
        }
}


void XwonderMainWindow::returnToZeroInScoreplayer() {
    stopScoreplayer();

    if(! xwConf->runWithoutCwonder) {
        lo_send(xwConf->cwonderAddr, "/WONDER/score/newtime", "iiii", 0, 0, 0, 0);
    }
}


void XwonderMainWindow::clearScoreplayer() {
    if(! xwConf->runWithoutCwonder) {
        lo_send(xwConf->cwonderAddr, "/WONDER/score/reset", "");
    }
}

void XwonderMainWindow::setStartScenarioInScoreplayer() {
    setModified();

    if(! xwConf->runWithoutCwonder) {
        lo_send(xwConf->cwonderAddr, "/WONDER/score/setStartScenario", "");
    }
}


//NOTE: I don't really know if these two work properly in the scoreplayer
//void XwonderMainWindow::undoInScoreplayer()
//{
//    if(!xwConf->runWithoutCwonder)
//        lo_send(xwConf->cwonderAddr, "/WONDER/score/undo","");
//}
//
//void XwonderMainWindow::redoInScoreplayer()
//{
//    if(!xwConf->runWithoutCwonder)
//        lo_send(xwConf->cwonderAddr, "/WONDER/score/redo","");
//}


//void XwonderMainWindow::saveWithScoreplayer()
//{
//    QString fullFilename;
//    if( ! localMode )
//    {
//        filenameDialog->exec();
//        fullFilename = projectname;
//    }
//    else
//        fullFilename = QFileDialog::getSaveFileName( this, "Save File", "." );
//
//    if( fullFilename.isEmpty() )
//        return;
//
//    if( ! xwConf->runWithoutCwonder )
//        lo_send( xwConf->cwonderAddr, "/WONDER/score/save", "s",
//                 ( fullFilename.toStdString() ).c_str());
//
//    setNotModified();
//    return;
//}


//void XwonderMainWindow::loadWithScoreplayer()
//{
//    QString fullFilename = "";
//    if( ! localMode )
//    {
//        filenameDialog->exec();
//        fullFilename = projectname;
//    }
//    else
//        fullFilename = QFileDialog::getOpenFileName( this, "Load File", "." );
//
//    if( fullFilename.isEmpty() )
//        return;
//
//    if( ! xwConf->runWithoutCwonder )
//        lo_send( xwConf->cwonderAddr, "/WONDER/score/load", "s", fullFilename.toStdString().c_str() );
//
//    setNotModified();
//    return;
//
//}


void XwonderMainWindow::enableMMC(bool enableIt, bool externalSet) {
    if(externalSet) {
        enableMMCAct->setChecked(enableIt);
    } else {
        if(! xwConf->runWithoutCwonder) {
            lo_send(xwConf->cwonderAddr, "/WONDER/score/enableMMC", "i", (int) enableIt);
        }
    }
}


void XwonderMainWindow::enableMSRC(bool enableIt, bool externalSet) {
    if(externalSet) {
        enableMSRCAct->setChecked(enableIt);
    } else {
        if(! xwConf->runWithoutCwonder) {
            lo_send(xwConf->cwonderAddr, "/WONDER/score/enableMSRC", "i", (int) enableIt);
        }
    }
}


void XwonderMainWindow::fetchClearScoreRequest() {
    // just make sure the user really wants to delete the whole score
    int answer = QMessageBox::warning(0, "XWonder - Clear Score", "Do you really want to delete\n"
                                      "all recorded score data?",
                                      QMessageBox::Yes,
                                      QMessageBox::Cancel | QMessageBox::Escape | QMessageBox::Default);

    if(answer == QMessageBox::Yes) {
        clearScoreplayer();
        setModified();
    } else {
        return;
    }
}


bool XwonderMainWindow::fetchUnsavedChanges() {
    // nothing to save if cwonder is not running
    if(xwConf->runWithoutCwonder) {
        return true;
    }

    if(wasModified) {
        int answer = QMessageBox::warning(0, "XWonder - File edited", "This file has been edited!\n"
                                          "Do you want to save your changes?",
                                          QMessageBox::Yes,
                                          QMessageBox::No,
                                          QMessageBox::Cancel | QMessageBox::Escape | QMessageBox::Default);

        if(answer == QMessageBox::Yes) {
            return save();
        } else if(answer == QMessageBox::No) {
            return true;
        } else {
            return false;
        }
    } else {
        return true;
    }
}


bool XwonderMainWindow::save() {
    if(projectname.isEmpty()) {
        return saveAs();
    } else {
        if(! xwConf->runWithoutCwonder) {
            lo_send(xwConf->cwonderAddr, "/WONDER/project/save", "");
        }

        setNotModified();
    }

    return true;
}

bool XwonderMainWindow::saveAs() {
    QString newFilename           = "" ;
    // save the projectname, since the filenameDialog overwrites it
    QString projectnameTempString = projectname ;

    if(! localMode) {
        filenameDialog->exec();
        newFilename = projectname;
        projectname = projectnameTempString;
    } else {
        newFilename = QFileDialog::getSaveFileName(this, "Save File", ".");
    }

    if(newFilename.isEmpty()) {
        return false;
    }

    // get rid of fileextension ".xml" if it came in
    if(projectname.endsWith(".xml")) {
        projectname = projectname.left(projectname.lastIndexOf(".xml"));
    }

    if(! xwConf->runWithoutCwonder) {
        lo_send(xwConf->cwonderAddr, "/WONDER/project/save", "s", newFilename.toStdString().c_str());
    }

    // cwonder makes the file under which it saved the current file, so we do this too
    projectname = newFilename;

    setNotModified();

    return true;
}


void XwonderMainWindow::setModified() {
    QString title = basicWindowTitle;

    if(projectname.isEmpty()) {
        title += " --- ";
    } else {
        title += " --- " + projectname;
    }

    title += "*";

    wasModified = true;

    setWindowTitle(title);

    if(! xwConf->runWithoutCwonder) {
        saveAct->setEnabled(true);
        saveAsAct->setEnabled(true);
    }
}


void XwonderMainWindow::setNotModified() {
    QString title = basicWindowTitle;

    if(! projectname.isEmpty()) {
        title += (" --- " + projectname);
    }

    setWindowTitle(title);

    wasModified = false;
}


void XwonderMainWindow::updateStatusBar(const Source& source) {
    SourceCoordinates coord = source.getCoordinates().mapGLCoordToWonderCoord();

    QString message = "Source Coordinates (in m): [" + QString::number(coord.x, 'f', 2) +
                      "|" + QString::number(coord.y, 'f', 2) + "]";

    if(source.isPlanewave()) {
        message += ", Orientation (in deg): " + QString::number(coord.orientation, 'f', 1);
    }

    statusBar()->showMessage(message, 5000);
}


void XwonderMainWindow::updateStatusBar(const SourceGroup& group) {
    SourceCoordinates coord = group.getCoordinates().mapGLCoordToWonderCoord();

    QString message = "Group Coordinates (in m): [" + QString::number(coord.x, 'f', 2) +
                      "|" + QString::number(coord.y, 'f', 2) + "]";

    statusBar()->showMessage(message, 5000);
}


void XwonderMainWindow::sendOSCSnapshotTake(unsigned int snapshotID, QString name) {
    if(! xwConf->runWithoutCwonder) {
        lo_send(xwConf->cwonderAddr, "/WONDER/snapshot/take", "is", snapshotID, name.toStdString().c_str());
    }

    setModified();
}

void XwonderMainWindow::sendOSCSnapshotRecall(unsigned int snapshotID, double transitionTime) {

    if(! xwConf->runWithoutCwonder) {
        lo_send(xwConf->cwonderAddr, "/WONDER/snapshot/recall", "if", snapshotID, (float) transitionTime);
    }
}


void XwonderMainWindow::sendOSCSnapshotRename(unsigned int snapshotID, QString newName) {
    if(! xwConf->runWithoutCwonder) {
        lo_send(xwConf->cwonderAddr, "/WONDER/snapshot/rename", "is", snapshotID, newName.toStdString().c_str());
    }

    setModified();
}


void XwonderMainWindow::sendOSCSnapshotCopy(unsigned int fromID, unsigned int toID) {
    //send OSC-Message also for the new name
    if(! xwConf->runWithoutCwonder) {
        lo_send(xwConf->cwonderAddr, "/WONDER/snapshot/copy", "ii", fromID, toID);
    }

    setModified();
}


void XwonderMainWindow::sendOSCSnapshotDelete(unsigned int snapshotID) {
    if(! xwConf->runWithoutCwonder) {
        lo_send(xwConf->cwonderAddr, "/WONDER/snapshot/delete", "i", snapshotID);
    }

    setModified();
}


void XwonderMainWindow::parseLoadedProjectXMLDump(QString xmlProject) {
    // because cwonder does the validation
    // we assume that only valid xml-strings arrive here

    QDomDocument xmlDocument;
    xmlDocument.setContent(xmlProject);
    QDomElement projectRoot = xmlDocument.documentElement();

    // if the load command was not issued by xwonder
    // get the projectname
    if(! myProjectLoadRequest) {
        projectname  = projectRoot.attribute("name");
    }

    // check if a scorefile is associated with this project
    // and set mode accordingly
    if(projectRoot.attribute("scorefile").isEmpty()) {
        setProjectWithoutScoreMode();
    } else {
        setProjectWithScoreMode();
    }

    // setup xwonder according to scenario
    QDomElement scenarioElement = projectRoot.firstChildElement("scenario");
    QDomElement sources         = scenarioElement.firstChildElement("source");

    // parse all sources, add each source and setup it's attributes
    for(; ! sources.isNull(); sources = sources.nextSiblingElement("source")) {
        Source temp(0, 0);
        QColor color(0, 255, 0);
        QDomNamedNodeMap attributes = sources.attributes();

        for(int i = 0; i < attributes.length(); ++i) {
            QDomAttr attr;

            if(attributes.item(i).isAttr()) {
                attr = attributes.item(i).toAttr();
            } else {
                continue;
            }

            if(attr.name() == "id") {
                temp.setID(attr.value().toInt());
            } else if(attr.name() == "name") {
                temp.setName(attr.value());
            } else if(attr.name() == "type") {
                bool planewave = false;

                if(attr.value().toInt() == 0) {
                    planewave = true;
                }

                temp.setType(planewave);
            } else if(attr.name() == "posx") {
                temp.setx(attr.value().toFloat());
            } else if(attr.name() == "posy") {
                temp.sety(attr.value().toFloat());
            } else if(attr.name() == "angle") {
                temp.setOrientation(attr.value().toFloat());
            } else if(attr.name() == "colorR") {
                color.setRed(attr.value().toInt());
            } else if(attr.name() == "colorG") {
                color.setGreen(attr.value().toInt());
            } else if(attr.name() == "colorB") {
                color.setBlue(attr.value().toInt());
            } else if(attr.name() == "groupId") {
                temp.setGroupID(attr.value().toInt());
            } else if(attr.name() == "invRotation") {
                temp.setRotationDirection((bool) attr.value().toInt());
            } else if(attr.name() == "invScaling") {
                temp.setScalingDirection((bool) attr.value().toInt());
            } else if(attr.name() == "doppler") {
                temp.setDopplerEffect((bool) attr.value().toInt());
            }
        }

        temp.setColor(color);
        temp.setCoordinates(temp.getCoordinates().mapGLCoordToWonderCoord());

        channelsView->setChannel(temp);
        sources3DView->setSource(temp);
    }

    QDomElement groups = scenarioElement.firstChildElement("group");

    // parse all sources, add each source and setup it's attributes
    for(; ! groups.isNull(); groups = groups.nextSiblingElement("group")) {
        SourceGroup temp(0, 0);
        QColor color(255, 0, 0);
        QDomNamedNodeMap attributes = groups.attributes();

        for(int i = 0; i < attributes.length(); ++i) {
            QDomAttr attr;

            if(attributes.item(i).isAttr()) {
                attr = attributes.item(i).toAttr();
            } else {
                continue;
            }

            if(attr.name() == "id") {
                temp.setGroupID(attr.value().toInt());
            } else if(attr.name() == "posx") {
                temp.setx(attr.value().toFloat());
            } else if(attr.name() == "posy") {
                temp.sety(attr.value().toFloat());
            } else if(attr.name() == "colorR") {
                color.setRed(attr.value().toInt());
            } else if(attr.name() == "colorG") {
                color.setGreen(attr.value().toInt());
            } else if(attr.name() == "colorB") {
                color.setBlue(attr.value().toInt());
            }

        }

        temp.setColor(color);
        temp.setCoordinates(temp.getCoordinates().mapGLCoordToWonderCoord());
        sources3DView->setGroup(temp);
    }

    // look for snapshots and read IDs and names
    // and set snapshotSelector accordingly
    QDomElement snapshotElement = projectRoot.firstChildElement("snapshot");

    for(; ! snapshotElement.isNull(); snapshotElement = snapshotElement.nextSiblingElement("snapshot")) {
        QString snapshotID   = snapshotElement.attribute("id");
        QString snapshotName = snapshotElement.attribute("name");
        snapshotSelector->takeSnapshot(snapshotID.toInt(), snapshotName);
    }

    if(myProjectLoadRequest) {
        setNotModified();
        myProjectLoadRequest = false;
    }
}
