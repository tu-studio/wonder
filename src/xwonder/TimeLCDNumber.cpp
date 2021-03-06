/*
 * (c) Copyright 2006-7 -- Hans-Joachim Mond
 *
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

#include "TimeLCDNumber.h"

#include <QContextMenuEvent>
#include <QCoreApplication>
#include <QDialog>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QRegExp>
#include <QRegExpValidator>
#include <QStringList>

#include "XwonderConfig.h"

TimeLCDNumber::TimeLCDNumber(QWidget* parent) : QLCDNumber(12, parent) {
    timeLE = new QLineEdit();

    // dialog for setting the time
    timeLEDialog = new QDialog(this, Qt::Dialog | Qt::FramelessWindowHint);
    dialogLayout = new QGridLayout();
    okButton     = new QPushButton("OK");
    cancelButton = new QPushButton("Cancel");

    dialogLayout->addWidget(timeLE, 0, 0, 1, 2);
    dialogLayout->addWidget(okButton, 1, 0);
    dialogLayout->addWidget(cancelButton, 1, 1);
    timeLEDialog->setLayout(dialogLayout);

    // connect Signals and Slots
    connect(okButton, SIGNAL(clicked()), timeLEDialog, SLOT(accept()));
    connect(timeLEDialog, SIGNAL(accepted()), this, SLOT(readTimeFromDialog()));
    connect(timeLE, SIGNAL(textEdited(const QString&)), this, SLOT(enableOKButton()));
    connect(cancelButton, SIGNAL(clicked()), timeLEDialog, SLOT(reject()));

    // set validators to prevent nonsense input
    QRegExp timeRegExp(
        "[0-9]{0,2}[:]{1}[0-5]{0,1}[0-9]{0,1}[:]{1}[0-5]{0,1}[0-9]{0,1}[:]{1}[0-9]{0,3}");
    timeLE->setValidator(new QRegExpValidator(timeRegExp, this));

    // palettes for recording mode or not recording mode
    setAutoFillBackground(true);
    setSegmentStyle(QLCDNumber::Flat);
    defaultPalette = palette();
    defaultPalette.setColor(foregroundRole(), Qt::white);
    defaultPalette.setColor(backgroundRole(), Qt::black);
    recordPalette = palette();
    recordPalette.setColor(backgroundRole(), Qt::red);
    recordPalette.setColor(foregroundRole(), Qt::black);
    setPalette(defaultPalette);

    // set initial time
    parseTimeString("00:00:00:000");
    // sendTimeByOSC();
}

void TimeLCDNumber::setTime(int h, int m, int s, int ms) {
    // set the internal time to the new values
    hours        = h;
    minutes      = m;
    seconds      = s;
    milliSeconds = ms;

    display(makeTimeString());
}

QString TimeLCDNumber::makeTimeString() {
    // construct a stringrepresentation of the time for displaying
    QString timeString;
    QString temp;

    if (hours < 10) {
        temp = "0" + QString::number(hours);
    } else {
        temp = QString::number(hours);
    }

    timeString += temp;
    timeString += ":";

    if (minutes < 10) {
        temp = "0" + QString::number(minutes);
    } else {
        temp = QString::number(minutes);
    }

    timeString += temp;
    timeString += ":";

    if (seconds < 10) {
        temp = "0" + QString::number(seconds);
    } else {
        temp = QString::number(seconds);
    }

    timeString += temp;
    timeString += ":";

    if (milliSeconds < 100) {
        temp = "0" + QString::number(milliSeconds);

        if (milliSeconds < 10) { temp.prepend("0"); }
    } else {
        temp = QString::number(milliSeconds);
    }

    timeString += temp;

    return timeString;
}

void TimeLCDNumber::setRecordMode(bool recEnabled) {
    // if in record mode show black numbers on red background
    // if not in record mode use default colors
    if (recEnabled) {
        setPalette(recordPalette);
        // setSegmentStyle(QLCDNumber::Filled);
    } else {
        setPalette(defaultPalette);
        // setSegmentStyle(QLCDNumber::Outline);
    }
}

void TimeLCDNumber::contextMenuEvent(QContextMenuEvent* event) {
    timeLE->setText(makeTimeString());
    timeLEDialog->move(event->globalPos());
    timeLEDialog->exec();
}

void TimeLCDNumber::enableOKButton() {
    QRegExp timeRegExp(
        "[0-9]{2}[:]{1}[0-5]{1}[0-9]{1}[:]{1}[0-5]{1}[0-9]{1}[:]{1}[0-9]{3}");
    bool inputOK = timeRegExp.exactMatch(timeLE->text());

    // if(timeLE->hasAcceptableInput())
    if (inputOK) {
        okButton->setEnabled(true);
    } else {
        okButton->setEnabled(false);
    }
}

void TimeLCDNumber::readTimeFromDialog() {
    parseTimeString(timeLE->text());
    sendTimeByOSC();
}

void TimeLCDNumber::sendTimeByOSC() {
    // construct a float containing the current time which can be send to scoreplayer
    // RegExp made sure this is a valid stringrepresentation of time

    // float newTime = ( hours        * 3600   ) +
    //                ( minutes      *   60   ) +
    //                ( seconds               ) +
    //                ( milliSeconds / 1000.f );

    // send OSCMessage to scoreplayer
    if (!xwConf->runWithoutCwonder) {
        lo_send(xwConf->cwonderAddr, "/WONDER/score/newtime", "iiii", hours, minutes,
                seconds, milliSeconds);
    }

    // lo_send( xwConf->cwonderAddr, "/WONDER/score/newtime", "f", newTime );
}

void TimeLCDNumber::wheelEvent(QWheelEvent* event) {
    // set up variable for internal calculations regarding wrap arounds
    int h  = hours;
    int m  = minutes;
    int s  = seconds;
    int ms = milliSeconds;

    int delta = event->angleDelta().y() / (8 * 15);

    // check over which element we are and set new value with check for wrap arounds
    int x = event->position().x();

    if (x <= 28) {  // hours
        h += delta;

        if (h < 0) {
            h = 0;
        } else if (h > 99) {
            h = 99;
        }
    } else if (x > 35 && x <= 55) {  // minutes
        m += delta;

        if (m < 0) {
            if (h == 0) {
                m = 0;
            } else {
                h--;
                m += 60;
            }
        } else if (m >= 60) {
            if (h == 99) {
                m = 59;
            } else {
                h++;
                m %= 60;
            }
        }
    } else if (x > 62 && x <= 82) {  // seconds
        s += delta;

        if (s < 0) {
            if (m == 0) {
                if (h == 0) {
                    s = 0;
                } else {
                    h--;
                    m = 59;
                    s += 60;
                }
            } else {
                m--;
                s += 60;
            }
        } else if (s >= 60) {
            if (m == 59) {
                if (h == 99) {
                    s = 59;
                } else {
                    h++;
                    m = 0;
                    s %= 60;
                }
            } else {
                m++;
                s %= 60;
            }
        }
    } else if (x > 90) {  // milliseconds
        // if( x > 107 )
        ms += (delta * 40);
        // else
        //   ms += ( delta * 10 );

        if (ms < 0) {
            if (s == 0) {
                if (m == 0) {
                    if (h == 0) {
                        ms = 0;
                    } else {
                        h--;
                        m = 59;
                        s = 59;
                        ms += 1000;
                    }
                } else {
                    m--;
                    s = 59;
                    ms += 1000;
                }
            } else {
                s--;
                ms += 1000;
            }
        } else if (ms >= 1000) {
            if (s == 59) {
                if (m == 59) {
                    if (h == 99) {
                        ms = 999;
                    } else {
                        h++;
                        m = 0;
                        s = 0;
                        ms %= 1000;
                    }
                } else {
                    m++;
                    s = 0;
                    ms %= 1000;
                }
            } else {
                s++;
                ms %= 1000;
            }
        }
    } else {  // do nothing
        return;
    }

    setTime(h, m, s, ms);
    sendTimeByOSC();
}

void TimeLCDNumber::parseTimeString(QString time) {
    // construct a float containing the current time which can be send to scoreplayer
    // RegExp made sure this is a valid stringrepresentation of time
    // QStringList timeTokens = timeLE->text().split( ":" );
    QStringList timeTokens = time.split(":");

    hours        = timeTokens[0].toInt();
    minutes      = timeTokens[1].toInt();
    seconds      = timeTokens[2].toInt();
    milliSeconds = timeTokens[3].toInt();

    display(time);
}
