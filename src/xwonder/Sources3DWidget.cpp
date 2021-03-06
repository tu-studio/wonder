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

// this is a bloody hack, because Mac OS X does not handle usage of QCursor:setPos
// very well, so in order to make the infinite movement of selected sources work,
// a call to CGAssociateMouseAndMouseCursorPosition( true ) is needed after
// programmatically setting the cursor position.
#ifdef __APPLE__
#include <ApplicationServices.h>
#endif

#include <QDebug>
#include <QKeyEvent>
#include <QMenu>
#include <QMouseEvent>
#include <QPointF>
#include <QTimer>
#include <QToolTip>
#include <QVector>
#include <QWheelEvent>
#include <cmath>

#include "Colors.h"
#include "Source.h"
#include "SourceGroup.h"
#include "SourcePositionDialog.h"
#include "Sources3DWidget.h"
#include "XwonderConfig.h"

// Constructor with lots of defaultvalues
Sources3DWidget::Sources3DWidget(QWidget* parent)
    : QGLWidget(parent)
    , selectionModeOn(false)
    , perspectiveOn(true)
    , viewLockOn(true)
    , clickLockOn(false)
    ,
    // highlightsOn( false ),
    // linesOn( true ),
    originOn(false)
    ,
    // drawTimeTics( false ),
    drawNames(true)
    , drawCoordinates(false)
    , readOn(true)
    , ignoreNextMouseMove(false)
    , roomList(0)
    , cubeList(0)
    , xrotation(0.0)
    , yrotation(0.0)
    , zrotation(0.0)
    , xposition(0.0)
    , yposition(0.0)
    , ypositionReset(0.0)
    , moveInc(0.2)
    , zoom(-30.0)
    , zoomReset(-30.0)
    , orthoBounds(15.0)
    , rotInc(5.0)
    , windowWidth(500.0)
    , windowHeight(500.0)
    ,
    // noSources( 0 ),
    // noSlices( 0 ), noGroups( 0 ),
    // sliceDist( 3.0 ), minSliceDist( 1.0 ),
    currSource(0)
    , currSlice(0)
    , currGroup(0)
    , soSources(0.3)
    , soSourcesInc(0.01)
    , maxSoSources(1.0)
    , minSoSources(0.01)
    , lodSources(10)
    , lodSourcesInc(2)
    , maxLodSources(50)
    , minLodSources(4)
    , noActiveGroups(0)

{
    setFormat(QGLFormat(QGL::DoubleBuffer | QGL::DepthBuffer | QGL::Rgba
                        | QGL::DirectRendering));

    quad = gluNewQuadric();

    setFocusPolicy(Qt::StrongFocus);

    // timer for constant framerate
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(updateGL()));
    // timer->start(100);    // timeout after   100 ms => 10 fps
    // timer->start(50);     // timeout after    50 ms => 20 fps
    timer->start(16);  // timeout after 16.67 ms => 60 fps
                       // explicit conversion to int = 16
}

Sources3DWidget::~Sources3DWidget() {
    if (quad) {
        gluDeleteQuadric(quad);
        quad = nullptr;
    }

    // TODO:check if this is really nescessary since Sources are children of "this"
    while (!sources.empty()) {
        QHash<unsigned int, Source*>::iterator it = sources.begin();
        sources.remove((*it)->getXID());
        delete (*it);
    }

    while (!groups.empty()) {
        QMap<int, SourceGroup*>::iterator it = groups.begin();
        groups.remove((*it)->getGroupID());
        delete (*it);
    }
}

void Sources3DWidget::init() {
    // do nothing if maximum number of allowed sources has been reached
    if (!(sources.size() < xwConf->maxNoSources)) { return; }

    // do nothing if maximum number of allowed groups has been reached
    if (!(groups.size() < xwConf->maxNoSources)) { return; }

    // add maximum number of sources allowed (and groups)
    for (int i = 1; i <= xwConf->maxNoSources; ++i) {
        Source* tempSource = new Source(i, i - 1, this);
        sources.insert(i, tempSource);
        SourceGroup* tempGroup = new SourceGroup(i, this);
        groups.insert(i, tempGroup);
    }
}

void Sources3DWidget::reset() {
    resetView();

    selectionModeOn = false;
    perspectiveOn   = true;
    viewLockOn      = true;
    clickLockOn     = false;
    originOn        = false;
    drawNames       = true;
    drawCoordinates = false;
    readOn          = true;

    currSource     = 0;
    currSlice      = 0;
    currGroup      = 0;
    noActiveGroups = 0;

    // set all sources and groups to their default values
    for (int i = 1; i <= xwConf->maxNoSources; ++i) {
        sources.value(i)->reset();
        groups.value(i)->reset();
    }
}

bool Sources3DWidget::xIDIsUsed(unsigned int xID) { return sources.contains(xID); }

void Sources3DWidget::resetIDAfterIDChange(unsigned int xID, int oldID) {
    if (!sources.contains(xID)) {
        return;
    } else {
        sources.value(xID)->setID(oldID);
    }
}

void Sources3DWidget::swapAfterIDChange(unsigned int xID, int oldID, int newID) {
    if (!sources.contains(xID)) {
        return;
    } else {
        QHash<unsigned int, Source*>::iterator sourcesIter;

        for (sourcesIter = sources.begin(); sourcesIter != sources.end(); ++sourcesIter)
            if ((sourcesIter.value()->getID() == newID) && sourcesIter.key() != xID) {
                sourcesIter.value()->setID(oldID);
            }
    }
}

const Source& Sources3DWidget::getSource(unsigned int xID) const {
    if (sources.contains(xID)) {
        return *(sources.value(xID));
    } else {
        return *(*(sources.constBegin()));
    }
}

const Source& Sources3DWidget::getSource(int sourceID) const {
    QHash<unsigned int, Source*>::const_iterator sourcesIter;

    for (sourcesIter = sources.begin(); sourcesIter != sources.end(); ++sourcesIter) {
        if (sourcesIter.value()->getID() == sourceID) { return *(sourcesIter.value()); }
    }

    // this should never happen
    return *(sources.begin().value());
}

void Sources3DWidget::activateSource(unsigned int xID, int sourceID,
                                     bool externalActivation) {
    // do nothing if xID is invalid or already active
    if ((!sources.contains(xID)) || sources.value(xID)->isActive()) { return; }

    if (!externalActivation) {
        Source* temp = sources.value(xID);
        temp->setID(sourceID);
        // new startposition
        int rowLength = 5;
        temp->setx(GLfloat(Source::getNoActiveSources() / rowLength));
        temp->sety(-1.0 * GLfloat((Source::getNoActiveSources() % rowLength)));

        // send OSC-Messages about new
        emit sourcePositionChanged(*temp);
        emit sourceOrientationChanged(*temp);
    }

    sources.value(xID)->activate();
}

void Sources3DWidget::deactivateSource(unsigned int xID, bool externalDeactivation) {
    if (sources.contains(xID) && sources.value(xID)->isActive()) {
        // unset currSource
        currSource = 0;

        // remove source from group, if it is member of a group
        if (sources.value(xID)->getGroupID() != 0) {
            int groupID = sources.value(xID)->getGroupID();

            if (groups.contains(groupID)) { groups.value(groupID)->deleteSource(xID); }
        }

        sources.value(xID)->deactivate();
    }
}

void Sources3DWidget::setSource(const Source& newSource) {
    foreach (Source* source, sources) {
        if (source->getID() == newSource.getID()) {
            source->activate();
            source->setName(newSource.getName());
            source->setType(newSource.isPlanewave());
            source->setCoordinates(newSource.getCoordinates());
            source->setColor(newSource.getColor());
            source->setGroupID(newSource.getGroupID());

            source->setRotationDirection(newSource.hasInvertedRotation());
            source->setScalingDirection(newSource.hasInvertedScaling());

            // add source to group it belongs to
            if (groups.contains(source->getGroupID())) {
                groups[source->getGroupID()]->addSource(source->getXID());
            }

            break;
        }
    }
}

void Sources3DWidget::setGroup(const SourceGroup& newGroup) {
    if (groups.contains(newGroup.getGroupID())) {
        SourceGroup* temp = groups.value(newGroup.getGroupID());

        temp->setCoordinates(newGroup.getCoordinates());
        temp->setColor(newGroup.getColor());
        activateGroup(temp->getGroupID(), true);
    }
}

unsigned int Sources3DWidget::getCurrentXID() const {
    if (currSource) {
        return currSource;
    } else {
        return 0;
    }
}

void Sources3DWidget::changeSourcePosition(int sourceID, float x, float y) {
    QHash<unsigned int, Source*>::iterator it;

    for (it = sources.begin(); it != sources.end(); ++it) {
        if ((*it)->getID() == sourceID && (*it)->isActive()) {
            SourceCoordinates coords(x, y, 0.0);
            coords             = coords.mapWonderCoordToGLCoord();
            coords.orientation = (*it)->getOrientation();
            (*it)->setCoordinates(coords);
            break;
        }
    }
}

void Sources3DWidget::changeSourceOrientation(int sourceID, float orientation) {
    QHash<unsigned int, Source*>::iterator it;

    for (it = sources.begin(); it != sources.end(); ++it) {
        if ((*it)->getID() == sourceID && (*it)->isActive()) {
            SourceCoordinates coords((*it)->getCoordinates());
            coords             = coords.mapGLCoordToWonderCoord();
            coords.orientation = orientation;
            coords             = coords.mapWonderCoordToGLCoord();
            (*it)->setCoordinates(coords);
            break;
        }
    }
}

