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

#include "jackringbuffer.hpp"

JackRingbuffer::JackRingbuffer(int size) { ringBuffer = jack_ringbuffer_create(size); }

JackRingbuffer::~JackRingbuffer() { jack_ringbuffer_free(ringBuffer); }

// write a void pointer. dont write NULL;
bool JackRingbuffer::write(void* ptr) {
    if (jack_ringbuffer_write_space(ringBuffer) >= sizeof(void*)) {
        jack_ringbuffer_write(ringBuffer, (char*)&ptr, sizeof(void*));
        return true;
    }

    return false;
}

void* JackRingbuffer::read() {
    void* readData;

    if (jack_ringbuffer_read_space(ringBuffer) >= sizeof(void*)) {
        jack_ringbuffer_read(ringBuffer, (char*)&readData, sizeof(void*));
        return readData;
    }

    return nullptr;
}
