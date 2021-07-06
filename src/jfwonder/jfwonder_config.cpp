/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *                                                                                   *
 *  WONDER - Wave field synthesis Of New Dimensions of Electronic music in Realtime  *
 *  http://swonder.sourceforge.net                                                   *
 *                                                                                   *
 *                                                                                   *
 *  Technische Universit�t Berlin, Germany                                           *
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

#include <getopt.h>
#include <string>
#include <cstring>
#include <cstdlib>
#include <cstdio>

#include "jfwonder_config.h"


using namespace std;


jfwonderConfig::jfwonderConfig( int argc, char **argv ) 
{
    verbose       = 0;
    jfverbose     = 0;
    daemon        = 0;
	controlRate   = 1024;
    listeningPort = "58600";
    user          = NULL;

    cwonderAddr  = NULL;
    cwonderHost  = "127.0.0.1";
    cwonderPort  = "58100";

    parse_args( argc, argv );
    cwonderAddr = lo_address_new( cwonderHost, cwonderPort );
}


jfwonderConfig::~jfwonderConfig() 
{
    if( cwonderAddr )
    {
        lo_address_free( cwonderAddr );
        cwonderAddr = NULL;
    }
}


void jfwonderConfig::parse_args( int argc, char **argv ) 
{
    int c;

    while( 1 )
    {
        static struct option long_options[] =
            {
                {"daemon",         no_argument,        0, 'd'},
                {"user",           required_argument,  0, 'u'},
                {"listeningport",  required_argument,  0, 'o'},
                {"cwonderhost",    required_argument,  0, 'i'},
                {"cwonderport",    required_argument,  0, 'p'},
				{"controlrate",    required_argument,  0, 'c'},
                {"verbose",        no_argument,        0, 'v'},
                {"jfverbose",      no_argument,        0, 'f'},
                {"help",           no_argument,        0, 'h'},
                { 0, 0, 0, 0 }
            };

        int option_index = 0;

        c = getopt_long( argc, argv, "du:o:i:p:c:vfh", 
                         long_options, &option_index );
        
        if( c == -1 )
            break;
        
        switch( c )
        {         
            case 'd':
                daemon = 1;
                break;
                
            case 'u':
                user = strdup( optarg );
                break;

            case 'o':
                listeningPort = strdup( optarg );
                break;

            case 'i':
                cwonderHost = strdup( optarg );
                break;

            case 'p':
                cwonderPort = strdup( optarg );
                break;

            case 'c':
                controlRate = atoi( optarg );
                break;

            case 'v':
                verbose = 1;
                break;

            case 'f':
                jfverbose = 1;
                break;
                
            case 'h':
                printf ("\njfwonder's commandline arguments:\n"
                        "--daemon,        -d \n"
                        "--user,          -u ( user that starts the process; part of name of pidfile; else the pid is used ) \n"
                        "--listeningport, -o ( port where jfwonder listens, default is 58600 ) \n"
                        "--cwonderhost,   -i ( ip-address where cwonder is running )\n"
                        "--cwonderport,   -p ( port where cwonder can be reached )\n"
						"--controlrate,   -c ( control rate for the frame time, default is 1024 )\n"
                        "--verbose,       -v \n"
                        "--jfverbose,     -f (print jackframetime) \n"
                        "--help,          -h \n\n");       
                exit( EXIT_SUCCESS );
                
            case '?':
                break;
         
            default:
                printf("?? getopt returned character code 0%o ??\n", c);

        }
    }
    
    
    // Print any remaining command line arguments (not options).
    if( optind < argc )
    {
        printf( "non-option ARGV-elements: " );
        while( optind < argc )
            printf( "%s ", argv[ optind++ ] );
        printf( "\n" );
        exit( EXIT_FAILURE );
    }
}


jfwonderConfig* jfConf = NULL;
