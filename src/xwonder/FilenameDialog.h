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

#ifndef FILENAMEDIALOG_H
#define FILENAMEDIALOG_H

#include <QDialog>

class QGridLayout;
class QHBoxLayout;
class QString;
class QLabel;
class QLineEdit;
class QPushButton;

// Doxygen description
/*!
 *      \brief
 *      Inputdialog for a filename if cwonder doesn't run on localhost
 *
 *      \details
 *      If cwonder does not run on localhost, then Xwonder can't provide filebrowsing, so
 * the user is prompted to input a filename for loading and saving a project in cwonder.
 *
 *      \author
 *      Hans-Joachim Mond
 *
 *      \date
 *      11.10.2007
 */

class FilenameDialog : public QDialog
{
    Q_OBJECT

  public:
    FilenameDialog(QWidget* parent);

    QString getFilename();

  private slots:
    void ok();

  private:
    QString filename;

    QGridLayout* layout;

    QLabel* explanationLabel;

    QLineEdit* filenameLE;

    QHBoxLayout* buttonsLayout;
    QPushButton* okButton;
    QPushButton* cancelButton;
};

#endif  // FILENAMEDIALOG_H
