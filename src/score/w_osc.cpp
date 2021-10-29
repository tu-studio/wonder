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

#include "w_osc.h"

#include "scoreplayer_config.h"
#include "scorecontrol.h"

#include <sys/time.h>
#include <cmath>
#include <iostream>
#include <sstream>

#include <cstdlib>

using std::cout;
using std::endl;
using std::cerr;
using std::endl;
using std::ostringstream;
using std::string;



W_Osc::W_Osc( const char* port )
{
    if( ( stp = lo_server_thread_new( port, error ) ) == NULL )
    {    
        cerr << "[ERROR-W_OSC]: can't connect to port: " << port << endl
             << "                trying another port ..." << endl;

        if( ( stp = lo_server_thread_new( NULL, error) ) == NULL )
        {
            cerr << "[ERROR-W_OSC]: creating osc server thread failed." << endl;

            if( scoreplayerConf->osc_verbose )
                cout << "[VERBOSE-scoreplayer]: exit." << endl;

            exit( EXIT_FAILURE );
        }
        else if( scoreplayerConf->osc_verbose )       
        {
            cout << "[VERBOSE-W_OSC]: creating osc serverthread done. listening to port: " 
                 << lo_server_thread_get_port( stp ) << endl; 
        }
        return;
    }
    
    if( scoreplayerConf->osc_verbose )
    {
        cout << "[VERBOSE-W_OSC]: creating osc serverthread done. listening to port: " 
             << lo_server_thread_get_port( stp ) << endl;
    }
    return;
}


lo_server W_Osc::get_server()
{
    return( lo_server_thread_get_server( stp ) );
}


W_Osc::~W_Osc()
{
    lo_server_thread_free( stp );
}


int W_Osc::get_port()
{
    if( stp != NULL)
        return lo_server_thread_get_port( stp );
    else 
        return 0;
}


int W_Osc::server_thread_stop()
{
    lo_server_thread_stop( stp );
    if( stp == NULL )
        return 0;
    else
        return 1;
}

int W_Osc::server_thread_start()
{
    lo_server_thread_start( stp );
    if( stp == NULL )
        return 1;
    else
        return 0;
}


void W_Osc::set_control_methods( ScoreControl* ctl )
{
    lo_server_thread_add_method(stp, "/WONDER/reply", "sis", reply_handler , ctl);

    lo_server_thread_add_method( stp, "/WONDER/global/maxNoSources", "i", global_maxNoSources_handler, ctl );

    lo_server_thread_add_method( stp, "/WONDER/source/activate",   "i",    source_activate_handler,   ctl );
    lo_server_thread_add_method( stp, "/WONDER/source/deactivate", "i",    source_deactivate_handler, ctl );
    lo_server_thread_add_method( stp, "/WONDER/source/name",       "is",   source_name_handler,       ctl );
    lo_server_thread_add_method( stp, "/WONDER/source/type",       "iif",  source_type_handler,       ctl );
    lo_server_thread_add_method( stp, "/WONDER/source/angle",      "ifff", source_angle_handler,      ctl );
    lo_server_thread_add_method( stp, "/WONDER/source/position",   "iffff", source_position_handler,  ctl );

    lo_server_thread_add_method( stp, "/WONDER/score/source/enableRecord", "ii", score_enable_record_source_handler, ctl);
    lo_server_thread_add_method( stp, "/WONDER/score/source/enableRead",   "ii", score_enable_read_source_handler, ctl);

    lo_server_thread_add_method( stp, "/WONDER/score/create", "s",  score_create_handler, ctl);
    lo_server_thread_add_method( stp, "/WONDER/score/save",   NULL, score_save_handler,   ctl); 
    lo_server_thread_add_method( stp, "/WONDER/score/load",   "s",  score_load_handler,   ctl); 
    lo_server_thread_add_method( stp, "/WONDER/score/play",   "",   score_play_handler,   ctl);
    lo_server_thread_add_method( stp, "/WONDER/score/stop",   "",   score_stop_handler,   ctl);

    lo_server_thread_add_method( stp, "/WONDER/score/newtime", "iiii", score_newtime_handler, ctl );

    lo_server_thread_add_method( stp, "/WONDER/score/reset", "", score_reset_handler,   ctl);

    lo_server_thread_add_method( stp, "/WONDER/score/setStartScenario", "", score_setStartscenario_handler, ctl); 

    lo_server_thread_add_method( stp, "/WONDER/score/enableRecord", "i", score_enable_record_handler, ctl);
    lo_server_thread_add_method( stp, "/WONDER/score/enableRead",   "i", score_enable_read_handler,   ctl);

    lo_server_thread_add_method( stp, "/WONDER/score/enableMMC",  "i", score_enable_mmc_handler,  ctl);
    lo_server_thread_add_method( stp, "/WONDER/score/enableMSRC", "i", score_enable_msrc_handler, ctl);

    lo_server_thread_add_method( stp, "/WONDER/score/status", "", score_status_handler, ctl);

    lo_server_thread_add_method( stp, "/WONDER/stream/score/ping", "i", cwonder_ping_handler, ctl);  

    lo_server_thread_add_method( stp, NULL, NULL, generic_handler, ctl );

}


void W_Osc::error( int num, const char* msg, const char* path )
{
    cerr << "[ERROR-W_OSC]: liblo server error " << num << " in path " << path << ": " << msg << endl;
}


