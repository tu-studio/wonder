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

#include "SourceGroup.h"

#include <QGLWidget>

#include "Colors.h"

SourceGroup::SourceGroup(int groupID, QObject* parent)
    : QObject(parent), active(false), groupID(groupID), coordinates(0.0, 0.0, 0.0) {
    // defaultcolor is red
    setColor(colors[red]);
}

void SourceGroup::reset() { deactivate(); }

void SourceGroup::setGroupID(int groupID) { this->groupID = groupID; }

bool SourceGroup::isActive() const { return active; }

void SourceGroup::activate() { active = true; }

void SourceGroup::deactivate() {
    active = false;

    // reset to default values
    coordinates = SourceCoordinates(0.0, 0.0, 0.0);
    setColor(colors[red]);
    sourceXIDs.clear();
}

int SourceGroup::getGroupID() const { return groupID; }

GLfloat SourceGroup::getx() const { return coordinates.x; }

qreal SourceGroup::getxRounded() const { return qRound(coordinates.x * 100.0) / 100.0; }

void SourceGroup::setx(GLfloat x) { coordinates.x = x; }

GLfloat SourceGroup::gety() const { return coordinates.y; }

qreal SourceGroup::getyRounded() const { return qRound(coordinates.y * 100.0) / 100.0; }

void SourceGroup::sety(GLfloat y) { coordinates.y = y; }

GLfloat SourceGroup::getOrientation() const { return coordinates.orientation; }

qreal SourceGroup::getOrientationRounded() const {
    return qRound(coordinates.orientation * 100.0) / 100.0;
}

void SourceGroup::setOrientation(GLfloat o) {
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

void SourceGroup::setPosition(GLfloat x, GLfloat y) {
    setx(x);
    sety(y);
}

SourceCoordinates SourceGroup::getCoordinates() const {
    return SourceCoordinates(getx(), gety(), getOrientation());
}

SourceCoordinates SourceGroup::getCoordinatesRounded() const {
    qreal x           = getxRounded();
    qreal y           = getyRounded();
    qreal orientation = getOrientationRounded();
    return SourceCoordinates(x, y, orientation);
}

void SourceGroup::setCoordinates(GLfloat x, GLfloat y, GLfloat orientation) {
    setx(x);
    sety(y);
    setOrientation(orientation);
}

void SourceGroup::setCoordinates(SourceCoordinates coords) {
    setCoordinates(coords.x, coords.y, coords.orientation);
}

const GLfloat* SourceGroup::getColor() const { return (const GLfloat* const)color; }

void SourceGroup::setColor(const GLfloat newColor[4]) {
    for (int i = 0; i < 4; ++i) {
        if (newColor[i] < 0.0 || newColor[i] > 1.0) {
            color[i] = 0.0;
        } else {
            color[i] = newColor[i];
        }
    }
}

void SourceGroup::setColor(const QColor newColor) {
    color[0] = newColor.redF();
    color[1] = newColor.greenF();
    color[2] = newColor.blueF();
    color[3] = newColor.alphaF();
}

bool SourceGroup::containsXID(unsigned int xID) { return sourceXIDs.contains(xID); }

QList<unsigned int> SourceGroup::getXIDs() const { return sourceXIDs; }

void SourceGroup::addSource(unsigned int xID) {
    if (!sourceXIDs.contains(xID)) { sourceXIDs.append(xID); }
}

void SourceGroup::deleteSource(unsigned int xID) {
    int i = sourceXIDs.indexOf(xID);

    if (i != -1) { sourceXIDs.removeAt(i); }
}