void Sources3DWidget::changeSourceColor(unsigned int xID, const QColor& newColor) {
    GLfloat color[4];
    color[0] = newColor.redF();
    color[1] = newColor.greenF();
    color[2] = newColor.blueF();
    color[3] = newColor.alphaF();

    if (sources.value(xID)->isActive()) {
        // change color of source
        sources.value(xID)->setColor(color);

        // change color of groupobject if source is member of a group and the group is
        // active then emit signal to change color for all other groupmembers
        int groupID = sources.value(xID)->getGroupID();

        if (groupID != 0) {
            if (groups.contains(groupID) && groups.value(groupID)->isActive()) {
                groups.value(groupID)->setColor(color);
                emit groupColorChanged(groupID, newColor);

                QList<unsigned int> sourceXIDs = groups.value(groupID)->getXIDs();

                QList<unsigned int>::iterator it;

                for (it = sourceXIDs.begin(); it != sourceXIDs.end(); ++it) {
                    if (sources.contains(*it) && sources.value(*it)->isActive()) {
                        sources.value(*it)->setColor(color);
                        emit sourceColorChanged(*it, newColor);  // for ChannelsWidget

                        if (*it != xID) {
                            emit sourceColorChanged(sources.value(*it)->getID(),
                                                    newColor);  // for cwonder
                        }
                    }
                }
            }
        }
    }
}

void Sources3DWidget::changeSourceColor(int sourceID, QColor newColor) {
    // this is for external color changes, groups are not affected by this
    QHash<unsigned int, Source*>::iterator it;

    for (it = sources.begin(); it != sources.end(); ++it) {
        if (it.value()->getID() == sourceID && it.value()->isActive()) {
            it.value()->setColor(newColor);
            break;
        }
    }
}

void Sources3DWidget::changeSourceVisible(unsigned int xID) {
    if (sources.value(xID)->isActive()) { sources.value(xID)->toggleVisible(); }
}

void Sources3DWidget::changeSourceRotationDirection(unsigned int xID, bool inverted) {
    if (sources.value(xID)->isActive()) {
        sources.value(xID)->setRotationDirection(inverted);
    }
}

void Sources3DWidget::changeSourceRotationDirection(int sourceID, bool inverted) {
    QHash<unsigned int, Source*>::iterator it;

    for (it = sources.begin(); it != sources.end(); ++it) {
        if (it.value()->getID() == sourceID && it.value()->isActive()) {
            it.value()->setRotationDirection(inverted);
            break;
        }
    }
}

void Sources3DWidget::changeSourceScalingDirection(unsigned int xID, bool inverted) {
    if (sources.value(xID)->isActive()) {
        sources.value(xID)->setScalingDirection(inverted);
    }
}

void Sources3DWidget::changeSourceScalingDirection(int sourceID, bool inverted) {
    QHash<unsigned int, Source*>::iterator it;

    for (it = sources.begin(); it != sources.end(); ++it) {
        if (it.value()->getID() == sourceID && it.value()->isActive()) {
            it.value()->setScalingDirection(inverted);
            break;
        }
    }
}

void Sources3DWidget::changeSourceType(unsigned int xID, bool planewave) {
    if (sources.value(xID)->isActive()) { sources.value(xID)->setType(planewave); }
}

void Sources3DWidget::changeSourceType(int sourceID, bool planewave) {
    QHash<unsigned int, Source*>::iterator it;

    for (it = sources.begin(); it != sources.end(); ++it)
        if ((*it)->getID() == sourceID && (*it)->isActive()) {
            (*it)->setType(planewave);
        }
}

void Sources3DWidget::changeSourceID(unsigned int xID, int oldID, int newID) {
    if (sources.value(xID)->isActive()) { sources.value(xID)->setID(newID); }
}

void Sources3DWidget::changeSourceName(unsigned int xID, const QString& name) {
    if (sources.value(xID)->isActive()) { sources.value(xID)->setName(name); }
}

void Sources3DWidget::changeSourceName(int sourceID, const QString& name) {
    QHash<unsigned int, Source*>::iterator it;

    for (it = sources.begin(); it != sources.end(); ++it) {
        if ((*it)->getID() == sourceID && (*it)->isActive()) { (*it)->setName(name); }
    }
}

void Sources3DWidget::changeReadMode(bool readEnabled) {
    if (xwConf->projectOnlyMode) { return; }

    readOn = readEnabled;
}

void Sources3DWidget::changeSourceRecordMode(unsigned int xID, bool recordEnabled) {
    if (xwConf->projectOnlyMode) { return; }

    if (sources.value(xID)->isActive()) {
        sources.value(xID)->setRecordMode(recordEnabled);
    }
}

void Sources3DWidget::changeSourceRecordMode(int sourceID, bool recordEnabled,
                                             bool notUsed) {
    if (xwConf->projectOnlyMode) { return; }

    QHash<unsigned int, Source*>::iterator it;

    for (it = sources.begin(); it != sources.end(); ++it) {
        if (it.value()->getID() == sourceID && it.value()->isActive()) {
            it.value()->setRecordMode(recordEnabled);
            break;
        }
    }
}

void Sources3DWidget::changeSourceReadMode(unsigned int xID, bool readEnabled) {
    if (xwConf->projectOnlyMode) { return; }

    if (sources.value(xID)->isActive()) { sources.value(xID)->setReadMode(readEnabled); }
}

void Sources3DWidget::changeSourceReadMode(int sourceID, bool readEnabled, bool notUsed) {
    if (xwConf->projectOnlyMode) { return; }

    QHash<unsigned int, Source*>::iterator it;

    for (it = sources.begin(); it != sources.end(); ++it) {
        if (it.value()->getID() == sourceID) {
            it.value()->setReadMode(readEnabled);
            break;
        }
    }
}

void Sources3DWidget::changeSourceGroupID(int sourceID, int groupID) {
    int previousGroupID = 0;
    unsigned int xID    = 0;

    // set groupID of source
    QHash<unsigned int, Source*>::iterator it;

    for (it = sources.begin(); it != sources.end(); ++it) {
        if (it.value()->getID() == sourceID) {
            previousGroupID = it.value()->getGroupID();
            xID             = it.value()->getXID();
            it.value()->setGroupID(groupID);
            break;
        }
    }

    // if such a group does exists, add source to this group
    // and cancel membership to previous group
    if (previousGroupID > 0)
        if (groups.contains(previousGroupID)) {
            groups[previousGroupID]->deleteSource(xID);
        }

    if (groups.contains(groupID) && groups.value(groupID)->isActive()) {
        groups[groupID]->addSource(xID);
    }
}

void Sources3DWidget::activateGroup(int groupID, bool externalActivation) {
    if (groups.contains(groupID) && groups.value(groupID)->isActive()) { return; }

    foreach (SourceGroup* temp, groups) {
        if ((groupID == -1 && !temp->isActive())
            || (groups.contains(groupID) && temp->getGroupID() == groupID)) {
            temp->activate();
            noActiveGroups++;
            groupID = temp->getGroupID();
            break;
        }
    }

    // check all sources for matching groupID and add them
    foreach (Source* source, sources) {
        if (source->getGroupID() == groupID) {
            groups.value(groupID)->addSource(source->getXID());
        }
    }

    if (!externalActivation) {
        groups.value(groupID)->setColor(colors[groupID % noColors]);
        const GLfloat* color = groups.value(groupID)->getColor();

        emit groupActivated(groupID);
        emit groupColorChanged(
            groupID,
            QColor((int)(color[0] * 255), (int)(color[1] * 255), (int)(color[2] * 255)));
    }
}

void Sources3DWidget::deactivateGroup(int groupID, bool externalDeactivation) {
    if (groupID == -1) { groupID = currGroup; }

    if (currGroup == groupID) { currGroup = 0; }

    if (groups.contains(groupID) && groups.value(groupID)->isActive()) {
        groups.value(groupID)->deactivate();
        noActiveGroups--;
    }

    if (!externalDeactivation) { emit groupDeactivated(groupID); }
}

void Sources3DWidget::changeGroupPosition(int groupID, float x, float y) {
    SourceCoordinates coords(x, y, 0.0);
    coords = coords.mapWonderCoordToGLCoord();

    if (groups.contains(groupID) && groups.value(groupID)->isActive()) {
        groups.value(groupID)->setPosition(coords.x, coords.y);
    }
}

void Sources3DWidget::changeGroupColor(int groupID, const QColor& newColor,
                                       bool externalSet) {
    GLfloat color[4];
    color[0] = newColor.redF();
    color[1] = newColor.greenF();
    color[2] = newColor.blueF();
    color[3] = newColor.alphaF();

    if (groups.contains(groupID) && groups.value(groupID)->isActive()) {
        groups.value(groupID)->setColor(color);
    }
}

void Sources3DWidget::selectSource(unsigned int xID) {
    if (sources.contains(xID)) {
        currGroup  = 0;
        currSource = xID;
        currSlice  = 1;
    }
}

// void Sources3DWidget::setNoTimesclices(unsigned int noSlices, unsigned int
// timegridDistance)
//{
//    this->noSlices = noSlices;
//    this->timegridDistance = timegridDistance;
//}

// int Sources3DWidget::getNoTimeSlices()
//{
//      return noSlices;
//}
//

void Sources3DWidget::updateRoompolygon() {
    initializeGL();
    resetView();
}

