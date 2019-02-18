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

#include "SnapshotNameDialog.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRegExp>
#include <QRegExpValidator>

SnapshotNameDialog::SnapshotNameDialog(bool displayOldName, const QString oldName, QWidget* parent) : QDialog(parent, Qt::Dialog) {  //|Qt::FramelessWindowHint)
    setModal(true);
    setWindowTitle("Enter Snapshotname:");

    layout        = new QVBoxLayout();
    nameLayout    = new QHBoxLayout();
    buttonsLayout = new QHBoxLayout();

    nameLabel    = new QLabel("Name");
    nameLE       = new QLineEdit();
    okButton     = new QPushButton("OK");
    cancelButton = new QPushButton("Cancel");

    nameLayout->addWidget(nameLabel);
    nameLayout->addWidget(nameLE);
    buttonsLayout->addWidget(okButton);
    buttonsLayout->addWidget(cancelButton);

    layout->addLayout(nameLayout);
    layout->addLayout(buttonsLayout);
    setLayout(layout);

    //Connect Signals and Slots
    connect(okButton,     SIGNAL(clicked()),                    this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()),                    this, SLOT(reject()));
    connect(nameLE,       SIGNAL(textEdited(const QString&)), this, SLOT(enableOKButton()));
    connect(this,         SIGNAL(accepted()),                   this, SLOT(readName()));

    //Set Validators to limit the length of the name to 15 characters
    QRegExp nameRegExp(".{1,15}");
    nameLE->setValidator(new QRegExpValidator(nameRegExp, this));

    okButton->setDisabled(true);

    if(displayOldName) {
        if(! oldName.isEmpty()) {
            nameLE->setText(oldName);
            enableOKButton();
        }
    }
}


void SnapshotNameDialog::enableOKButton() {
    if(nameLE->hasAcceptableInput()) {
        okButton->setEnabled(true);
    } else {
        okButton->setEnabled(false);
    }
}


void SnapshotNameDialog::readName() {
    newName = nameLE->text();
}
