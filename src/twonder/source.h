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

#include "angle.h"
#include "delaycoeff.h"
#include "delayline.h"
#include "interpolat.h"
#include "vector2d.h"
#include "vector3d.h"

class Speaker;
class DelayCoeff;

class Source
{
  public:
    virtual ~Source();

    virtual DelayCoeff getDelayCoeff(const Speaker& spk) = 0;

    virtual DelayCoeff getTargetDelayCoeff(const Speaker& spk, wonder_frames_t blocksize) = 0;

    virtual void doInterpolationStep(wonder_frames_t blocksize) = 0;

    int getType() { return type; }

    bool hasDopplerEffect() { return dopplerEffect; }

    void setDopplerEffect(bool useDoppler) { dopplerEffect = useDoppler; }

    virtual void reset() {
        type          = 1;
        dopplerEffect = true;
    }

  protected:
    int type;  // 0 = planewave, 1 = point source
    bool dopplerEffect;
};

class PositionSource : public Source
{
  public:
    Interpolat<Vector3D> position;

    PositionSource(const Vector3D& position);

    virtual void reset() {
        position.setCurrentValue(Vector3D(0.0f, 0.0f, 0.0f));
        position.setTargetValue(Vector3D(0.0f, 0.0f, 0.0f));
        Source::reset();
    }
};

class PointSource : public PositionSource
{
  public:
    PointSource(const Vector3D& p) : PositionSource(p) {
        type          = 1;
        dopplerEffect = true;
        didFocusCalc  = false;
        wasFocused    = false;
    }
    PointSource() = delete;
    ~PointSource() = default;

    DelayCoeff getDelayCoeff(const Speaker& spk);

    DelayCoeff getTargetDelayCoeff(const Speaker& spk, wonder_frames_t blocksize);

    void doInterpolationStep(wonder_frames_t blocksize);

  private:
    DelayCoeff calcDelayCoeff(const Speaker& spk, const Vector3D& vec);

    bool isFocused(const Vector3D& src);
    bool didFocusCalc;
    bool wasFocused;
};

class PlaneWave : public PositionSource
{
  public:
    // A plane wave has an originating Position and a direction. The position is inherited
    // from PositionSource.
    Interpolat<Angle> angle;

    PlaneWave() = delete;
    ~PlaneWave() = default;
    PlaneWave(const Vector3D& position, float angle)
        : PositionSource(position), angle(angle) {
        type          = 0;
        dopplerEffect = true;
    }

    DelayCoeff getDelayCoeff(const Speaker& spk);

    DelayCoeff getTargetDelayCoeff(const Speaker& spk, wonder_frames_t blocksize);

    void doInterpolationStep(wonder_frames_t blocksize);

    virtual void reset() {
        angle.setCurrentValue(0.0f);
        angle.setTargetValue(0.0f);
        PositionSource::reset();
    }

  private:
    DelayCoeff calcDelayCoeff(const Speaker& spk, const Angle& angle);
};

class SourceAggregate
{
  public:
    SourceAggregate(Vector3D initialPos, float negDelay);

    ~SourceAggregate();

    void reset();

    DelayLine* inputline;
    Source* source;
    bool active;
    float angle;  // for preserving angle when type of source is changed back and forth
};

class SourceArray
{
  public:
    SourceArray(int noSources, float negDelay);
    ~SourceArray();

    std::vector<SourceAggregate*> array;
};
