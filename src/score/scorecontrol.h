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

#ifndef SCORECONTROL_H
#define SCORECONTROL_H

#include <string>
#include <set>
#include <vector>

#include "score_line.h"
#include "file_io.h"

#include <lo/lo.h> 

class Scenario;
class MTC_Com;


struct osc_message
{
    const char* path;
    const char* types;
    lo_arg**    argv;
    int         argc;
};


class ScoreControl
{
 public:    
    ScoreControl();
    ~ScoreControl();

    // initialise everything that depends on runtime value of maxNoSources 
    // which is set via OSC by cwonder at initial connection.
    // should only be called after maxNoSources has been set.
    void init();

    // startScenario describes all the sources with their attributes at time 0.0s
    Scenario* startScenario;

    // currentScenario describes the current situation of all sources
    Scenario* currentScenario;

     
    // the errorMessage is set by every function which is called from an osc callback. 
    // errorMessage is readable from w_osc - the osc class.
    std::string errorMessage;

    // osc_msg is set from w_osc every time an osc message is received with the last osc message data.
    osc_message osc_msg;

    // the main object which handles the mtc and mmc communication with the external sequencer
    MTC_Com* mtccom;        

    // flag is set true when connection to cwonder was successfull
    bool connection2cwonder; 

    // flag: recording mode is on: true, off: false
    bool record;

    // flag: read mode is on: true, off: false
    bool read;

    // for startup: if a project is not loaded in cwonder use current system state as startScenario and clear score.
    bool project_loaded;

    // this array contains recordmode flag of each source
    bool* recordmodes;

    // this array contains readmode flag of each source
    bool* readmodes;

    // is used for playback while the score_recordbuffer and the score_recordbuffer_offline is used for recordings
    std::multiset< ScoreLine > playscore;

    // playback iterator of the current score: pointer to the current scoreline.
    std::multiset< ScoreLine >::iterator sl_playscore_iter;

    // score recording buffer for threadsafety.
    std::multiset< ScoreLine > score_recordbuffer;

    // record buffer for offline recording, i.e. recording while not playing
    std::multiset< ScoreLine > score_recordbuffer_offline;

    // class for import and export of score files
    File_IO file_io; 

    
    // this is called from mtccom and plays back the score data (or records it), 0 = success
    int score_play_function();

    // send out the current state of the scenario as osc messages, 0 = success
    int send_currentScenario();

    // set the error message for the osc callbacks
    void setErrorMessage( std::string error );

    // set the last incomming osc message - called from osc callbacks
    void set_osc_message( const char* path, const char* types, lo_arg** argv, int argc, lo_message msg );

    // this is executed at startup for connection with cwonder on the score stream. 0 = success, -1 = connection failed
    int connect2cwonder();

    // send current mtc time to visual stream
    int send_mtc_time();

    // set scoreplayer to play mode
    bool score_play();

    // stop scoreplayer 
    bool score_stop();

    // set scoreplayer to record mode
    bool score_enable_record( bool enabled );

    // enable readmode of scoreplayer, i.e. playback of recorded score data
    bool score_enable_read( bool enabled );

    // enable a source for recording
    bool score_enable_record_source( int id, bool enabled );

    // enable reading (playback of score data) for this source 
    bool score_enable_read_source( int id, bool enabled );

    // set scoreplayer to new time - this concerns the playscore iterator and the external audio-senquencer
    bool score_newtime( int hours, int minutes, int seconds, int milliSeconds );

    // resets current score - deletes it's content
    bool score_reset();

    // save score to path: path. path can be NULL, absolute or just a filename - 
    // it is then saved to the current project path
    bool score_save( const char* path );

    // load score from scorefile path. path can be absolute or just a filename -
    //  it is then searched in the current project path
    bool score_load( const char* path );

    // set the current system state as the startScenario -
    // the startScenario is save in the scorefile as orchestra description
    bool score_setStartScenario( );

    // enable midi machine control mode
    bool score_enable_mmc( bool enabled );
 
    // enable midi system realtime control mode
    bool score_enable_msrc( bool enabled );

    bool score_show_mididevices();

    // creates a new and empty scorefile
    bool score_create( std::string filename );

    // (de)activate a source
    bool set_source_active( int index, bool active );

    // set name of source
    bool set_source_name( int index, std::string name );
    
    // set type of source: 0 plane, 1 point 
    bool set_source_type( int index, float timetag );

    // set angle on plane wave: id float degree duration time
    bool set_source_angle( int index, float duration, float timetag );

    // set position of source: id x y duration time 
    bool set_source_position( int index, float duration, float timetag);

    // send current status of the scoreplayer to the visual stream and print it to stdout
    bool send_scoreplayer_status();  


private:
    // name of current scorefile ( absolute path )
    std::string scorefile;

    // initialises mtc, mmc and other transport related values to 0s
    void startup_init();

    // set the playscore iterator to new time and sends out the current system state. in seconds.
    void set_play_time( float t );

    /// helperfunction for coverting time from mtc to float
    float mtc2f();

    bool new_scoreline( int index, float timetag, float duration );

}; // end of class ScoreControl
 
#endif // SCORECONTROL_H