void Sources3DWidget::resetView() {
    xrotation = yrotation = zrotation = 0.0;
    xposition                         = 0.0;
    yposition                         = ypositionReset;
    zoom                              = zoomReset;  //-30.0; // - noSlices;
    orthoBounds                       = 15.0;
}

void Sources3DWidget::lockView(bool lockIt) { viewLockOn = lockIt; }

void Sources3DWidget::lockClick(bool lockIt) { clickLockOn = lockIt; }

// void Sources3DWidget::showTimeTics( bool showThem )
//{
//    drawTimeTics = showThem;
//}

void Sources3DWidget::showOrigin(bool showIt) { originOn = showIt; }

// void Sources3DWidget::showTimesliceConnections( bool showThem )
//{
//    linesOn = showThem;
//}

void Sources3DWidget::showPerspectiveView(bool showIt) {
    perspectiveOn = showIt;
    project();
}

// void Sources3DWidget::showLighting( bool showIt )
//{
//    if( showIt )
//        glEnable( GL_LIGHTING );
//    else
//        glDisable( GL_LIGHTING );
//}

void Sources3DWidget::showNames(bool showThem) { drawNames = showThem; }

void Sources3DWidget::showCoordinates(bool showThem) { drawCoordinates = showThem; }

// void Sources3DWidget::setSliceDist( GLfloat inc )
//{
//    sliceDist += inc;
//    if( sliceDist < minSliceDist )
//      sliceDist = minSliceDist;
//}

void Sources3DWidget::changeSourceLod(GLint inc) {
    lodSources += inc;

    if (lodSources > maxLodSources) {
        lodSources = maxLodSources;
    } else if (lodSources < minLodSources) {
        lodSources = minLodSources;
    }
}

void Sources3DWidget::changeSourceSize(GLfloat inc) {
    soSources += inc;

    if (soSources > maxSoSources) {
        soSources = maxSoSources;
    } else if (soSources < minSoSources) {
        soSources = minSoSources;
    }

    genCubeList();
    genSphereList();
    genPlanewaveSourceList();
}

void Sources3DWidget::genRoomList() {
    if (glIsList(roomList)) { glDeleteLists(roomList, 1); }

    // displaylist for the room
    // if running in demomode use defaultroom, otherwise cwonder will provide a
    // roompolygon
    roomList = glGenLists(1);
    glNewList(roomList, GL_COMPILE);
    glBegin(GL_LINE_STRIP);

    if (xwConf->runWithoutCwonder) {
        // standardroom 20mx20m
        glVertex2f(-10.0, 0.0);
        glVertex2f(-10.0, -20.0);
        glVertex2f(10.0, -20.0);
        glVertex2f(10.0, 0.0);
        glVertex2f(-10.0, 0.0);
        ypositionReset = 10.0;
        zoomReset      = -30.0;
        soSources      = 0.3;
        resetView();

        // TU Berlin H104
        // H104 is NOT rectangular, but square+trapezoid
        // glVertex2f(  10.0, -12.85);
        // glVertex2f(  10.0,   4.05);
        // glVertex2f(   7.7,  12.85);
        // glVertex2f(  -7.7,  12.85);
        // glVertex2f( -10.0,   4.05);
        // glVertex2f( -10.0, -12.85);
        // glVertex2f(  10.0, -12.85);
        // ypositionReset = ;
    } else {
        // iterate over roomPoints and add to displaylist ( use "-" for y because of
        // Wonder's awkward coordinate system ) because we draw a LINE_STRIP use the first
        // Vertex as the last one as well
        QVector<QPointF>::const_iterator it;
        GLfloat maxYValue = 0.0;

        for (it = xwConf->roomPoints.constBegin(); it != xwConf->roomPoints.constEnd();
             ++it) {
            glVertex2f((GLfloat)(*it).x(), (GLfloat) - (*it).y());

            if ((*it).y() > maxYValue) { maxYValue = (*it).y(); }
        }

        ypositionReset = maxYValue / 2.0;
        zoomReset      = ypositionReset * (-3.0);
        soSources      = maxYValue / 60.0;
        // close the polygon
        glVertex2f((GLfloat)(xwConf->roomPoints.begin()->x()),
                   (GLfloat) - (xwConf->roomPoints.begin()->y()));
    }

    glEnd();
    glEndList();
}

void Sources3DWidget::genCubeList() {
    if (glIsList(cubeList)) { glDeleteLists(cubeList, 1); }

    GLfloat a     = soSources;
    cubeList      = glGenLists(1);
    GLfloat norm  = sqrt(3 * a * a);
    GLfloat anorm = a / norm;

    glNewList(cubeList, GL_COMPILE);
    glBegin(GL_QUADS);
    glNormal3f(-anorm, -anorm, anorm);
    glVertex3f(-a, -a, a);
    glNormal3f(anorm, -anorm, anorm);
    glVertex3f(a, -a, a);
    glNormal3f(anorm, anorm, anorm);
    glVertex3f(a, a, a);
    glNormal3f(-anorm, anorm, anorm);
    glVertex3f(-a, a, a);
    glNormal3f(anorm, -anorm, anorm);
    glVertex3f(a, -a, a);
    glNormal3f(anorm, -anorm, -anorm);
    glVertex3f(a, -a, -a);
    glNormal3f(anorm, anorm, -anorm);
    glVertex3f(a, a, -a);
    glNormal3f(anorm, anorm, anorm);
    glVertex3f(a, a, a);
    glNormal3f(anorm, -anorm, -anorm);
    glVertex3f(a, -a, -a);
    glNormal3f(-anorm, -anorm, -anorm);
    glVertex3f(-a, -a, -a);
    glNormal3f(-anorm, anorm, -anorm);
    glVertex3f(-a, a, -a);
    glNormal3f(anorm, anorm, -anorm);
    glVertex3f(a, a, -a);
    glNormal3f(-anorm, -anorm, -anorm);
    glVertex3f(-a, -a, -a);
    glNormal3f(-anorm, -anorm, anorm);
    glVertex3f(-a, -a, a);
    glNormal3f(-anorm, anorm, anorm);
    glVertex3f(-a, a, a);
    glNormal3f(-anorm, anorm, -anorm);
    glVertex3f(-a, a, -a);
    glNormal3f(-anorm, anorm, anorm);
    glVertex3f(-a, a, a);
    glNormal3f(anorm, anorm, anorm);
    glVertex3f(a, a, a);
    glNormal3f(anorm, anorm, -anorm);
    glVertex3f(a, a, -a);
    glNormal3f(-anorm, anorm, -anorm);
    glVertex3f(-a, a, -a);
    glNormal3f(-anorm, -anorm, anorm);
    glVertex3f(-a, -a, a);
    glNormal3f(anorm, -anorm, anorm);
    glVertex3f(a, -a, a);
    glNormal3f(anorm, -anorm, -anorm);
    glVertex3f(a, -a, -a);
    glNormal3f(-anorm, -anorm, -anorm);
    glVertex3f(-a, -a, -a);
    glEnd();
    glEndList();
}

void Sources3DWidget::genSphereList() {
    if (glIsList(sphereList)) { glDeleteLists(sphereList, 1); }

    sphereList = glGenLists(1);

    glNewList(sphereList, GL_COMPILE);
    drawSphere(1, soSources);
    glEndList();
}

void Sources3DWidget::normalize(GLfloat* a) {
    GLfloat d = sqrt(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);
    a[0] /= d;
    a[1] /= d;
    a[2] /= d;
}

void Sources3DWidget::drawTriangle(GLfloat* a, GLfloat* b, GLfloat* c, int div, float r) {
    if (div <= 0) {
        glNormal3fv(a);
        glVertex3f(a[0] * r, a[1] * r, a[2] * r);
        glNormal3fv(b);
        glVertex3f(b[0] * r, b[1] * r, b[2] * r);
        glNormal3fv(c);
        glVertex3f(c[0] * r, c[1] * r, c[2] * r);
    } else {
        GLfloat ab[3];
        GLfloat ac[3];
        GLfloat bc[3];

        for (int i = 0; i < 3; ++i) {
            ab[i] = (a[i] + b[i]) / 2;
            ac[i] = (a[i] + c[i]) / 2;
            bc[i] = (b[i] + c[i]) / 2;
        }

        normalize(ab);
        normalize(ac);
        normalize(bc);
        drawTriangle(a, ab, ac, div - 1, r);
        drawTriangle(b, bc, ab, div - 1, r);
        drawTriangle(c, ac, bc, div - 1, r);
        drawTriangle(ab, bc, ac, div - 1, r);
    }
}

void Sources3DWidget::drawSphere(int ndiv, float radius) {
    static const GLfloat X = 0.525731112119133606;
    static const GLfloat Z = 0.850650808352039932;

    static GLfloat vdata[12][3] = {{-X, 0.0, Z}, {X, 0.0, Z},   {-X, 0.0, -Z},
                                   {X, 0.0, -Z}, {0.0, Z, X},   {0.0, Z, -X},
                                   {0.0, -Z, X}, {0.0, -Z, -X}, {Z, X, 0.0},
                                   {-Z, X, 0.0}, {Z, -X, 0.0},  {-Z, -X, 0.0}};

    static GLuint tindices[20][3] = {
        {0, 4, 1}, {0, 9, 4},  {9, 5, 4},  {4, 5, 8},  {4, 8, 1},  {8, 10, 1}, {8, 3, 10},
        {5, 3, 8}, {5, 2, 3},  {2, 7, 3},  {7, 10, 3}, {7, 6, 10}, {7, 11, 6}, {11, 0, 6},
        {0, 1, 6}, {6, 1, 10}, {9, 0, 11}, {9, 11, 2}, {9, 2, 5},  {7, 2, 11}};

    glBegin(GL_TRIANGLES);

    for (auto& tindice : tindices) {
        drawTriangle(vdata[tindice[0]], vdata[tindice[1]], vdata[tindice[2]], ndiv,
                     radius);
    }

    glEnd();
}

