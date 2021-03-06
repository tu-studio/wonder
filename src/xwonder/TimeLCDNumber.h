/*
 * (c) Copyright 2006-7 -- Simon Schampijer, Hans-Joachim Mond
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

#ifndef TIMELCDNUMBER_H
#define TIMELCDNUMBER_H

#include <QLCDNumber>
#include <QString>

class QGridLayout;
class QPushButton;
class QLineEdit;
class QContextMenuEvent;

// Doxygen description
/*!
 *      \brief
 *      Timedisplay synchronized to connected sequencer via scoreplayer
 *
 *      \details
 *      To navigate a score in the scoreplayer module this display shows the current
 * positione in time in format h:m:s:frames. It is editable via a context menu, by
 * mousewheel or by click'n'drag.
 *
 *      \author
 *      Hans-Joachim Mond
 *
 *      \date
 *      16.07.2000
 */

class TimeLCDNumber : public QLCDNumber
{
    Q_OBJECT
  public:
    TimeLCDNumber(QWidget* parent = 0);
    void setRecordMode(bool recEnabled);

  public slots:
    void setTime(int h, int m, int s, int ms);

  protected:
    void contextMenuEvent(QContextMenuEvent* event);

  private slots:
    void readTimeFromDialog();
    void sendTimeByOSC();
    void enableOKButton();

  protected:
    void wheelEvent(QWheelEvent* event);

  private:
    void parseTimeString(QString time);
    QString makeTimeString();

    QLineEdit* timeLE;

    // palettes for recording mode or not recording mode
    QPalette defaultPalette;
    QPalette recordPalette;
    // editing dialog
    QDialog* timeLEDialog;
    QGridLayout* dialogLayout;
    QPushButton* okButton;
    QPushButton* cancelButton;

    // the actual time that is displayed
    int hours;
    int minutes;
    int seconds;
    int milliSeconds;
};

#endif  // TIMELCDNUMBER_H
