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

#pragma once

#include <cmath>



class Vector3D {

public:
    Vector3D();
    Vector3D(const Vector3D& initValues);
    Vector3D(float x, float y, float z);
    float& operator[](int i) {
        return (i == 0) ? v0 : ((i == 1) ? v1 : v2);
    }

    float operator[](int i) const {
        return (i == 0) ? v0 : ((i == 1) ? v1 : v2);
    }

    float length() const {
        return std::sqrt(v0 * v0 + v1 * v1 + v2 * v2);
    }

    void set(const float x, const float y, const float z) {
        v0 = x;
        v1 = y;
        v2 = z;
    }

    float x() {
        return v0;
    }

    float y() {
        return v1;
    }

    float z() {
        return v2;
    }

private:
    float v0, v1, v2;
};


inline Vector3D::Vector3D() {
    v0 = 0.0;
    v1 = 0.0;
    v2 = 2.0;
}

inline Vector3D::Vector3D(float x, float y, float z) {
    v0 = x;
    v1 = y;
    v2 = z;
}

inline Vector3D::Vector3D(const Vector3D& src) {
    v0 = src.v0;
    v1 = src.v1;
    v2 = src.v2;
}

inline float operator * (const Vector3D& a, const Vector3D& b) {
    return a[ 0 ] * b[ 0 ] + a[ 1 ] * b[ 1 ] + a[ 2 ] * b[ 2 ];
}

inline Vector3D operator * (const float f, const Vector3D& b) {
    return Vector3D(f * b[ 0 ], f * b[ 1 ], f * b[ 2 ]);
}

inline Vector3D operator * (const Vector3D& b, const float f) {
    return Vector3D(f * b[ 0 ], f * b[ 1 ], f * b[ 2 ]);
}

inline Vector3D operator + (const Vector3D& a, const Vector3D& b) {
    Vector3D res;
    res[ 0 ] = a[ 0 ] + b[ 0 ];
    res[ 1 ] = a[ 1 ] + b[ 1 ];
    res[ 2 ] = a[ 2 ] + b[ 2 ];

    return res;
}

inline Vector3D& operator += (Vector3D& a, const Vector3D& b) {
    a[ 0 ] += b[ 0 ];
    a[ 1 ] += b[ 1 ];
    a[ 2 ] += b[ 2 ];

    return a;
}

inline Vector3D operator - (const Vector3D& a, const Vector3D& b) {
    Vector3D res;
    res[ 0 ] = a[ 0 ] - b[ 0 ];
    res[ 1 ] = a[ 1 ] - b[ 1 ];
    res[ 2 ] = a[ 2 ] - b[ 2 ];

    return res;
}