void Sources3DWidget::genPlanewaveSourceList() {
    // orientation is 0.0, which means arrow points to the right along the x-axis
    if (glIsList(planewaveSourceList)) { glDeleteLists(planewaveSourceList, 1); }

    GLfloat a     = soSources;
    GLfloat b     = soSources / 3.0;
    GLfloat norm  = sqrt(2 * a * a + b * b);
    GLfloat anorm = a / norm;
    GLfloat bnorm = b / norm;

    GLfloat a2         = a * 2;
    GLfloat a3         = a * 3;
    GLfloat aThird     = a / 3;
    GLfloat norm2      = sqrt(2 * aThird * aThird + a2 * a2);
    GLfloat a2norm     = a2 / norm2;
    GLfloat aThirdnorm = aThird / norm2;

    planewaveSourceList = glGenLists(1);
    glNewList(planewaveSourceList, GL_COMPILE);
    glPushMatrix();
    glRotatef(-90.0, 0.0, 0.0, 1.0);
    glBegin(GL_QUADS);  // base
    glNormal3f(-anorm, -bnorm, anorm);
    glVertex3f(-a, -b, a);
    glNormal3f(anorm, -bnorm, anorm);
    glVertex3f(a, -b, a);
    glNormal3f(anorm, bnorm, anorm);
    glVertex3f(a, b, a);
    glNormal3f(-anorm, bnorm, anorm);
    glVertex3f(-a, b, a);
    glNormal3f(anorm, -bnorm, anorm);
    glVertex3f(a, -b, a);
    glNormal3f(anorm, -bnorm, -anorm);
    glVertex3f(a, -b, -a);
    glNormal3f(anorm, bnorm, -anorm);
    glVertex3f(a, b, -a);
    glNormal3f(anorm, bnorm, anorm);
    glVertex3f(a, b, a);
    glNormal3f(anorm, -bnorm, -anorm);
    glVertex3f(a, -b, -a);
    glNormal3f(-anorm, -bnorm, -anorm);
    glVertex3f(-a, -b, -a);
    glNormal3f(-anorm, bnorm, -anorm);
    glVertex3f(-a, b, -a);
    glNormal3f(anorm, bnorm, -anorm);
    glVertex3f(a, b, -a);
    glNormal3f(-anorm, -bnorm, -anorm);
    glVertex3f(-a, -b, -a);
    glNormal3f(-anorm, -bnorm, anorm);
    glVertex3f(-a, -b, a);
    glNormal3f(-anorm, bnorm, anorm);
    glVertex3f(-a, b, a);
    glNormal3f(-anorm, bnorm, -anorm);
    glVertex3f(-a, b, -a);
    glNormal3f(-anorm, bnorm, anorm);
    glVertex3f(-a, b, a);
    glNormal3f(anorm, bnorm, anorm);
    glVertex3f(a, b, a);
    glNormal3f(anorm, bnorm, -anorm);
    glVertex3f(a, b, -a);
    glNormal3f(-anorm, bnorm, -anorm);
    glVertex3f(-a, b, -a);
    glNormal3f(-anorm, -bnorm, anorm);
    glVertex3f(-a, -b, a);
    glNormal3f(anorm, -bnorm, anorm);
    glVertex3f(a, -b, a);
    glNormal3f(anorm, -bnorm, -anorm);
    glVertex3f(a, -b, -a);
    glNormal3f(-anorm, -bnorm, -anorm);
    glVertex3f(-a, -b, -a);
    glEnd();
    glBegin(GL_LINES);  // arrow
    glNormal3f(0.0, 0.0, 0.0);
    glVertex3f(0.0, 0.0, 0.0);
    glNormal3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, a3, 0.0);
    glNormal3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, a3, 0.0);
    glNormal3f(aThirdnorm, a2norm, aThirdnorm);
    glVertex3f(aThird, a2, aThird);
    glNormal3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, a3, 0.0);
    glNormal3f(aThirdnorm, a2norm, -aThirdnorm);
    glVertex3f(aThird, a2, -aThird);
    glNormal3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, a3, 0.0);
    glNormal3f(-aThirdnorm, a2norm, -aThirdnorm);
    glVertex3f(-aThird, a2, -aThird);
    glNormal3f(0.0, 1.0, 0.0);
    glVertex3f(0.0, a3, 0.0);
    glNormal3f(-aThirdnorm, a2norm, aThirdnorm);
    glVertex3f(-aThird, a2, aThird);
    glEnd();
    glPopMatrix();
    glEndList();
}

void Sources3DWidget::update(bool srcPositionChanged, bool srcOrientationChanged,
                             bool grpChange, bool grpPositionChanged) {
    if (grpChange && currGroup) {
        if (groups.contains(currGroup) && groups.value(currGroup)->isActive()) {
            SourceGroup* tempGroup = groups.value(currGroup);

            if (grpPositionChanged) {
                emit groupPositionChanged(*tempGroup);
                emit displayGroupCoordinates(*tempGroup);
            }

            if (srcPositionChanged) {
                QList<unsigned int> sourceXIDs = tempGroup->getXIDs();
                QList<unsigned int>::iterator it;

                for (it = sourceXIDs.begin(); it != sourceXIDs.end(); ++it) {
                    if (sources.contains(*it)) {
                        Source* temp = sources.value(*it);
                        emit sourcePositionChanged(*temp);
                    }
                }
            }
        }
    } else if (currSource > 0 && currSlice > 0) {
        if (sources.contains(currSource)) {
            Source* temp = sources.value(currSource);

            if (srcPositionChanged) { emit sourcePositionChanged(*temp); }

            if (srcOrientationChanged) { emit sourceOrientationChanged(*temp); }

            emit displaySourceCoordinates(*temp);
        }
    }
}

void Sources3DWidget::displaySourceContextDialog() {
    if (sources.contains(currSource) && sources.value(currSource)->isActive()) {
        Source* temp = sources.value(currSource);
        sourceDialog = new SourcePositionDialog(*temp, this);
        connect(sourceDialog, SIGNAL(accepted()), this, SLOT(changeSourceCoordinates()));
        sourceDialog->exec();
    }
}

void Sources3DWidget::displayGroupContexDialog(int groupID) {
    QMenu menu(this);

    if (!(noActiveGroups == groups.size())) {
        QAction* activateGroupAct = menu.addAction("Add Group");
        connect(activateGroupAct, SIGNAL(triggered()), this, SLOT(activateGroup()));
    }

    if (groupID != 0) {
        QAction* deactivateGroupAct = menu.addAction("Delete this Group");
        connect(deactivateGroupAct, SIGNAL(triggered()), this, SLOT(deactivateGroup()));
    }

    if (!menu.isEmpty()) { menu.exec(mapToGlobal(eventStartPos)); }
}

void Sources3DWidget::changeSourceCoordinates() {
    // set new sourcecoordinates according to entries in sourceDialog
    // check whether position, orientation or both changed and emit appropriate signal
    // if( sourceDialog->applyToAllBreakpointsBox->checkState() )
    //{
    // for(unsigned int i=0;i<noSlices;++i)
    //{
    //    Source* source = sources[currSource-1]->at(i);
    //    SourceCoordinates oldCoords = source->getCoordinates();
    //    SourceCoordinates newCoords = sourceDialog->newCoordinates;

    //    if(oldCoords == newCoords)
    //      break;
    //    else
    //    {
    //      bool posChanged = false;
    //      bool oriChanged = false;

    //      if(oldCoords.x != newCoords.x || oldCoords.y != newCoords.y)
    //          posChanged = true;
    //      if(oldCoords.orientation != newCoords.orientation)
    //          oriChanged = true;
    //
    //      source->setCoordinates(sourceDialog->newCoordinates);
    //      update(posChanged,oriChanged);
    //    }
    //}
    //}
    // else
    {
        if (sources.contains(currSource) && sources.value(currSource)->isActive()) {
            sources.value(currSource)->setCoordinates(sourceDialog->newCoordinates);

            // emit appropriate signals according to type
            if (sources.value(currSource)->isPlanewave()) {
                update(true, true);
            } else {
                update(true, false);
            }
        }
    }
}

void Sources3DWidget::initializeGL() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);
    glShadeModel(GL_SMOOTH);
    glClearColor(0.0, 0.0, 0.0, 0.0);

    // define light
    glEnable(GL_LIGHT0);
    GLfloat light0_position[] = {1.0, 1.0, -0.3, 0.0};
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);

    // a second light
    glEnable(GL_LIGHT1);
    GLfloat light1_position[]     = {1.0, 1.0, 0.8, 0.0};
    GLfloat light1_diffusecolor[] = {0.5, 0.5, 0.5, 1.0};
    glLightfv(GL_LIGHT1, GL_AMBIENT, light1_diffusecolor);
    glLightfv(GL_LIGHT1, GL_POSITION, light1_position);

    // Ambientlight-model
    GLfloat lmodel_ambient[] = {0.25, 0.25, 0.25, 0.25};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

    // define material
    glMaterialfv(GL_FRONT, GL_SPECULAR, colors[black]);
    glMaterialf(GL_FRONT, GL_SHININESS, 70.0);

    // generate displaylists for room, cube and PlanewaveSource
    genRoomList();
    genCubeList();
    genSphereList();
    genPlanewaveSourceList();
}

