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

#include "StreamClientWidget.h"

#include <QCloseEvent>
#include <QTableWidget>
#include <QSizePolicy>
#include <QVBoxLayout>

#include <QDebug>

StreamClientWidget::StreamClientWidget(QWidget* parent) : QDialog(parent) {
    layout = new QVBoxLayout();
    layout->setSizeConstraint(QLayout::SetMaximumSize);
    layout->setAlignment(Qt::AlignCenter);

    setWindowTitle("Stream clients");

    table = new QTableWidget(0, 3, this);

    table->setHorizontalHeaderLabels(QStringList() << "IP-Address" << "Port" << "Name");
    table->setSelectionBehavior(QAbstractItemView::SelectRows);

    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->setSortingEnabled(true);

    setLayout(layout);
    layout->addWidget(table);

    setMinimumSize(layout->sizeHint());

    resize(table->sizeHint() * 1.3);
}

void StreamClientWidget::insert(QString host, QString port, QString name) {
    table->setSortingEnabled(false);

    int row = table->rowCount();
    table->insertRow(row);

    QTableWidgetItem* hostItem = new QTableWidgetItem(host);
    QTableWidgetItem* portItem = new QTableWidgetItem(port);
    QTableWidgetItem* nameItem = new QTableWidgetItem(name);

    table->setItem(row, 0, hostItem);
    table->setItem(row, 1, portItem);
    table->setItem(row, 2, nameItem);
    table->setSortingEnabled(true);
}

void StreamClientWidget::remove(QString host, QString port, QString name) {
    for(int i = 0; i < table->rowCount(); ++i) {
        if(table->item(i, 0)->text() == host  &&  table->item(i, 1)->text()  == port &&  table->item(i, 2)->text() == name) {
            table->removeRow(i);
            break;
        }
    }
}

void StreamClientWidget::closeEvent(QCloseEvent* event) {
    emit closedMyself();
    event->accept();
}
