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

#include "scoreplayer_config.h"

#include "wonder_path.h"
#include "config.h"

#include <getopt.h>
#include <libxml++/libxml++.h>

#include <sstream>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <cstring>


using namespace std;

ScoreplayerConfig::ScoreplayerConfig( int argc, char **argv ) 
{
    verbose            = 0;
    osc_verbose        = 0;
    mtc_verbose        = 0;
    screendump_verbose = 0;

    listeningPort = "58300";
    cwonderHost   = "127.0.0.1";
    cwonderPort   = "58100";
    
    scorefilePath = "";

    scoreplayerConfigfile = join( INSTALL_PREFIX, "share/wonder3/configs/scoreplayer_config.xml" ); 
    dtdPath               = join( INSTALL_PREFIX, "share/wonder3/dtd" ); 

    maxNoSources = 0;

    name = "scoreplayer";

    // Now parse the commandline args...
    parse_args( argc, argv );

    // get cwonder's osc address
    cwonderAddr = lo_address_new( cwonderHost, cwonderPort );
}


void ScoreplayerConfig::parse_args( int argc, char **argv ) 
{
    int c;
    int option_index = 0;

    static struct option long_options[] =
        {
            { "configfile",            required_argument,  0, 'c' },
            { "listeningport",         required_argument,  0, 'o' },
            { "cwonderhost",           required_argument,  0, 'i' },
            { "cwonderport",           required_argument,  0, 'p' },
            { "name",                  required_argument,  0, 'n' },
            { "verbose",               no_argument,        0, 'v' },
            { "osc_verbose",           no_argument,        0, 'w' },
            { "mtc_verbose",           no_argument,        0, 'x' },
            { "screendump_verbose",    no_argument,        0, 'y' },
            { "help",                  no_argument,        0, 'h' },
            { 0, 0, 0, 0 }
        };
    
    string addr;

    while( 1 )
    {   
        c = getopt_long (argc, argv, "c:o:i:p:n:vwxyh", long_options, &option_index);
        
        if(c == -1)
            break;
        
        switch( c )
        {         
            case 'c':
                scoreplayerConfigfile = strdup( optarg );
                break;
            case 'o':
                listeningPort = strdup( optarg );
                if( verbose )
                    cout << "[VERBOSE-ScoreplayerConfig]: listening port is set to: " << listeningPort << endl;
                break;            
            case 'i':
                cwonderHost = strdup( optarg );
                if( verbose )
                    cout << "[VERBOSE-ScoreplayerConfig]: cwonder host is set to: " << cwonderHost << endl;
                break;
            case 'p':
                cwonderPort = strdup( optarg );
                if( verbose )
                    cout << "[VERBOSE-ScoreplayerConfig]: cwonder's port is set to: " << cwonderPort << endl;
                break;            
            case 'n':
                name = strdup( optarg );
                if( verbose )
                    cout << "[VERBOSE-ScoreplayerConfig]: the name is set to: " << name << endl;
                break;            
            case 'v':
                verbose = 1;
                break;
            case 'w':
                osc_verbose = 1;
                break;
            case 'x':
                mtc_verbose = 1;
                break;
            case 'y':
                screendump_verbose = 1;
                break;
            case 'h':
                cout << "******************************************************************" << endl;
                cout << "*                                                                *" << endl;
                cout << "*   usage:                                                       *" << endl;
                cout << "*     -v : enable verbose mode                                   *" << endl;
                cout << "*     -w : enable osc verbose mode                               *" << endl;
                cout << "*     -x : enable mtc verbose mode                               *" << endl;
                cout << "*     -y : enable screendump verbose mode. damn high traffic.    *" << endl;
                cout << "*                                                                *" << endl;
                cout << "*     -o : set osc listening port      : default is 58300        *" << endl;
                cout << "*     -i : set ip of cwonder           : default is 127.0.0.1    *" << endl;
                cout << "*     -p : set port of cwonder         : default is 58100        *" << endl;
                cout << "*     -n : set name                    : default is scoreplayer  *" << endl;
                cout << "*                                                                *" << endl;
                cout << "*                                                                *" << endl;
                cout << "*     -c : absolute path of the configuration file               *" << endl;
                cout << "*                                                                *" << endl;
                cout << "*     -h     : just print out help info                          *" << endl;
                cout << "*     --help : just print out help info                          *" << endl;
                cout << "*                                                                *" << endl;
                cout << "******************************************************************" << endl;
                cout << endl;
                cout << endl;
                exit( EXIT_SUCCESS );             
                break;

            case '?':
                break;
         
            default:
                cout << "?? getopt returned character code: " << (char)c << " ???" << endl;
        }
    }
        
    // print any remaining command line arguments (not options).
    if( optind < argc )
    {
        cout << "non-option ARGV-elements: ";
        while ( optind < argc )
            printf( "%s ", argv[ optind++ ] );
        printf( "\n" );
        exit ( EXIT_FAILURE );
    }
}