void Sources3DWidget::resizeGL(int width, int height) {
    windowWidth  = (GLfloat)width;
    windowHeight = (GLfloat)height;

    glViewport(0, 0, (GLsizei)width, (GLsizei)height);
    project();
}

void Sources3DWidget::project() {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

    if (perspectiveOn) {
        gluPerspective(60.0, windowWidth / windowHeight, 1.0, 1000.0);
    } else {
        if (windowWidth <= windowHeight)
            glOrtho(-orthoBounds, orthoBounds, -orthoBounds * windowHeight / windowWidth,
                    orthoBounds * windowHeight / windowWidth, -1.0, 1000.0);
        else
            glOrtho(-orthoBounds * windowWidth / windowHeight,
                    orthoBounds * windowWidth / windowHeight, -orthoBounds, orthoBounds,
                    -1.0, 1000.0);
    }
}

void Sources3DWidget::paintGL() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    bool lightWasOn   = false;
    GLfloat lineWidth = 0.0;
    glGetFloatv(GL_LINE_WIDTH, &lineWidth);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glPushMatrix();
    // View-transformations
    glTranslatef(xposition, yposition, zoom);
    glRotatef(xrotation, 1.0, 0.0, 0.0);
    glRotatef(yrotation, 0.0, 1.0, 0.0);
    glRotatef(zrotation, 0.0, 0.0, 1.0);

    // draw timeslices and source
    glPushMatrix();
    // Lightswitch for roomframe
    lightWasOn = glIsEnabled(GL_LIGHTING);
    glDisable(GL_LIGHTING);

    // indicate clicklock
    if (clickLockOn) {
        glColor4fv(colors[red]);
        renderText(10, 15, "Stickyclick active");
    }

    // highlight current timeslice
    if (currSlice) {
        glColor4fv(colors[yellow]);
    } else {
        glColor4fv(colors[huntersgreen]);
    }

    glLineWidth(lineWidth + 2.0);
    glCallList(roomList);
    glLineWidth(lineWidth);

    // display roomname and indicate viewlock
    if (viewLockOn) {
        renderText(10, height() - 10, xwConf->roomName);
    } else {
        renderText(10, height() - 10, xwConf->roomName + " ( View unlocked ) ");
    }

    // display timetics
    // if(drawTimeTics)
    //{
    //    QString sliceString = QString::number(i)+"s";
    //    //H104
    //    renderText(10.2,-12.85,0.0,sliceString);
    //    //Standardroom 20mx20m
    //    //renderText(10.5,-10.0,0.0,sliceString);
    //}
    // mark room-origin
    if (originOn) {
        glPointSize(7.0);
        glBegin(GL_POINTS);
        glColor3f(1.0, 0.0, 0.0);
        glVertex3f(0.0, 0.0, 0.1);
        glEnd();
        glPointSize(1.0);
        // renderText(-0.05,13.1,0.0,"[0|0]");
    }

    if (lightWasOn) { glEnable(GL_LIGHTING); }

    // draw sources and connections to groups
    QHash<unsigned int, Source*>::const_iterator it;

    for (it = sources.constBegin(); it != sources.constEnd(); ++it) {
        Source* temp = (*it);

        if (temp->isVisible() && temp->isActive()) {
            glPushMatrix();
            glTranslatef(temp->getx(), temp->gety(), 0.0);

            if ((currSlice && currSource == temp->getXID())
                || (currGroup != 0 && (temp->getGroupID() == currGroup))) {
                glColor4fv(colors[gold]);
            } else {
                glColor4fv(temp->getColor());
            }

            // draw visual cue for read disabled sources
            if (!temp->isReadEnabled() || !readOn) { glColor4fv(colors[gray]); }

            if (temp->isPlanewave()) {
                glPushMatrix();
                // draw source icon
                glRotatef(temp->getOrientation(), 0.0, 0.0, 1.0);
                glCallList(planewaveSourceList);
            } else {  // point source
                glPushMatrix();
                // draw source icon
                // gluQuadricDrawStyle( quad, GLU_FILL );
                // gluSphere( quad, soSources, lodSources, lodSources );
                glCallList(sphereList);
            }

            // end of cues for read disabled sources
            if (!temp->isReadEnabled() || !readOn) { glColor4fv(temp->getColor()); }

            // draw visual cue for current source
            if (currSource == temp->getXID()) {
                float radius = soSources + soSources;
                glBegin(GL_LINE_LOOP);
                glVertex3f(radius, 0.0, 0.0);
                glVertex3f(0.0, radius, 0.0);
                glVertex3f(-radius, 0.0, 0.0);
                glVertex3f(0.0, -radius, 0.0);
                glEnd();
            }

            // draw visual cue for record enabled sources
            if (temp->isRecordEnabled()) {
                float dist   = soSources / 2.0;
                float radius = soSources + dist;
                glColor4fv(colors[red]);
                glBegin(GL_POLYGON);
                glVertex3f(radius, -dist, 0.0);
                glVertex3f(radius, dist, 0.0);
                glVertex3f(dist, radius, 0.0);
                glVertex3f(-dist, radius, 0.0);
                glVertex3f(-radius, dist, 0.0);
                glVertex3f(-radius, -dist, 0.0);
                glVertex3f(-dist, -radius, 0.0);
                glVertex3f(dist, -radius, 0.0);
                glEnd();
            }

            glPopMatrix();
            glColor4fv(temp->getColor());

            // draw source coordinates
            if (drawCoordinates) {
                SourceCoordinates coord =
                    temp->getCoordinatesRounded().mapGLCoordToWonderCoord();
                QString coordsString =
                    QString::number(coord.x) + " | " + QString::number(coord.y);

                if (temp->isPlanewave()) {
                    coordsString += (" || " + QString::number(coord.orientation));
                }

                renderText(soSources + soSources / 2.0, soSources, 0.0, coordsString);
            }

            // draw source name
            if (drawNames) {
                renderText(soSources + soSources / 1.3, -(soSources + soSources / 0.7),
                           0.0, temp->getName());
            }

            glPopMatrix();
            // draw connection to groupobject if source is member of a group
            int groupID = temp->getGroupID();

            if (groupID && groups.contains(groupID)
                && groups.value(groupID)->isActive()) {
                glBegin(GL_LINE_STRIP);
                glVertex3f(temp->getx(), temp->gety(), 0.0);
                glVertex3f(groups.value(groupID)->getx(), groups.value(groupID)->gety(),
                           0.0);
                glEnd();
            }
        }
    }

    // draw groupobjects
    QMap<int, SourceGroup*>::const_iterator it2;  // second iterator in this method

    for (it2 = groups.constBegin(); it2 != groups.constEnd(); ++it2) {
        SourceGroup* temp = it2.value();

        if (temp->isActive()) {
            glPushMatrix();

            if (temp->getGroupID() == currGroup) {
                glColor4fv(colors[gold]);
            } else {
                glColor4fv(temp->getColor());
            }

            glTranslatef(temp->getx(), temp->gety(), 0.0);
            glRotatef(temp->getOrientation(), 0.0, 0.0, 1.0);
            glCallList(cubeList);
            glPopMatrix();
        }
    }

    glPopMatrix();
    glPopMatrix();
}

