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
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef SOURCES3DWIDGET_H
#define SOURCES3DWIDGET_H

//-------------------------------------Includes-----------------------------------//

//#include <cmath>

#include <GL/glu.h>

#ifdef __gl_glext_h_
#ifndef __glext_h_
#define __glext_h_ 1
#endif
#endif


#include <QGLWidget>
//#include <QList>
#include <QMap>
#include <QHash>
#include <QPoint>

#include "SourceCoordinates.h"

class QKeyEvent;
class QMouseEvent;
class QWheelEvent;
class QTimer;

class Source;
class SourceGroup;
class SourcePositionDialog;

//--------------------------------------------------------------------------------//

//Doxygen description
/*!
 *      \brief
 *      Interactive visualisation of sound sources in the reproduction room.
 *
 *      \details
 *      This is a three dimensional visualisation of sound sources in the reproduction room. Rotatable and
 *      scalable. It is interactive and lets the user click and drag sources intuitively. It utilizes OpenGL.
 *
 *      \author
 *      Hans-Joachim Mond
 *
 *      \date
 *      29.04.2008
 */

class Sources3DWidget: public QGLWidget {
    Q_OBJECT

public:
    Sources3DWidget(QWidget* parent = 0);
    ~Sources3DWidget();

    void reset();

    const Source& getSource(unsigned int xID) const;
    const Source& getSource(int sourceID) const;
    void          setSource(const Source& newSource);
    void          setGroup(const SourceGroup& newGroup);
    unsigned int  getCurrentXID() const;
    bool          xIDIsUsed(unsigned int xID);
    void          resetIDAfterIDChange(unsigned int xID, int oldID);
    void          swapAfterIDChange(unsigned int xID, int oldID, int newID);
    //void          setMouseSensitivity( float newSensitivity );

public slots:
    void init();
    void updateRoompolygon();

    void resetView();
    void lockView(bool lockIt);
    void lockClick(bool lockIt);
    //void showTimeTics(bool showThem);
    void showOrigin(bool showIt);
    //void showTimesliceConnections(bool showThem);
    void showPerspectiveView(bool showIt);
    //void showLighting(bool showIt);
    void showNames(bool showThem);
    void showCoordinates(bool showThem);


    void activateSource(unsigned int xID, int sourceID, bool externalActivation = false);
    void deactivateSource(unsigned int xID, bool externalDeactivation = false);
    void changeSourcePosition(int sourceID, float x, float y);
    void changeSourceOrientation(int sourceID, float orientation);
    void changeSourceVisible(unsigned int xID);
    void changeSourceID(unsigned int xID, int oldID, int newID);
    void changeSourceColor(unsigned int xID, const QColor& newColor);
    void changeSourceColor(int id, QColor newColor);
    void changeSourceType(unsigned int xID, bool planewave);
    void changeSourceType(int id, bool planewave);
    void changeSourceName(unsigned int xID, const QString& name);
    void changeSourceName(int id, const QString& name);
    void changeSourceRotationDirection(unsigned int xID, bool inverted);
    void changeSourceRotationDirection(int id, bool inverted);
    void changeSourceScalingDirection(unsigned int xID, bool inverted);
    void changeSourceScalingDirection(int id, bool inverted);
    void changeSourceRecordMode(unsigned int xID, bool recordEnabled);
    void changeSourceRecordMode(int id, bool recordEnabled, bool notUsed);
    void changeSourceReadMode(unsigned int xID, bool readEnabled);
    void changeSourceReadMode(int id, bool readEnabled, bool notUsed);
    void changeSourceGroupID(int id, int groupID);

    void activateGroup(int groupID = -1, bool externalActivation = false);
    void deactivateGroup(int groupID = -1, bool externalDeactivation = false);
    void changeGroupPosition(int groupID, float x, float y);
    void changeGroupColor(int groupID, const QColor& newColor, bool externalSet);

    void selectSource(unsigned int xID);
    void changeReadMode(bool readEnabled);

    //void setNoTimesclices(unsigned int noSlices, unsigned int timegridDistance);

signals:
    void sourceTypeChanged(unsigned int xID, bool planewave);
    void sourcePositionChanged(const Source& source);
    void sourceOrientationChanged(const Source& source);
    void sourceRecordModeChanged(unsigned int xID, bool recordEnabled);
    void sourceColorChanged(unsigned int xID, const QColor& newColor);
    void sourceColorChanged(int id, const QColor& newColor);
    void sourceGroupIDChanged(int id, int groupID);

