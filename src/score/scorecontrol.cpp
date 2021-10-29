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

#include "scorecontrol.h"

#include "project.h"
#include "mtc_com.h"
#include "scoreplayer_config.h"
#include "wonder_path.h"

#include <iostream>
#include <sstream>
#include <fstream>

#include <ctime>
#include <cstdlib>

using std::cout;
using std::cerr;
using std::endl;
using std::string;

ScoreControl::ScoreControl()
{
    record             = false;
    read               = true;
    connection2cwonder = false;
    project_loaded     = false;

    errorMessage = "";  

    mtccom = new MTC_Com( this ); // mtccom on the heap, could be on the stack as well

    file_io.setScoreControl( this );

    scorefile = "";
}


ScoreControl::~ScoreControl()
{
    if( mtccom )
    {
        delete mtccom;
        mtccom = NULL;
    }
    if( currentScenario )
    { 
        delete currentScenario;
        currentScenario = NULL;
    }
    if( recordmodes )
    {
        delete [] recordmodes;
        recordmodes = NULL;
    }
    if( readmodes )
    {
        delete [] readmodes;
        readmodes = NULL;
    }
    if( startScenario )
    {
        delete startScenario;
        startScenario = NULL;
    }
}

void ScoreControl::init()
{
    startScenario   = new Scenario( scoreplayerConf->maxNoSources ) ;
    currentScenario = new Scenario( scoreplayerConf->maxNoSources );

    recordmodes = new bool[ scoreplayerConf->maxNoSources ];
    readmodes   = new bool[ scoreplayerConf->maxNoSources ];

    for( int i = 0 ; i < scoreplayerConf->maxNoSources; ++i )
    {
        recordmodes[ i ] = false ;
        readmodes[ i ]   = true ;
    }
} 


// this function is called by midi_process() function of rtmidi thread
// each time a mtc quarterframe message arrives
int ScoreControl::score_play_function()
{    
    // setting score iterator if necessary
    float t = mtc2f();

    // position_jump is true when a time jump happened somewhere: maybe by /WONDER/score/newtime
    if( mtccom->position_jump ) 
    {
        set_play_time( t );
        send_currentScenario();
        mtccom->position_jump = false;
        return 0;
    }

    if( playscore.empty() )
    {
        if( scoreplayerConf->screendump_verbose )
            cout << "[VERBOSE-ScoreControl::score_play_function()]: skipped playing score because score is empty" 
                 << endl;
        return 0;
    }           
    
    if( sl_playscore_iter == playscore.end() )
    {
        if( scoreplayerConf->screendump_verbose )
            cout << "[VERBOSE-ScoreControl::score_play_function()]: score reached end" 
                 << endl;
        return 0;
    }
    
    // maybe xx msec prediction in future? then change t+0.00
    while(     ( sl_playscore_iter->timestamp <= t + 0.00 ) 
            && ( sl_playscore_iter            != playscore.end() ) )
    {
        // deleting all data for record enabled sources
        int sourceId = sl_playscore_iter->intArgs[ 0 ];
        if( record  &&  recordmodes[ sourceId ] )
        {
            if( sl_playscore_iter == playscore.begin() )
            {
                playscore.erase( sl_playscore_iter );
                sl_playscore_iter = playscore.begin();               
                if( scoreplayerConf->verbose )
                    cout << "[VERBOSE-ScoreControl::score_play_function()]: removed first line from score" << endl;
            }
            else
            {
                std::multiset< ScoreLine >::iterator tmp_play_iter = sl_playscore_iter;
		sl_playscore_iter--;
		playscore.erase( tmp_play_iter );
		sl_playscore_iter++;
                if( scoreplayerConf->verbose )
                    cout << "[VERBOSE-ScoreControl::score_play_function()]: removed line from score: " << endl;
            }
            continue;
        }
        else if( read && readmodes[ sourceId ] )
        {
            lo_message tmp_osc_mesg = lo_message_new();
            
            if( sl_playscore_iter->method == "/WONDER/source/activate" )
            {
                //TODO: implement this
                //lo_message_add_int32( tmp_osc_mesg, sourceId );

            }
            if( sl_playscore_iter->method == "/WONDER/source/deactivate" )
            {
                //TODO: implement this
                //lo_message_add_int32( tmp_osc_mesg, sourceId );
            }
            if( sl_playscore_iter->method == "/WONDER/source/name" )
            {
                //TODO: implement this
                //lo_message_add_int32( tmp_osc_mesg, sourceId );
            }
            if( sl_playscore_iter->method == "/WONDER/source/position" )
            {
                lo_message_add_int32( tmp_osc_mesg, sourceId );
                lo_message_add_float( tmp_osc_mesg, sl_playscore_iter->floatArgs[ 0 ] );
                lo_message_add_float( tmp_osc_mesg, sl_playscore_iter->floatArgs[ 1 ] );
                lo_message_add_float( tmp_osc_mesg, sl_playscore_iter->floatArgs[ 3 ] );

                currentScenario->sourcesVector[ sourceId ].pos[ 0 ] = sl_playscore_iter->floatArgs[ 0 ];
                currentScenario->sourcesVector[ sourceId ].pos[ 1 ] = sl_playscore_iter->floatArgs[ 1 ];
            }
            else if( sl_playscore_iter->method == "/WONDER/source/angle" )
            {
                lo_message_add_int32( tmp_osc_mesg, sourceId );
                lo_message_add_float( tmp_osc_mesg, sl_playscore_iter->floatArgs[ 0 ] );
                lo_message_add_float( tmp_osc_mesg, sl_playscore_iter->floatArgs[ 2 ] );

                currentScenario->sourcesVector[ sourceId ].angle = sl_playscore_iter->floatArgs[ 0 ];
            }
            else if( sl_playscore_iter->method == "/WONDER/source/type" )
            {
                lo_message_add_int32( tmp_osc_mesg, sourceId );
                lo_message_add_int32( tmp_osc_mesg, sl_playscore_iter->intArgs[ 1 ] );

                currentScenario->sourcesVector[ sourceId ].type = sl_playscore_iter->intArgs[ 1 ];
            }
            else
            {   
                cerr << "[ERROR-ScoreControl::score_play_function()]: method not supported: "
                     << sl_playscore_iter->method << endl;
                return -1;
            }
            // could be sent as osc bundle, ... only as separate osc messages at this time
            if( lo_send_message( scoreplayerConf->cwonderAddr, sl_playscore_iter->method.c_str(), tmp_osc_mesg ) > -1 )
            {
                if( scoreplayerConf->screendump_verbose )
                {
                    cout << "[VERBOSE-ScoreControl::score_play_function()]: successfully sent osc_message from scoreline: "
                         << sl_playscore_iter->timestamp << ": ";
                    lo_message_pp( tmp_osc_mesg );
                }
            }
            else
            {
                cerr << "[ERROR-ScoreControl::score_play_function()]: " 
                     << lo_address_errstr( scoreplayerConf->cwonderAddr ) << endl;
            }

            lo_message_free( tmp_osc_mesg );
        }
        sl_playscore_iter++; 
    }

    return 0;
}


