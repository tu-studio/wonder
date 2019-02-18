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

#ifndef SNAPSHOTNAMEDIALOG_H
#define SNAPSHOTNAMEDIALOG_H

#include <QDialog>

class QHBoxLayout;
class QVBoxLayout;
class QLabel;
class QLineEdit;
class QPushButton;

//Doxygen description
/*!
 *      \brief
 *      Inputdialog used when adding snapshots.
 *
 *      \details
 *      If a snapshot is added the user is prompted to enter a name for the new snapshot. Empty entries are not allowed,
 *      so the OK-button is only enabled if acceptable input exists.
 *
 *      \author
 *      Hans-Joachim Mond
 *
 *      \date
 *      11.10.2007
 */

class SnapshotNameDialog : public QDialog {
    Q_OBJECT

public:
    SnapshotNameDialog(bool displayOldName, const QString oldName = "", QWidget* parent = 0);

    QString newName;

private slots:
    void readName();
    void enableOKButton();

private:
    QVBoxLayout* layout;
    QHBoxLayout* nameLayout;

    QLabel*    nameLabel;
    QLineEdit* nameLE;

    QHBoxLayout* buttonsLayout;
    QPushButton* okButton;
    QPushButton* cancelButton;
};

#endif // SNAPSHOTNAMEDIALOG_H
