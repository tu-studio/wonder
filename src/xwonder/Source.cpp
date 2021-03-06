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

#include "Source.h"

#include <QDebug>
#include <QGLWidget>

#include "Colors.h"

int Source::noActiveSources = 0;

//--------------------------------constructors--------------------------------//

Source::Source(unsigned int xID, int sourceID, QObject* parent)
    : QObject(parent)
    , sourceID(sourceID)
    , xID(xID)
    , groupID(0)
    , coordinates(0.0, 0.0, 0.0)
    , name("")
    , active(false)
    , visible(true)
    , planewave(false)
    , recordEnabled(false)
    , readEnabled(true)
    , invertedRotation(false)
    , invertedScaling(false)
    , dopplerEffect(true) {
    // defaultcolor is green
    setColor(colors[green]);
}

Source::Source(unsigned int xID, int sourceID, bool planewave, QString name, float x,
               float y, float orientation, QObject* parent)
    : QObject(parent)
    , sourceID(sourceID)
    , xID(xID)
    , groupID(0)
    , coordinates(x, y, orientation)
    , name(name)
    , active(false)
    , visible(true)
    , planewave(planewave)
    , recordEnabled(false)
    , readEnabled(true)
    , invertedRotation(false)
    , invertedScaling(false)
    , dopplerEffect(true) {
    // defaultcolor is green
    setColor(colors[green]);
}

Source::Source(unsigned int xID, int sourceID, bool planewave, QString name,
               SourceCoordinates coords, QObject* parent)
    : QObject(parent)
    , sourceID(sourceID)
    , xID(xID)
    , groupID(0)
    , coordinates(coords)
    , name(name)
    , active(false)
    , visible(true)
    , planewave(planewave)
    , recordEnabled(false)
    , readEnabled(true)
    , invertedRotation(false)
    , invertedScaling(false)
    , dopplerEffect(true) {
    // defaultcolor is green
    setColor(colors[green]);
}

Source::Source(const Source& source, QObject* parent) : QObject(parent) {
    sourceID = source.getID();
    xID      = source.getXID();
    groupID  = source.getGroupID();

    active                  = source.isActive();
    coordinates.x           = source.getCoordinates().x;
    coordinates.y           = source.getCoordinates().y;
    coordinates.orientation = source.getCoordinates().orientation;
    name                    = source.getName();
    visible                 = source.isVisible();
    planewave               = source.isPlanewave();
    recordEnabled           = source.isRecordEnabled();
    readEnabled             = source.isReadEnabled();
    invertedRotation        = source.hasInvertedRotation();
    invertedScaling         = source.hasInvertedScaling();
    dopplerEffect           = source.hasDopplerEffect();

    setColor(source.getColor());
}

//----------------------------end of constructors-----------------------------//

//-----------------------------------getter-----------------------------------//

int Source::getNoActiveSources() { return noActiveSources; }

int Source::getID() const { return sourceID; }

unsigned int Source::getXID() const { return xID; }

int Source::getGroupID() const { return groupID; }

GLfloat Source::getx() const { return coordinates.x; }

GLfloat Source::gety() const { return coordinates.y; }

qreal Source::getxRounded() const { return qRound(coordinates.x * 100.0) / 100.0; }

qreal Source::getyRounded() const { return qRound(coordinates.y * 100.0) / 100.0; }

GLfloat Source::getOrientation() const { return coordinates.orientation; }

qreal Source::getOrientationRounded() const {
    return qRound(coordinates.orientation * 100.0) / 100.0;
}

SourceCoordinates Source::getCoordinates() const {
    return SourceCoordinates(getx(), gety(), getOrientation());
}

SourceCoordinates Source::getCoordinatesRounded() const {
    qreal x           = getxRounded();
    qreal y           = getyRounded();
    qreal orientation = getOrientationRounded();
    return SourceCoordinates(x, y, orientation);
}

QString Source::getName() const { return name; }

const GLfloat* const Source::getColor() const { return (const GLfloat* const)color; }

bool Source::hasDopplerEffect() const { return dopplerEffect; }

bool Source::hasInvertedRotation() const { return invertedRotation; }

bool Source::hasInvertedScaling() const { return invertedScaling; }

