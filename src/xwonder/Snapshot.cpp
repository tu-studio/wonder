/*
 * (c) Copyright 2006-7 -- Hans-Joachim Mond
 * sWONDER:
 * Wave field synthesis Of New Dimensions of Electronic music in Realtime
 * http://swonder.sourceforge.net
 *
 * created at the Technische Universitaet Berlin, Germany
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#include "Snapshot.h"


//--------------------------------constructors--------------------------------//

Snapshot::Snapshot(int snapshotID, QString name, QObject* parent) : QObject(parent),
    snapshotID(snapshotID),
    name(name) {
}

//----------------------------end of constructors-----------------------------//


//-----------------------------------getter-----------------------------------//

int Snapshot::getSnapshotID() const {
    return snapshotID;
}


QString Snapshot::getName() const {
    return name;
}


QList<unsigned int> Snapshot::getXIDs() const {
    return sources.keys();
}


Source& Snapshot::getSourceByXid(unsigned int xID) {
    // if no source with that xID does exist an invalid one (i.e. xID 0)
    // is inserted and returned
    if(sources.contains(xID)) {
        return *(sources.value(xID));
    } else {
        Source* invalidSource = new Source(0, 0, this);
        sources.insert(invalidSource->getXID(), invalidSource);
        return *(sources.value(invalidSource->getXID()));
    }
}


bool Snapshot::xIDIsUsed(unsigned int xID) const {
    if(sources.contains(xID)) {
        return true;
    } else {
        return false;
    }
}


bool Snapshot::sourceIDIsUsed(int sourceID) const {
    QHash<unsigned int, Source*>::const_iterator it;

    for(it = sources.constBegin(); it != sources.constEnd(); ++it) {
        if(it.value()->getID() == sourceID) {
            return true;
        }
    }

    return false;
}

//-------------------------------end of getter--------------------------------//


//-----------------------------------setter-----------------------------------//

void Snapshot::setSnapshot(Snapshot& snapshot) {
    // delete all sources, we want a fresh start
    while(! sources.empty()) {
        QHash<unsigned int, Source*>::iterator it = sources.begin();
        delete sources.take(it.key());
    }

    // now copy the data from the incoming snapshot
    QList<unsigned int> xIDs = snapshot.getXIDs();
    QList<unsigned int>::const_iterator it;

    for(it = xIDs.constBegin(); it != xIDs.constEnd(); ++it) {
        sources.insert(*it, new Source(snapshot.getSourceByXid(*it), this));
    }
}


void Snapshot::setName(QString newName) {
    name = newName;
}

//-------------------------------end of setter--------------------------------//


void Snapshot::addSource(const Source& source) {
    sources.insert(source.getXID(), new Source(source, this));
}


void Snapshot::deleteSource(unsigned int xID) {
    if(sources.contains(xID)) {
        delete sources.take(xID);
    }
}