int ScoreControl::send_mtc_time()
{
    int succ = lo_send( scoreplayerConf->cwonderAddr, "/WONDER/stream/visual/send", "siiii", "/WONDER/mtctime", 
                        mtccom->mtc_time[ 0 ],
                        mtccom->mtc_time[ 1 ],
                        mtccom->mtc_time[ 2 ],
                        mtccom->mtc_time[ 3 ] * 1000 / ( int ) mtccom->framerate_float );
    
    if( succ < 0 )
        cerr << "[ERROR-ScoreControl::send_mtc_time]: sending mtc to visual stream failed " << endl;
    
    return succ;
}


int ScoreControl::send_currentScenario()
{
    if( scoreplayerConf->verbose )
        cout << "[VERBOSE-ScoreControl::send_currentScenario()]: sending current state: " << endl; 

    // if global read mode is turned off, do nothing
    if( ! read )
        return 0;

    for( int i = 0; i < scoreplayerConf->maxNoSources; ++i )
    {
        // if read mode of this source is disabled proceed with the next one
        if( ! readmodes[ i ] )
            continue;
        
        Source* source = & currentScenario->sourcesVector[ i ];

        //TODO: implement activate/deactivate
        if( lo_send( scoreplayerConf->cwonderAddr, "/WONDER/source/type", "ii", i, source->type ) < 0 )
        {
            cerr << "[ERROR-ScoreControl::send_currentScenario()]: sending osc /WONDER/source/type failed " << endl;
            return -1;
        }

        if( scoreplayerConf->osc_verbose )
        {
            cout << "[VERBOSE-ScoreControl::send_currentScenario()]: sent current type of source " 
                 << i << endl;        
        }

        if( lo_send( scoreplayerConf->cwonderAddr, "/WONDER/source/angle", "if", i, source->angle ) < 0 )
        {
            cerr << "[ERROR-ScoreControl::send_currentScenario()]: sending osc /WONDER/source/angle failed " << endl;
            return -1;
        }

        if( scoreplayerConf->osc_verbose )
        {
            cout << "[VERBOSE-ScoreControl::send_currentScenario()]: sent current angle of source " << i << endl;        
        }

        if( lo_send( scoreplayerConf->cwonderAddr, "/WONDER/source/position", "iff", i, source->pos[ 0 ], source->pos[ 1 ] )  < 0 )
        {
            cerr << "[ERROR-ScoreControl::send_currentScenario()]: sending osc /WONDER/source/position failed " << endl;
            return -1;
        }               
        else if( scoreplayerConf->osc_verbose )
        {
            cout << "[VERBOSE-ScoreControl::send_currentScenario()]: sent current position of source " << i << endl;     
        }

        //TODO: implement name
        //if( source->name  != lastScenario->sourcesVector[ i ].name )
        //{
        //    if( lo_send( scoreplayerConf->cwonderAddr, "/WONDER/source/name", "is", i, source->name.c_str() ) < 0 )
        //    {
        //        cerr << "[ERROR-ScoreControl::send_currentScenario()]: sending osc /WONDER/source/name failed " << endl;
        //        return -1;
        //    }

        //    if( scoreplayerConf->osc_verbose )
        //    {
        //        cout << "[VERBOSE-ScoreControl::send_currentScenario()]: sent current name of source " << i << endl;        
        //    }
        //}
    }

    return 0;
}


