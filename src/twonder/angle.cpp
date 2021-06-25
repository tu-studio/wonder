/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                                                   *
 *  WONDER - Wave field synthesis Of New Dimensions of Electronic music in Realtime  *
 *  http://swonder.sourceforge.net                                                   *
 *                                                                                   *
 *                                                                                   *
 *  Technische Universität Berlin, Germany                                           *
 *  Audio Communication Group                                                        *
 *  www.ak.tu-berlin.de                                                              *
 *  Copyright 2006-2008                                                              *
 *                                                                                   *
 *                                                                                   *
 *  This program is free software; you can redistribute it and/or modify             *
 *  it under the terms of the GNU General Public License as published by             *
 *  the Free Software Foundation; either version 2 of the License, or                *
 *  (at your option) any later version.                                              *
 *                                                                                   *
 *  This program is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 *  GNU General Public License for more details.                                     *
 *                                                                                   *
 *  You should have received a copy of the GNU General Public License                *
 *  along with this program; if not, write to the Free Software                      *
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.       *
 *                                                                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "angle.h"

#include <cmath>

#include "vector3d.h"

Angle::Angle(float angle) {
    this->angle =
        (angle < 0) ? (2 * M_PI + fmodf(angle, 2 * M_PI)) : (fmodf(angle, 2 * M_PI));

    if (this->angle >= M_PI) { this->angle -= (2 * M_PI); }
}

Angle::Angle(const Angle& other) {
    angle = (other.angle < 0) ? (2 * M_PI + fmodf(other.angle, 2 * M_PI))
                              : (fmodf(other.angle, 2 * M_PI));
}

const Angle& Angle::operator=(float angle) {
    this->angle =
        (angle < 0) ? (2 * M_PI + fmodf(angle, 2 * M_PI)) : (fmodf(angle, 2 * M_PI));

    return *this;
}

const Angle& Angle::operator=(const Angle& angle) {
    this->angle =
        (angle < 0) ? (2 * M_PI + fmodf(angle, 2 * M_PI)) : (fmodf(angle, 2 * M_PI));

    return *this;
}


Angle Angle::operator-(const Angle& other) const { return Angle(angle - other.angle); }

Angle Angle::operator+(float addAngle) const { return Angle(angle + addAngle); }

Angle Angle::operator+(const Angle& other) const { return Angle(angle + other.angle); }

Angle Angle::operator*(float factor) const { return Angle(angle * factor); }

Angle::operator float() const { return angle; }

Vector3D Angle::getNormal() const { return Vector3D(cosf(angle), sinf(angle), 0); }
