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

#include "commandqueue.hpp"
#include "types.hpp"

template<class T>
class Interpolat
{
  public:
    Interpolat(const T& value)
        : currentValue(value), targetValue(value), targetTime(0.f) {}

    const T& getCurrentValue() const { return currentValue; }

    const T getTargetValue(wonder_frames_t blocksize) const {
        if (targetTime == (wonder_frames_t)0) {
            return targetValue;
        } else {
            return currentValue
                   + (targetValue - currentValue)
                         * ((float)blocksize / (float)targetTime.getTime());
        }
    }

    void doInterpolationStep(wonder_frames_t blocksize) {
        if (targetTime == (wonder_frames_t)0) {
            currentValue = targetValue;
        } else {
            currentValue = currentValue
                           + (targetValue - currentValue)
                                 * ((float)blocksize / (float)targetTime.getTime());
            targetTime -= blocksize;
        }
    }

    void setTargetValue(const T& value, TimeStamp transitionTime = 0.f) {
        targetValue = value;
        targetTime  = transitionTime;
    }

    void setCurrentValue(const T& value) {
        currentValue = value;
        targetTime   = (wonder_frames_t)0;
    }

  private:
    T currentValue;
    T targetValue;

    TimeStamp targetTime;
};
