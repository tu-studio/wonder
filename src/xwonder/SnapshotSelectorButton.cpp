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

#include "SnapshotSelectorButton.h"

#include <QAction>
#include <QContextMenuEvent>
#include <QKeyEvent>
#include <QMenu>

SnapshotSelectorButton::SnapshotSelectorButton(unsigned int snapshotID,
                                               const QString& name, QWidget* parent)
    : QPushButton(name, parent) {
    this->name       = name;
    this->snapshotID = snapshotID;
    connect(this, SIGNAL(clicked(bool)), this, SLOT(emitClicked(bool)));
}

unsigned int SnapshotSelectorButton::getSnapshotID() const { return snapshotID; }

QString SnapshotSelectorButton::getName() const { return name; }

void SnapshotSelectorButton::setName(QString newName) {
    name = newName;
    setText(name);
}

void SnapshotSelectorButton::contextMenuEvent(QContextMenuEvent* event) {
    QMenu menu(this);
    QAction* takeSnapshotAct = menu.addAction("Take Snapshot");
    QAction* deleteMeAct     = menu.addAction("Delete this Snapshot");
    menu.addSeparator();  // visually separate the destructive actions above
    QAction* renameAct       = menu.addAction("Rename this Snapshot");
    QAction* copySnapshotAct = menu.addAction("Copy this Snapshot");

    connect(takeSnapshotAct, SIGNAL(triggered()), this, SLOT(takeSnapshot()));
    connect(deleteMeAct, SIGNAL(triggered()), this, SLOT(deleteMe()));
    connect(renameAct, SIGNAL(triggered()), this, SLOT(renameSnapshot()));
    connect(copySnapshotAct, SIGNAL(triggered()), this, SLOT(copySnapshot()));

    menu.exec(event->globalPos());
}

void SnapshotSelectorButton::keyPressEvent(QKeyEvent* event) { event->ignore(); }

void SnapshotSelectorButton::takeSnapshot() { emit takeSnapshotSignal(snapshotID, name); }

void SnapshotSelectorButton::copySnapshot() { emit copySnapshotSignal(snapshotID); }

void SnapshotSelectorButton::renameSnapshot() { emit renameSnapshotSignal(snapshotID); }

void SnapshotSelectorButton::deleteMe() { emit deleteMeSignal(snapshotID); }

void SnapshotSelectorButton::emitClicked(bool boolnotused) { emit clicked(snapshotID); }
