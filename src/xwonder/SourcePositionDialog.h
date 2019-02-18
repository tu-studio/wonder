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

#ifndef SOURCEPOSITIONDIALOG_H
#define SOURCEPOSITIONDIALOG_H

#include <QDialog>

#include "SourceCoordinates.h"

class QGridLayout;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QPushButton;

class Source;

//Doxygen description
/*!
 *      \brief
 *      Contextdialog for explicitly setting a source position
 *
 *      \details
 *      If the user does not want to click and drag a source but instead set the position (and orientation)
 *      explicitly, this contextdialog is used. It matches the type of source and displays its current position
 *      (and orientation). Validation of inputdata is done.
 *
 *
 *      \author
 *      Hans-Joachim Mond
 *
 *      \date
 *      28.01.2008
 */

class SourcePositionDialog : public QDialog {
    Q_OBJECT

public:
    SourcePositionDialog(const Source& source, QWidget* parent = 0);

    SourceCoordinates newCoordinates;

private slots:
    void readLEs();
    void enableOKButton();

private:
    QGridLayout* layout;

    QLabel* xposLabel;
    QLabel* yposLabel;
    QLabel* orientationLabel;

    QLineEdit* xposLE;
    QLineEdit* yposLE;
    QLineEdit* orientationLE;

    QHBoxLayout* buttonsLayout;
    QPushButton* okButton;
    QPushButton* cancelButton;
}; // class SourcePositionDialog

#endif //SOURCEPOSITIONDIALOG_H