void ScoreControl::setErrorMessage( string error )
{
    errorMessage.clear();
    errorMessage = error;
}


void ScoreControl::set_osc_message( const char* path, const char* types, lo_arg** argv, int argc, lo_message msg )
{
    osc_msg.path  = path;
    osc_msg.types = types;
    osc_msg.argv  = argv;
    osc_msg.argc  = argc;
}


bool ScoreControl::new_scoreline( int sourceId, float timetag, float duration )
{
    ScoreLine sl;
    
    float timestamp = mtc2f();

    if( scoreplayerConf->screendump_verbose )
    {
        cout << "[ScorePlayer::new_scoreline]: time difference check: "       << endl;
        cout << "         time (cwonder): "  << timetag     << "   time (mtc): " << timestamp
             << "   diff: cwonder - mtc = "  << timetag - timestamp << endl;    
    }
    
    sl.timestamp = timestamp; // set mtc time as timestamp ; 
    sl.method    = string( osc_msg.path );

    //TODO: implement activate, deactivate, name
    if( sl.method == "/WONDER/source/activate" )
    {
    }
    else if( sl.method == "/WONDER/source/deactivate" )
    {
    }
    else if( sl.method == "/WONDER/source/name" )
    {
    }
    else if( sl.method == "/WONDER/source/position" )
    {
        sl.intArgs.push_back( sourceId ); 
        sl.floatArgs.push_back( osc_msg.argv[ 1 ]->f ); 
        sl.floatArgs.push_back( osc_msg.argv[ 2 ]->f ); 
        sl.floatArgs.push_back( duration ); 
        sl.floatArgs.push_back( timestamp ); 
    }
    else if( sl.method == "/WONDER/source/type" )
    {
        sl.intArgs.push_back( sourceId ); 
        sl.intArgs.push_back( osc_msg.argv[ 1 ]->i ); 
        sl.floatArgs.push_back( timestamp );
    }
    else if( sl.method == "/WONDER/source/angle" )
    {
        sl.intArgs.push_back( sourceId ); 
        sl.floatArgs.push_back( osc_msg.argv[ 1 ]->f ); 
        sl.floatArgs.push_back( duration ); 
        sl.floatArgs.push_back( timestamp );
    }

    // online record mode, i.e. time is running
    if( mtccom->sc_play )
        score_recordbuffer.insert( sl );
    else // offline record mode
    {
        // check for previous scorelines of same type at the same time for the same id
        // and erase them in the offline record buffer
        typedef std::multiset< ScoreLine >::iterator SI;
        SI lowerBound = score_recordbuffer_offline.lower_bound( sl );
        SI upperBound = score_recordbuffer_offline.upper_bound( sl );

        for( SI iter = lowerBound; iter != upperBound; ++iter )
        {
            // end search if end of record buffer is reached
            if( iter == score_recordbuffer_offline.end() )
                break;

            // compare new scoreline to existing ones and delete them if necessary
            if( sl ==  *iter ) 
                score_recordbuffer_offline.erase( iter-- );
        }
        
        // now finally insert the new scoreline
        score_recordbuffer_offline.insert( sl );
    }

    if( scoreplayerConf->screendump_verbose )
    {
        cout << "[VERBOSE-ScoreControl::new_scoreline]: added new scoreline: recordbuffer list_size: " 
             << score_recordbuffer.size() << endl << " offline recordbuffer list_size: " 
             << score_recordbuffer_offline.size() << endl;
    }
    return true;   
}

// TODO: implement this
bool ScoreControl::set_source_active( int sourceId, bool active )
{
    if( sourceId >= 0  &&  sourceId < ( int ) currentScenario->sourcesVector.size() )
    {
        if( record && recordmodes[ sourceId ] ) // etc. transports checking...
        {   
            // update in current scenario
            // currentScenario->>sourcesVector[sourceId]. ...
            
        }       
        // new_scoreline(sourceId, 0.0, 0.0);
    }
    return true;
}

// TODO: implement this
bool ScoreControl::set_source_name( int sourceId, std::string name)
{
    if( sourceId >= 0 && sourceId < ( int ) currentScenario->sourcesVector.size() )
    {
        if( record && recordmodes[ sourceId ] ) // etc. transports checking...
        {
            if( name != currentScenario->sourcesVector[ sourceId ].name )
                currentScenario->sourcesVector[ sourceId ].name  = name;

            //new_scoreline(sourceId, timetag, 0.0); // type is not using duration info: id, type, angle, time           
        }
    }
    return true;
}

