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

#ifndef TIMELINEWIDGET_H
#define TIMELINEWIDGET_H

#include <QDialog>

class QCloseEvent;

class QGridLayout;
class QLabel;
class QPushButton;

#include "TimeLCDNumber.h"

//Doxygen description
/*!
 *      \brief
 *      Currently not used!
 *
 *      \details
 *      Not used and implemented yet. Will provide navigation features in scorefiles.
 *
 *      \author
 *      Hans-Joachim Mond
 *
 *      \date
 *      29.12.2007
 */

class TimelineWidget : public QDialog { //QWidget
    Q_OBJECT

public:
    TimelineWidget(QWidget* parent = 0);

public slots:
    //this will be used by the incoming time-message from scoreplayer
    void setTime(int h, int m, int s, int ms);

protected:
    void closeEvent(QCloseEvent* event);

signals:
    void closedMe();

private:
    QGridLayout* layout;
    QLabel* timeLabel;
    TimeLCDNumber* timeLCD;
    //QLabel* gridLabel;
    //QLineEdit* gridLineEdit;
    //QComboBox* gridBox;
    //QLabel* durationLabel;
    //QLineEdit* durationLineEdit;
    //QComboBox* durationBox;
    QWidget* timeInput;
};

#endif //TIMELINEWIDGET_H