void Sources3DWidget::mousePressEvent(QMouseEvent* event) {
    // if clicklock is used, mouseTracking is activated and left button is pressed, do
    // nothing
    if (clickLockOn && hasMouseTracking() && (event->buttons() == Qt::RightButton)) {
        return;
    }

    if (event->buttons() == Qt::LeftButton + Qt::RightButton && selectionModeOn
        && (currSource != 0 || currGroup != 0)) {
        return;
    }

    // remember position the event occurred
    lastMousePos = eventStartPos = event->pos();
    bool contextDialogUsed       = false;

    // detect if a black pixel is selected -> no selectionmode
    GLubyte pixel[3];
    GLint viewport[4];
    glGetIntegerv(GL_VIEWPORT, viewport);
    glReadPixels(event->x(), viewport[3] - event->y(), 1, 1, GL_RGB, GL_UNSIGNED_BYTE,
                 pixel);

    currSource = 0;
    currSlice  = 0;
    // currGroup = 0;

    if (pixel[0] == 0 && pixel[1] == 0 && pixel[2] == 0) {
        selectionModeOn = false;
    } else {
        selectionModeOn = true;
    }

    // GL-picking and determine current source and timeslice
    if (selectionModeOn) {
        const GLuint Bufsize = 512;
        GLuint selectBuf[Bufsize];
        GLint hits;
        GLint viewport[4];

        glGetIntegerv(GL_VIEWPORT, viewport);
        glSelectBuffer(Bufsize, selectBuf);
        glRenderMode(GL_SELECT);
        glInitNames();
        // Content of namestack for each hit is: a b1 b2 a*c
        // with  a = number of names, b1 and b2 = min and max z-values, c = names for this
        // hit ( "a" number of names)

        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        gluPickMatrix((GLdouble)event->x(), (GLdouble)(viewport[3] - event->y()), 1.0,
                      1.0, viewport);

        if (perspectiveOn) {
            gluPerspective(60.0, windowWidth / windowHeight, 1.0, 200.0);
        } else {
            if (windowWidth <= windowHeight)
                glOrtho(-orthoBounds, orthoBounds,
                        -orthoBounds * windowHeight / windowWidth,
                        orthoBounds * windowHeight / windowWidth, -1.0, 100.0);
            else
                glOrtho(-orthoBounds * windowWidth / windowHeight,
                        orthoBounds * windowWidth / windowHeight, -orthoBounds,
                        orthoBounds, -1.0, 100.0);
        }

        // render GL-scene in SelectionBuffer (only the sources and groupobjects)
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        glPushMatrix();
        // View-transformations
        glTranslatef(xposition, yposition, zoom);
        glRotatef(xrotation, 1.0, 0.0, 0.0);
        glRotatef(yrotation, 0.0, 1.0, 0.0);
        glRotatef(zrotation, 0.0, 0.0, 1.0);
        {
            glPushName(1);  // name for timeslice
            glPushMatrix();
            // draw sources
            QHash<unsigned int, Source*>::const_iterator it;

            for (it = sources.constBegin(); it != sources.constEnd(); ++it) {
                Source* temp = (*it);

                if (temp->isVisible() && temp->isActive()) {
                    glPushName(temp->getXID());  // name for source
                    glPushMatrix();
                    glTranslatef(temp->getx(), temp->gety(), 0.0);
                    // gluQuadricDrawStyle( quad, GLU_FILL );
                    // gluSphere( quad, soSources, lodSources, lodSources);
                    glCallList(cubeList);
                    glPopMatrix();
                    glPopName();
                }
            }

            // draw groupobjects
            QMap<int, SourceGroup*>::const_iterator
                it2;  // second iterator in this method

            for (it2 = groups.constBegin(); it2 != groups.constEnd(); ++it2) {
                SourceGroup* temp = it2.value();

                if (temp->isActive()) {
                    // name indicating that this is a group (one more than number of
                    // sources )
                    glPushName(sources.count() + 1);
                    glPushName(temp->getGroupID());  // name of group
                    glPushMatrix();
                    glTranslatef(temp->getx(), temp->gety(), 0.0);
                    // gluQuadricDrawStyle (quad, GLU_FILL );
                    // gluSphere( quad, soSources, lodSources, lodSources );
                    glCallList(cubeList);
                    glPopMatrix();
                    glPopName();
                    glPopName();
                }
            }

            glPopMatrix();
            glPopName();
        }
        glPopMatrix();

        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        glFlush();

        hits               = glRenderMode(GL_RENDER);
        GLuint* pSelectBuf = selectBuf;

        // determine current source (or group) and timeslice
        GLuint assumedHit = 0;
        GLuint hitIndexes[hits];  // begin of each hitrecord in pSelectBuf
        GLuint currZ = pSelectBuf[1];

        for (int i = 0; i < hits; ++i) {
            if (i == 0) {
                hitIndexes[i] = 0;
            } else {
                hitIndexes[i] = hitIndexes[i - 1] + pSelectBuf[hitIndexes[i - 1]]
                                + 3;  // 3 for a+b1+b2, see above
            }

            if (pSelectBuf[hitIndexes[i] + 1] < currZ) { assumedHit = i; }
        }

        unsigned int sliceHit  = 0;
        unsigned int sourceHit = 0;
        bool groupHit          = false;

        if (hits > 0) {
            if (pSelectBuf[hitIndexes[assumedHit]] == 3) {
                groupHit = true;
            } else {
                sourceHit = pSelectBuf[hitIndexes[assumedHit] + 4];
            }

            sliceHit = pSelectBuf[hitIndexes[assumedHit] + 3];
        }

        if (sliceHit > 0) { currSlice = sliceHit; }

        if (sourceHit > 0) {
            // if( sourceHit <= (unsigned int) sources.count() )
            {
                currSource = sourceHit;

                // add or remove source from current group
                if ((currGroup != 0) && (event->buttons() == Qt::LeftButton)
                    && (event->modifiers() == Qt::CTRL) && (sources.contains(currSource))
                    && (sources.value(currSource)->isActive())
                    && (groups.contains(currGroup))
                    && (groups.value(currGroup)->isActive())) {
                    Source* temp = sources.value(currSource);

                    unsigned int sourceXID = temp->getXID();

                    // remove from group
                    if (groups.value(currGroup)->containsXID(sourceXID)) {
                        groups.value(currGroup)->deleteSource(sourceXID);
                        temp->setGroupID(0);
                        emit sourceGroupIDChanged(temp->getID(), 0);
                        currSource = 0;
                    } else {  // add to group
                        groups.value(currGroup)->addSource(sourceXID);
                        temp->setGroupID(currGroup);
                        emit sourceGroupIDChanged(temp->getID(), currGroup);
                        // adjusting all colors (groups and sources)
                        const GLfloat* color = groups.value(currGroup)->getColor();
                        temp->setColor(color);  // groups.value(currGroup)->getColor());
                        QColor newColor;
                        newColor.setRedF(color[0]);
                        newColor.setGreenF(color[1]);
                        newColor.setBlueF(color[2]);
                        newColor.setAlphaF(color[3]);
                        emit sourceColorChanged(temp->getXID(),
                                                newColor);  // for ChannelsWidget
                        emit sourceColorChanged(temp->getID(), newColor);  // for cwonder

                        // delete membership of source to other group than this one
                        QMap<int, SourceGroup*>::iterator it;

                        for (it = groups.begin(); it != groups.end(); ++it)
                            if ((it.key() != currGroup)
                                && (it.value()->containsXID(sourceXID))) {
                                it.value()->deleteSource(sourceXID);
                            }

                        currSource = 0;
                    }
                } else {
                    currGroup = 0;
                }
            }
        } else if (groupHit) {
            currGroup = pSelectBuf[hitIndexes[assumedHit] + 3
                                   + pSelectBuf[hitIndexes[assumedHit]] - 1];
            // TODO: Think about dead code
            // sliceHit  = pSelectBuf[ hitIndexes[ assumedHit ] + 3 ];
        }

        update(false, false);

        // rightclick generates contextdialog for the selected source or group
        if ((event->buttons() == Qt::RightButton) && (currSource != 0)) {
            displaySourceContextDialog();
            contextDialogUsed = true;
        } else if ((event->buttons() == Qt::RightButton) && (currGroup != 0)) {
            displayGroupContexDialog(currGroup);
            contextDialogUsed = true;
        }
    } else {
        if ((event->buttons() == Qt::RightButton)) {
            contextDialogUsed = true;
            displayGroupContexDialog();
        } else {
            currGroup = 0;
        }
    }

    // set cursor to center of this widget to provide infinite movement
    // ( so we don't hit the screenborders )
    if (!contextDialogUsed) {
        setCursor(Qt::BlankCursor);
        QPoint center(width() / 2, height() / 2);
        lastMousePos        = center;
        ignoreNextMouseMove = true;
        QCursor::setPos(mapToGlobal(center));
    } else {
        QCursor::setPos(mapToGlobal(eventStartPos));
    }

#ifdef __APPLE__
    CGAssociateMouseAndMouseCursorPosition(true);
#endif

    // clicklock
    if ((event->buttons() == Qt::LeftButton)) {
        if (clickLockOn) {
            if (hasMouseTracking()) {
                setMouseTracking(false);
            } else {
                setMouseTracking(true);
            }
        } else {
            setMouseTracking(false);
        }
    }
}