bool ScoreControl::set_source_type( int sourceId, float timetag )
{
    if( sourceId >= 0 && sourceId < ( int ) currentScenario->sourcesVector.size() )
    {
        if( osc_msg.argv[ 1 ]->i != currentScenario->sourcesVector[ sourceId ].type )
        {
            currentScenario->sourcesVector[ sourceId ].type  = osc_msg.argv[ 1 ]->i;

            if( record && recordmodes[ sourceId ] )        
                new_scoreline( sourceId, timetag, 0.0 ); // type is not using duration info: id, type, time           
        }
    }   
    else
    {
        setErrorMessage( "[ERROR-ScoreControl::set_source_type()]: invalid sourceId.\n" );      
        cerr << errorMessage << endl;
        return false;
    }

    return true;
}


bool ScoreControl::set_source_angle( int sourceId, float duration, float timetag )
{
    if( sourceId >= 0 && sourceId < ( int ) currentScenario->sourcesVector.size() )
    {
        if( osc_msg.argv[ 1 ]->f != currentScenario->sourcesVector[ sourceId ].angle )
        {
            currentScenario->sourcesVector[ sourceId ].angle = osc_msg.argv[ 1 ]->f;

            if( record && recordmodes[ sourceId ] )        
                new_scoreline( sourceId, timetag, duration ); 
        }
    }   
    else
    {
        setErrorMessage( "[ERROR-ScoreControl::set_source_angle()]: invalid sourceId.\n" );      
        cerr << errorMessage << endl;
        return false;
    }
    
    return true;
}

bool ScoreControl::score_create( string filename )
{
    score_stop();
    std::ostringstream errormsg;    
    std::ifstream fin;
    Glib::ustring filepath = filename;

    if( ! isabs( filepath ) )
        filepath = join( scoreplayerConf->scorefilePath, filepath );
        
    string            stringpath = filepath;
    string::size_type pos_beg    = stringpath.find( ".xml", 0 );
    
    if( pos_beg >= stringpath.length() )
        filepath = filepath + ".xml";    
    
    playscore.clear( );
    score_recordbuffer.clear( );

    *startScenario = *currentScenario;

    scorefile = filepath;

    if( scoreplayerConf->verbose )
        cout << "[VERBOSE-ScoreControl::score_create()]: scorefile created: " << scorefile << endl;
    
    return true;
}


bool ScoreControl::set_source_position( int sourceId, float duration, float timetag)
{
    // check for valid source id
    if( sourceId >= 0  &&  sourceId < ( int ) currentScenario->sourcesVector.size() )
    {
        // check redundancy, if source is already at this position just do nothing
        if(    currentScenario->sourcesVector[ sourceId ].pos[ 0 ] != osc_msg.argv[ 1 ]->f
            || currentScenario->sourcesVector[ sourceId ].pos[ 1 ] != osc_msg.argv[ 2 ]->f )
        {
            currentScenario->sourcesVector[ sourceId ].pos[ 0 ] = osc_msg.argv[ 1 ]->f;
            currentScenario->sourcesVector[ sourceId ].pos[ 1 ] = osc_msg.argv[ 2 ]->f;

            if( record && recordmodes[ sourceId ] )
                new_scoreline( sourceId, timetag, duration );
        }
    }
    else
    {
        setErrorMessage( "[ERROR-ScoreControl::set_source_position()]: src id is out of range\n" );
        cerr << errorMessage << endl;      
        return false;
    }
    

    return true;
}


bool ScoreControl::score_play()
{
    // clear the record buffer, if data is in offline buffer write it first before recording
    if( record )
    {   
        if(  ! score_recordbuffer_offline.empty() )
        {
            playscore.insert( score_recordbuffer_offline.begin(), score_recordbuffer_offline.end() );
            score_recordbuffer_offline.clear();
        }
        score_recordbuffer.clear();
    }
    
    set_play_time( mtc2f() ); // important for setting the playscore iterator to the new playscore
    send_currentScenario();

    mtccom->sc_play = true; // flag shows mtccom that scorecontrol is in play mode
    bool success = mtccom->send_play(); // only works when the audio sequencer speaks mmc or msrc
    
    if( ! success )
    {
        setErrorMessage( "[ERROR-ScoreControl::score_play()]:  sending command play failed\n" );
        cerr << errorMessage << endl;      
        return false;
    }
    
    if( scoreplayerConf->verbose )  
        cout << "[VERBOSE-ScoreControl::score_play()]: playmode is: " << mtccom->sc_play << endl;      
    return true;
}


bool ScoreControl::score_stop()
{
    mtccom->sc_play = false;
    
    bool success = mtccom->send_stop();

    if( ! success )
    {
        setErrorMessage( "[ERROR-ScoreControl::score_stop()]: sending stop msg failed.\n" );      
        cerr << errorMessage << endl;
        return false;
    }
    
    score_enable_record( false );

    if( scoreplayerConf->verbose )  
        cout << "[VERBOSE-ScoreControl::score_stop()]: score is stopped." << endl;

    if( ! score_recordbuffer.empty() )
    {
        // copy everything collected in the recordbuffer into the playbuffer
        playscore.insert( score_recordbuffer.begin(), score_recordbuffer.end() );
        score_recordbuffer.clear();
    }
    
    return true;
}


