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

#include "types.hpp"

//----------------------------------TimeStamp--------------------------------//

class TimeStamp
{
  public:
    static void initSampleRate(int newSampleRate);

    TimeStamp();
    TimeStamp(wonder_frames_t time);
    TimeStamp(float sec);

    void setTime(wonder_frames_t time);

    // get the time in samples
    wonder_frames_t getTime() const;

    float getTimeInSeconds();

    void addsec(float sec);

    wonder_frames_t operator-(TimeStamp const& other) const;
    TimeStamp& operator-=(TimeStamp const& other);
    bool operator<(TimeStamp const& other) const;
    bool operator>=(TimeStamp const& other) const;
    bool operator==(TimeStamp const& other) const;

    void show(const char* name);

  protected:
    // the value of the timestamp in samples
    wonder_frames_t time;

    static int sampleRate;
};

//-------------------------------end of TimeStamp----------------------------//

//---------------------------------TimeStampSc-------------------------------//

// special timestamp class for the scoreplayer

class TimeStampSc : public TimeStamp
{
  public:
    TimeStampSc(int period);
    TimeStampSc(wonder_frames_t time, int period);
    TimeStampSc(float sec, int period, float scaleFact = 1.0);

    bool operator<(TimeStampSc const& other) const;
    bool operator>=(TimeStampSc const& other) const;
    bool operator==(TimeStampSc const& other) const;

    void setTime(wonder_frames_t time, int wraps);

    void setPeriod(int period);

    void setLastTime(int nowTime);

    // Call this to update the timestamp at each period. This
    // wraps around and increments a counter (called wraps) at each wrap.
    // Use this function when you want to update the record  time.
    void update(wonder_frames_t nowTime);

    float getTimeInSeconds();

    void show(char* name);

  private:
    // since a uint_32 wraps at (4294967295U), only 27 hours (when using a sampling rate
    // of 44100) can be recorded. To record longer scores the timestamp wraps are stored
    // in wraps
    int wraps;

    int period;

    // used in update
    wonder_frames_t lastTime;
};

//-----------------------------end of TimeStampSc----------------------------//