int W_Osc::reply_handler( handlerArgs )
{
    if( scoreplayerConf->osc_verbose )
    {
        lo_address client = lo_message_get_source( msg );
        cout << "[VERBOSE-W_OSC::/WONDER/reply]: received reply from server: "
             << lo_address_get_hostname( client ) << ":" 
             << lo_address_get_port    ( client ) << " " 
             << path << " ," 
             << types << " " 
             << &argv[ 0 ]->s << " " 
             <<  argv[ 1 ]->i << " "
             << &argv[ 2 ]->s << " " << endl; 
    }

    if( string( &argv[ 0 ]->s ) == "/WONDER/stream/score/connect" )
    {
        if( argv[ 1 ]->i == 0 )
            ( ( ScoreControl* ) user_data )->connection2cwonder = true;
        else
            ( ( ScoreControl* ) user_data )->connection2cwonder = false;
    }
    return 0;
}


int W_Osc::global_maxNoSources_handler( handlerArgs )
{
    // this handler is only allowed to be called once with a valid
    // value at startup, so ignore further messages
    static bool noSourcesIsSet = false;

    if( ! noSourcesIsSet )
    {
        if( argv[ 0 ]->i > 0 )
        {
            scoreplayerConf->maxNoSources= argv[ 0 ]->i;
            noSourcesIsSet = true;
            ( ( ScoreControl* ) user_data )->init();
        }
    }
    return 0;
}


int W_Osc::source_activate_handler( handlerArgs )
{    
    lo_address client = lo_message_get_source( msg );
    
    int status;
    ostringstream reply_string;

    ( (ScoreControl*) user_data )->set_osc_message( path, types, argv, argc, msg );    

    if( ( ( ScoreControl* ) user_data )->set_source_active( argv[ 0 ]->i, true ) )
    {
        reply_string << "[VERBOSE-W_OSC]: /WONDER/source/activate " << argv[ 0 ]->i 
                     << " " << " successfully invoked."; 
        status = 0;
    }
    else
    {
        reply_string << ( ( ScoreControl* ) user_data )->errorMessage;
        status = 1;
    }

    // Send back a reply
    int succ = lo_send( client, "/WONDER/stream/visual/send", "ssis", "/WONDER/reply", 
                        "/WONDER/source/activate", status, reply_string.str().c_str() );
    if( succ < 0 ) 
        cerr << "[ERROR-W_OSC]: sending reply failed: " << lo_address_errstr( client ) << endl; 
    
    return 0;
}


int W_Osc::source_deactivate_handler( handlerArgs )
{    
    lo_address client = lo_message_get_source( msg );
    
    int status;
    ostringstream reply_string;

    ( (ScoreControl*) user_data )->set_osc_message( path, types, argv, argc, msg );    

    if( ( ( ScoreControl* ) user_data )->set_source_active( argv[ 0 ]->i, false ) )
    {
        reply_string << "[VERBOSE-W_OSC]: /WONDER/source/deactivate " << argv[ 0 ]->i 
                     << " " << " successfully invoked."; 
        status = 0;
    }
    else
    {
        reply_string << ( ( ScoreControl* ) user_data )->errorMessage;
        status = 1;
    }


    // Send back a reply
    int succ = lo_send( client, "/WONDER/stream/visual/send", "ssis", "/WONDER/reply", 
                        "/WONDER/source/deactivate", status, reply_string.str().c_str());
    if( succ < 0 ) 
        cerr << "[ERROR-W_OSC]: sending reply failed: " << lo_address_errstr( client ) << endl; 
    
    return 0;
}


int W_Osc::source_name_handler( handlerArgs )
{
    lo_address client = lo_message_get_source( msg );
    
    int status;
    ostringstream reply_string;

    ( (ScoreControl*) user_data )->set_osc_message( path, types, argv, argc, msg );    

    if( ( ( ScoreControl* ) user_data )->set_source_name( argv[ 0 ]->i, &argv[ 1 ]->s ) )
    {
        reply_string << "[VERBOSE-W_OSC]: /WONDER/source/name " << argv[ 0 ]->i 
                     << " " << " successfully invoked."; 
        status = 0;
    }
    else
    {
        reply_string << ( ( ScoreControl* ) user_data )->errorMessage;
        status = 1;
    }

    // Send back a reply
    int succ = lo_send( client, "/WONDER/stream/visual/send", "ssis", "/WONDER/reply", 
                        "/WONDER/source/name", status, reply_string.str().c_str());
    if( succ < 0 ) 
        cerr << "[ERROR-W_OSC]: sending reply failed: " << lo_address_errstr( client ) << endl; 
    
    return 0;
}