bool ScoreControl::score_enable_record( bool enabled )
{
    record = enabled;

    // if exiting record mode write everything collected in the offline recordbuffer into the playbuffer
    if( ( ! record ) && ( ! score_recordbuffer_offline.empty() ) )
    {
        playscore.insert( score_recordbuffer_offline.begin(), score_recordbuffer_offline.end() );
        score_recordbuffer_offline.clear();
    }
        

    if( scoreplayerConf->verbose )
        cout << "[VERBOSE-ScoreControl::score_enable_record]: record mode is: " << record << endl;
    
    return true;
}


bool ScoreControl::score_enable_read( bool enabled )
{
    read = enabled;

    if( enabled )
        score_play_function();

    if( scoreplayerConf->verbose )
        cout << "[VERBOSE-ScoreControl::score_enable_read]: read mode is: " << record << endl;
    
    return true;
}


bool ScoreControl::score_enable_record_source( int id, bool enabled )
{    
    recordmodes[ id ] = enabled;
    
    if( enabled )
        score_play_function();

    if( scoreplayerConf->verbose )
        cout << "[VERBOSE-ScoreControl::score_enable_record_source]: record mode of source " 
             << id << " is: " << enabled << endl;
    
    return true;
}


bool ScoreControl::score_enable_read_source( int id, bool enabled )
{    
    readmodes[ id ] = enabled;
    
    if( scoreplayerConf->verbose )
        cout << "[VERBOSE-ScoreControl::score_read_source]: read mode of source " << id << " is: " << enabled << endl;
    
    return true;
}


bool ScoreControl::score_newtime( int hours, int minutes, int seconds, int milliSeconds )
{
    // send mmc goto msg to sequencer, actualize mtc time
    bool  success = false;

    // send new time via midi
    success = mtccom->send_mmc_goto( hours, minutes, seconds, milliSeconds );
    
    // set new system time: scoreplayer, xwonder, osc clients.
    if( success )
    {   
        mtccom->mtc_time[ 0 ] = hours;
        mtccom->mtc_time[ 1 ] = minutes;
        mtccom->mtc_time[ 2 ] = seconds;
        mtccom->mtc_time[ 3 ] = ( int ) ( ( ( double ) milliSeconds / 1000.0 ) * mtccom->framerate_float );
        
        mtccom->position_jump = true;
        if(  ! score_recordbuffer_offline.empty() )
        {
            playscore.insert( score_recordbuffer_offline.begin(), score_recordbuffer_offline.end() );
            score_recordbuffer_offline.clear();
        }
        score_play_function();

        return true;  
    }    
    else
    {
        setErrorMessage( "[ERROR-ScoreControl::score_newtime(float)]: sending mmc_goto msg failed!\n" );      
        cerr << errorMessage << endl;
        return false;
    }
}


bool ScoreControl::score_reset( )
{
    playscore.clear( );
    score_recordbuffer.clear( );
    *startScenario = *currentScenario;

    if( scoreplayerConf->verbose )
    {
        cout << "[VERBOSE-ScoreControl::score_reset()]: resetted current score: ";
    }
    
    return true;
}



bool ScoreControl::score_save( const char* path )
{
    std::ostringstream istr;
    std::ostringstream err; 

    // look for dtd file
    
    string dtdFile = join( scoreplayerConf->dtdPath, "scoreplayer_score.dtd" );
    std::ifstream fin;    
    fin.open( dtdFile.c_str(), std::ios_base::in );
    
    if( ! fin.is_open() )
    {   
        std::ostringstream os; 

        os.str("");
        os << "[ERROR-ScoreControl::score_save()]: can't find dtd: " << dtdFile << " does not exist." << endl; 
        
        setErrorMessage( os.str() );
        cerr << errorMessage << endl;   
        return false;
    }
    else
    {
        if( scoreplayerConf->verbose )
            cout << "[VERBOSE-ScoreControl::score_save()]: using dtd: " << dtdFile << endl;
    }
    
    score_stop();
    int succ = lo_send( scoreplayerConf->cwonderAddr, "/WONDER/stream/visual/send", "s", "/WONDER/score/stop" );
    if( succ < 0 )
        cerr << "[ERROR-ScoreControl::score_save()]: sending OSC message to visual stream failed: err_nr:" << succ << endl;

    string tmppath = "";
    
    if( path == NULL ) // is NULL when no path is given from user
    {
        if( scorefile.empty() )
        {
            if( scoreplayerConf->verbose )
                cerr << "[VERBOSE-ScoreControl::score_save()]: saving scorefile failed, a score must be created or loaded first. " << endl;
            return false;
        }
        else
        {
            tmppath = scorefile;
            if( scoreplayerConf->verbose )
                cout << "[VERBOSE-ScoreControl::score_save()]: saving scorefile: " << tmppath << endl;
        }
    }   
    else // check if path is exists and is absoulte, if not prepend scorefilepath.
    {   
        if( isabs( string( path ) ) )
            tmppath = string( path );
        else
            tmppath = join( scoreplayerConf->scorefilePath, string( path ) );

        //append ".xml", if necessary
        string::size_type pos_beg = tmppath.find( ".xml", 0 );
        
        if( pos_beg >= tmppath.length() )
            tmppath = tmppath + ".xml"; 
    }
    
    if( scoreplayerConf->verbose )
    {
        cout << "[VERBOSE-ScoreControl::score_save(path)]: writing xml score file: " << tmppath << " ..." << endl << endl;
        cout << "             writing scorefile can take up to minutes " << endl << endl;
    }        
    
    int success = file_io.writeXML( tmppath, &playscore, startScenario ); // write current score to new path    
    if( success == 0 )
    {
        istr.str("");
        istr << "[VERBOSE-ScoreControl::score_save(path)]: score is successfully saved to path: " << tmppath << std::ends;
        if( scoreplayerConf->verbose )
            cout << istr.str() << endl;

        scorefile = tmppath;

        return true;
    }
    else if( success == 1 )
    {
        istr.str("");
        istr << "[VERBOSE-ScoreControl::score_save(path)]: writing scorefile skipped because score is empty" << std::ends;
        setErrorMessage( istr.str() );  
        if( scoreplayerConf->verbose )
            cout << errorMessage;

        return false;
    }
    else if( success == -1 )
    {
        return false; // error handling is implemented in file_io::writeXML() 
    }
    else
    {
        istr.str("");
        istr << "[ERROR-ScoreControl::score_save(path)]: saving score to path: " <<  tmppath  << " failed!" << std::ends;
        setErrorMessage( istr.str() );  
        if( scoreplayerConf->verbose )
            cerr << errorMessage;

        return false;           
    }
}