void Sources3DWidget::mouseMoveEvent(QMouseEvent* event) {
    if (ignoreNextMouseMove) {
        ignoreNextMouseMove = false;
        return;
    }

    // pointer to mouseEvent, needed for clickLock
    QMouseEvent* delegateEvent;

    // add left button to event if clickLock is used
    if (clickLockOn) {
        delegateEvent =
            new QMouseEvent(QEvent::MouseMove, event->pos(), Qt::NoButton,
                            event->buttons() | Qt::LeftButton, event->modifiers());
    } else {
        delegateEvent = event;
    }

    GLfloat dx           = GLfloat(event->x() - lastMousePos.x());
    GLfloat dy           = GLfloat(event->y() - lastMousePos.y());
    int mouseGranularity = 80;  // 40;

    setCursor(Qt::BlankCursor);

    // Selection with a valid source or group
    if (selectionModeOn && (currSource != 0 || currGroup != 0) && currSlice != 0) {
        switch (delegateEvent->buttons()) {
        case Qt::LeftButton: {
            // move sources and/or sourcegroup
            Source* temp1          = nullptr;
            SourceGroup* tempGroup = nullptr;

            if (currSource != 0 && sources.contains(currSource)
                && sources.value(currSource)->isActive()) {
                temp1 = sources.value(currSource);
            } else if (currGroup != 0 && groups.contains(currGroup)
                       && groups.value(currGroup)->isActive()) {
                tempGroup = groups.value(currGroup);
            }

            switch (delegateEvent->modifiers()) {
            case Qt::CTRL:        // movement along x -axis
                if (tempGroup) {  // group
                    tempGroup->setx(tempGroup->getx() + (dx / mouseGranularity));
                    QList<unsigned int> sourceXIDs = tempGroup->getXIDs();
                    QList<unsigned int>::iterator it;

                    for (it = sourceXIDs.begin(); it != sourceXIDs.end(); ++it) {
                        temp1 = sources.value(*it);
                        temp1->setx(temp1->getx() + (dx / mouseGranularity));
                    }

                    update(true, false, true, true);
                } else if (temp1) {  // single source
                    temp1->setx(temp1->getx() + (dx / mouseGranularity));
                    update(true, false);
                }

                break;

            case Qt::SHIFT:       // movement along y -axis
                if (tempGroup) {  // group
                    tempGroup->sety(tempGroup->gety() - (dy / mouseGranularity));
                    QList<unsigned int> sourceXIDs = tempGroup->getXIDs();
                    QList<unsigned int>::iterator it;

                    for (it = sourceXIDs.begin(); it != sourceXIDs.end(); ++it) {
                        temp1 = sources.value(*it);
                        temp1->sety(temp1->gety() - (dy / mouseGranularity));
                    }

                    update(true, false, true, true);
                } else if (temp1) {  // single source
                    temp1->sety(temp1->gety() - (dy / mouseGranularity));
                    update(true, false);
                }

                break;

            // group rotation
            case Qt::ALT:
                if (tempGroup) {
                    SourceCoordinates groupCoords = tempGroup->getCoordinates();
                    SourceCoordinates newCoords;

                    Source* sourceToSet = nullptr;

                    // angle of movement
                    double alpha    = dy / mouseGranularity * 0.5;
                    double negAlpha = -dy / mouseGranularity * 0.5;

                    double sinAlpha    = sin(alpha);
                    double cosAlpha    = cos(alpha);
                    double sinNegAlpha = sin(negAlpha);
                    double cosNegAlpha = cos(negAlpha);

                    // group coordinates
                    double xg = groupCoords.x;
                    double yg = groupCoords.y;

                    // old coordinates of source to move
                    double xp = 0.0;
                    double yp = 0.0;

                    // new coordinates of source to move
                    double x = 0.0;
                    double y = 0.0;

                    QList<unsigned int> sourceXIDs = tempGroup->getXIDs();
                    QList<unsigned int>::iterator it;

                    for (it = sourceXIDs.begin(); it != sourceXIDs.end(); ++it) {
                        if (sources.contains(*it)) {
                            sourceToSet = sources.value(*it);
                        } else {
                            continue;
                        }

                        xp = sourceToSet->getx();
                        yp = sourceToSet->gety();

                        if (sourceToSet->hasInvertedRotation()) {
                            x = cosNegAlpha * (xp - xg) + sinNegAlpha * (yg - yp) + xg;
                            y = sinNegAlpha * (xp - xg) + cosNegAlpha * (yp - yg) + yg;
                        } else {
                            x = cosAlpha * (xp - xg) + sinAlpha * (yg - yp) + xg;
                            y = sinAlpha * (xp - xg) + cosAlpha * (yp - yg) + yg;
                        }

                        sourceToSet->setPosition(x, y);
                    }
                }

                update(true, false, true);
                break;

            //  group scaling
            case Qt::ALT + Qt::SHIFT:
                if (tempGroup) {
                    SourceCoordinates groupCoords = tempGroup->getCoordinates();
                    SourceCoordinates newCoords;

                    Source* sourceToSet = nullptr;

                    double scale    = 1 - (dy / mouseGranularity * 0.5);
                    double negScale = 1 - (-dy / mouseGranularity * 0.5);

                    // scale margin to prevent "lock in" of sources onto groupcenter
                    double marginSquared = 0.0025;

                    // group coordinates
                    double xg = groupCoords.x;
                    double yg = groupCoords.y;

                    // coordinates of source to move
                    double xp = 0.0;
                    double yp = 0.0;

                    // new coordinates of source to move
                    double x = 0.0;
                    double y = 0.0;

                    QList<unsigned int> sourceXIDs = tempGroup->getXIDs();
                    QList<unsigned int>::iterator it;

                    for (it = sourceXIDs.begin(); it != sourceXIDs.end(); ++it) {
                        if (sources.contains(*it)) {
                            sourceToSet = sources.value(*it);
                        } else {
                            continue;
                        }

                        xp = sourceToSet->getx();
                        yp = sourceToSet->gety();

                        if (sourceToSet->hasInvertedScaling()) {
                            x = (xp - xg) * negScale + xg;
                            y = (yp - yg) * negScale + yg;
                        } else {
                            x = (xp - xg) * scale + xg;
                            y = (yp - yg) * scale + yg;
                        }

                        // to prevent "lock in" of sources on position of group
                        // center keep minimum distance so that expansion is
                        // always possible
                        double distanceSquared =
                            (x - xg) * (x - xg) + (y - yg) * (y - yg);

                        if (distanceSquared < 0) { distanceSquared *= -1.0; }

                        if (distanceSquared > marginSquared) {
                            sourceToSet->setPosition(x, y);
                        }
                    }
                }

                update(true, false, true);
                break;

            default:  // free movement of group or source
                if (tempGroup) {
                    tempGroup->setx(tempGroup->getx() + (dx / mouseGranularity));
                    tempGroup->sety(tempGroup->gety() - (dy / mouseGranularity));
                    QList<unsigned int> sourceXIDs = tempGroup->getXIDs();
                    QList<unsigned int>::iterator it;

                    for (it = sourceXIDs.begin(); it != sourceXIDs.end(); ++it) {
                        temp1 = sources.value(*it);
                        temp1->sety(temp1->gety() - (dy / mouseGranularity));
                        temp1->setx(temp1->getx() + (dx / mouseGranularity));
                    }

                    update(true, false, true, true);
                } else if (temp1) {
                    temp1->sety(temp1->gety() - (dy / mouseGranularity));
                    temp1->setx(temp1->getx() + (dx / mouseGranularity));
                    update(true, false);
                }

                break;
            }

            break;
        }

        case Qt::LeftButton + Qt::RightButton: {
            // move groupobject without sources belonging to this group
            if (currGroup != 0 && groups.contains(currGroup)
                && groups.value(currGroup)->isActive()) {
                SourceGroup* tempGroup = groups.value(currGroup);
                tempGroup->sety(tempGroup->gety() - (dy / mouseGranularity));
                tempGroup->setx(tempGroup->getx() + (dx / mouseGranularity));
                update(false, false, true, true);
                break;
            }

            // if no group or source is selected we have nothing to do
            if (currSource == 0) { break; }

            // rotate planewave-sources
            Source* temp1 = sources.value(currSource);

            // timesliceList* tempAll = sources[currSource-1];

            // for point sources there is nothing to do here
            if (!temp1->isPlanewave()) {
                break;
            } else {
                switch (delegateEvent->modifiers()) {
                // all timeslices (ALT pressed)
                // case Qt::ALT:
                // for(unsigned int i=0;i<noSlices;++i)
                //      tempAll->at(i)->setOrientation(tempAll->at(i)->getOrientation()-dx);
                // break;
                // rotate the source
                default:
                    temp1->setOrientation(temp1->getOrientation() - dx);
                    break;
                }

                update(false, true);
            }

            break;
        }
        }
    } else {  // no selection, navigation of the whole image
        if (!viewLockOn) {
            switch (delegateEvent->buttons()) {
            case Qt::LeftButton:
                if (delegateEvent->modifiers() == Qt::CTRL) {  // move whole image
                    xposition += dx / mouseGranularity;
                    yposition -= dy / mouseGranularity;
                } else if (delegateEvent->modifiers() == Qt::SHIFT) {  // z-axis rotation
                    zrotation -= 180 * dy / width();
                } else if (delegateEvent->modifiers()
                           == Qt::ALT) {  // rotate image along x- and/or y-axis
                    xrotation += 180 * dy / width();
                    yrotation += 180 * dx / height();
                } else {  // zoom in/out
                    if (perspectiveOn) {
                        zoom += dy / (mouseGranularity / 3);
                    } else {
                        orthoBounds += dy / (mouseGranularity / 2);
                        project();
                    }
                }

                break;
                // case Qt::RightButton:
                // if(delegateEvent->modifiers() == Qt::SHIFT)
                // setSliceDist(dy/mouseGranularity);
                // break;
            }
        }
    }

    // if movement occurred
    if (dx != 0 || dy != 0) {
        QPoint center(width() / 2, height() / 2);
        ignoreNextMouseMove = true;
        QCursor::setPos(mapToGlobal(center));

#ifdef __APPLE__
        CGAssociateMouseAndMouseCursorPosition(true);
#endif
    }

    // if clickLock is used, destroy temporary event
    if (clickLockOn) { delete delegateEvent; }
}

void Sources3DWidget::mouseReleaseEvent(QMouseEvent* event) {
    // if clickLock is used we should not set a new mouse position
    // because this will trigger a move event, that would cause jumping sources
    if (clickLockOn) {
        if (!hasMouseTracking()) { setCursor(Qt::ArrowCursor); }
    } else if (!(event->button() == Qt::RightButton
                 && event->buttons() == Qt::LeftButton)) {
        QCursor::setPos(mapToGlobal(eventStartPos));
#ifdef __APPLE__
        CGAssociateMouseAndMouseCursorPosition(true);
#endif
        setCursor(Qt::ArrowCursor);
    }
}

void Sources3DWidget::wheelEvent(QWheelEvent* event) {
    if (!viewLockOn) {
        // if( event->modifiers() == Qt::SHIFT )
        // setSliceDist( event->delta() / ( 8 * 30.0 ) );
        // else
        {
            if (perspectiveOn) {
                zoom -= event->angleDelta().y() / (8 * 30.0);
            } else {
                orthoBounds += event->angleDelta().y() / (8 * 30.0);
                project();
            }
        }
    }
}

