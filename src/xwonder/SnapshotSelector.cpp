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

#include "SnapshotSelector.h"

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMenu>
#include <QVBoxLayout>

#include "SnapshotNameDialog.h"
#include "SnapshotSelectorButton.h"

SnapshotSelector::SnapshotSelector(QWidget* parent) : QDialog(parent) {
    newSnapshotName = "";

    setWindowTitle("Snapshot-Selector");
    setMinimumSize(300, 100);
    // setSizeGripEnabled( false );

    layout        = new QVBoxLayout();
    buttonsLayout = new QGridLayout();
    timeLayout    = new QHBoxLayout();

    timeLabel = new QLabel("Transition time [s]:");
    timeSB    = new QDoubleSpinBox(this);
    timeSB->setMaximum(3600.0);  // I guess 1h is more than enough time...
    timeSB->setValue(0.0);

    timeLayout->addWidget(timeLabel);
    timeLayout->addWidget(timeSB);

    layout->addLayout(timeLayout, 1);
    layout->addLayout(buttonsLayout, 3);

    snapshotNameDialog = nullptr;
    setLayout(layout);

    phantomButton = new SnapshotSelectorButton(0, "phantom");
}

void SnapshotSelector::takeSnapshot(unsigned int snapshotID, QString name) {
    // creat new button, make connections and show it

    // align buttons in rows of 2
    int row    = buttons.count() / 2;
    int column = buttons.count() % 2;

    SnapshotSelectorButton* newButton =
        new SnapshotSelectorButton(snapshotID, name, this);

    buttons.insert(snapshotID, newButton);

    connect(newButton, SIGNAL(clicked(unsigned int)), this,
            SLOT(recallSnapshot(unsigned int)));
    connect(newButton, SIGNAL(renameSnapshotSignal(unsigned int)), this,
            SLOT(renameSnapshot(unsigned int)));
    connect(newButton, SIGNAL(copySnapshotSignal(unsigned int)), this,
            SLOT(copySnapshot(unsigned int)));
    connect(newButton, SIGNAL(takeSnapshotSignal(unsigned int, QString)), this,
            SIGNAL(takeSnapshotSignal(unsigned int, QString)));
    connect(newButton, SIGNAL(deleteMeSignal(unsigned int)), this,
            SLOT(deleteSnapshot(unsigned int)));

    if (column == 1) {
        buttonsLayout->removeWidget(phantomButton);
        buttonsLayout->addWidget(newButton, row, column);
        setMinimumSize(300, sizeHint().height() + 50);
        phantomButton->hide();
        adjustSize();
    } else {
        buttonsLayout->addWidget(newButton, row, column);
        buttonsLayout->addWidget(phantomButton, row, column + 1);
        phantomButton->hide();
        adjustSize();
    }
}

void SnapshotSelector::recallSnapshot(unsigned int snapshotID) {
    emit recallSnapshotSignal(snapshotID, timeSB->value());
}

void SnapshotSelector::renameSnapshot(unsigned int snapshotID) {
    if (!buttons.contains(snapshotID)) { return; }

    SnapshotSelectorButton* snapshotButton = buttons.value(snapshotID);

    QString oldName = snapshotButton->getName();

    newSnapshotName    = "";
    snapshotNameDialog = new SnapshotNameDialog(true, oldName, this);
    connect(snapshotNameDialog, SIGNAL(accepted()), this, SLOT(getNameFromDialog()));
    snapshotNameDialog->exec();

    // if user cancels the namedialog do not add a new snapshot
    if (newSnapshotName.isEmpty()) { return; }

    snapshotButton->setName(newSnapshotName);

    emit renameSnapshotSignal(snapshotID, newSnapshotName);
}

void SnapshotSelector::reorderButtons() {
    QMap<unsigned int, SnapshotSelectorButton*>::iterator iter;
    int i = 0;

    for (iter = buttons.begin(); iter != buttons.end(); ++iter, ++i) {
        buttonsLayout->removeWidget(*iter);
        buttonsLayout->addWidget(*iter, i / 2, i % 2);
    }
}

void SnapshotSelector::deleteSnapshot(unsigned int snapshotID) {
    if (!buttons.contains(snapshotID)) { return; }

    SnapshotSelectorButton* temp = buttons.value(snapshotID);

    buttonsLayout->removeWidget(temp);
    buttons.take(snapshotID)->deleteLater();

    reorderButtons();
    adjustSize();
    emit deleteSnapshotSignal(snapshotID);
}

void SnapshotSelector::takeSnapshot() {
    // getting a new name for the new Snapshot
    newSnapshotName    = "";
    snapshotNameDialog = new SnapshotNameDialog(false, "", this);
    connect(snapshotNameDialog, SIGNAL(accepted()), this, SLOT(getNameFromDialog()));
    snapshotNameDialog->exec();

    // if user cancels the namedialog do not add a new snapshot
    if (newSnapshotName.isEmpty()) { return; }

    // setting up the id of the new snapshot
    // use the first one that is not used
    unsigned int snapshotID = 1;

    while (buttons.contains(snapshotID)) {
        ++snapshotID;

        // check for snapshotID wraparound
        if (snapshotID == 0) { return; }
    }

    takeSnapshot(snapshotID, newSnapshotName);

    emit takeSnapshotSignal(snapshotID, newSnapshotName);
}

void SnapshotSelector::copySnapshot(unsigned int fromID) {
    if (!buttons.contains(fromID)) { return; }

    unsigned int toID = 1;

    while (buttons.contains(toID)) {
        ++toID;

        // check for newSnapshotID wraparound
        if (toID == 0) { return; }
    }

    takeSnapshot(toID, buttons.value(fromID)->getName());
    emit copySnapshotSignal(fromID, toID);
}

void SnapshotSelector::getNameFromDialog() {
    newSnapshotName = snapshotNameDialog->newName;
}

void SnapshotSelector::contextMenuEvent(QContextMenuEvent* event) {
    QMenu menu(this);
    QAction* takeSnapshotAct = menu.addAction("Take new Snapshot");
    connect(takeSnapshotAct, SIGNAL(triggered()), this, SLOT(takeSnapshot()));
    menu.exec(event->globalPos());
}

void SnapshotSelector::keyPressEvent(QKeyEvent* event) {
    QApplication::sendEvent(parent(), event);
}

void SnapshotSelector::mousePressEvent(QMouseEvent* event) { setFocus(); }

void SnapshotSelector::showEvent(QShowEvent* event) {
    setFocus();
    adjustSize();
    QDialog::showEvent(event);
}

void SnapshotSelector::reset() {
    QList<SnapshotSelectorButton*> list = buttons.values();

    while (!list.empty()) {
        buttonsLayout->removeWidget(list.last());
        list.takeLast()->deleteLater();
    }

    buttons.clear();
}

void SnapshotSelector::closeEvent(QCloseEvent* event) {
    emit closedMyself();
    event->accept();
}