// loads whole scorefile  
bool ScoreControl::score_load( const char* path )
{   
    // Check if the scorefile exists
    std::ostringstream errormsg;    
    std::ifstream fin;
    Glib::ustring filepath = string( path );

    if( ! filepath.empty() )
    {
        if( ! isabs( filepath ) )
            filepath = join( scoreplayerConf->scorefilePath, filepath );

        fin.open( filepath.c_str(), std::ios_base::in );
        
        if( fin.is_open() )
        {
            if( scoreplayerConf->verbose )
                cout << "[VERBOSE-ScoreControl::score_load()]: scorepath is valid: " << filepath << endl;
        }
        else
        {       
        
            string stringpath         = filepath;
            string::size_type pos_beg = stringpath.find( ".xml", 0 );
            
            if( pos_beg >= stringpath.length() )
                filepath = filepath + ".xml";
            
            fin.open( filepath.c_str(), std::ios_base::in );
        
            if( fin.is_open() )
            {
                if( scoreplayerConf->verbose )
                    cout << "[VERBOSE-ScoreControl::score_load()]: scorepath is valid: " << filepath << endl;
            }
            else
            {           
                errormsg.str();
                errormsg << "[ERROR-ScoreControl::score_load()]: score file: " 
                         << filepath << " does not exist." << endl;
                setErrorMessage( errormsg.str() );
                if( scoreplayerConf->verbose )
                    cerr << errormsg.str() << endl;

                return false;
            }    
        }
    }
    else
    {
        setErrorMessage (" [ERROR-ScoreControl::score_load()]: filepath is NULL. loading score file failed.!\n" );
        if( scoreplayerConf->verbose )
            cerr << errorMessage << endl;

        return false;
    }
    
    score_stop();  
    int succ = lo_send( scoreplayerConf->cwonderAddr, "/WONDER/stream/visual/send", "s", "/WONDER/score/stop" );
    if( succ < 0 )
    {
        cerr << "[ERROR-ScoreControl::score_load()]: sending OSC message to visual stream failed: err_nr:" 
             << succ << endl;   
    }

    if( scoreplayerConf->verbose )
    {
        cout << "[VERBOSE-ScoreControl::score_load()]: loading scorefile: " << filepath << endl;
        cout << "                loading scorefile can take up to minutes. depends on score size" << endl << endl; 
    }
    
    // clear all present data
    playscore.clear( );
    score_recordbuffer.clear( );

    int result = file_io.readXML( filepath, &playscore, startScenario );
    if(  result == -1 )
    {
        if( scoreplayerConf->verbose )
            cerr << errorMessage;
        return false;
    }
    else
    {
        scorefile = filepath;
        if( scoreplayerConf->verbose )
            cout << "[VERBOSE-ScoreControl::score_load(path)]: reading xml file successfull. " << endl;
    }

    float mtc_fr = 25.0;
    switch( mtccom->mtc_framerate )
    {
        case 0:
            mtc_fr = 24.0; break;
        case 1:
            mtc_fr = 25.0; break;
        case 2:
            mtc_fr = 30.0; break;
        case 3:
            mtc_fr = 30.0; break;
    }    
    mtccom->send_mmc_goto( mtccom->mtc_time[ 0 ], mtccom->mtc_time[ 1 ], mtccom->mtc_time[ 2 ], mtccom->mtc_time[ 3 ] * ( int ) mtc_fr );
    
    send_scoreplayer_status();
    set_play_time( mtc2f() );
    send_currentScenario();
    
    if( scoreplayerConf->verbose )        
        cout << "[VERBOSE-ScoreControl::score_load()]: loading scorefile done\n" << endl;
    
    project_loaded = true;
    
    return true;
}


