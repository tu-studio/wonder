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

#include "commandqueue.h"


//---------------------------------Command------------------------------------//

Command::~Command() {
}


void Command::execute() {
}


Command::Command() : timeStamp(0.f) {
}


Command::Command(TimeStamp timeStamp) : timeStamp(timeStamp) {
}


bool Command::operator < (const Command& other) {
    return timeStamp < other.timeStamp;
}


TimeStamp Command::getTimeStamp() const {
    return timeStamp;
}

//-----------------------------end of Command---------------------------------//



//-------------------------------CommandList----------------------------------//

CommandList::CommandList() : std::list<Command*>() {
}


CommandList::CommandList(Command* cmd) : std::list<Command*>() {
    push_back(cmd);
}


CommandList::~CommandList() {
    while(!empty()) {
        delete front();
        pop_front();
    }
}

//----------------------------end of CommandList------------------------------//



//------------------------------CommandQueue----------------------------------//

CommandQueue::CommandQueue() {
    pthread_mutex_init(&mutex, NULL);
}


CommandQueue::~CommandQueue() {
    pthread_mutex_destroy(&mutex);
}


void CommandQueue::put(Command* command) {
    CommandList* commandList = new CommandList(command);

    pthread_mutex_lock(&mutex);

    while(!write(reinterpret_cast<void*>(commandList))) {
        std::this_thread::sleep_for(std::chrono::microseconds(100));
    }

    pthread_mutex_unlock(&mutex);
}


CommandList* CommandQueue::get() {
    return reinterpret_cast<CommandList*>(read());
}

//---------------------------end of CommandQueue------------------------------//



//--------------------------------FreeQueue-----------------------------------//

FreeQueue::FreeQueue() {
    pthread_mutex_init(&mutex, NULL);
}


FreeQueue::~FreeQueue() {
    pthread_mutex_destroy(&mutex);
}


void FreeQueue::put(CommandList* commandList) {
    // This tries to put the list of commands onto the free Queue.
    // If the queue is full, then the list will not get freed producing a memory leak.
    //
    // XXX: For safety reasons it is better to free the object in that case.
    //      An audible click is better than the whole system breaking down due to memory shortage

    if(!write(reinterpret_cast<void*>(commandList))) {
        delete commandList;
    }
}


void FreeQueue::flush() {
    CommandList* commandListToDelete;

    pthread_mutex_lock(&mutex);

    while((commandListToDelete = reinterpret_cast<CommandList*>(read())) != NULL) {
        delete commandListToDelete;
    }

    pthread_mutex_unlock(&mutex);
}

//-----------------------------end of FreeQueue-------------------------------//
