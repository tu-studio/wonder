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

#ifndef XWONDERMAINWINDOW_H
#define XWONDERMAINWINDOW_H

//----------------------------------includes----------------------------------//

#include <QMainWindow>
class QHBoxLayout;
class QScrollArea;
class QStatusBar;
class QToolButton;
class QAction;
class QMenu;
class QToolBar;
class QCloseEvent;

class Sources3DWidget;
class ChannelsWidget;
class SnapshotSelector;
class StreamClientWidget;
class TimeLCDNumber;
class FilenameDialog;
class Source;
class SourceGroup;

//----------------------------------------------------------------------------//

//----------------------------------Doxygen-----------------------------------//
/*!
 *      \brief
 *      The main interface to xwonder.
 *
 *      \details
 *      This is the main window of xwonder. Complete application interface with menubar,
 * toolbar, statusbar and the two custom wigdets ChannelsWidget (left screen side) and
 * Sources3DWidget (right screen side). It connects these widgets and dispatches
 * OSC-messages to the appropriate receiver. Coordination of cwonder and scoreplayer is
 * done here. It keeps track of changes worth saving and handles all file I/O according to
 * cwonder's location (local/remote).
 *
 *      \author
 *      Hans-Joachim Mond
 *
 *      \date
 *      09.08.2008
 *
 *      \todo
 *      - extend contextmenu for groups (remove all members, setPostionDialog (extend
 * SourcesDialog))
 *      - complete doxygen documentation
 *      - lo_send returnvalues should be evaluated (i.e. more errorchecking)
 *
 */

class XwonderMainWindow : public QMainWindow
{
    Q_OBJECT

  public:
    XwonderMainWindow(QWidget* parent = 0);
    virtual QSize minimumSizeHint() const;

  protected:
    // event handlers
    void closeEvent(QCloseEvent* event);
    void keyPressEvent(QKeyEvent* event);

  private slots:
    // outgoing OSC
    void sendOSCSourceActivate(int sourceID);
    void sendOSCSourceDeactivate(int sourceID);
    void sendOSCSourcePosition(const Source& source);
    void sendOSCSourceOrientation(const Source& source);
    void sendOSCSourceGroupID(int sourceID, int groupID);
    void sendOSCSourceType(unsigned int xID, bool planewave);
    void sendOSCSourceName(unsigned int xID, const QString& name);
    void sendOSCSourceColor(unsigned int xID, const QColor& color);
    void sendOSCSourceColor(int sourceID, const QColor& color);
    void sendOSCSourceRecordMode(unsigned int xID, bool recEnabled);
    void sendOSCSourceReadMode(unsigned int xID, bool readEnabled);
    void sendOSCSourceRotationDirection(unsigned int xID, bool inverted);
    void sendOSCSourceScalingDirection(unsigned int xID, bool inverted);
    void sendOSCSourceDopplerEffetChanged(unsigned int xID, bool dopplerOn);

    void sendOSCGroupActivate(int groupID);
    void sendOSCGroupDeactivate(int groupID);
    void sendOSCGroupPosition(const SourceGroup& group);
    void sendOSCGroupColor(int groupID, const QColor& newColor);

    void sendOSCNotifyIDChange(unsigned int xID, int oldID, int newID);

    void sendOSCSnapshotTake(unsigned int snapshotID, QString newName);
    void sendOSCSnapshotRecall(unsigned int sourceID, double transitionTime);
    void sendOSCSnapshotDelete(unsigned int snapshotID);
    void sendOSCSnapshotCopy(unsigned int fromID, unsigned int toID);
    void sendOSCSnapshotRename(unsigned int snapshotID, QString newName);

    // incoming OSC
    void fetchOSCProjectXMLDump(QString xmlProject);
    void fetchOSCErrorMessage(QString errorMsg);

    // (de)activate sources
    void activateSource(int sourceID);
    void deactivateSource(int sourceID);

    // snapshots and snapshotselector
    void showSnapshotSelector(bool showIt);
    void snapshotSelectorClosedItself();

    // show streamclientwidget
    void showStreamClientWidget(bool showIt);
    void streamClientWidgetClosedItself();

    // void showTimeSelector(bool showIt);
    // void timeSelectorClosedItself();

    // scoreplayer
    void startScoreplayer();
    void stopScoreplayer();
    void setRecordmodeInScoreplayer(bool recEnabled, bool externaSet = false);
    void setReadmodeInScoreplayer(bool readEnabled, bool externaSet = false);
    void returnToZeroInScoreplayer();
    void fetchClearScoreRequest();
    void clearScoreplayer();
    void setStartScenarioInScoreplayer();
    // void undoInScoreplayer();
    // void redoInScoreplayer();
    void enableMMC(bool enableIt, bool externalSet = false);
    void enableMSRC(bool enableIt, bool externalSet = false);

    // functions for setting xwonder's transportbuttons according to messages sent by
    // scoreplayer
    void scoreplayerStopped();
    void scoreplayerStarted();

    // cwonder project
    void newProjectWithScore();
    void newProjectWithoutScore();
    void newProject();
    void openProject();
    bool save();
    bool saveAs();
    void setFilename();

    // set/unset flag for changes worth saving
    void setModified();
    void setNotModified();