void Sources3DWidget::keyPressEvent(QKeyEvent* event) {
    switch (event->key()) {
    case Qt::Key_W:  // toggle type of source
        if (currSource && sources.contains(currSource)) {
            Source* temp = sources.value(currSource);
            temp->setType(temp->isPlanewave() ? false : true);
            emit sourceTypeChanged(temp->getXID(), temp->isPlanewave());
        }

        break;

    case Qt::Key_R:  // toggle recordmode of source

        // record can only be used in projects with score
        if (xwConf->projectOnlyMode) {
            return;
        } else if (currGroup && groups.contains(currGroup)
                   && groups.value(currGroup)->isActive()) {
            // toggle record mode of all source of the current group
            // if one source of this group is enabled for recording toggle all off
            // otherwise toggle all on
            bool enableRecords       = true;
            QList<unsigned int> xIDs = groups.value(currGroup)->getXIDs();
            QList<unsigned int>::iterator it;

            for (it = xIDs.begin(); it != xIDs.end(); ++it) {
                if (sources.contains(*it)) {
                    if (sources.value(*it)->isRecordEnabled()) {
                        enableRecords = false;
                        break;
                    }
                }
            }

            for (it = xIDs.begin(); it != xIDs.end(); ++it) {
                if (sources.contains(*it)) {
                    sources.value(*it)->setRecordMode(enableRecords);
                    emit sourceRecordModeChanged(*it, enableRecords);
                }
            }
        } else if (currSource) {
            // toggle record mode of current source
            if (sources.contains(currSource)) {
                sources.value(currSource)->toggleRecordMode();
                emit sourceRecordModeChanged(
                    sources.value(currSource)->getXID(),
                    sources.value(currSource)->isRecordEnabled());
            }
        }

        break;

    case Qt::Key_Delete:
    case Qt::Key_Backspace:  // delete source

        // if group is selected then delete groupobject
        if (currGroup) {
            deactivateGroup(currGroup);
        } else {  // dispatch event
            event->ignore();
        }

        break;

    case Qt::Key_X:  // x-axis rotation
        if (!viewLockOn) {
            if (event->modifiers() == Qt::ShiftModifier) {
                xrotation -= rotInc;
            } else {
                xrotation += rotInc;
            }
        }

        break;

    case Qt::Key_Y:  // y-axis rotation
        if (!viewLockOn) {
            if (event->modifiers() == Qt::ShiftModifier) {
                yrotation += rotInc;
            } else {
                yrotation -= rotInc;
            }
        }

        break;

    case Qt::Key_Z:  // z-axis rotation
        if (!viewLockOn) {
            if (event->modifiers() == Qt::ShiftModifier) {
                zrotation += rotInc;
            } else {
                zrotation -= rotInc;
            }
        }

        break;

    case Qt::Key_H:  // zoom out
        if (!viewLockOn) {
            if (perspectiveOn) {
                zoom += 1.0;
            } else {
                orthoBounds -= 1.0;
                project();
            }
        }

        break;

    case Qt::Key_G:  // zoom in
        if (!viewLockOn) {
            if (perspectiveOn) {
                zoom -= 1.0;
            } else {
                orthoBounds += 1.0;
                project();
            }
        }

        break;

    // case Qt::Key_M:
    //    if(glIsEnabled(GL_COLOR_MATERIAL))
    //    {
    //      glColor4fv(colors[gray]);
    //      glDisable(GL_COLOR_MATERIAL);
    //    }
    //    else
    //      glEnable(GL_COLOR_MATERIAL);
    //    break;
    // case Qt::Key_H:
    //    if(highlightsOn)
    //    {
    //      glMaterialfv(GL_FRONT, GL_SPECULAR, colors[black]);
    //      highlightsOn = !highlightsOn;
    //    }
    //    else
    //    {
    //      glMaterialfv(GL_FRONT, GL_SPECULAR, colors[white]);
    //      highlightsOn = !highlightsOn;
    //    }
    //    break;
    case Qt::Key_B:  // source size
        if (event->modifiers() == Qt::ShiftModifier) {
            changeSourceSize(soSourcesInc);
        } else {
            changeSourceSize(-soSourcesInc);
        }

        break;

    // case Qt::Key_D: // level of detail
    //    if( event->modifiers() == Qt::ShiftModifier )
    //        changeSourceLod( lodSourcesInc );
    //    else
    //        changeSourceLod( -lodSourcesInc );
    //    break;
    case Qt::Key_Up:
        if (currSlice == 0 && !viewLockOn) {
            yposition += moveInc;
        } else {
            if (currGroup == 0 && currSource == 0) {
                return;
            } else {
                if (currGroup && groups.contains(currGroup)
                    && groups.value(currGroup)->isActive()) {
                    if (event->modifiers() == Qt::ALT) {
                        SourceGroup* temp = groups.value(currGroup);
                        temp->sety(temp->gety() + moveInc);
                        update(false, false, true, true);
                    } else {
                        SourceGroup* temp = groups.value(currGroup);
                        temp->sety(temp->gety() + moveInc);

                        QList<unsigned int> sourceXIDs =
                            groups.value(currGroup)->getXIDs();
                        QList<unsigned int>::iterator it;

                        for (it = sourceXIDs.begin(); it != sourceXIDs.end(); ++it) {
                            if (sources.contains(*it)) {
                                Source* temp = sources.value(*it);
                                temp->sety(temp->gety() + moveInc);
                            }
                        }

                        update(true, false, true, true);
                    }
                } else if (sources.contains(currSource)) {
                    Source* temp = sources.value(currSource);
                    temp->sety(temp->gety() + moveInc);
                    update(true, false);
                }
            }
        }

        break;

    case Qt::Key_Down:
        if (currSlice == 0 && !viewLockOn) {
            yposition -= moveInc;
        } else {
            if (currGroup == 0 && currSource == 0) {
                return;
            } else {
                if (currGroup && groups.contains(currGroup)
                    && groups.value(currGroup)->isActive()) {
                    if (event->modifiers() == Qt::ALT) {
                        SourceGroup* temp = groups.value(currGroup);
                        temp->sety(temp->gety() - moveInc);
                        update(false, false, true, true);
                    } else {
                        SourceGroup* temp = groups.value(currGroup);
                        temp->sety(temp->gety() - moveInc);

                        QList<unsigned int> sourceXIDs =
                            groups.value(currGroup)->getXIDs();
                        QList<unsigned int>::iterator it;

                        for (it = sourceXIDs.begin(); it != sourceXIDs.end(); ++it) {
                            if (sources.contains(*it)) {
                                Source* temp = sources.value(*it);
                                temp->sety(temp->gety() - moveInc);
                            }
                        }

                        update(true, false, true, true);
                    }
                } else if (sources.contains(currSource)) {
                    Source* temp = sources.value(currSource);
                    temp->sety(temp->gety() - moveInc);
                    update(true, false);
                }
            }
        }

        break;

    case Qt::Key_Right:
        if (currSlice == 0 && !viewLockOn) {
            xposition += moveInc;
        } else {
            if (currGroup == 0 && currSource == 0) {
                return;
            } else {
                if (currGroup && groups.contains(currGroup)
                    && groups.value(currGroup)->isActive()) {
                    if (event->modifiers() == Qt::ALT) {
                        SourceGroup* temp = groups.value(currGroup);
                        temp->setx(temp->getx() + moveInc);
                        update(false, false, true, true);
                    } else {
                        SourceGroup* temp = groups.value(currGroup);
                        temp->setx(temp->getx() + moveInc);

                        QList<unsigned int> sourceXIDs =
                            groups.value(currGroup)->getXIDs();
                        QList<unsigned int>::iterator it;

                        for (it = sourceXIDs.begin(); it != sourceXIDs.end(); ++it) {
                            if (sources.contains(*it)) {
                                Source* temp = sources.value(*it);
                                temp->setx(temp->getx() + moveInc);
                            }
                        }

                        update(true, false, true, true);
                    }
                } else if (sources.contains(currSource)) {
                    Source* temp = sources.value(currSource);
                    temp->setx(temp->getx() + moveInc);
                    update(true, false);
                }
            }
        }

        break;

    case Qt::Key_Left:
        if (currSlice == 0 && !viewLockOn) {
            xposition -= moveInc;
        } else {
            if (currGroup == 0 && currSource == 0) {
                return;
            } else {
                if (currGroup && groups.contains(currGroup)
                    && groups.value(currGroup)->isActive()) {
                    if (event->modifiers() == Qt::ALT) {
                        SourceGroup* temp = groups.value(currGroup);
                        temp->setx(temp->getx() - moveInc);
                        update(false, false, true, true);
                    } else {
                        SourceGroup* temp = groups.value(currGroup);
                        temp->setx(temp->getx() - moveInc);

                        QList<unsigned int> sourceXIDs =
                            groups.value(currGroup)->getXIDs();
                        QList<unsigned int>::iterator it;

                        for (it = sourceXIDs.begin(); it != sourceXIDs.end(); ++it) {
                            if (sources.contains(*it)) {
                                Source* temp = sources.value(*it);
                                temp->setx(temp->getx() - moveInc);
                            }
                        }

                        update(true, false, true, true);
                    }
                } else if (sources.contains(currSource)) {
                    Source* temp = sources.value(currSource);
                    temp->setx(temp->getx() - moveInc);
                    update(true, false);
                }
            }
        }

        break;

    default:
        event->ignore();
    }
}
