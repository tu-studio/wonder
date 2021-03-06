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

//----------------------------------includes----------------------------------//

#include "AddChannelsDialog.h"

#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QSpinBox>
#include <QWidget>

//----------------------------------------------------------------------------//

AddChannelsDialog::AddChannelsDialog(int noSourcesLeft, QWidget* parent)
    : QDialog(parent, Qt::Dialog), noSources(2) {
    setWindowTitle("Add multiple Sources");

    layout = new QGridLayout();
    QString noSourcesString =
        "Number of Sources (max: " + QString::number(noSourcesLeft) + ")";
    inputLabel  = new QLabel(noSourcesString);
    noSourcesSB = new QSpinBox();
    noSourcesSB->setRange(1, noSourcesLeft);

    okButton     = new QPushButton("OK");
    cancelButton = new QPushButton("Cancel");

    layout->addWidget(inputLabel, 0, 0, 1, 2);
    layout->addWidget(noSourcesSB, 1, 0, 1, 2);
    layout->addWidget(okButton, 2, 0);
    layout->addWidget(cancelButton, 2, 1);
    setLayout(layout);

    // connect signals and slots
    connect(noSourcesSB, SIGNAL(valueChanged(int)), this, SLOT(changeNoSources(int)));
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));

    // set a reasnonable default value
    noSourcesSB->setValue(noSources);
}

void AddChannelsDialog::changeNoSources(int newNoSources) { noSources = newNoSources; }
