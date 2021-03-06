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

#include "rtcommandengine.h"

#include <algorithm>

RTCommandEngine::RTCommandEngine() : commandQueue(), freeQueue(), accumulatedCommands() {
    commandsToBeFreed = nullptr;
}

RTCommandEngine::~RTCommandEngine() {
    if (commandsToBeFreed) { delete commandsToBeFreed; }
}

void RTCommandEngine::put(Command* cmd) {
    commandQueue.put(cmd);
    freeQueue.flush();
}

void RTCommandEngine::mergeIncomingCommands() {
    CommandList* queuedCommandList;

    while ((queuedCommandList = commandQueue.get()) != nullptr) {
        accumulatedCommands.merge(*queuedCommandList);

        if (commandsToBeFreed == nullptr) {
            commandsToBeFreed = queuedCommandList;
        } else {
            if (commandsToBeFreed->empty()) {
                freeQueue.put(queuedCommandList);
            } else {
                freeQueue.put(commandsToBeFreed);
                commandsToBeFreed = queuedCommandList;
            }
        }
    }
}

CommandList* RTCommandEngine::getDueCommands(TimeStamp timeStamp) {
    if ((!accumulatedCommands.empty()) && (commandsToBeFreed != nullptr)) {
        CommandList::iterator it = accumulatedCommands.begin();

        while (it != accumulatedCommands.end() && ((*it)->getTimeStamp() < timeStamp)) {
            ++it;
        }

        if (it != accumulatedCommands.begin()) {
            returnValueList.splice(returnValueList.end(), accumulatedCommands,
                                   accumulatedCommands.begin(), it);
            return &returnValueList;
        } else {
            return nullptr;
        }
    } else {
        return nullptr;
    }
}

void RTCommandEngine::scheduleCommandListForDeletion(CommandList* commandList) {
    commandsToBeFreed->splice(commandsToBeFreed->end(), *commandList,
                              commandList->begin(), commandList->end());
}

void RTCommandEngine::processCommand(Command* command) { command->execute(); }

void RTCommandEngine::evaluateCommands(TimeStamp timeStamp) {
    mergeIncomingCommands();

    CommandList* commandList = getDueCommands(timeStamp);

    if (commandList) {
        // XXX: evaluate commands that came in first first, basically what a queue should
        // do,
        //      but the queue used here is actually a LIFO ringbuffer, so we have to
        //      reverse the list of commands -> ugly
        commandList->reverse();
        std::for_each(commandList->begin(), commandList->end(), processCommand);
        scheduleCommandListForDeletion(commandList);
    }
}
