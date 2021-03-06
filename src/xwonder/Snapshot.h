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

#ifndef SNAPSHOT_H
#define SNAPSHOT_H

//----------------------------------includes----------------------------------//

#include <QHash>
#include <QObject>

#include "Source.h"

//----------------------------------------------------------------------------//

//----------------------------------Doxygen-----------------------------------//
/*!
 *      \brief
 *      Named container for data about multiple sources
 *
 *      \details
 *      Description of multiple sources arranged in one snapshot. Snapshots are named and
 * have a unique id. This class is needed because cwonder does not fully communicate about
 * all aspects of the sources in the snapshots, i.e. how many sources are in the snapshot
 * and which could be discarded in respect to the previous snapshot.
 *
 *      \author
 *      Hans-Joachim Mond
 *
 *      \date
 *      03.04.2008
 */
//----------------------------------------------------------------------------//

class Snapshot : public QObject
{
    Q_OBJECT

  public:
    // constructors
    Snapshot(int snapshotID, QString name, QObject* parent = 0);

    // getter
    int getSnapshotID() const;
    QString getName() const;
    QList<unsigned int> getXIDs() const;
    Source& getSourceByXid(unsigned int xID);

    // bool getter
    bool xIDIsUsed(unsigned int xID) const;
    bool sourceIDIsUsed(int sourceID) const;

    // setter
    void setSnapshot(Snapshot& newSnapshot);
    void setName(QString newName);

    void addSource(const Source&);
    void deleteSource(unsigned int xID);

  private:
    int snapshotID;
    QString name;

    QHash<unsigned int, Source*> sources;  // keys are the xIDs

};  // class Snapshot

#endif  // SSNAPSHOT_H
