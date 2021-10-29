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

 



#ifndef SCORESAXPARSER_H
#define SCORESAXPARSER_H

#include <libxml++/libxml++.h>

#include <set>

#include "score_line.h"

class Scenario;


// scoresaxparser is needed for the parsing process of the xml scorefile.
// the overridden virtual functions are called by the SaxParser during parsing process. 
class ScoreSaxParser : public xmlpp::SaxParser
{
 public:
    std::multiset<ScoreLine>* score;
    Scenario* orc;
    ScoreSaxParser( std::multiset<ScoreLine>* sc_score, Scenario* sc_orc );
    virtual ~ScoreSaxParser();

 protected:
    virtual void on_start_element( const Glib::ustring& name, const AttributeList& properties );
    virtual void on_warning( const Glib::ustring& text );
    virtual void on_error( const Glib::ustring& text );
    virtual void on_fatal_error( const Glib::ustring& text );
};

#endif