int W_Osc::source_type_handler( handlerArgs )
{    
    lo_address client = lo_message_get_source (msg);
    if( scoreplayerConf->osc_verbose )
    {
        cout << "[VERBOSE-W_OSC]: received osc message from: " << lo_address_get_hostname( client ) 
             << ":" << lo_address_get_port( client );  
        cout << ": " << path << " ," << types << " " << argv[ 0 ]->i 
             << " " << argv[ 1 ]->i << " " << argv[ 2 ]->f << endl;
    }
    
    int status;
    ostringstream reply_string;

    ( ( ScoreControl* ) user_data )->set_osc_message( path, types, argv, argc, msg );    

    if ( ( ( ScoreControl* ) user_data )->set_source_type( argv[ 0 ]->i, argv[ 2 ]->f ) )
    {
        reply_string << "[VERBOSE-W_OSC]: /WONDER/source/type " << " successfully invoked. arguments: " 
                     << argv[ 0 ]->i << " " << argv[ 1 ]->i << " " << argv[ 2 ]->f; 
        status = 0;
    }
    else
    {
        reply_string << ( ( ScoreControl* ) user_data )->errorMessage;
        status = 1;
    }
    // Send back a reply
    int succ = lo_send( client, "/WONDER/stream/visual/send", "ssis", "/WONDER/reply", "/WONDER/source/type", 
                        status, reply_string.str().c_str());
    if( succ < 0 ) 
        cerr << "[ERROR-W_OSC]: sending reply failed: " << lo_address_errstr( client ) << endl; 
    else if( scoreplayerConf->osc_verbose )
    {
        cout << "[VERBOSE-W_OSC]: a reply is sent to host: " << lo_address_get_hostname( client )
             << ":" << lo_address_get_port( client ) << endl;
        cout << "     REPLY: " << reply_string.str().c_str()<< endl;
    }
    
    return 0;
}


int W_Osc::source_angle_handler( handlerArgs )
{
    lo_address client = lo_message_get_source( msg );
    if( scoreplayerConf->osc_verbose )
    {
        cout << "[VERBOSE-W_OSC]: received osc message from: " << lo_address_get_hostname( client ) 
             << ":" << lo_address_get_port( client );  
        cout << ": " << path << " ," << types << " " << argv[ 0 ]->i 
             << " " << argv[ 1 ]->f << " " << argv[ 2 ]->f << " " << argv[ 3 ]->f << endl;
    }

    int status;
    ostringstream reply_string;

    ( ( ScoreControl* ) user_data )->set_osc_message( path, types, argv, argc, msg );

    if ( ( ( ScoreControl* ) user_data )->set_source_angle( argv[ 0 ]->i, argv[ 2 ]->f, argv[ 3 ]->f ) ) 
    {
        reply_string << "[VERBOSE-W_OSC]: /WONDER/source/angle successfully invoked. arguments: " 
                     << argv[ 0 ]->i << " " << argv[ 1 ]->f << " " << argv[ 2 ]->f 
                     << " " << argv[ 3 ]->f;
        status = 0;
    }
    else
    {
        reply_string << ( ( ScoreControl* ) user_data )->errorMessage;
        status = 1;
    }


    // Send back a reply
    int succ = lo_send( client, "/WONDER/stream/visual/send", "ssis", "/WONDER/reply", "/WONDER/source/angle", 
                        status, reply_string.str().c_str());
    if( succ < 0 )
        cerr << "[ERROR-W_OSC]: sending reply failed: " << lo_address_errstr( client ) << endl;     
    else if( scoreplayerConf->osc_verbose )
    {
        cout << "[VERBOSE-W_OSC]: a reply is sent to host: " << lo_address_get_hostname( client )
             << ":" << lo_address_get_port( client ) << endl;
        cout << "     REPLY: " << reply_string.str().c_str()<< endl;
    }
    return 0;
}


int W_Osc::source_position_handler( handlerArgs )
{
    lo_address client = lo_message_get_source( msg );

    if( scoreplayerConf->osc_verbose && scoreplayerConf->screendump_verbose )
    {
        cout << "[VERBOSE-W_OSC]: received osc message from: " << lo_address_get_hostname( client ) 
             << ":" << lo_address_get_port( client )  
             << ": " << path << " ," << types << " " << argv[ 0 ]->i 
             << " " << argv[ 1 ]->f << " " << argv[ 2 ]->f 
             << " " << argv[ 3 ]->f << " " << argv[ 4 ]->f << endl;
    }
    
    int status;
    ostringstream reply_string;
    
    ( ( ScoreControl* ) user_data )->set_osc_message( path, types, argv, argc, msg );
    
    if( ( ( ScoreControl* ) user_data )->set_source_position( argv[ 0 ]->i, argv[ 3 ]->f, argv[ 4 ]->f ) )
    {
        reply_string << "[VERBOSE-W_OSC]: /WONDER/source/position successfully invoked. arguments: "
                     << argv[ 0 ]->i << " " << argv[ 1 ]->f << " " 
                     << argv[ 2 ]->f << " " << argv[ 3 ]->f << " " 
                     << argv[ 4 ]->f;
        status = 0;
    }
    else
    {
        reply_string << ((ScoreControl*)user_data)->errorMessage;
        status = 1;
    }
    
/* 
    sending a reply each position update means huge overhead, so better leave it out 
    // Send back a reply
    int succ = lo_send( client, "/WONDER/stream/visual/send", "ssis", "/WONDER/reply", "/WONDER/source/position", 
                        status, reply_string.str().c_str());
    if (succ<0)
        cerr << "[ERROR-W_OSC]: sending reply failed: " << lo_address_errstr(client) << endl; 
*/
    
    if( scoreplayerConf->osc_verbose && scoreplayerConf->screendump_verbose )
    {
        cout << "[VERBOSE-W_OSC]: a reply is sent to host: " << lo_address_get_hostname( client )
                  << ":" << lo_address_get_port( client ) << endl;
        cout << "     REPLY: " << reply_string.str().c_str()<< endl;
    }
    
    return 0;
}


