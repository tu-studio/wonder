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

#include <jack/jack.h>

#include <vector>

#include "speakersegment.h"
#include "vector2d.h"
#include "vector3d.h"

class Speaker
{
  public:
    Speaker(float x, float y, float z, float normalx, float normaly, float normalz,
            float cosAlpha);

    float* outPtr;

    const Vector2D& getPos() const { return pos; }

    const Vector3D& get3DPos() const { return pos3D; }

    const Vector2D& getNormal() const { return normal; }

    const Vector3D& get3DNormal() const { return normal3D; }

    const float getCosAlpha() const { return cosAlpha; }

  private:
    Vector2D pos;
    Vector3D pos3D;
    Vector2D normal;
    Vector3D normal3D;
    float cosAlpha;
};

class SpkArray
{
  public:
    SpkArray(std::string filename);
    ~SpkArray();

    std::vector<Speaker*> array;

  private:
    void addSpeakers(Segment* segment);
};
