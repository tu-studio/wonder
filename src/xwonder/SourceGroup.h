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

#ifndef SOURCEGROUP_H
#define SOURCEGROUP_H

//----------------------------------includes----------------------------------//

#include <QObject>

#include "SourceCoordinates.h"

//----------------------------------------------------------------------------//

// Doxygen description
/*!
 *      \brief
 *      List of sources for grouping.
 *
 *      \details
 *      Sources can be group to coordinate movement. Groups are identified by their unique
 * group ID. Membership of sources is noted via their xids in a list.
 *
 *      \author
 *      Hans-Joachim Mond
 *
 *      \date
 *      29.04.2008
 */

class SourceGroup : public QObject
{
    Q_OBJECT
  public:
    SourceGroup(int groupID, QObject* parent = 0);

    void reset();

    bool isActive() const;
    int getGroupID() const;
    SourceCoordinates getCoordinates() const;
    SourceCoordinates getCoordinatesRounded() const;
    GLfloat getx() const;
    GLfloat gety() const;
    qreal getxRounded() const;
    qreal getyRounded() const;
    GLfloat getOrientation() const;
    qreal getOrientationRounded() const;
    void setGroupID(int groupID);
    void setCoordinates(GLfloat x, GLfloat y, GLfloat orientation);
    void setCoordinates(SourceCoordinates coords);
    void setPosition(GLfloat x, GLfloat y);
    void setx(GLfloat x);
    void sety(GLfloat y);
    void setOrientation(GLfloat o);
    const GLfloat* getColor() const;
    void setColor(const GLfloat* const newColor);
    void setColor(const QColor newColor);
    bool containsXID(unsigned int xID);
    QList<unsigned int> getXIDs() const;
    // QString getName() const;
    // void setName(QString newName);

  public slots:
    void activate();
    void deactivate();
    void addSource(unsigned int xID);
    void deleteSource(unsigned int xID);

  private:
    bool active;
    int groupID;
    SourceCoordinates coordinates;
    GLfloat color[4];
    // QString name;
    QList<unsigned int> sourceXIDs;

};  // class SourceGroup

#endif  // SOURCEGROUP_H