int W_Osc::score_create_handler( handlerArgs )
{
    lo_address client = lo_message_get_source( msg );
    if( scoreplayerConf->osc_verbose )
    {
        cout << "[VERBOSE-W_OSC]: received osc message from: " 
             << lo_address_get_hostname( client ) << ":" 
             << lo_address_get_port( client ) << ": "   
             << path << endl;
    }
    
    int status;
    ostringstream reply_string;
    
    ( ( ScoreControl* ) user_data )->set_osc_message( path, types, argv, argc, msg );

    if( ( ( ScoreControl* ) user_data )->score_create( &argv[ 0 ]->s ) )
    {
        reply_string << "[VERBOSE-W_OSC]: /WONDER/score/create successfully invoked.";
        status = 0;
    }
    else
    {
        reply_string << ( ( ScoreControl* ) user_data )->errorMessage;
        status = 1;
    }
    // Send back a reply
    int succ = lo_send( client, "/WONDER/stream/visual/send", "ssis", "/WONDER/reply", "/WONDER/score/create", 
                        status, reply_string.str().c_str());
    if( succ < 0 )
        cerr << "[ERROR-W_OSC]: sending reply failed: " << lo_address_errstr( client ) << endl; 
    else if( scoreplayerConf->osc_verbose )
    {
        cout << "[VERBOSE-W_OSC]: a reply is sent to host: " << lo_address_get_hostname( client )
             << ":" << lo_address_get_port( client ) << endl;
        cout << "     REPLY: " << reply_string.str().c_str()<< endl;
    }

    return 0;
}


int W_Osc::score_stop_handler( handlerArgs )
{
    lo_address client = lo_message_get_source( msg );
    if(scoreplayerConf->osc_verbose)
    {
        cout << "[VERBOSE-W_OSC]: received osc message from: " << lo_address_get_hostname( client )
             << ":" << lo_address_get_port( client ) << ": "   
             << path << endl;
    }

    int status;
    ostringstream reply_string;

    ( ( ScoreControl* ) user_data )->set_osc_message( path, types, argv, argc, msg );

    if( ( ( ScoreControl* ) user_data )->score_stop() )
    {
        reply_string << "[VERBOSE-W_OSC]: /WONDER/score/stop successfully invoked.";
        status = 0;
    }
    else
    {
        reply_string << ( ( ScoreControl* ) user_data )->errorMessage;
        status = 1;
    }

    // Send back a reply
    int succ = lo_send( client, "/WONDER/stream/visual/send", "ssis", "/WONDER/reply", "/WONDER/score/stop", 
                        status, reply_string.str().c_str());
    if( succ < 0 )
    {
        cerr << "[ERROR-W_OSC]: sending reply failed: " << lo_address_errstr( client ) << endl; 
    }
    else if( scoreplayerConf->osc_verbose )
    {
        cout << "[VERBOSE-W_OSC]: a reply is sent to host: " << lo_address_get_hostname( client )
             << ":" << lo_address_get_port( client ) << endl;
        cout << "     REPLY: " << reply_string.str().c_str()<< endl;
    }

    return 0;
}


int W_Osc::score_play_handler( handlerArgs )
{
    lo_address client = lo_message_get_source( msg );
    if(scoreplayerConf->osc_verbose)
    {
        cout << "[VERBOSE-W_OSC]: received osc message from: " << lo_address_get_hostname(client) 
             << ":" << lo_address_get_port(client) << ": " << path << endl;
    }

    int status;;
    ostringstream reply_string;

    ( ( ScoreControl* ) user_data )->set_osc_message( path, types, argv, argc, msg );

    if( ( ( ScoreControl* ) user_data )->score_play() )
    {
        reply_string << "[VERBOSE-W_OSC]: /WONDER/score/play successfully invoked.";
        status = 0;
    }
    else
    {
        reply_string << ( ( ScoreControl* ) user_data )->errorMessage;
        status = 1;
    }

    int succ = lo_send( client, "/WONDER/stream/visual/send", "ssis", "/WONDER/reply", "/WONDER/score/play",
                        status, reply_string.str().c_str());
    if( succ < 0 )
        cerr << "[ERROR-W_OSC]: sending reply failed: " << lo_address_errstr( client ) << endl; 
    else if( scoreplayerConf->osc_verbose )
    {
        cout << "[VERBOSE-W_OSC]: a reply is sent to host: " << lo_address_get_hostname( client )
             << ":" << lo_address_get_port( client ) << endl;
        cout << "     REPLY: " << reply_string.str().c_str()<< endl;
    }
    
    return 0;
}


int W_Osc::score_enable_record_handler( handlerArgs )
{
    lo_address client = lo_message_get_source( msg );
    if( scoreplayerConf->osc_verbose )
    {
        cout << "[VERBOSE-W_OSC]: received osc message from: "
             << lo_address_get_hostname( client ) << ":" 
             << lo_address_get_port( client ) << ": "   
             << path << " ," << types << " " << argv[ 0 ]->i << endl;
    }

    int status;
    ostringstream reply_string;

    ( ( ScoreControl* ) user_data )->set_osc_message( path, types, argv, argc, msg );

    if ( ( ( ScoreControl* ) user_data )->score_enable_record( ( bool ) argv[ 0 ]->i ) )
    {
        reply_string << "[VERBOSE-W_OSC]: /WONDER/score/record successfully invoked: arguments: " << argv[ 0 ]->i;
        status = 0;
    }
    else
    {
        reply_string << ( ( ScoreControl* ) user_data)->errorMessage;
        status = 1;
    }

    // Send back a reply
    int succ = lo_send( client, "/WONDER/stream/visual/send", "ssis", "/WONDER/reply", "/WONDER/score/enableRecord", 
                        status, reply_string.str().c_str());
    if( succ < 0 )
    {
        cerr << "[ERROR-W_OSC]: sending reply failed: " << lo_address_errstr( client ) << endl;     
    }
    else if( scoreplayerConf->osc_verbose )
    {
        cout << "[VERBOSE-W_OSC]: a reply is sent to host: " << lo_address_get_hostname( client )
             << ":" << lo_address_get_port( client ) << endl;
        cout << "     REPLY: " << reply_string.str().c_str()<< endl;
    }
    return 0;
}


