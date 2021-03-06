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

#ifndef XWONDERCONFIG_H
#define XWONDERCONFIG_H

//----------------------------------includes----------------------------------//

#include <lo/lo.h>

#include <QObject>
#include <QPointF>
#include <QString>
#include <QVector>
#include <string>

//----------------------------------------------------------------------------//

//----------------------------------Doxygen-----------------------------------//
/*!
 *      \brief
 *      Global configuration for xwonder
 *
 *      \details
 *      Global configuration data used in Xwonder, most important cwonders IP address and
 * port.<br> If given it parses the commandline arguments, otherwise it uses default
 * values.<br> Argument names and values are checked and validated.<br>
 *
 *      \author
 *      Hans-Joachim Mond
 *
 *      \date
 *      06.08.2008
 */
//----------------------------------------------------------------------------//

class XwonderConfig : public QObject
{
    Q_OBJECT

  public:
    // constructors
    XwonderConfig(int argc, char* argv[], QObject* parent = 0);

    lo_address cwonderAddr;
    std::string cwonderHost;
    std::string cwonderPort;
    std::string xwonderPort;
    int maxNoSources;
    bool verbose;
    bool runWithoutCwonder;
    bool projectOnlyMode;

    int cwonderPingTimeout;  // in milliseconds

    QString roomName;

    // the name of this application, which is
    // used when connecting to cwonder
    QString name;

    // Roompolygon stored as 2D-vertices
    QVector<QPointF> roomPoints;

  private:
    void parseArgs(int argc, char* argv[]);
    void abort(QString message);

    // we need classmembers and not local objects to return a pointer to
    // (i.e. no ".c_str()" on local stringobjects)
    std::string cwonderHostTempString;
    std::string cwonderPortTempString;
    std::string xwonderPortTempString;

};  // class XwonderGlobalConfig

extern XwonderConfig* xwConf;

#endif  // XWONDERCONFIG_H
