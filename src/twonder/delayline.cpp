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

#include "delayline.h"
#include "delaycoeff.h"
#include <cstdio>

#include <cmath>

DelayLine::DelayLine(float maxDelay) : maxDelay(maxDelay), writePos(0), readPos(0), readPosF(0.0) {
    lineLength  = 65536;
    lineLengthF = ((float) lineLength) - 0.5;

    // create new array of fixed length and initialize all values with 0.0
    line = new float[lineLength];

    for(int i = 0; i < lineLength; ++i) {
        line[i] = 0.0;
    }
}


DelayLine::~DelayLine() {
    delete [] line;
}



void DelayLine::put(float* samples, unsigned int nsamples) {
    // XXX: maybe use memcpy... or std::copy
    for(unsigned int i = 0; i < nsamples; ++i) {
        line[ writePos ] = samples[i];
        ++writePos;

        // wrap around if end of buffer is reached
        if(writePos >= lineLength) {
            writePos = 0;
        }
    }
}


void DelayLine::get(unsigned int sampleDelay, float* samples, unsigned int nsamples) {
    // calculate starting position
    readPos = writePos - nsamples - sampleDelay;

    // wrap around backwards if beginning of buffer is reached
    if(readPos < 0) {
        readPos += lineLength;
    }

    // actually this should never happen
    if((readPos < 0) || (readPos >= lineLength)) {
        //printf("Error! readPos = %i in DelayLine::get()\n", readPos);
        // prevent crash, audible clicks are better than complete failure
        readPos = 0;
    }

    // read block of samples
    for(unsigned int i = 0; i < nsamples; ++i) {
        samples[i] += line[ readPos ];
        ++readPos;

        // wrap around if end of buffer is reached
        if(readPos >= lineLength) {
            readPos = 0;
        }
    }
}


void DelayLine::get(DelayCoeff& coeff, float* samples, unsigned int nsamples) {
    // calculate starting position
    readPos = writePos - nsamples - coeff.getSampleDelayRounded(maxDelay);

    // wrap around backwards if beginning of buffer is reached
    if(readPos < 0) {
        readPos += lineLength;
    }

    // actually this should never happen
    if((readPos < 0) || (readPos >= lineLength)) {
        //printf("Error! readPos = %i in DelayLine::get()\n", readPos);
        // prevent crash, audible clicks are better than complete failure
        readPos = 0;
    }

    // read block of samples, scaled according to delay coefficient
    for(unsigned int i = 0; i < nsamples; ++i) {
        samples[i] += line[ readPos ] * coeff.getFactor();
        ++readPos;

        // wrap around if end of buffer is reached
        if(readPos >= lineLength) {
            readPos = 0;
        }
    }
}


// This Function does a linear interpolation between the Coefficients during the rendering.
// So this is basically able to implement movements of sources.
// Note that the resampling algorithm implemented here is bad.
// no samplevalue interpolation.

void DelayLine::get(DelayCoeff& coeff0, DelayCoeff& coeff1, float* samples, unsigned int nsamples) {
    // calculate starting position
    readPosF =  writePos - coeff0.getSampleDelay(maxDelay) - nsamples;

    // wrap around backwards if beginning of buffer is reached
    if(readPosF <= -1.0) {
        readPosF += lineLength;
    }

    // actually this should never happen
    if((readPosF <= -1.0) || (readPosF >= lineLength)) {
        //printf("Error! readPosF = %f in DelayLine::get()\n", readPosF);
        // prevent crash, audible clicks are better than complete failure
        readPosF = 0;
    }

    float readPosStepSize  = 1.0 - (((coeff1.getSampleDelay(maxDelay)) - coeff0.getSampleDelay(maxDelay)) / (nsamples));

    float factor        =   coeff0.getFactor();
    float factorsDelta = (coeff1.getFactor() - coeff0.getFactor()) / (nsamples - 1);

    for(unsigned int i = 0; i < nsamples; ++i) {
        samples[i] += line[(unsigned int)(readPosF + 0.5) ] * factor;

        readPosF   += readPosStepSize;
        factor       += factorsDelta;

        if(readPosF >= lineLengthF) {
            readPosF -= lineLength;
        }
    }
}

// This function does a linear interpolation between the coefficients during the rendering.
// So this is basically able to implement movements of sources.
// This does linear resampling. The array indizes are calculated using a modified version
// of the Bresenham algorithm. This function is faster than the function above. And the result is also
// better.