int W_Osc::score_enable_read_handler( handlerArgs )
{
    lo_address client = lo_message_get_source( msg );
    if( scoreplayerConf->osc_verbose )
    {
        cout << "[VERBOSE-W_OSC]: received osc message from: "
             << lo_address_get_hostname( client ) << ":" 
             << lo_address_get_port( client ) << ": "   
             << path << " ," << types << " " << argv[ 0 ]->i << endl;
    }

    int status;
    ostringstream reply_string;

    ( ( ScoreControl* ) user_data )->set_osc_message( path, types, argv, argc, msg );

    if ( ( ( ScoreControl* ) user_data )->score_enable_read( ( bool ) argv[ 0 ]->i ) )
    {
        reply_string << "[VERBOSE-W_OSC]: /WONDER/score/record successfully invoked: arguments: " 
                     << argv[ 0 ]->i;
        status = 0;
    }
    else
    {
        reply_string << ( ( ScoreControl* ) user_data)->errorMessage;
        status = 1;
    }

    // Send back a reply
    int succ = lo_send( client, "/WONDER/stream/visual/send", "ssis", "/WONDER/reply", "/WONDER/score/enableRead", 
                        status, reply_string.str().c_str());
    if( succ < 0 )
        cerr << "[ERROR-W_OSC]: sending reply failed: " << lo_address_errstr( client ) << endl;     
    else if( scoreplayerConf->osc_verbose )
    {
        cout << "[VERBOSE-W_OSC]: a reply is sent to host: " << lo_address_get_hostname( client )
             << ":" << lo_address_get_port( client ) << endl;
        cout << "     REPLY: " << reply_string.str().c_str()<< endl;
    }

    return 0;
}


int W_Osc::score_enable_record_source_handler( handlerArgs )
{
    lo_address client = lo_message_get_source( msg );
    if( scoreplayerConf->osc_verbose )
    {
        cout << "[VERBOSE-W_OSC]: received osc message from: " 
             << lo_address_get_hostname( client ) << ":" 
             << lo_address_get_port( client ) << ": "   
             << path << " ," << types << " " << argv[ 0 ]->i << " " << argv[ 1 ]->i << endl;
    }

    int status;
    ostringstream reply_string;

    ( ( ScoreControl* ) user_data )->set_osc_message( path, types, argv, argc, msg );

    if ( ( ( ScoreControl* ) user_data )->score_enable_record_source( argv[ 0 ]->i, ( bool )argv[ 1 ]->i ) )
    {
        reply_string << "[VERBOSE-W_OSC]: /WONDER/score/source/enableRecord successfully invoked. arguments: " 
                     << argv[ 0 ]->i << " " << argv[ 1 ]->i;
        status = 0;
    }
    else
    {
        reply_string << ( ( ScoreControl* ) user_data )->errorMessage;
        status = 1;
    }

    // Send back a reply
    int succ = lo_send( client, "/WONDER/stream/visual/send", "ssis", "/WONDER/reply", "/WONDER/score/source/enableRecord", 
                        status, reply_string.str().c_str());
    if( succ < 0 )
        cerr << "[ERROR-W_OSC]: sending reply failed: " << lo_address_errstr( client ) << endl;     
    else if( scoreplayerConf->osc_verbose )
    {
        cout << "[VERBOSE-W_OSC]: a reply is sent to host: " << lo_address_get_hostname( client )
             << ":" << lo_address_get_port( client ) << endl;
        cout << "     REPLY: " << reply_string.str().c_str()<< endl;
    }

    return 0;
}


int W_Osc::score_enable_read_source_handler( handlerArgs )
{
    lo_address client = lo_message_get_source( msg );
    if( scoreplayerConf->osc_verbose )
    {
        cout << "[VERBOSE-W_OSC]: received osc message from: "
             << lo_address_get_hostname( client ) << ":" << lo_address_get_port( client ) << ": "   
             << path << " ," << types << " " << argv[ 0 ]->i << " " << argv[ 1 ]->i << endl;
    }

    int status;
    ostringstream reply_string;

    ( ( ScoreControl* ) user_data )->set_osc_message( path, types, argv, argc, msg );

    if ( ( ( ScoreControl* ) user_data )->score_enable_read_source( argv[ 0 ]->i, ( bool ) argv[ 1 ]->i ) )
    {
        reply_string << "[VERBOSE-W_OSC]: /WONDER/score/source/enableRead successfully invoked. arguments: " 
                     << argv[ 0 ]->i << " " << argv[ 1 ]->i;
        status = 0;
    }
    else
    {
        reply_string << ( ( ScoreControl* ) user_data )->errorMessage;
        status = 1;
    }

    // Send back a reply
    int succ = lo_send( client, "/WONDER/stream/visual/send", "ssis", "/WONDER/reply", "/WONDER/score/source/enableRead", 
                        status, reply_string.str().c_str());
    if( succ < 0 )
        cerr << "[ERROR-W_OSC]: sending reply failed: " << lo_address_errstr( client ) << endl;     
    else if( scoreplayerConf->osc_verbose )
    {
        cout << "[VERBOSE-W_OSC]: a reply is sent to host: " << lo_address_get_hostname( client )
             << ":" << lo_address_get_port( client ) << endl;
        cout << "     REPLY: " << reply_string.str().c_str()<< endl;
    }

    return 0;
}


