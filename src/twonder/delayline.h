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

class DelayCoeff;

class DelayLine
{
  public:
    // This function must be called before instantiating any DelayLine objects in order
    // to setup the crossfade factors according to the runtime audio blocksize !
    static void initFadeBuffers(const unsigned int blockSize);

    enum CrossfadeType
    {
        dB3,
        dB6
    };

    // maxDelay is the maximum possible negative delay this DelayLine can handle. In
    // meters.
    DelayLine(float maxDelay = 10.0);
    ~DelayLine();

    // XXX WARNING: must write power of 2 only;
    void put(float* samples, unsigned int nsamples);

    // read samples from this delayLine, make sure to set the contents of samples to 0.0
    // before
    void get(unsigned int sampleDelay, float* samples, unsigned int nsamples);
    void get(DelayCoeff& coeff, float* samples, unsigned int nsamples);
    void get(DelayCoeff& coeff0, DelayCoeff& coeff1, float* samples,
             unsigned int nsamples);
    void getInterp(DelayCoeff& coeff0, DelayCoeff& coeff1, float* samples,
                   unsigned int nsamples);

    void getFadej(DelayCoeff& coeff0, DelayCoeff& coeff1, float* samples,
                  unsigned int nsamples, CrossfadeType fadeType = dB3);

  private:
    // The length of this buffer in samples.
    // XXX WARNING: must be power of two.
    int lineLength;
    float lineLengthF;

    float maxDelay;
    float* line;
    int writePos;
    int readPos;
    float readPosF;

    // arrays of actual blockSize ( determined at runtime ) containing
    // factors for crossfading with either a -3dB or -6dB crossover point
    static float* fadeIn3dB;
    static float* fadeIn6dB;
};
