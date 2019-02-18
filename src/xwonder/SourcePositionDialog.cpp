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


#include "SourcePositionDialog.h"
#include "Source.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRegExp>
#include <QRegExpValidator>


SourcePositionDialog::SourcePositionDialog(const Source& source, QWidget* parent)
    : QDialog(parent, Qt::Dialog) {
    setModal(true);
    setWindowTitle(source.getName());

    xposLabel        = new QLabel("x [m]:");
    yposLabel        = new QLabel("y [m]:");
    orientationLabel = new QLabel("Orientation [deg]:");
    xposLabel->       setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    yposLabel->       setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    orientationLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);

    //initialize LineEdits with values of current source
    newCoordinates                 = source.getCoordinatesRounded();
    SourceCoordinates mappedCoords = SourceCoordinates::mapGLCoordToWonderCoord(newCoordinates);

    // round to two decimals ( thus accuracy is cm )
    //qreal xRounded = qRound( mappedCoords.x * 100.0 ) / 100.0 ;
    //qreal yRounded = qRound( mappedCoords.y * 100.0 ) / 100.0 ;
    xposLE        = new QLineEdit(QString::number(mappedCoords.x));
    yposLE        = new QLineEdit(QString::number(mappedCoords.y));
    orientationLE = new QLineEdit(QString::number(mappedCoords.orientation));

    layout = new QGridLayout();
    layout->addWidget(xposLabel,  0, 0);
    layout->addWidget(xposLE,    0, 1);
    layout->addWidget(yposLabel, 1, 0);
    layout->addWidget(yposLE,    1, 1);

    if(source.isPlanewave()) {
        layout->addWidget(orientationLabel, 2, 0);
        layout->addWidget(orientationLE,    2, 1);
    }

    buttonsLayout = new QHBoxLayout();
    okButton      = new QPushButton("OK");
    cancelButton  = new QPushButton("Cancel");
    buttonsLayout->addWidget(okButton);
    buttonsLayout->addWidget(cancelButton);

    layout->addLayout(buttonsLayout, 4, 0, 1, 2);

    setLayout(layout);

    //Connect Signals and Slots
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
    connect(xposLE, SIGNAL(textEdited(const QString&)), this, SLOT(enableOKButton()));
    connect(yposLE, SIGNAL(textEdited(const QString&)), this, SLOT(enableOKButton()));
    connect(this, SIGNAL(accepted()), this, SLOT(readLEs()));
    connect(orientationLE, SIGNAL(textEdited(const QString&)), this, SLOT(enableOKButton()));

    //Set Validators to prevent nonsense input
    QRegExp xyPosRegExp("[-]{0,1}[0-9]{1,3}([.][0-9]{1,2}){0,1}");
    xposLE->setValidator(new QRegExpValidator(xyPosRegExp, this));
    yposLE->setValidator(new QRegExpValidator(xyPosRegExp, this));

    QRegExp orientationRegExp("[-]{0,1}[0-9]{1,3}([.][0-9]){0,1}");
    orientationLE->setValidator(new QRegExpValidator(orientationRegExp, this));
}


void SourcePositionDialog::enableOKButton() {
    if(xposLE->hasAcceptableInput() && yposLE->hasAcceptableInput() && orientationLE->hasAcceptableInput()) {
        okButton->setEnabled(true);
    } else {
        okButton->setDisabled(true);
    }
}

void SourcePositionDialog::readLEs() {
    newCoordinates = SourceCoordinates::mapWonderCoordToGLCoord(SourceCoordinates(xposLE->text().toFloat(), yposLE->text().toFloat(), orientationLE->text().toFloat()));
}