int W_Osc::score_newtime_handler( handlerArgs )
{
    lo_address client = lo_message_get_source( msg );
    if( scoreplayerConf->osc_verbose )
    {
        cout << "[VERBOSE-W_OSC]: received osc message from: " << lo_address_get_hostname( client ) 
             << ":" << lo_address_get_port(client) << ": "   
             << path << " ," << types << " " << argv[ 0 ]->i << argv[ 1 ]->i << argv[ 2 ]->i << argv[ 3 ]->i << endl;
    }

    int status;
    ostringstream reply_string;

    ( ( ScoreControl* ) user_data )->set_osc_message( path, types, argv, argc, msg );

    if ( (( ScoreControl* ) user_data )->score_newtime( argv[ 0 ]->i, argv[ 1 ]->i, argv[ 2 ]->i, argv[ 3 ]->i ) )
    {
        reply_string << "[VERBOSE-W_OSC]: /WONDER/score/newtime successfully invoked. arguments: " 
                     << argv[ 0 ]->i << ":" << argv[ 1 ]->i << ":" << argv[ 2 ]->i << ":" << argv[ 3 ]->i;
        status = 0;
    }
    else
    {
        reply_string << ( ( ScoreControl* ) user_data )->errorMessage;
        status = 1;
    }
    // Send back a reply
    int succ = lo_send( client, "/WONDER/stream/visual/send", "ssis", "/WONDER/reply", "/WONDER/score/newtime",
                        status, reply_string.str().c_str());
    if (succ < 0 )
        cerr << "[ERROR-W_OSC]: sending reply failed: " << lo_address_errstr( client ) << endl; 
    else if( scoreplayerConf->osc_verbose )
    {
        cout << "[VERBOSE-W_OSC]: a reply is sent to host: " << lo_address_get_hostname( client )
             << ":" << lo_address_get_port( client ) << endl;
        cout << "     REPLY: " << reply_string.str().c_str()<< endl;
    }
    return 0;
}


int W_Osc::score_reset_handler( handlerArgs )
{
    lo_address client = lo_message_get_source( msg );
    if( scoreplayerConf->osc_verbose )
    {
        cout << "[VERBOSE-W_OSC]: received osc message from: " << lo_address_get_hostname( client ) 
             << ":" << lo_address_get_port( client ) << ": "   
             << path << endl;
    }

    int status;
    ostringstream reply_string;

    ( ( ScoreControl* ) user_data )->set_osc_message( path, types, argv, argc, msg );

    if( ( ( ScoreControl* ) user_data )->score_reset() )
    {
      reply_string << "[VERBOSE-W_OSC]: /WONDER/score/reset successfully invoked.";
      status = 0;
    }
    else
    {
        reply_string << ( ( ScoreControl* ) user_data )->errorMessage;
        status = 1;
    }
    // Send back a reply
    int succ = lo_send( client, "/WONDER/stream/visual/send", "ssis", "/WONDER/reply", "/WONDER/score/reset", 
                        status, reply_string.str().c_str());
    if( succ<0 )
        cerr << "[ERROR-W_OSC]: sending reply failed: " << lo_address_errstr( client ) << endl; 
    else if( scoreplayerConf->osc_verbose )
    {
        cout << "[VERBOSE-W_OSC]: a reply is sent to host: " << lo_address_get_hostname( client )
             << ":" << lo_address_get_port( client ) << endl;
        cout << "     REPLY: " << reply_string.str().c_str()<< endl;
    }
    
    return 0;
}


int W_Osc::score_save_handler( handlerArgs )
{
    ostringstream reply_string;
    char* cp          = NULL;
    int   status      = 0;
    int   succ        = 0;
    lo_address client = lo_message_get_source( msg );

    if( argc == 1  &&  types[ 0 ] == 's' )    
    {
        cp = &argv[ 0 ]->s;
        if( scoreplayerConf->osc_verbose )
            cout << "[VERBOSE-W_OSC]: received osc message from: " << lo_address_get_hostname( client ) 
                 << ":" << lo_address_get_port( client ) << ": "   
                 << path << " ," << types << " " << &argv[ 0 ]->s << endl;
    }
    else if( argc == 0 )
    {
        cp = NULL;
        if( scoreplayerConf->osc_verbose )
        {
            cout << "[VERBOSE-W_OSC]: received osc message from: " << lo_address_get_hostname( client ) 
                 << ":" << lo_address_get_port( client ) << ": "   
                 << path << endl;
        }
    }
    else 
    {
        reply_string << "[ERROR-W_OSC]: /WONDER/score/save with wrong arguments. candidates are NULL or a pathname.";
        goto wrong_arg;
    }    

    ( ( ScoreControl* ) user_data )->set_osc_message( path, types, argv, argc, msg );
    if( ( ( ScoreControl* ) user_data )->score_save( cp ) )
    {
        reply_string << "[VERBOSE-W_OSC]: /WONDER/score/save successfully invoked.";
        status = 0;
    }
    else
    {
        reply_string << ( ( ScoreControl* ) user_data )->errorMessage;
        status = 1;
    }
    // Send back a reply
    succ = lo_send( client, "/WONDER/stream/visual/send", "ssis", "/WONDER/reply", 
                    "[scoreplayer::W_OSC::/WONDER/score/save]", status, reply_string.str().c_str());
    if ( succ < 0 )
        cerr << "[ERROR-W_OSC]: sending reply failed: " << lo_address_errstr( client ) << endl;     

 wrong_arg:
    if( scoreplayerConf->osc_verbose  &&  succ >= 0 )
    {
        cout << "[VERBOSE-W_OSC]: a reply is sent to host: " << lo_address_get_hostname( client )
             << ":" << lo_address_get_port( client ) << endl;
        cout << "     REPLY: " << reply_string.str().c_str()<< endl;
    }
    
    return 0;
}


