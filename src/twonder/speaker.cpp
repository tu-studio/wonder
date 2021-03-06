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

#include "speaker.h"

#include <iostream>

Speaker::Speaker(float x, float y, float z, float normalx, float normaly, float normalz,
                 float cosAlpha) {
    pos[0] = x;
    pos[1] = y;

    pos3D[0] = x;
    pos3D[1] = y;
    pos3D[2] = z;

    normal[0] = normalx;
    normal[1] = normaly;

    normal3D[0] = normalx;
    normal3D[1] = normaly;
    normal3D[2] = normalz;

    this->cosAlpha = cosAlpha;
}

SpkArray::SpkArray(std::string filename) {
    SegmentArray segarr(filename);

    std::vector<Segment*>::iterator segiter;

    for (segiter = segarr.segments.begin(); segiter != segarr.segments.end(); ++segiter) {
        addSpeakers(*segiter);
    }
}

SpkArray::~SpkArray() {
    while (!empty()) {
        delete back();
        pop_back();
    }
}

void SpkArray::addSpeakers(Segment* segment) {
    float cosAlpha           = 1.0;
    Vector3D speakerPosition = segment->start;
    Vector3D speakerSpacing(0.0, 0.0, 0.0);

    if (segment->noSpeakers > 1) {
        speakerSpacing =
            (segment->end - segment->start) * (1.0 / (segment->noSpeakers - 1));

        // Calculate energy compensation for elevations of speakers (i.e.
        // speakerSpacing[2] != 0)
        Vector2D xyVec(speakerSpacing[0], speakerSpacing[1]);
        float alpha = atanf(speakerSpacing[2] / xyVec.length());
        cosAlpha    = cosf(alpha);
    }

    for (int i = 0; i < segment->noSpeakers; ++i) {
        Speaker* spk = new Speaker(speakerPosition[0], speakerPosition[1],
                                   speakerPosition[2], segment->normal[0],
                                   segment->normal[1], segment->normal[2], cosAlpha);
        push_back(spk);
        speakerPosition += speakerSpacing;
    }
}