    void groupActivated(int groupID);
    void groupDeactivated(int groupID);
    void groupPositionChanged(const SourceGroup& group);
    void groupColorChanged(int groupID, const QColor& newColor);

    void displaySourceCoordinates(const Source& source);
    void displayGroupCoordinates(const SourceGroup& group);
    void sliceSelected(bool haveSelection);
    void modified();

protected:
    void initializeGL();
    void resizeGL(int width, int heigth);
    void paintGL();
    void keyPressEvent(QKeyEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseMoveEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);

private slots:
    void changeSourceCoordinates();

private:
    void project();
    //void setSliceDist(GLfloat inc);
    void changeSourceLod(GLint inc);
    void changeSourceSize(GLfloat inc);
    void genRoomList();
    void genCubeList();

    void genSphereList();
    // auxilliary functions for sphere creation
    void drawSphere(int ndiv, float radius = 1.0);
    void drawTriangle(GLfloat* a, GLfloat* b, GLfloat* c, int div, float r);
    void normalize(GLfloat* a);

    void genPlanewaveSourceList();
    void update(bool sourcePositionChanged, bool sourceOrientationChanged, bool groupChange = false, bool groupPositionChanged = false);
    void displaySourceContextDialog();
    void displayGroupContexDialog(int groupID = 0);

    QPoint lastMousePos;
    QPoint eventStartPos;
    //bool localCursorSetEvent;
    SourceCoordinates eventStartSourceCoords;

    GLUquadric* quad;

    SourcePositionDialog* sourceDialog;

    // selectionMode, to move sources
    bool selectionModeOn;

    // perspective or orthographic projection
    bool perspectiveOn;

    // roomview changeable on/off
    bool viewLockOn;

    // lock mouse to selected source/group until next click
    bool clickLockOn;

    //// light-highlights on/off
    //bool highlightsOn;

    //// connection between source on/off
    //bool linesOn;

    // show roomorigin on/off
    bool originOn;

    //// render pointsources as cubes or spheres
    //bool drawSpheresOn;

    //// show point in time of every timeslice
    //bool drawTimeTics;

    // draw the names of the sources
    bool drawNames;

    // draw the coordinates of the sources
    bool drawCoordinates;

    // flag for global readMode in scoreplayer
    bool readOn;

    // this is a flag for the mouseMoveEvent-handler
    // QCursor:setPos() triggers a mouseMoveEvent
    // which we do not want to process, because
    // it makes everything "jump" from it's location
    bool ignoreNextMouseMove;

    // drawLists
    GLuint roomList;
    GLuint cubeList;
    GLuint sphereList;
    GLuint planewaveSourceList;

    //View- und projection-transformations
    GLfloat xrotation;
    GLfloat yrotation;
    GLfloat zrotation;
    GLfloat xposition;
    GLfloat yposition;
    GLfloat ypositionReset;
    GLfloat moveInc;
    GLfloat zoom;
    GLfloat zoomReset;
    GLfloat orthoBounds;
    GLfloat rotInc;
    GLfloat windowWidth;
    GLfloat windowHeight;

    //number of (no) sources, timeslices and sourcegroups. timeslicedistance in time and visualization
    //unsigned int noSources;
    //unsigned int noSlices;
    //unsigned int noGroups;
    //GLfloat sliceDist;
    //GLfloat minSliceDist;
    //unsigned int timegridDistance;

    //Id of current source, current timeslice, current sourcegroup (0 = no selection)
    unsigned int currSource; // xID
    unsigned int currSlice;
    int          currGroup;  // groupID
    GLfloat soSources;
    GLfloat soSourcesInc;
    GLfloat maxSoSources;
    GLfloat minSoSources;

    //"Level of Detail" of sources, less is cpu-friendlier
    GLint lodSources;
    GLint lodSourcesInc;
    GLint maxLodSources;
    GLint minLodSources;

    // this is for future use, when scoresegments can be edited offline
    // for now take the simpler approach with only the current timeslice
    // and hash via xIDs
    ////List with pointers to lists, which contain pointers to objects of class Source
    //typedef QList<Source*> timesliceList; //one source over time
    //QList<timesliceList*> sources;

    // keys are the xIDs
    QHash< unsigned int, Source* > sources;

    //Map of sourcegroups (which contain lists of xIDs of sources), keys are the group IDs
    QMap< int, SourceGroup* > groups;

    //Timer for constant framerate
    QTimer* timer;

    int noActiveGroups;

};

#endif //SOURCES3DWIDGET_H