int W_Osc::score_load_handler( handlerArgs )
{
    lo_address client = lo_message_get_source( msg );

    if( scoreplayerConf->osc_verbose )
    {
        cout << "[VERBOSE-W_OSC]: received osc message from: " << lo_address_get_hostname( client ) 
             << ":" << lo_address_get_port( client ) << ": "   
             << path << " ," << types << " " << &argv[ 0 ]->s << endl;
    }

    int status = 0;
    ostringstream reply_string;

    ( ( ScoreControl* ) user_data )->set_osc_message( path, types, argv, argc, msg );

    if( ( ( ScoreControl* ) user_data )->score_load( &argv[ 0 ]->s ) )
    {
        reply_string << "[VERBOSE-W_OSC]: /WONDER/score/load successfully invoked. arguments: " 
                     << &argv[ 0 ]->s;
        status = 0;
    }
    else
    {
        reply_string << ( ( ScoreControl* ) user_data )->errorMessage;
        status = 1;
    }
    // Send back a reply
    int succ = lo_send( client, "/WONDER/stream/visual/send", "ssis", "/WONDER/reply",
                        "/WONDER/score/load", status, reply_string.str().c_str());
    if( succ < 0 )
        cerr << "[ERROR-W_OSC]: sending reply failed: " << lo_address_errstr(client) << endl; 
    else if( scoreplayerConf->osc_verbose )
    {
        cout << "[VERBOSE-W_OSC]: a reply is sent to host: " << lo_address_get_hostname( client )
             << ":" << lo_address_get_port( client ) << endl;
        cout << "     REPLY: " << reply_string.str().c_str()<< endl;
    }

    return 0;
}


int W_Osc::score_setStartscenario_handler( handlerArgs )
{
    lo_address client = lo_message_get_source( msg );
    if( scoreplayerConf->osc_verbose )
    {
        cout << "[VERBOSE-W_OSC]: received osc message from: " << lo_address_get_hostname( client ) 
             << ":" << lo_address_get_port( client ) << ": "   
             << path << endl;
    }

    int status;
    ostringstream reply_string;

    ( ( ScoreControl* ) user_data )->set_osc_message( path, types, argv, argc, msg );

    if( ( ( ScoreControl* ) user_data )->score_setStartScenario() )
    {
        reply_string << "[VERBOSE-W_OSC]: /WONDER/score/setStartScenario successfully invoked.";
        status = 0;
    }
    else
    {
        reply_string << ( ( ScoreControl* ) user_data )->errorMessage;
        status = 1;
    }
    // Send back a reply
    int succ = lo_send( client, "/WONDER/stream/visual/send", "ssis", "/WONDER/reply", "/WONDER/score/setStartscene", 
                        status, reply_string.str().c_str());
    if( succ < 0 )
        cerr << "[ERROR-W_OSC]: sending reply failed: " << lo_address_errstr( client ) << endl;     
    else if( scoreplayerConf->osc_verbose )
    {
        cout << "[VERBOSE-W_OSC]: a reply is sent to host: " << lo_address_get_hostname( client ) 
             << ":" << lo_address_get_port( client ) << endl;
        cout << "     REPLY: " << reply_string.str().c_str()<< endl;
    }
    
    return 0;
}


int W_Osc::score_enable_mmc_handler( handlerArgs )
{
    lo_address client = lo_message_get_source( msg );
    if( scoreplayerConf->osc_verbose )
        cout << "[VERBOSE-W_OSC]: received osc message from: " << lo_address_get_hostname( client ) 
             << ":" << lo_address_get_port( client ) << ": "   
             << path << " ," << types << " " << argv[ 0 ]->i << endl;

    int status;
    ostringstream reply_string;

    ( ( ScoreControl* ) user_data )->set_osc_message( path, types, argv, argc, msg );

    if ( ( ( ScoreControl* ) user_data )->score_enable_mmc( ( bool ) argv[ 0 ]->i ) )
    {
        reply_string << "[VERBOSE-W_OSC]: /WONDER/score/enable_mmc successfully invoked." 
                     << argv[ 0 ]->i;
        status = 0;
    }
    else
    {
        reply_string << ( ( ScoreControl* ) user_data )->errorMessage;
        status = 1;
    }
    // Send back a reply
    int succ = lo_send( client, "/WONDER/stream/visual/send", "ssis", "/WONDER/reply", "/WONDER/score/enable_mmc", 
                        status, reply_string.str().c_str());
    if( succ < 0 )
        cerr << "[ERROR-W_OSC]: sending reply failed: " << lo_address_errstr( client ) << endl;     
    else if( scoreplayerConf->osc_verbose )
    {
        cout << "[VERBOSE-W_OSC]: a reply is sent to host: " << lo_address_get_hostname( client )
             << ":" << lo_address_get_port( client ) << endl;
        cout << "     REPLY: " << reply_string.str().c_str()<< endl;
    }
    
    return 0;
}


