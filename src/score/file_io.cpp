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

#include "file_io.h"

#include "scorecontrol.h"
#include "wonder_path.h"
#include "project.h"
#include "scoresaxparser.h"
#include "scoreplayer_config.h"

#include <iostream>
#include <fstream>
#include <sstream>

using std::endl;
using std::cout;
using std::cerr;
using std::string;


void File_IO::setScoreControl( ScoreControl* newScoreControl )
{
    scoreControl = newScoreControl;
}


int File_IO::writeXML( string filepath, std::multiset<ScoreLine>* score, Scenario* orc )
{    
    xmlpp::Element*  rootNode;
    xmlpp::Element*  orcNode;
    xmlpp::Element*  scoreNode;
    xmlpp::Document* document_out;

    document_out = new xmlpp::Document;

    std::ostringstream os; 
    std::ostringstream istr;

    // write system date and time to xml header
    char   timestr[ 20 ];
    time_t seconds = time( 0 );
    struct tm* ptm = localtime( &seconds ); 
    
    sprintf( timestr, "%04i-%02i-%02i %02i:%02i:%02i",
            ( int ) ptm->tm_year + 1900,
            ( int ) ptm->tm_mon  + 1,
            ( int ) ptm->tm_mday,
            ( int ) ptm->tm_hour,
            ( int ) ptm->tm_min,
            ( int ) ptm->tm_sec );

    document_out->set_internal_subset( "scorefile", "", "scoreplayer_score.dtd" );      

    // Create the root node and set it's attribute
    rootNode = document_out->create_root_node( "scorefile", "", "scoreplayer_score.dtd" ); 
    
    rootNode->set_attribute( "date", timestr, "" );
    os.str( "" );
    os << filepath.c_str();
    rootNode->set_attribute("path", os.str(), ""); 


    // write orchestra part of scorefile 

    // create the node and set it's attribute
    orcNode = rootNode->add_child( "orchestra" );
    
    if( orcNode )
    {   
        for( int i = 0; i < scoreplayerConf->maxNoSources; ++i )
        {
            xmlpp::Element* nodeSrc;
            nodeSrc = orcNode->add_child( "src" );

            os.str( "" );
            os << orc->sourcesVector[ i ].id;
            nodeSrc->set_attribute( "id", os.str(), "" );
            
            os.str( "" );
            os << orc->sourcesVector[ i ].type;
            nodeSrc->set_attribute( "type", os.str(), "" );
            
            os.str( "" );
            os << orc->sourcesVector[ i ].pos[ 0 ];
            nodeSrc->set_attribute( "posx", os.str(), "" );
            
            os.str( "" );
            os << orc->sourcesVector[ i ].pos[ 1 ];
            nodeSrc->set_attribute( "posy", os.str(), "" );
            
            os.str( "" );
            os << orc->sourcesVector[ i ].angle;
            nodeSrc->set_attribute( "angle", os.str(), "" );
        }
    }
    
    // write scorelines    
    
    // create the node and set it's attribute
    scoreNode = rootNode->add_child( "score" );     

    std::multiset< ScoreLine >::iterator scoreIter;
    for( scoreIter = score->begin(); scoreIter != score->end(); ++scoreIter )
    {
        xmlpp::Element* nodeChild1;

        if( scoreIter->method == "/WONDER/source/position" )
        {
            nodeChild1 = scoreNode->add_child( "pos" );

            os.str( "" );
            os << scoreIter->intArgs[ 0 ];
            nodeChild1->set_attribute( "id", os.str(), "" ); 
            os.str( "" );
            os << scoreIter->floatArgs[ 0 ];
            nodeChild1->set_attribute( "x", os.str(), "" ); 
            os.str( "" );
            os << scoreIter->floatArgs[ 1 ];
            nodeChild1->set_attribute( "y", os.str(), "" ); 
            os.str( "" );
            os << scoreIter->timestamp;
            nodeChild1->set_attribute( "t", os.str(), "" ); 
            os.str( "" );
            os << scoreIter->floatArgs[ 3 ];
            nodeChild1->set_attribute( "dur", os.str(), "" ); 
        }           
        else if( scoreIter->method == "/WONDER/source/angle" )
        {
            nodeChild1 = scoreNode->add_child( "angle" );

            os.str( "" );
            os << scoreIter->intArgs[ 0 ];
            nodeChild1->set_attribute( "id", os.str(), "" ); 
            os.str( "" );
            os << scoreIter->floatArgs[ 0 ];
            nodeChild1->set_attribute( "angle", os.str(), "" ); 
            os.str( "" );
            os << scoreIter->timestamp;
            nodeChild1->set_attribute( "t", os.str(), "" ); 
            os.str( "" );
            os << scoreIter->floatArgs[ 2 ];
            nodeChild1->set_attribute( "dur", os.str(), "" ); 
        }           
        else if( scoreIter->method == "/WONDER/source/type" )
        {
            nodeChild1 = scoreNode->add_child( "type" );

            os.str("");
            os << scoreIter->intArgs[ 0 ];
            nodeChild1->set_attribute("id", os.str(), ""); 
            os.str("");
            os << scoreIter->intArgs[ 1 ];
            nodeChild1->set_attribute("type", os.str(), ""); 
            os.str("");
            os << scoreIter->floatArgs[ 0 ];
            nodeChild1->set_attribute("t", os.str(), ""); 
        }           
        else
        {
            // unknown OSC method! this can only happen when messages are recorded
            // with new_scoreline() which are currently not supported by write
            istr.str("");
            istr << "[ERROR-file_io::writeXML()]: writing score to xml file failed. " 
                 << scoreIter->method << " is not supported." << endl;
            scoreControl->setErrorMessage( istr.str() ); 
            cout << scoreControl->errorMessage;
            return -1;
        }       
    } 

    // validate the Document
    if( scoreplayerConf->verbose )
        cout << "               validating score ... " << endl;
    
    try 
    {
        string dtdFile = join ( scoreplayerConf->dtdPath, "scoreplayer_score.dtd" );
        xmlpp::DtdValidator validator( dtdFile );      
        validator.validate( document_out );
        if( scoreplayerConf->verbose )
            cout << "[File_IO::write_XML()]: Validation successfull" << endl;
    }
    catch( const xmlpp::validity_error& ex )
    {
        if( scoreplayerConf->verbose )
        {
            cerr << "[File_IO::write_XML()]: Error: validating the document_out"
                 << endl << ex.what() << endl;
        }
        return -1;
    }
    
    if( scoreplayerConf->verbose )
        cout << "              validating score done ! ... " << endl;
    
    // write Dom representation to file
    if( scoreplayerConf->verbose )
        cout << "               writing " << filepath << " ... " << endl;

    document_out->write_to_file_formatted( filepath );            
    
    if( document_out != NULL )
        delete document_out; 
    
    return 0;
}

