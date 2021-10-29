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

#include <iostream>
#include <cstdio>
#include <cstdlib>

#include <lo/lo.h>

#include "rtmidi/RtMidi.h"

#include "scorecontrol.h"
#include "scoreplayer_config.h"
#include "w_osc.h"
#include "mtc_com.h"
#include "wonder_path.h"


using std::endl;
using std::cout;
using std::cerr;


int main( int argc, char** argv ) 
{
    // create config and parse commandline arguments
    scoreplayerConf = new ScoreplayerConfig( argc, argv );

    // read configfile 
    int ret = scoreplayerConf->readconfig();
    if( ret != 0 )
    {
        if(ret == 1)
            cerr << "[ERROR-scoreplayer] config file does not exist: " << scoreplayerConf->scoreplayerConfigfile << endl;
        else if( ret == 2 )
            cerr << "[ERROR-scoreplayer] dtd file does not exist."  << endl;
        else if( ret == 3 )
            cerr << "[ERROR-scoreplayer] dtd error. Check if the xml file got the correct dtd." << endl;
        else
            cerr << "[ERROR-scoreplayer] xml error. Check if the xml file is correct." << endl;

        exit( EXIT_FAILURE );
    }

    if( scoreplayerConf->verbose )
    {
        cout << endl;
        cout << endl;
        cout << "******************************************************************" << endl;
        cout << "*******************   welcome to scoreplayer   *******************" << endl;
        cout << "******************************************************************" << endl;
        cout << endl;
        cout << endl;

        scoreplayerConf->showValues();
    }
    
    // make default scorefiles path
    if( makedirs( scoreplayerConf->scorefilePath.c_str(), mode_t( 0700 ) ) != 0 )
        cerr <<  "[ERROR-scoreplayer::main] error making default scorefile path." << endl;

    ScoreControl scoreControl;
    if( scoreplayerConf->verbose )
    {
        cout << "[VERBOSE-scoreplayer::main]: constructing of scorecontroler done " << endl;    
        cout << "[VERBOSE-scoreplayer::main]: setting up osc engine ... " << endl;    
    }

    W_Osc osccom( scoreplayerConf->listeningPort );

    osccom.set_control_methods( &scoreControl );

    if( osccom.server_thread_start() == 0 )
    {
        if( scoreplayerConf->verbose )
            cout << "[VERBOSE-scoreplayer::main]: osc server thread is now running" << endl;
    }
    else
    {
        cerr << "[Error-scoreplayer::main]: failure while starting osc server thread" << endl;
        exit( EXIT_FAILURE );
    }
    
    int retVal = scoreControl.connect2cwonder();
    if( retVal != 0 )
    {
        cerr << "could not establish connection to cwonder. exiting now..." << endl;
        exit( EXIT_FAILURE );    
    } 

    // wait for cwonder to send setup data, if it does not work then exit
    while( scoreplayerConf->maxNoSources == 0 )
    {
        static int timeoutCounter = 0;
        sleep( 1 );

        if( timeoutCounter == 5 )
        {
            cerr << "could not establish connection to cwonder, exiting now... " << endl;
            exit( EXIT_FAILURE );
        }
        timeoutCounter++;
    }

    // main loop
    while( true )
    {
        usleep( 500000 );

        try
        {
            scoreControl.mtccom->midiin->openVirtualPort( "scoreplayer_in" );
        }
        catch( RtError& error )
        {
            cerr << "[Error-scoreplayer::main]: reconnecting virtual midi port failed" << endl;
        }
    }

    cout << "[VERBOSE-scoreplayer::main]: exiting " << endl;
    
    delete scoreplayerConf;
    return 0;
}