bool ScoreControl::score_setStartScenario( )
{
    *startScenario = *currentScenario;
    if( scoreplayerConf->verbose )
        cout << "[VERBOSE-ScoreControl::score_setStartScenario()]: startScenario is written" << endl;
    return true;
}


bool ScoreControl::score_enable_mmc( bool enabled )
{    
    if( scoreplayerConf->verbose )
    {
        if( enabled )
            cout << "[VERBOSE-ScoreControl::score_enable_mmc()]: mmc mode is enabled" << endl;
        else
            cout << "[VERBOSE-ScoreControl::score_enable_mmc()]: mmc mode is disabled" << endl;
    }

    mtccom->mmc_enabled = enabled;
    return true;
}

bool ScoreControl::score_enable_msrc( bool enabled )
{    
    if( scoreplayerConf->verbose )
    {
        if( enabled )
            cout << "[VERBOSE-ScoreControl::score_enable_msrc()]: msrc mode is enabled" << endl;
        else
            cout << "[VERBOSE-ScoreControl::score_enable_msrc()]: msrc mode is disabled" << endl;
    }

    mtccom->msrc_enabled = enabled;
    return true;
}


bool ScoreControl::score_show_mididevices()
{   
    mtccom->show_mididevices();
    return true;
}


int ScoreControl::connect2cwonder()
{
    for( int i = 0; i < 5; ++i )
    {   
        record = true;

        if( scoreplayerConf->verbose )
            cout << "[VERBOSE-scoreplayer::main]: sending connection request to cwonder..." << endl;

        int succ = lo_send( scoreplayerConf->cwonderAddr, "/WONDER/stream/score/connect", "s", scoreplayerConf->name.c_str() );
        if( succ < 0 )
        {
            setErrorMessage( "[ERROR-scoreplayer::main]: sending OSC connection request to cwonder failed!" );
            cerr << errorMessage << endl;
        }
        
        // wait for reply from cwonder
        sleep( 4 );

        if( connection2cwonder )
        {
            if( scoreplayerConf->verbose )
                cout << "[VERBOSE-scoreplayer::main]: successfully connected to cwonder" << endl;

            record = false;
            if( project_loaded ) // means scorefile is read successfully
            {
                score_recordbuffer.clear( ); // clear temporary recorded startScenario data
                startup_init();
                return 0;
            }
            else // no scorefile is loaded, so use cwonder's status report which is recorded into currentScenario -> or score_recordbuffer
            {
                *startScenario = *currentScenario;
                score_recordbuffer.clear( ); // clear temporary recorded startScenario data
                startup_init();
                return 0;
            }
        }
    }
    
    if( ! connection2cwonder )
    {
        record = false;
        setErrorMessage( "[ERROR-scoreplayer]: connecting cwonder failed" );
        cerr <<  errorMessage << endl;
        return -1;
    }

    return 0;
}


void ScoreControl::startup_init()
{
    mtccom->send_stop();                 // general stop
    mtccom->send_mmc_goto( 0, 0, 0, 0 ); // set all sequencers to zero
    send_scoreplayer_status();           // sending mtctime, recordmodes and transports to visual stream
}


void ScoreControl::set_play_time( float t )
{
    // set mtc_time first ! 
    send_mtc_time(); // update time in  wonder/xwonder -> send to visual stream
    int counter      = 0;
    *currentScenario = *startScenario;
    
    // set current scenario
    for( sl_playscore_iter = playscore.begin(); 
         sl_playscore_iter->timestamp <= t  &&  sl_playscore_iter != playscore.end(); 
         ++sl_playscore_iter )
    {
        int sourceId = sl_playscore_iter->intArgs[ 0 ];
        //TODO: (de)activate, name 
        if( sl_playscore_iter->method      == "/WONDER/source/type" )
            currentScenario->sourcesVector[ sourceId ].type  = sl_playscore_iter->intArgs[ 1 ];
        else if( sl_playscore_iter->method == "/WONDER/source/angle" )
            currentScenario->sourcesVector[ sourceId ].angle = sl_playscore_iter->floatArgs[ 0 ];
        else if( sl_playscore_iter->method == "/WONDER/source/position" )
        {
            currentScenario->sourcesVector[ sourceId ].pos[ 0 ] = sl_playscore_iter->floatArgs[ 0 ];
            currentScenario->sourcesVector[ sourceId ].pos[ 1 ] = sl_playscore_iter->floatArgs[ 1 ];
        }
        counter++;
    }
    
    if( scoreplayerConf->verbose )
    {
        cout << "[VERBOSE-ScoreControl::set_play_time(float)]: size of score: " << playscore.size() 
             << ", score iterator is set to element: " << counter;
        
        if( playscore.size() != 0  &&  sl_playscore_iter != playscore.end() )
        {
            cout << ", time " << sl_playscore_iter->timestamp;
            cout << ", id "   << sl_playscore_iter->intArgs[ 0 ] << endl;
        }
        else
            cout << endl;
    }
}


