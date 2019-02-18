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


#ifndef ADDCHANNELSDIALOG_H
#define ADDCHANNELSDIALOG_H


//----------------------------------includes----------------------------------//

#include <QDialog>

class QWidget;
class QPushButton;
class QGridLayout;
class QLabel;
class QSpinBox;

//----------------------------------------------------------------------------//


//----------------------------------Doxygen-----------------------------------//
/*!
 *      \brief
 *      Convenience dialog for adding multiple channels at once.
 *
 *      \details
 *      This dialog uses a rangelimited SpinBox to set the public member noSources which should be read
 *      by the widget executing this dialog if accepted.
 *
 *      \author
 *      Hans-Joachim Mond
 *
 *      \date
 *      20.01.2008
 */
//----------------------------------------------------------------------------//


class AddChannelsDialog : public QDialog {
    Q_OBJECT

public:
    // constructors
    AddChannelsDialog(int noSourcesLeft, QWidget* parent = 0);

    int noSources;

private slots:
    void changeNoSources(int newNoSources);

private:
    QGridLayout* layout;
    QLabel*      inputLabel;
    QSpinBox*    noSourcesSB;
    QPushButton* okButton;
    QPushButton* cancelButton;

}; // class AddChannelsDialog

#endif //ADDCHANNELSDIALOG_H
