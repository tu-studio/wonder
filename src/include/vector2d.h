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

#include <iostream>
#include <valarray>

class Vector2D
{
  public:
    Vector2D();
    Vector2D(const Vector2D&);
    Vector2D(float x, float y);

    float& operator[](int i) { return i == 0 ? v0 : v1; }

    float operator[](int i) const { return i == 0 ? v0 : v1; }

    float length() const { return std::sqrt(v0 * v0 + v1 * v1); }

    void set(const float x, const float y) {
        v0 = x;
        v1 = y;
    }

    Vector2D& operator=(const Vector2D& other) {
        v0 = other[0];
        v1 = other[1];
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& out, const Vector2D& vec) {
        out << vec.v0 << " | " << vec.v1;
        return out;
    }

  private:
    float v0, v1;
};

inline Vector2D::Vector2D() {
    v0 = 0.0;
    v1 = 0.0;
}

inline Vector2D::Vector2D(float x, float y) {
    v0 = x;
    v1 = y;
}

inline Vector2D::Vector2D(const Vector2D& src) {
    v0 = src.v0;
    v1 = src.v1;
}

inline float operator*(const Vector2D& a, const Vector2D& b) {
    return a[0] * b[0] + a[1] * b[1];
}

inline Vector2D operator*(const float f, const Vector2D& b) {
    return Vector2D(f * b[0], f * b[1]);
}

inline Vector2D operator*(const Vector2D& b, const float f) {
    return Vector2D(f * b[0], f * b[1]);
}

inline Vector2D operator+(const Vector2D& a, const Vector2D& b) {
    Vector2D res;
    res[0] = a[0] + b[0];
    res[1] = a[1] + b[1];

    return res;
}

inline Vector2D operator-(const Vector2D& a, const Vector2D& b) {
    Vector2D res;
    res[0] = a[0] - b[0];
    res[1] = a[1] - b[1];

    return res;
}