bool Source::isActive() const { return active; }

bool Source::isVisible() const { return visible; }

bool Source::isPlanewave() const { return planewave; }

bool Source::isRecordEnabled() const { return recordEnabled; }

bool Source::isReadEnabled() const { return readEnabled; }

//-------------------------------end of getter--------------------------------//

//-----------------------------------setter-----------------------------------//

void Source::reset() {
    sourceID = xID - 1;
    groupID  = 0;

    coordinates = SourceCoordinates(0.0, 0.0, 0.0);

    name             = "";
    active           = false;
    visible          = true;
    planewave        = false;
    recordEnabled    = false;
    readEnabled      = true;
    invertedRotation = false;
    invertedScaling  = false;
    dopplerEffect    = true;

    setColor(colors[green]);
}

void Source::set(const Source& sourceWithData) {
    sourceID = sourceWithData.getID();
    groupID  = sourceWithData.getGroupID();

    active                  = sourceWithData.isActive();
    coordinates.x           = sourceWithData.getCoordinates().x;
    coordinates.y           = sourceWithData.getCoordinates().y;
    coordinates.orientation = sourceWithData.getCoordinates().orientation;
    name                    = sourceWithData.getName();
    visible                 = sourceWithData.isVisible();
    planewave               = sourceWithData.isPlanewave();
    recordEnabled           = sourceWithData.isRecordEnabled();
    readEnabled             = sourceWithData.isReadEnabled();
    invertedRotation        = sourceWithData.hasInvertedRotation();
    invertedScaling         = sourceWithData.hasInvertedScaling();
    dopplerEffect           = sourceWithData.hasDopplerEffect();

    setColor(sourceWithData.getColor());
}

void Source::setXID(unsigned int newXID) { xID = newXID; }

void Source::setx(GLfloat x) { coordinates.x = x; }

void Source::sety(GLfloat y) { coordinates.y = y; }

void Source::setPosition(GLfloat x, GLfloat y) {
    setx(x);
    sety(y);
}

void Source::setOrientation(GLfloat o) {
    if (o > 180.0) {
        while (o > 180.0) { o -= 360.0; }

        coordinates.orientation = o;
    } else if (o < -180.0) {
        while (o < -180.0) { o += 360.0; }

        coordinates.orientation = o;
    } else {
        coordinates.orientation = o;
    }
}

void Source::setCoordinates(GLfloat x, GLfloat y, GLfloat orientation) {
    setx(x);
    sety(y);
    setOrientation(orientation);
}

void Source::setCoordinates(SourceCoordinates coords) {
    setCoordinates(coords.x, coords.y, coords.orientation);
}

void Source::setColor(const GLfloat newColor[4]) {
    for (int i = 0; i < 4; ++i) {
        if (newColor[i] < 0.0 || newColor[i] > 1.0) {
            color[i] = 0.5;
        } else {
            color[i] = newColor[i];
        }
    }
}

void Source::setColor(QColor newColor) {
    color[0] = newColor.redF();
    color[1] = newColor.greenF();
    color[2] = newColor.blueF();
    color[3] = newColor.alphaF();
}

void Source::setName(QString newName) { name = newName; }

void Source::setID(int newID) { sourceID = newID; }

void Source::setGroupID(unsigned int newGroupID) { groupID = newGroupID; }

void Source::setType(bool planewave) { this->planewave = planewave; }

void Source::activate() {
    active = true;
    noActiveSources++;
}

void Source::deactivate() {
    active = false;
    reset();
    noActiveSources--;
}

void Source::setVisible(bool visible) { this->visible = visible; }

void Source::setRecordMode(bool enabled) { recordEnabled = enabled; }

void Source::toggleRecordMode() { recordEnabled = !recordEnabled; }

void Source::setReadMode(bool enabled) { readEnabled = enabled; }

void Source::setRotationDirection(bool inverted) { invertedRotation = inverted; }

void Source::setScalingDirection(bool inverted) { invertedScaling = inverted; }

void Source::setDopplerEffect(bool dopplerOn) { dopplerEffect = dopplerOn; }

//-------------------------------end of setter--------------------------------//

void Source::toggleVisible() { visible = !visible; }
