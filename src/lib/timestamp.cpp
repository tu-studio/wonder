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

#include "timestamp.hpp"

#include <iostream>

#include "constants.hpp"

//----------------------------------TimeStamp--------------------------------//

int TimeStamp::sampleRate = 0;

void TimeStamp::initSampleRate(int newSampleRate) { sampleRate = newSampleRate; }

TimeStamp::TimeStamp() : time(0) {}

TimeStamp::TimeStamp(wonder_frames_t time) : time(time) {}

TimeStamp::TimeStamp(float sec) { time = wonder_frames_t(sec * sampleRate); }

void TimeStamp::setTime(wonder_frames_t time) { this->time = time; }

wonder_frames_t TimeStamp::getTime() const { return time; }

float TimeStamp::getTimeInSeconds() {
    if (sampleRate == 0) { return 0.0; }

    return ((float)time) / sampleRate;
}

void TimeStamp::addsec(float sec) {
    wonder_frames_t samples = wonder_frames_t(sec * sampleRate);

    if (samples >= WONDER_MAX_FRAMES / 2) {
        std::cout << std::endl
                  << "[V-TimeStamp::addsamp] The number of samples (" << samples
                  << ") is bigger than WONDER_MAX_FRAMES/2" << std::endl;
    } else {
        time += samples;
    }
}

bool TimeStamp::operator<(TimeStamp const& other) const {
    if (this->time < other.time) {
        if (other.time < (this->time + (WONDER_MAX_FRAMES / 2))) { return true; }
    } else {
        if (this->time > (other.time - (WONDER_MAX_FRAMES / 2))) { return true; }
    }

    return false;
}

wonder_frames_t TimeStamp::operator-(TimeStamp const& other) const {
    TimeStamp tempTimeStamp;

    if (time >= other.time) {
        tempTimeStamp.time = time - other.time;
    } else {
        tempTimeStamp.time = 0;
    }

    return tempTimeStamp.time;
}

TimeStamp& TimeStamp::operator-=(TimeStamp const& other) {
    if (time >= other.time) {
        time = time - other.time;
    } else {
        time = 0;
    }

    return *this;
}

bool TimeStamp::operator>=(TimeStamp const& other) const { return (!(*this < other)); }

bool TimeStamp::operator==(TimeStamp const& other) const { return time == other.time; }

void TimeStamp::show(const char* name) {
    std::cout << name << "timestamp in samples: " << time
              << "  in seconds: " << (float)time / sampleRate << std::endl;
}

//-------------------------------end of TimeStamp----------------------------//

//---------------------------------TimeStampSc-------------------------------//

TimeStampSc::TimeStampSc(int period)
    : TimeStamp((wonder_frames_t)0), wraps(0), period(period), lastTime(0) {}

TimeStampSc::TimeStampSc(wonder_frames_t time, int period)
    : TimeStamp(time), wraps(0), period(period), lastTime(0) {}

TimeStampSc::TimeStampSc(float sec, int period, float scaleFactor)
    : wraps(0), period(period), lastTime(0) {
    float tsec   = sec * scaleFactor;
    float maxsmp = WONDER_MAX_FRAMES / sampleRate;

    while (tsec >= maxsmp) {
        tsec -= maxsmp;
        wraps++;
    }

    time = wonder_frames_t(tsec * sampleRate);
}

bool TimeStampSc::operator<(TimeStampSc const& other) const {
    if (this->wraps < other.wraps) {
        return true;
    } else {
        if (time < other.time) {
            return true;
        } else {
            return false;
        }
    }
}

bool TimeStampSc::operator>=(TimeStampSc const& other) const {
    return (!(*this < other));
}

bool TimeStampSc::operator==(TimeStampSc const& other) const {
    return ((time == other.time) && (wraps == other.wraps));
}

void TimeStampSc::setTime(wonder_frames_t time, int wraps) {
    this->time  = time;
    this->wraps = wraps;
}

void TimeStampSc::setPeriod(int period) { this->period = period; }

void TimeStampSc::setLastTime(int nowTime) { lastTime = nowTime; }

void TimeStampSc::update(wonder_frames_t nowTime) {
    int tmpper = 0;

    if (nowTime < lastTime) {
        std::cout << std::endl
                  << "[V-TimeStampSc::update] throw away nowtime=" << nowTime
                  << " lasttime=" << lastTime << std::endl;
    } else {
        tmpper = nowTime - lastTime;

        if (time >= WONDER_MAX_FRAMES - tmpper) { wraps++; }

        if (tmpper != period) {
            std::cout << std::endl
                      << "[V-TimeStampSc::update] Out of sync: " << tmpper << std::endl;
        }

        time += tmpper;
        lastTime = nowTime;
    }
}

float TimeStampSc::getTimeInSeconds() {
    float seconds = 0.0;

    for (int tmh = wraps; tmh > 0; --tmh) { seconds += WONDER_MAX_FRAMES / sampleRate; }

    seconds += (float)time / sampleRate;
    return seconds;
}

void TimeStampSc::show(char* name) {
    std::cout << name << "  time ( in samples ): " << time
              << "  in seconds: " << (float)time / sampleRate
              << "  lasttime: " << lastTime << " wraps: " << wraps << std::endl;
}

//-----------------------------end of TimeStampSc----------------------------//