int File_IO::readXML( Glib::ustring filepath, std::multiset<ScoreLine>* score, Scenario* orc )
{
    // XXX: this is ineffective since the file is first read by a DOM parser for validation and then
    // by a custom SAX parser for reading data

    // dtd validation
    try
    {

        xmlpp::DomParser domParser;
        domParser.parse_file( filepath );
        xmlpp::Document* doc = domParser.get_document();
        xmlpp::DtdValidator validator( join( scoreplayerConf->dtdPath, "scoreplayer_score.dtd") );
        try 
        {
            validator.validate( doc );
            if( scoreplayerConf->verbose )
                cout << "[VERBOSE-File_IO::write_XML()]: validation of XML file was successfull" << endl;
        }
        catch( const xmlpp::validity_error& ex )
        {
            if( scoreplayerConf->verbose )
            {
                cerr << "[ERROR-File_IO::write_XML()]: Error: validating XML file failed" << endl << "               " 
                     << ex.what() << endl;
            }
            std::ostringstream ostr; 
            ostr.str("");
            ostr << "[ERROR-File_IO::read_XML()]: Exception caught: " << endl << ex.what() << endl;

            //XXX: short hack to prevent error messages from getting to long
            std::string temp = ostr.str();
            if( temp.length() > 150 )
                temp= temp.substr( 0, temp.find( "\n", 150 ) ); 

            scoreControl->setErrorMessage( temp );     

            return -1;
        }

        ScoreSaxParser parser( score, orc );
        parser.set_substitute_entities( true ); 
        parser.parse_file( filepath );
    }
    catch( const xmlpp::exception& ex )
    {
        std::ostringstream ostr; 
        ostr.str("");
        ostr << "[ERROR-File_IO::read_XML()]: Exception caught: " << endl << ex.what() << endl;

        //XXX: short hack to prevent error messages from getting to long
        std::string temp = ostr.str();
        if( temp.length() > 150 )
            temp= temp.substr( 0, temp.find( "\n", 150 ) ); 

        scoreControl->setErrorMessage( temp );     

        return -1;
    }
    
    return 0;
}
