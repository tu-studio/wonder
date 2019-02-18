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

#include <chrono>
#include <list>
#include <thread>

#include "jackringbuffer.h"
#include "pthread.h"
#include "timestamp.h"



class Command {
public:
    Command();
    Command(TimeStamp timeStamp);

    virtual ~Command();

    virtual void execute();

    bool operator < (const Command& other);

    TimeStamp getTimeStamp() const;

private:
    TimeStamp timeStamp;
};



class CommandList : public std::list<Command*> {
public:
    CommandList();
    CommandList(Command* commandd);

    ~CommandList();
};



class CommandQueue : private JackRingbuffer {
public:
    CommandQueue();

    ~CommandQueue();

    void put(Command* command);

    CommandList* get();

private:
    pthread_mutex_t mutex;
};



class FreeQueue : private JackRingbuffer {
public:
    FreeQueue();

    ~FreeQueue();

    void put(CommandList* commandList);

    void flush();

private:
    pthread_mutex_t mutex;
};