void DelayLine::getInterp(DelayCoeff& coeff0, DelayCoeff& coeff1, float* samples, unsigned int nsamples) {
    int signedNSamples = nsamples;

    float factor  = coeff0.getFactor();
    float dfactor = (coeff1.getFactor() - coeff0.getFactor()) / signedNSamples;

    int c0delay = coeff0.getSampleDelayRounded(maxDelay);
    int c1delay = coeff1.getSampleDelayRounded(maxDelay);



    // XXX: dt < 0 !!!!
    int readPos = writePos - signedNSamples - c0delay;

    if(readPos < 0) {
        readPos += lineLength;
    }

    if((readPos < 0) || (readPos >= lineLength)) {
        //printf("Error! readPos = %i in DelayLine::getInterp()\n", readPos);
        // prevent crash, audible clicks are better than complete failure
        readPos = 0;
    }

    // if no interpolation is needed, don't do it
    if(c0delay == c1delay) {
        for(unsigned int i = 0; i < nsamples; ++i) {
            samples[i] += line[ readPos ] * factor;
            ++readPos;

            if(readPos >= lineLength) {
                readPos = 0;
            }
        }

        return;
    }

    // XXX: c0 - c1 ?
    int    dt       = c0delay - c1delay + signedNSamples;
    float* readptr  = &(line[ readPos ]);
    float  interAdd = (float) dt / (float) signedNSamples;


    // #define INTERPOL_MUST_BE_1 1.0
#define INTERPOL_MUST_BE_1 0.5

    if(dt < 0) {
        while(interAdd <= -1.0) {
            interAdd += 1.0;
        }

        if(-dt <= signedNSamples) {
            float interpol = INTERPOL_MUST_BE_1;
            int   acc      = signedNSamples / 2;

            for(int i = 0; i < signedNSamples; ++i) {
                float out = *readptr;

                out     *= interpol;
                interpol = 1.0 - interpol;

                --readptr;

                if(readptr < line) {
                    readptr += lineLength;
                }

                out      += (*readptr) * interpol;
                interpol += interAdd;

                if(interpol < 0.0) {
                    interpol += 1.0;
                }

                interpol = 1.0 - interpol;

                acc += dt; // dt < 0

                if(acc < 0) {
                    acc += signedNSamples;
                } else {
                    ++readptr;

                    if(readptr >= (line + lineLength)) {
                        readptr -= lineLength;

                        // prevent crash, audible clicks are better than complete failure
                        if(readptr >= (line + lineLength)) {
                            readptr = line;
                        }
                    }
                }

                samples[i] += out * factor;
                factor       += dfactor;
            }
        } else {
            float interpol = INTERPOL_MUST_BE_1;
            int   sadd     = dt / signedNSamples + 1; // XXX:is this right? or: /(x+1) ???
            int   newDt    = (-dt) + (dt / signedNSamples) * signedNSamples;     // XXX:right? or: ( a + b ) * c ???
            int   acc      = signedNSamples / 2;

            for(int i = 0; i < signedNSamples; ++i) {
                float out = *readptr;

                out     *= interpol;
                interpol = 1.0 - interpol;

                --readptr;

                if(readptr < line) {
                    readptr += lineLength;

                    // prevent crash, audible clicks are better than complete failure
                    if(readptr < (line)) {
                        readptr = line;
                    }
                }

                out += (*readptr) * interpol;

                interpol += interAdd;

                if(interpol < 0) {
                    interpol += 1;
                }

                interpol = 1.0 - interpol;

                acc -= newDt; // dt < 0

                if(acc <= 0) {
                    acc += signedNSamples;
                    --readptr;
                }

                readptr += sadd;

                if(readptr < line) {
                    readptr += lineLength;

                    // prevent crash, audible clicks are better than complete failure
                    if(readptr < (line)) {
                        readptr = line;
                    }
                } else if(readptr >= (line + lineLength)) {
                    readptr -= lineLength;

                    // prevent crash, audible clicks are better than complete failure
                    if(readptr >= (line + lineLength)) {
                        readptr = line;
                    }
                }

                samples[i] += out * factor;
                factor       += dfactor;
            }
        }
    } else { // dt >=0
        while(interAdd >= 1.0) {
            interAdd -= 1.0;
        }

        if(dt <= signedNSamples) {

            float interpol = INTERPOL_MUST_BE_1;
            int   acc     = signedNSamples / 2;

            for(int i = 0; i < signedNSamples; ++i) {

                float out = *readptr;

                out     *= interpol;
                interpol = 1.0 - interpol;

                ++readptr;

                if(readptr >= (line + lineLength)) {
                    readptr -= lineLength;

                    // prevent crash, audible clicks are better than complete failure
                    if(readptr >= (line + lineLength)) {
                        readptr = line;
                    }
                }

                out      += (*readptr) * interpol;
                interpol += interAdd;

                if(interpol >= 1.0) {
                    interpol -= 1.0;
                }

                interpol = 1.0 - interpol;

                acc -= dt; // dt > 0

                if(acc < 0) {
                    acc += signedNSamples;
                } else {
                    --readptr;

                    if(readptr < line) {
                        readptr += lineLength;

                        // prevent crash, audible clicks are better than complete failure
                        if(readptr < (line)) {
                            readptr = line;
                        }
                    }
                }

                samples[i] += out * factor;
                factor       += dfactor;
            }
        } else {
            float interpol = INTERPOL_MUST_BE_1;
            int sadd       = dt / signedNSamples - 1; // XXX: is this right? or: /(x-1) ???
            int newDt      = (dt) - (dt / signedNSamples) * signedNSamples;     //XXX: right? or: ( a - b ) * c ???
            int acc        = signedNSamples / 2;

            for(int i = 0; i < signedNSamples; ++i) {
                float out = *readptr;

                out     *= interpol;
                interpol = 1.0 - interpol;

                readptr++;

                if(readptr >= (line + lineLength)) {
                    readptr -= lineLength;

                    // prevent crash, audible clicks are better than complete failure
                    if(readptr >= (line + lineLength)) {
                        readptr = line;
                    }
                }

                out      += (*readptr) * interpol;
                interpol += interAdd;

                if(interpol >= 1.0) {
                    interpol -= 1.0;
                }

                interpol = 1.0 - interpol;

                acc -= newDt;

                if(acc <= 0) {
                    acc += signedNSamples;
                    readptr++;
                }

                readptr += sadd;

                if(readptr >= (line + lineLength)) {
                    readptr -= lineLength;

                    // prevent crash, audible clicks are better than complete failure
                    if(readptr >= (line + lineLength)) {
                        readptr = line;
                    }
                }

                samples[i] += out * factor;
                factor       += dfactor;
            }
        }
    }
}

