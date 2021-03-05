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

//Doxygen description
/*! \file
*      \brief
*      Some useful colordefinitions
*
*      \details
*      These are some definitions for colors which are used by the sourcewidgets and the sources3Dwidget for
*      initialization of new colors.
*
*      \author
*      Hans-Joachim Mond
*
*      \date
*      27.01.2008
*/

#ifndef COLORS_H
#define COLORS_H

#include <QGLWidget>

//!number of available initial colors
constexpr int noColors = 20;

//!names of colors for accessing the colors array
enum colorNames {
    green,
    blue,
    scarlet,
    greencopper,
    wheat,
    limegreen,
    red,
    yellow,
    huntersgreen,
    aquamarine,
    blueviolet,
    brown,
    cadetBlue,
    coral,
    cornflowerBlue,
    darkGreen,
    darkOliveGreen,
    darkOrchid,
    darkSlateBlue,
    reddish,
    white,
    black,
    gold,
    gray
};

//!color definitions for OpenGL
static GLfloat colors[][4] = {
    {0.0, 1.0, 0.0, 1.0},                       //green
    {0.0, 0.0, 1.0, 1.0},                       //blue
    {0.55, 0.09, 0.09, 1.0},                    //scarlet
    {0.32, 0.49, 0.46, 1.0},                    //greencopper
    {0.847059, 0.847059, 0.09, 0.74902},        //wheat
    {0.196078, 0.8, 0.196078, 1.0},             //limegreen
    {1.0, 0.0, 0.0, 1.0},                       //red
    {1.0, 1.0, 0.0, 1.0},                       //yellow
    {0.13, 0.37, 0.31, 1.0},                    //huntersgreen
    {0.439216, 0.858824, 0.576471, 1.0},        //aquamarine
    {0.62352, 0.372549, 0.623529, 1.0},         //blueViolet
    {0.647059, 0.164706, 0.164706, 1.0},        //brown
    {0.372549, 0.623529, 0.623529, 1.0},        //cadetBlue
    {1.0, 0.498039, 0.0, 1.0},                  //coral
    {0.258824, 0.258824, 0.435294, 1.0},        //cornflowerBlue
    {0.184314, 0.309804, 0.184314, 1.0},        //darkGreen
    {0.309804, 0.309804, 0.184314, 1.0},        //darkOliveGreen
    {0.6, 0.196078, 0.8, 1.0},                  //darkOrchid
    {0.419608, 0.137255, 0.556863, 1.0},        //darkSlateBlue
    {0.919608, 0.137255, 0.556863, 1.0},        //reddish
    {1.0, 1.0, 1.0, 1.0},                       //white
    {0.0, 0.0, 0.0, 1.0},                       //black
    {0.89164, 0.79648, 0.13648, 1.0},           //gold
    {0.6, 0.6, 0.6, 1.0}
};                      //gray

#endif //COLORS_H
