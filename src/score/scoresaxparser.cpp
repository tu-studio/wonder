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

#include "scoresaxparser.h"

#include "project.h"

#include <iostream>
#include <fstream>
#include <sstream>

using std::istringstream;
using std::multiset;
using std::cout;
using std::endl;


ScoreSaxParser::ScoreSaxParser( multiset<ScoreLine>* sc_score, Scenario* sc_orc ) : xmlpp::SaxParser()
{
    score = sc_score;
    orc   = sc_orc;
    set_validate( true );
}

ScoreSaxParser::~ScoreSaxParser()
{
}

void ScoreSaxParser::on_start_element( const Glib::ustring& name, const AttributeList& attributes )
{
    ScoreLine sl;
    xmlpp::SaxParser::AttributeList::const_iterator iter = attributes.begin();

    // parse orchestra node
    if( name == "src" )
    {
        int _id;
        istringstream iss( iter->value );
        iss >> std::dec >> _id;
        orc->sourcesVector[ _id ].id = _id; 
        iter++;

        istringstream iss2( iter->value );
        iss2 >> std::dec >> orc->sourcesVector[ _id ].type;
        iter++;

        istringstream iss3( iter->value );
        iss3 >> std::dec >> orc->sourcesVector[ _id ].pos[ 0 ];
        iter++;

        istringstream iss4( iter->value );
        iss4 >> std::dec >> orc->sourcesVector[ _id ].pos[ 1 ];
        iter++;

        istringstream iss9( iter->value );
        iss9 >> std::dec >> orc->sourcesVector[ _id ].angle;
        iter++;

    } // end of orchestra node parsing
    // now parse the score events
    else if( name == "pos" )
    {
        sl.method = "/WONDER/source/position";

        // id
        sl.intArgs.push_back( atoi( iter->value.c_str() ) );
        ++iter; 

        // x
        sl.floatArgs.push_back( atof( iter->value.c_str() ) );
        ++iter;

        // y
        sl.floatArgs.push_back( atof( iter->value.c_str() ) );
        ++iter;

        // timestamp
        sl.floatArgs.push_back( atof( iter->value.c_str() ) );
        sl.timestamp = atof( iter->value.c_str() );
        ++iter;

        // duration
        sl.floatArgs.push_back( atof( iter->value.c_str() ) );


        score->insert( sl );
    }
    else if( name == "type" )
    {
        sl.method = "/WONDER/source/type";

        // id
        sl.intArgs.push_back( atoi( iter->value.c_str() ) );
        ++iter;

        // type
        sl.intArgs.push_back( atoi( iter->value.c_str() ) );
        ++iter;

        // timestamp
        sl.floatArgs.push_back( atof( iter->value.c_str() ) );
        sl.timestamp = atof( iter->value.c_str() );
        ++iter;

        score->insert( sl );
    }    
    else if( name == "angle" )
    {
        sl.method = "/WONDER/source/angle";

        // id
        sl.intArgs.push_back( atoi( iter->value.c_str() ) );
        ++iter;

        // angle
        sl.floatArgs.push_back( atof( iter->value.c_str() ) );
        ++iter;

        // timestamp
        sl.floatArgs.push_back( atof( iter->value.c_str() ) );
        sl.timestamp = atof( iter->value.c_str() );
        ++iter;

        // duration
        sl.intArgs.push_back( atoi( iter->value.c_str() ) );

        score->insert( sl );
    }
    //TODO: implement (de)activate, name
}

void ScoreSaxParser::on_warning( const Glib::ustring& text )
{
    cout << "on_warning(): " << text << endl;
}

void ScoreSaxParser::on_error( const Glib::ustring& text )
{
    cout << "on_error(): " << text << endl;
}

void ScoreSaxParser::on_fatal_error( const Glib::ustring& text )
{
    cout << "on_fatal_error(): " << text << endl;
}