    void toggleAddChannelActs(unsigned int notUsed1 = 0, int notUsed2 = 0,
                              bool notUsed3 = false);
    void shiftFocusToSources3DWidget(unsigned int notUsed);
    void updateStatusBar(const Source&);
    void updateStatusBar(const SourceGroup&);

    void setDemoMode();

    // dispatch all keys related to the sources view
    void dispatchViewRelatedKeys(QKeyEvent* event);

  private:
    // widget initialisation
    void createActions();
    void createMenus();
    void createToolBars();

    void enableActions();
    void disableActions();

    // modes according to cwonder project with or without associated score
    void setNoProjectMode();
    void setProjectWithScoreMode();
    void setProjectWithoutScoreMode();
    void setMode(bool projectOnlyMode);  // otherwise projectWithScoreMode

    // cwonder
    void parseLoadedProjectXMLDump(QString xmlProject);
    // void clearSnapshotsList();

    bool fetchUnsavedChanges();

    // if cwonder does not run on localhost, then project and score names are
    // only filenames without the path part
    QString projectname;       // absolute filename for cwonderproject (without ".xml")
    QString basicWindowTitle;  // base for the text that will be displayed in xwonders
                               // windowtitle

    // true if cwonder is running on same computer
    bool localMode;

    // cwonder knows two kinds of projects, with or without score. Xwonder provides the
    // adequate interface and functionality. If projectOnlyMode  is false, we are in
    // projectWithScoreMode.
    bool projectOnlyMode;

    // flag for scoreplayers current mode, playing or stopped
    bool scoreplayerIsPlaying;

    // permanent widgets of the main window (according to mode)
    Sources3DWidget* sources3DView;
    QScrollArea* channelsScrollArea;
    ChannelsWidget* channelsView;
    SnapshotSelector* snapshotSelector;
    StreamClientWidget* streamClientWidget;
    TimeLCDNumber* timeLCD;
    QToolBar* scoreplayerToolBar;
    QStatusBar* theStatusBar;
    // TimelineWidget*    timeSelector;
    QWidget* centralWidget;
    QHBoxLayout* hLayout;

    // this dialog is needed if cwonder runs on a different machine and thus filebrowsing
    // is not possible
    FilenameDialog* filenameDialog;

    // access to transportbuttons for setting their palettes
    QToolButton* globalRecButton;
    QToolButton* globalReadButton;
    QToolButton* startButton;
    QToolButton* stopButton;

    // palettes for transportbuttons
    QPalette notPressedPalette;
    QPalette redPalette;
    QPalette greenPalette;
    QPalette bluePalette;

    // actions

    // project/file related
    QAction* newProjectWithScoreAct;
    QAction* newProjectWithoutScoreAct;
    QAction* openProjectAct;
    QAction* saveAct;
    QAction* saveAsAct;
    QAction* exitAct;

    // display/view related
    QAction* resetViewAct;
    QAction* showPerspectiveViewAct;
    // QAction* showLightingAct;
    // QAction* showTimesliceConnectionsAct;
    // QAction* showTimeTicsAct;
    QAction* showCoordOriginAct;
    QAction* showNamesAct;
    QAction* showCoordinatesAct;
    QAction* lockViewAct;
    QAction* lockClickAct;
    QAction* showSnapshotSelectorAct;
    QAction* showStreamClientWidgetAct;
    QAction* showTimeSelectorAct;

    // source/snapshot related
    QAction* newChannelAct;
    QAction* newChannelsAct;
    QAction* takeSnapshotAct;

    // scoreplayer related
    QAction* startScoreplayerAct;
    QAction* stopScoreplayerAct;
    QAction* setRecordmodeInScoreplayerAct;
    QAction* setReadmodeInScoreplayerAct;
    QAction* returnToZeroInScoreplayerAct;
    QAction* clearScoreplayerAct;
    QAction* setStartScenarioInScoreplayerAct;
    // QAction* undoInScoreplayerAct;
    // QAction* redoInScoreplayerAct;
    // QAction* loadWithScoreplayerAct;
    // QAction* saveWithScoreplayerAct;
    QAction* enableMMCAct;
    QAction* enableMSRCAct;

    // actions for hiding/showing menus
    QAction* viewMenuAct;
    QAction* channelMenuAct;
    QAction* snapshotSelectorMenuAct;
    QAction* streamClientWidgetMenuAct;
    QAction* scoreplayerMenuAct;

    // menus
    QMenu* fileMenu;
    QMenu* newMenu;
    QMenu* openMenu;
    QMenu* saveMenu;
    QMenu* viewMenu;
    QMenu* channelMenu;
    QMenu* snapshotSelectorMenu;
    QMenu* streamClientWidgetMenu;
    QMenu* scoreplayerMenu;

    // unsaved changes have to be fetched if the current Snapshot/Score has been edited
    bool wasModified;

    // whether Xwonder issued a loadrequest to cwonder or someone else
    bool myProjectLoadRequest;

    // score related data
    // unsigned int scoreDuration;    //in ms
    // unsigned int timegridDistance; //in ms

};  // class XwonderMainWindow

#endif  // XWONDERMAINWINDOW_H