int ScoreplayerConfig::readconfig()
{
    // check if file exist
    ifstream fin( scoreplayerConfigfile.c_str(), ios_base::in );

    if( ! fin.is_open() )
    {
        // cout << endl <<"[E-wonderconfig] file=" << filepath << " does not exist." << endl;
        return 1; // file does not exist
    }
    fin.close();

    // Read the Dom representation from a xml-file
    try
    {
        xmlpp::DomParser parser;
        parser.set_substitute_entities();
        parser.parse_file( scoreplayerConfigfile );
        if( parser )
        {
            xmlpp::Node* root = parser.get_document()->get_root_node();
            if( root ) 
            {        
                // validate the current dom representation
                // but first find the dtd
                string dtdFile;
                dtdFile = join( dtdPath, "scoreplayer_config.dtd" );
                fin.open( dtdFile.c_str(), ios_base::in );
                if( ! fin.is_open() )
                {
                    return 2; // dtd file does not exist
                }
                         
                try
                {
                    xmlpp::DtdValidator validator( dtdFile.c_str() );
                    validator.validate( parser.get_document() );
                }
                catch( const xmlpp::validity_error& ex )
                {
                    return 3; /// dtd validation error
                }

                getSettings( root );
            }
        }
    }
    catch( const exception& ex )
    {
        //// cout << "Exception caught: " << ex.what() << endl;
        return 4; // xml error
    }   

    return 0;
}


void ScoreplayerConfig::getSettings( xmlpp::Node* node )
{    
    xmlpp::NodeSet nset = node->find( "/scoreplayer_config/settings" );

    if( nset.size() > 0 )
    {
        if( const xmlpp::Element* nodeElement = dynamic_cast<const xmlpp::Element*>( *nset.begin() ) ) 
        {
            const xmlpp::Element::AttributeList& attributes = nodeElement->get_attributes();
            xmlpp::Element::AttributeList::const_iterator iter;

            for(iter = attributes.begin(); iter != attributes.end(); ++iter)
            {
                const xmlpp::Attribute* attribute = *iter;
                
                Glib::ustring nodename = attribute->get_name();
            
                stringstream ss( attribute->get_value() );
                if ( nodename == "scorepath" )
                    ss >> scorefilePath; 
            }                   
        }
    }
}


void ScoreplayerConfig::showValues()
{
    cout << "[VERBOSE-scoreglobalconfig]: " << endl;
    cout << "     cwonder (host:port)     : " << lo_address_get_hostname( cwonderAddr) << ":" << lo_address_get_port( cwonderAddr ) << endl;    
    cout << "     scoreplayer (port)      : " << listeningPort << endl;    
    cout << "     scoreplayer             : " << listeningPort << endl;    
    cout << "     name                    : " << name << endl;    
    cout << "                               " << endl;
    cout << "     verbose                 : " << verbose << endl;
    cout << "     osc_verbose             : " << osc_verbose << endl;
    cout << "     mtcverbose              : " << mtc_verbose << endl;
    cout << "     screendump_verbose      : " << screendump_verbose << endl;
    cout << "                               " << endl;
    cout << "                               " << endl;
    cout << "     DTD path                : " << dtdPath << endl;
    cout << "     scorefile path          : " << scorefilePath<< endl;
    cout << "     configfile path         : " << scoreplayerConfigfile << endl;
    cout << endl;
}


ScoreplayerConfig* scoreplayerConf = NULL;
