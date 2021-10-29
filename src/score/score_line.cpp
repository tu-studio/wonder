/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *                                                                                   *
 *  WONDER - Wave field synthesis Of New Dimensions of Electronic music in Realtime  *
 *  http://swonder.sourceforge.net                                                   *
 *                                                                                   *
 *                                                                                   *
 *  Technische Universität Berlin, Germany                                           *
 *  Audio Communication Group                                                        *
 *  www.ak.tu-berlin.de                                                              *
 *  Copyright 2006-2008                                                              *
 *                                                                                   *
 *                                                                                   *
 *  This program is free software; you can redistribute it and/or modify             *
 *  it under the terms of the GNU General Public License as published by             *
 *  the Free Software Foundation; either version 2 of the License, or                *
 *  (at your option) any later version.                                              *
 *                                                                                   *
 *  This program is distributed in the hope that it will be useful,                  *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
 *  GNU General Public License for more details.                                     *
 *                                                                                   *
 *  You should have received a copy of the GNU General Public License                *
 *  along with this program; if not, write to the Free Software                      *
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.       *
 *                                                                                   *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#include "score_line.h"

#include <iostream>

using std::cout;
using std::endl;
using std::string;
using std::vector;
using std::ostream;



ScoreLine::ScoreLine()
{
    timestamp = 0;
    method    = "";
}


bool ScoreLine::operator < ( const ScoreLine& sl ) const
{
    if( timestamp < sl.timestamp )
        return true;
    else
        return false;
}           


bool ScoreLine::operator == ( const ScoreLine& sl ) const
{
    if(    timestamp    == sl.timestamp
        && method       == sl.method
        && intArgs[ 0 ] == sl.intArgs[ 0 ] ) 
        return true;

    return false;
}

ostream& operator << ( ostream& out, const ScoreLine& sl ) 
{
    out << "method = " << sl.method << endl;

    out << "timestamp = " << sl.timestamp << endl;

    out << "integer arguments: " << endl;
    for( vector< int >::const_iterator it = sl.intArgs.begin(); it != sl.intArgs.end(); ++it )
        out << *it << endl;

    out << "floating point arguments: " << endl;
    for( vector< float >::const_iterator it = sl.floatArgs.begin(); it != sl.floatArgs.end(); ++it )
        out << *it << endl;

    out << "string arguments: " << endl;
    for( vector< string >::const_iterator it = sl.stringArgs.begin(); it != sl.stringArgs.end(); ++it )
        out << *it << endl;

    return out;
}

