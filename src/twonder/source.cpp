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

#include "source.h"

#include "delaycoeff.h"
#include "listener.h"
#include "listener_array.h"
#include "speaker.h"
#include "twonder_config.h"

Source::~Source() {
    // nothing
}

PositionSource::PositionSource(const Vector3D& p) : position(p) {
    // nothing
}

PointSource::~PointSource() {
    // nothing
}

DelayCoeff PointSource::getDelayCoeff(const Speaker& speaker, ListenerArray& listeners) {
    return calcDelayCoeff(speaker, position.getCurrentValue(), listeners);
}

DelayCoeff PointSource::getTargetDelayCoeff(const Speaker& speaker,
                                            wonder_frames_t blocksize,
                                            ListenerArray& listeners) {
    return calcDelayCoeff(speaker, position.getTargetValue(blocksize), listeners);
}

void PointSource::doInterpolationStep(wonder_frames_t blocksize) {
    position.doInterpolationStep(blocksize);
}

DelayCoeff PointSource::calcDelayCoeff(const Speaker& speaker, const Vector3D& sourcePos,
                                       ListenerArray& listeners) {
    // rendering currently depends on the position of only one listener (listenerID = 0)
    Listener* listener   = dynamic_cast<Listener*>(listeners.at(0));
    Vector2D listenerPos = listener->getPos();

    // calculate the distance between listener and speaker
    Vector2D lisToSpkVec = speaker.getPos() - listenerPos;
    float spkFocusLimit  = lisToSpkVec.length();

    Vector3D srcToSpkVec   = speaker.get3DPos() - sourcePos;
    float normalProjection = srcToSpkVec * speaker.get3DNormal();
    float srcToSpkDistance = srcToSpkVec.length();
    float delay            = srcToSpkDistance;
    float cosphi           = normalProjection / srcToSpkDistance;

    // define a circular area around the speakers in which we adjust the amplitude factor
    // to get a smooth transition when moving through the speakers ( e.g. from focussed to
    // non-focussed sources )
    float transitionRadius = twonderConf->speakerDistance * 1.5;  // XXX: empirical value

    // if source is in front of speaker
    if (normalProjection < 0.0) {
        // don't render this source if the distance between source and speaker is bigger
        // than the distance between listener and speaker
        if (srcToSpkDistance > spkFocusLimit) { return DelayCoeff(0.0, 0.0); }

        // don't render this source if it in front of a this speaker
        // but is not a focussed source
        if ((!isFocused(sourcePos)) && (srcToSpkDistance > transitionRadius)) {
            return DelayCoeff(0.0, 0.0);
        }

        // if rendering focussed sources we need to use a "negative delay",
        // i.e. make use of a certain amount of already added pre delay
        // and so we don't get any phase inversion we only use positve numbers
        // for our calculations
        delay            = -delay;
        cosphi           = -cosphi;
        normalProjection = -normalProjection;
    }

    float amplitudeFactor = 0.0;

    // calculate amplitudefactor according to being in- or outside the transition area
    // around the speakers
    if (srcToSpkDistance > transitionRadius) {
        amplitudeFactor =
            (sqrtf(twonderConf->reference / (twonderConf->reference + normalProjection)))
            * (cosphi / sqrtf(srcToSpkDistance));
    } else {
        float behind =
            sqrtf(twonderConf->reference / (twonderConf->reference + transitionRadius))
            / sqrtf(transitionRadius);
        float focuss =
            sqrtf(twonderConf->reference / (twonderConf->reference - transitionRadius))
            / sqrtf(transitionRadius);

        amplitudeFactor = behind
                          + (transitionRadius - normalProjection) / (2 * transitionRadius)
                                * (focuss - behind);
    }

    return DelayCoeff(delay, amplitudeFactor * speaker.getCosAlpha());
}

bool PointSource::isFocused(const Vector3D& sourcePos) const {
    if (twonderConf->ioMode == IOM_ALWAYSOUT) { return false; }

    if (twonderConf->ioMode == IOM_ALWAYSIN) { return true; }

    int noPoints = twonderConf->renderPolygon.size();
    float xSrc   = sourcePos[0];
    float ySrc   = sourcePos[1];
    float xnew;
    float ynew;
    float xold;
    float yold;
    float x1;
    float y1;
    float x2;
    float y2;
    bool inside = false;

    xold = twonderConf->renderPolygon[noPoints - 1][0];
    yold = twonderConf->renderPolygon[noPoints - 1][1];

    for (int i = 0; i < noPoints; ++i) {
        xnew = twonderConf->renderPolygon[i][0];
        ynew = twonderConf->renderPolygon[i][1];

        if (xnew > xold) {
            x1 = xold;
            y1 = yold;
            x2 = xnew;
            y2 = ynew;
        } else {
            x1 = xnew;
            y1 = ynew;
            x2 = xold;
            y2 = yold;
        }

        if ((xSrc > xnew) == (xSrc <= xold)
            && ((ySrc - y1) * (x2 - x1) < (y2 - y1) * (xSrc - x1))) {
            inside = !inside;
        }

        xold = xnew;
        yold = ynew;
    }

    return inside;
}

PlaneWave::~PlaneWave() {
    // nothing
}

DelayCoeff PlaneWave::getDelayCoeff(const Speaker& speaker, ListenerArray& listeners) {
    return calcDelayCoeff(speaker, angle.getCurrentValue());
}

DelayCoeff PlaneWave::getTargetDelayCoeff(const Speaker& speaker,
                                          wonder_frames_t blocksize,
                                          ListenerArray& listeners) {
    return calcDelayCoeff(speaker, angle.getTargetValue(blocksize));
}

void PlaneWave::doInterpolationStep(wonder_frames_t blocksize) {
    angle.doInterpolationStep(blocksize);
}

DelayCoeff PlaneWave::calcDelayCoeff(const Speaker& speaker, const Angle& ang) {
    Vector3D diff = speaker.get3DPos() - position.getCurrentValue();
    float cospsi  = (ang.getNormal() * diff) / diff.length();
    float delay   = diff.length() * cospsi;

    // factor is determined by angle between speaker normal and direction of the plane
    // wave
    float factor = speaker.get3DNormal() * ang.getNormal();

    if (factor <= 0.0) {
        return DelayCoeff(0.0, 0.0);
    } else {
        return DelayCoeff(delay, factor * speaker.getCosAlpha() * twonderConf->planeComp);
    }
}

SourceAggregate::SourceAggregate(Vector3D initialPos, float negDelay) {
    source    = new PointSource(initialPos);
    inputline = new DelayLine(negDelay);
    active    = false;
    angle     = 0.0;
}

SourceAggregate::~SourceAggregate() {
    delete inputline;
    delete source;
}

void SourceAggregate::reset() {
    source->reset();
    angle = 0.0f;
}

SourceArray::SourceArray(int noSources, float negDelay) {
    for (int i = 0; i < noSources; ++i) {
        Vector3D defaultSourcePos(0.0f, 1.0f, 2.0f);
        SourceAggregate* temp = new SourceAggregate(defaultSourcePos, negDelay);
        push_back(temp);
    }
}

SourceArray::~SourceArray() {
    while (!empty()) {
        delete back();
        pop_back();
    }
}