float ScoreControl::mtc2f()
{
    float mtc_fr = 0.0; 
    switch( mtccom->mtc_framerate )
    {
        case 0:
            mtc_fr = 24.0; break;
        case 1:
            mtc_fr = 25.0; break;
        case 2:
            mtc_fr = 30.0; break;
        case 3:
            mtc_fr = 30.0; break;
    }    
    float t =   ( float ) mtccom->mtc_time[ 0 ] * 3600.0 
              + ( float ) mtccom->mtc_time[ 1 ] *   60.0
              + ( float ) mtccom->mtc_time[ 2 ] 
              + ( float ) mtccom->mtc_time[ 3 ] / mtc_fr 
              + ( float ) mtccom->mtc_time[ 4 ] / mtc_fr / 4.0;
    return t;
}    

bool ScoreControl::send_scoreplayer_status()
{
    if( scoreplayerConf->verbose )
        cout << "[VERBOSE-ScoreControl::send_scoreplayer_status]: sending score status information with osc " << endl;

    lo_message statusmessage = lo_message_new();
    string osc_method        = "/WONDER/score/status";  
    lo_message_add_string( statusmessage, osc_method.c_str() );
    lo_message_add_int32(  statusmessage, ( int ) mtccom->sc_play );  
    lo_message_add_int32(  statusmessage, ( int ) record );  
    lo_message_add_int32(  statusmessage, ( int ) read );  

    for( int i = 0; i < scoreplayerConf->maxNoSources; ++i )
    {
        lo_message_add_int32( statusmessage, ( int ) recordmodes [ i ] );  
        lo_message_add_int32( statusmessage, ( int ) readmodes[ i ] );  
    }
    
    if( lo_send_message( scoreplayerConf->cwonderAddr, "/WONDER/stream/visual/send", statusmessage ) < 0 )
    {
        setErrorMessage( "[ERROR-ScoreControl::send_scoreplayer_status()]: sending osc score status to visual stream failed. " );
        cerr << errorMessage << endl;
        lo_message_free( statusmessage );
        return false;
    }
    
    lo_message_free( statusmessage );
    
    if( send_mtc_time() < 0 )
    {
        setErrorMessage( "[ERROR-ScoreControl::send_scoreplayer_status()]: sending mtc time to visual stream failed." );
        cerr << errorMessage << endl;
        return false;
    }    

    if( scoreplayerConf->verbose )
    {
        cout << endl;
        cout << "INTERNAL STATUS:" << endl;
        cout << " cwonder               : " << lo_address_get_hostname( scoreplayerConf->cwonderAddr ) 
             << ":" << lo_address_get_port( scoreplayerConf->cwonderAddr ) << endl;    
        cout << " listeningport         : " << scoreplayerConf->listeningPort << endl;    
        cout << "                                        " << endl;
        cout << " verbose               : " << scoreplayerConf->verbose << endl;
        cout << " osc_verbose           : " << scoreplayerConf->osc_verbose << endl;
        cout << " mtc_verbose           : " << scoreplayerConf->mtc_verbose << endl;
        cout << " screendump_verbose    : " << scoreplayerConf->screendump_verbose << endl;
        cout << "                           " << endl;
        cout << "                                        " << endl;
        cout << " DTD path              : " << scoreplayerConf->dtdPath << endl;
        cout << " scorefile path        : " << scoreplayerConf->scorefilePath<< endl;
        cout << " configfile            : " << scoreplayerConf->scoreplayerConfigfile << endl;
        cout << "                                        " << endl;
        cout << " play status           : " << mtccom->sc_play << endl;
        cout << " record status         : " << record << endl;
        cout << " read status           : " << read << endl;
        cout << " recordmodes (sources) : " << endl;
        cout << "                       : " ;
        for( int i = 0; i < scoreplayerConf->maxNoSources; ++i ) 
        {
            cout << "recordmodes[ " << i << " ]  : " << recordmodes[ i ] << ";   ";
            if( ( i % 5 ) == 0 )
             {
                cout << endl;
                cout << "                       : ";
            }
        }
        cout << endl;
        cout << " readmodes (sources)  : " << endl;
        cout << "                      : " ;
        for( int i = 0; i < scoreplayerConf->maxNoSources; ++i ) 
        {
            cout << "readmodes[ " << i << " ]  : " << readmodes[ i ] << ";   ";
            if( ( i % 5 ) == 0 )
            {
                cout << endl;
                cout << "                      : ";
            }
        }
        cout << endl;
        
        cout << "      size of score                     : " << playscore.size() << endl;
        
        cout << "      mtc framerate (0: 24, 1: 25, 2: 30(drop), 3: 30(non drop): " << mtccom->mtc_framerate << endl;
        
        cout << "      current MIDI input port           : " << mtccom->PortNameIn << endl;
        cout << "      current MIDI output port          : " << mtccom->PortNameOut << endl;
        cout << endl;
    }
    
    return true;
}
