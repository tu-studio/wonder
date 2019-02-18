/*

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

#ifndef SNAPSHOTSELECTORBUTTON_H
#define SNAPSHOTSELECTORBUTTON_H

#include <QPushButton>

class QContextMenuEvent;
class QKeyEvent;

//Doxygen description
/*!
 *      \brief
 *      A pushbutton for the SnapshotSelector
 *
 *      \details
 *      This is a extended QPushbutton with a unique snapshot-id associated with it. It has a
 *      contextmenu with which snapshots can be set, renamed, copied and deleted. It emits signals
 *      accordingly.
 *
 *      \author
 *      Hans-Joachim Mond
 *
 *      \date
 *      09.04.2008
 */

class SnapshotSelectorButton : public QPushButton {
    Q_OBJECT

public:
    SnapshotSelectorButton(unsigned int snapshotID, const QString& name, QWidget* parent = 0);

    unsigned int getSnapshotID() const;

    QString getName() const;
    void    setName(QString newName);

protected:
    void contextMenuEvent(QContextMenuEvent* event);
    void keyPressEvent(QKeyEvent*         event);

signals:
    void clicked(unsigned int snapshotID);
    void takeSnapshotSignal(unsigned int snapshotID, QString name);
    void renameSnapshotSignal(unsigned int snapshotID);
    void copySnapshotSignal(unsigned int snapshotID);
    void deleteMeSignal(unsigned int snapshotID);

private slots:
    void emitClicked(bool boolnotused);
    void copySnapshot();
    void renameSnapshot();
    void takeSnapshot();
    void deleteMe();

private:
    unsigned int snapshotID;
    QString      name;

}; // class SnapshotSelectorButton


#endif // SNAPSHOTSELECTORBUTTON_H