int W_Osc::score_enable_msrc_handler( handlerArgs )
{
    lo_address client = lo_message_get_source( msg );
    if( scoreplayerConf->osc_verbose )
        cout << "[VERBOSE-W_OSC]: received osc message from: " << lo_address_get_hostname( client ) 
             << ":" << lo_address_get_port( client ) << ": "   
             << path << " ," << types << " " << argv[ 0 ]->i << endl;

    int status;
    ostringstream reply_string;

    ( ( ScoreControl* ) user_data )->set_osc_message( path, types, argv, argc, msg );

    if ( ( ( ScoreControl* ) user_data )->score_enable_msrc( ( bool ) argv[ 0 ]->i ) )
    {
        reply_string << "[VERBOSE-W_OSC]: /WONDER/score/enable_msrc successfully invoked." 
                     << argv[ 0 ]->i;
        status = 0;
    }
    else
    {
        reply_string << ( ( ScoreControl* ) user_data )->errorMessage;
        status = 1;
    }
    // Send back a reply
    int succ = lo_send( client, "/WONDER/stream/visual/send", "ssis", "/WONDER/reply", "/WONDER/score/enable_msrc", 
                        status, reply_string.str().c_str());
    if( succ < 0 )
        cerr << "[ERROR-W_OSC]: sending reply failed: " << lo_address_errstr( client ) << endl;     
    else if( scoreplayerConf->osc_verbose )
    {
        cout << "[VERBOSE-W_OSC]: a reply is sent to host: " << lo_address_get_hostname( client )
             << ":" << lo_address_get_port( client ) << endl;
        cout << "     REPLY: " << reply_string.str().c_str()<< endl;
    }
    
    return 0;
}


int W_Osc::score_status_handler( handlerArgs )
{
    lo_address client = lo_message_get_source( msg );
    if( scoreplayerConf->osc_verbose )
    {
        cout << "[VERBOSE-W_OSC]: received osc message from: " << lo_address_get_hostname( client ) 
             << ": " << lo_address_get_port( client ) 
             << ": " << path << endl;
    }

    int status;
    ostringstream reply_string;

    ( ( ScoreControl* ) user_data )->set_osc_message( path, types, argv, argc, msg );

    if ( ( ( ScoreControl*)user_data)->send_scoreplayer_status() )
    {
        reply_string << "[VERBOSE-W_OSC]: /WONDER/score/status successfully invoked.";
        status = 0;
    }
    else
    {
        reply_string << ( ( ScoreControl* ) user_data )->errorMessage;
        status = 1;
    }
    // Send back a reply
    int succ = lo_send( client, "/WONDER/stream/visual/send", "ssis", "/WONDER/reply", "/WONDER/score/status", 
                        status, reply_string.str().c_str());
    if( succ < 0 ) 
        cerr << "[ERROR-W_OSC]: sending reply failed: " << lo_address_errstr(client) << endl; 
    else if( scoreplayerConf->osc_verbose )
    {
        cout << "[VERBOSE-W_OSC]: a reply is sent to host: " << lo_address_get_hostname( client )
             << ":" << lo_address_get_port( client ) << endl;
        cout << "     REPLY: " << reply_string.str().c_str()<< endl;
    }
    
    return 0;
}


int W_Osc::cwonder_ping_handler( handlerArgs )
{
    lo_address client = lo_message_get_source( msg );
    if( scoreplayerConf->osc_verbose && scoreplayerConf->screendump_verbose )
        cout << "[VERBOSE-W_OSC]: received osc message from: " << lo_address_get_hostname( client ) 
             << ":" << lo_address_get_port( client ) << ": "   
             << path << " ," << types << " " << argv[ 0 ]->i << endl;
    
    // Send back pong
    int succ = lo_send( client, "/WONDER/stream/score/pong", "i", argv[ 0 ]->i );
    if( succ < 0 ) 
        cerr << "[ERROR-W_OSC]: sending pong failed: " << lo_address_errstr( client ) << endl; 
    else if( scoreplayerConf->osc_verbose && scoreplayerConf->screendump_verbose )
        cout << "[VERBOSE-W_OSC::cwonder_ping_handler]: successfully sent osc /WONDER/stream/score/pong" << endl; 
    
    return 0;
}


int W_Osc::generic_handler( handlerArgs )
{
    lo_address from = lo_message_get_source( msg );
    lo_send( from, "/WONDER/reply", "sis", path, 1, "[scoreplayer]: No handler implemented for message" );

    if( scoreplayerConf->osc_verbose )
    {
        std::ostringstream os; 
        cout << "[VERBOSE-W_OSC]: generic handler: received osc message: (" 
             << argc << " args): " << path << " ," << types << endl;
        for( int i = 0; i < argc; ++i )
        {
            switch( types[ i ] )
            {
                case 's':
                    os.str( "" );
                    os << " " << &argv[ i ]->s;
                    break;
                case 'i':
                    os.str( "" );
                    os << " " << argv[ i ]->i;
                    break;
                case 'f':
                    os.str( "" );
                    os << " " << argv[ i ]->f;
                    break;
                default:
                    cout << "[ERROR-W_OSC::generic_handler]: a parameter doesn't match any known format" 
                         << endl; break;
            } 
            cout << os.str();
        } 
        cout << endl;
    }
    
    return 0;
}
