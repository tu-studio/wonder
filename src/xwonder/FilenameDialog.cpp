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

 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "FilenameDialog.h"

#include <QLabel>
#include <QString>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QPushButton>

FilenameDialog::FilenameDialog(QWidget* parent) : QDialog(parent) {
    filename = "";

    setModal(true);
    setWindowTitle("Distributed Application!");

    layout           = new QGridLayout();
    explanationLabel = new QLabel("Not all parts of WONDER are running on the same computer.\nUnfortunately we can't provide filebrowsing because of this.\nPlease enter a filename:");
    filenameLE       = new QLineEdit();
    buttonsLayout    = new QHBoxLayout();
    okButton         = new QPushButton("Ok");
    cancelButton     = new QPushButton("Cancel");

    buttonsLayout->addWidget(okButton);
    buttonsLayout->addWidget(cancelButton);

    layout->addWidget(explanationLabel, 0, 0);
    layout->addWidget(filenameLE,       1, 0);
    layout->addLayout(buttonsLayout,    2, 0);

    setLayout(layout);

    connect(okButton,     SIGNAL(clicked()), this, SLOT(ok()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
};


void FilenameDialog::ok() {
    filename = filenameLE->text();
    accept();
}


QString FilenameDialog::getFilename() {
    return filename;
}