// Linear amplitude interpolation.
// Crossfade between the delay positions.

void DelayLine::getFadej(DelayCoeff& coeff0, DelayCoeff& coeff1, float* samples, unsigned int nsamples, CrossfadeType fadeType) {
    // determine fade type, invalid values cause a default usage of the -3dB crossover point
    float* fadeIn;

    if(fadeType == dB6) {
        fadeIn  = fadeIn6dB;
    } else { // dB3
        fadeIn  = fadeIn3dB;
    }

    int c0delay = coeff0.getSampleDelayRounded(maxDelay);
    int c1delay = coeff1.getSampleDelayRounded(maxDelay);

    float factor0  = coeff0.getFactor();
    float factor1  = coeff1.getFactor();

    int readPos0 = writePos - nsamples - c0delay;

    if(readPos0 < 0) {
        readPos0 += lineLength;
    }

    if((readPos0 < 0) || (readPos0 >= lineLength)) {
        //printf("Error! readPos0 = %i in DelayLine::getFadej()\n", readPos0);
        // prevent crash, audible clicks are better than complete failure
        readPos0 = 0;
    }

    int readPos1 = writePos - nsamples - c1delay;

    if(readPos1 < 0) {
        readPos1 += lineLength;
    }

    if((readPos1 < 0) || (readPos1 >= lineLength)) {
        //printf("Error! readPos1 = %i in DelayLine::getFadej()\n", readPos1);
        // prevent crash, audible clicks are better than complete failure
        readPos1 = 0;
    }

    // if no crossfade is needed, don't do it
    if(c0delay == c1delay) {
        for(unsigned int i = 0; i < nsamples; ++i) {
            samples[i] += line[ readPos0 ] * factor0;
            ++readPos0;

            if(readPos0 >= lineLength) {
                readPos0 = 0;
            }
        }
    } else { // we need a crossfade
        for(unsigned int i = 0; i < nsamples; ++i) {

            // actual sample value is sum of time-shifted, scaled and faded input samples
            // fade out is achieved via reversal of fade in
            samples[i] += line[ readPos0 ] * factor0 * fadeIn[ nsamples - i - 1 ];
            samples[i] += line[ readPos1 ] * factor1 * fadeIn[i];

            ++readPos0;
            ++readPos1;

            if(readPos0 >= lineLength) {
                readPos0 = 0;
            }

            if(readPos1 >= lineLength) {
                readPos1 = 0;
            }
        }
    }
}


float* DelayLine::fadeIn3dB  = NULL;
float* DelayLine::fadeIn6dB  = NULL;

void DelayLine::initFadeBuffers(const unsigned int blockSize) {
    fadeIn3dB  = new float[ blockSize ];
    fadeIn6dB  = new float[ blockSize ];

    float stepSize = 1.0 / (blockSize - 1);

    for(unsigned int i = 0; i < blockSize; i++) {
        float scaleFactor6dB =  i * stepSize;

        fadeIn6dB [i] = scaleFactor6dB;
        fadeIn3dB [i] = sqrt(scaleFactor6dB);
    }
}
