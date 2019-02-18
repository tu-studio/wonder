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

#ifndef SOURCECOORDINATES_H
#define SOURCECOORDINATES_H

#include <QGLWidget>

//Doxygen description
/*!
 *      \brief
 *      Internal coordinate system (in accordance with OpenGL)
 *
 *      \details
 *      Coordinates and orientation of a single source.
 *      WONDER uses a somewhat awkward coordinate system so a mapping to OpenGL and overall generally
 *      usefull coordinates is needed.
 *      Conventions internal to Xwonder:
 *      - origin is in the middle of the room
 *      - x-axis points to the right
 *      - y-axis points upwards
 *      - degrees increase counterclockwise
 *
 *      \author
 *      Hans-Joachim Mond
 *
 *      \date
 *      31.12.2007
 */

class SourceCoordinates {
public:
    SourceCoordinates(GLfloat initX = 0.0, GLfloat initY = 0.0, GLfloat initOrientation = 0.0)
        : x(initX), y(initY), orientation(initOrientation) {};

    SourceCoordinates mapGLCoordToWonderCoord() {
        return mapGLCoordToWonderCoord(*this);
    }

    SourceCoordinates mapWonderCoordToGLCoord() {
        return mapWonderCoordToGLCoord(*this);
    }

    static SourceCoordinates mapGLCoordToWonderCoord(SourceCoordinates glCoord) {
        //WONDER has the origin in the middle of the frontal speakerarray, y-axis points downwoard inside the room, degrees increase clockwise
        SourceCoordinates WonderCoord(0.0, 0.0, 0.0);
        WonderCoord.x = glCoord.x;
        WonderCoord.y = -(glCoord.y);
        WonderCoord.orientation = -glCoord.orientation;
        return WonderCoord;
    }

    static SourceCoordinates mapWonderCoordToGLCoord(SourceCoordinates glCoord) {
        //WONDER has the origin in the middle of the frontal speakerarray, y-axis points downwoard inside the room, degrees increase clockwise
        SourceCoordinates GLCoord(0.0, 0.0, 0.0);
        GLCoord.x = glCoord.x;
        GLCoord.y = -glCoord.y;
        GLCoord.orientation = -glCoord.orientation;
        return GLCoord;
    }

    bool operator == (const SourceCoordinates& other) {
        return (other.x == x  &&  other.y == y  &&  other.orientation == orientation);
    }

    SourceCoordinates& operator = (const SourceCoordinates& other) {
        if(this == &other) {
            return *this;
        }

        this->x = other.x;
        this->y = other.y;
        this->orientation = other.orientation;
        return *this;
    }

    GLfloat x;
    GLfloat y;
    GLfloat orientation;
};

#endif //SOURCECOORDINATES_H
