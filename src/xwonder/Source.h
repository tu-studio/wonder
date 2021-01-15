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


#ifndef SOURCE_H
#define SOURCE_H


//----------------------------------includes----------------------------------//

#include <QObject>

#include "SourceCoordinates.h"

//----------------------------------------------------------------------------//


//----------------------------------Doxygen-----------------------------------//
/*!
 *      \brief
 *      Encapsulation of a soundsource
 *
 *      \details
 *      A single soundsource, described by name, id, xid, groupId, type, position, color, visibility and
 *      record status. Xwonder uses its own id-system with xid, because the id of a source can change
 *      and thus a unique unchangeable identifier is needed.
 *
 *      \author
 *      Hans-Joachim Mond
 *
 *      \date
 *      18.07.2008
 */
//----------------------------------------------------------------------------//


class Source: public QObject {
    Q_OBJECT

public:
    // constructors
    Source(unsigned int xID, int sourceID, QObject* parent = 0);
    Source(unsigned int xID, int sourceID, bool planewave, QString name, float x, float y,
           float orientation, QObject* parent = 0);
    Source(unsigned int xID, int sourceID, bool planewave, QString name, SourceCoordinates coords,
           QObject* parent = 0);
    Source(const Source& source, QObject* parent = 0);

    void reset();

    // getter
    int getID()                               const;
    int getGroupID()                          const;
    unsigned int getXID()                     const;
    GLfloat getx()                            const;
    GLfloat gety()                            const;
    GLfloat getOrientation()                  const;
    qreal getxRounded()                       const;
    qreal getyRounded()                       const;
    qreal getOrientationRounded()             const;
    SourceCoordinates getCoordinates()        const;
    SourceCoordinates getCoordinatesRounded() const;
    QString getName()                         const;
    const GLfloat* const getColor()           const;

    static int getNoActiveSources();

    // bool getter
    bool isActive()            const;
    bool isVisible()           const;
    bool isPlanewave()         const;
    bool isRecordEnabled()     const;
    bool isReadEnabled()       const;
    bool hasInvertedRotation() const;
    bool hasInvertedScaling()  const;
    bool hasDopplerEffect()    const;

    // setter
    void set(const Source& sourceWithData);   // copy all data except xID
    void setXID(unsigned int newXID);
    void setx(GLfloat x);
    void sety(GLfloat y);
    void setPosition(GLfloat x, GLfloat y);
    void setOrientation(GLfloat orientation);
    void setCoordinates(GLfloat x, GLfloat y, GLfloat orientation);
    void setCoordinates(SourceCoordinates coords);
    void setColor(const GLfloat* const newColor);
    void setColor(QColor newColor);
    void setName(QString newName);


public slots:
    // setter slots
    void activate();
    void deactivate();
    void setID(int newID);
    void setGroupID(unsigned int newGroupID);
    void setType(bool planewave);
    void setVisible(bool visible);
    void setRecordMode(bool enabled);
    void setReadMode(bool enabled);
    void setRotationDirection(bool inverted);
    void setScalingDirection(bool inverted);
    void setDopplerEffect(bool dopplerOn);

    void toggleVisible();
    void toggleRecordMode();


private:
    // IDs
    int sourceID;         // wonder channel id
    unsigned int xID;     // xwonder internal id, unchangeable
    int groupID;          // group id of the group to which this source belongs, 0 = no group

    SourceCoordinates coordinates;
    GLfloat color[ 4 ];
    QString name;
    bool active;
    bool visible;
    bool planewave;
    bool recordEnabled;
    bool readEnabled;
    bool invertedRotation;
    bool invertedScaling;
    bool dopplerEffect;

    static int noActiveSources;

}; // class Source

#endif // SOURCE_H
