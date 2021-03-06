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

#ifndef SNAPSHOTSELECTOR_H
#define SNAPSHOTSELECTOR_H

#include <QDialog>
#include <QMap>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QLabel;
class QDoubleSpinBox;
class QCloseEvent;

class SnapshotSelectorButton;
class SnapshotNameDialog;

// Doxygen description
/*!
 *      \brief
 *      Provides buttons for selecting (setting, etc.) snapshots
 *
 *      \details
 *      Dynamic collection of buttons for taking, recalling, deleting, renaming and
 * copying snapshots. Buttons are arranged in two rows. Transition time between snapshots
 * is set by a spinbox. Transitions are not visualized in xwonder, because the other
 * WONDER-modules do not communicate about source positions constantly.
 *
 *      \author
 *      Hans-Joachim Mond
 *
 *      \date
 *      31.01.2008
 */

class SnapshotSelector : public QDialog
{
    Q_OBJECT

  public:
    SnapshotSelector(QWidget* parent = 0);

    void takeSnapshot(unsigned int snapshotId, QString name);
    void reset();

  public slots:
    void takeSnapshot();
    void copySnapshot(unsigned int snapshotId);

  protected:
    void closeEvent(QCloseEvent* event);
    void contextMenuEvent(QContextMenuEvent* event);
    void keyPressEvent(QKeyEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void showEvent(QShowEvent* event);

  signals:
    void takeSnapshotSignal(unsigned int snapshotID, QString name);
    void recallSnapshotSignal(unsigned int snapshotID, double transitionTime);
    void deleteSnapshotSignal(unsigned int snapshotID);
    void renameSnapshotSignal(unsigned int snapshotID, QString newName);
    void copySnapshotSignal(unsigned int fromID, unsigned int toID);
    void closedMyself();
    void modified();

  private slots:
    void recallSnapshot(unsigned int snapshotId);
    void renameSnapshot(unsigned int snapshotId);
    void deleteSnapshot(unsigned int snapshotId);
    void getNameFromDialog();

  private:
    void reorderButtons();

    QMap<unsigned int, SnapshotSelectorButton*> buttons;

    SnapshotSelectorButton* phantomButton;

    QGridLayout* buttonsLayout;
    QVBoxLayout* layout;
    QHBoxLayout* timeLayout;
    QLabel* timeLabel;
    QDoubleSpinBox* timeSB;

    SnapshotNameDialog* snapshotNameDialog;

    QString newSnapshotName;
    unsigned int transitionTime;

};  // class SnapshotSelector

#endif  // SNAPSHOTSELECTOR_H
