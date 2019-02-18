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

#include "TimelineWidget.h"
#include <QGridLayout>
#include <QCloseEvent>
#include <QLabel>
#include <QPushButton>

TimelineWidget::TimelineWidget(QWidget* parent) : QDialog(parent) { //QWidget(parent)
    setWindowTitle("Scoretime");
    layout = new QGridLayout();

    timeLabel = new QLabel("Position:");
    timeLCD =  new TimeLCDNumber();

    timeLCD->setAutoFillBackground(true);
    QPalette pal = timeLCD->palette();
    pal.setColor(timeLCD->backgroundRole(), Qt::black);
    timeLCD->setPalette(pal);

    layout->addWidget(timeLabel, 0, 0);
    layout->addWidget(timeLCD, 0, 1);

    //gridLabel = new QLabel("Time resolution:");
    //gridLineEdit = new QLineEdit();
    ////QRegExp gridRegExp_min(
    ////QRegExp gridRegExp_s(
    ////QRegExp gridRegExp_ms(
    //gridBox = new QComboBox();
    //gridBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    //gridBox->addItem("min");
    //gridBox->addItem("s");
    //gridBox->addItem("ms");
    //gridBox->addItem("Off");

    //durationLabel = new QLabel("Duration:");
    //durationLineEdit = new QLineEdit();
    //QRegExp timeRegExp("[1-9][0-9]{0,2}[.][0-9]{1,3}");
    //durationLineEdit->setValidator(new QRegExpValidator(timeRegExp,this));
    //durationBox = new QComboBox();
    //durationBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    //durationBox->addItem("h");
    //durationBox->addItem("min");
    //durationBox->addItem("s");


    //layout->addWidget(gridLabel,0,0);
    //layout->addWidget(gridLineEdit,0,1);
    //layout->addWidget(gridBox,0,2);
    //layout->addWidget(durationLabel,1,0);
    //layout->addWidget(durationLineEdit,1,1);
    //layout->addWidget(durationBox,1,2);

    setLayout(layout);
}

void TimelineWidget::setTime(int h, int m, int s, int ms) {
    //construct a stringrepresentation of the time for displaying
    QString timeString;
    QString temp;

    if(h < 10) {
        temp = "0" + QString::number(h);
    } else {
        temp = QString::number(h);
    }

    timeString += temp;
    timeString += ":";

    if(m < 10) {
        temp = "0" + QString::number(m);
    } else {
        temp = QString::number(m);
    }

    timeString += temp;
    timeString += ":";

    if(s < 10) {
        temp = "0" + QString::number(s);
    } else {
        temp = QString::number(s);
    }

    timeString += temp;
    timeString += ":";

    if(ms < 100) {
        temp = "0" + QString::number(ms);

        if(ms < 10) {
            temp.prepend("0");
        }
    } else {
        temp = QString::number(ms);
    }

    timeString += temp;

    //timeLCD->setTime(timeString);
}

void TimelineWidget::closeEvent(QCloseEvent* event) {
    emit closedMe();
    event->accept();
}

//#include <QApplication>
//int main(int argc, char* argv[])
//{
//    QApplication app(argc, argv);
//
//    TimelineWidget t;
//    t.show();
//    return app.exec();
//}
