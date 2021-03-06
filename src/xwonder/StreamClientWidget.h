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

#ifndef STREAMCLIENTWIDGET_H
#define STREAMCLIENTWIDGET_H

#include <QDialog>

class QCloseEvent;
class QTableWidget;
class QVBoxLayout;

class StreamClientWidget : public QDialog
{
    Q_OBJECT

  public:
    StreamClientWidget(QWidget* parent = 0);

  public slots:
    void insert(QString host, QString port, QString name);
    void remove(QString host, QString port, QString name);

  protected:
    void closeEvent(QCloseEvent* event);

  signals:
    void closedMyself();

  private:
    QVBoxLayout* layout;
    QTableWidget* table;
};

#endif  // STREAMCLIENTWIDGET_H
