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

/**
 * w_osc (deprecated shortname from wonder_osc)
 */

#ifndef OSCSERVER_H
#define OSCSERVER_H

#include <string>

#include <lo/lo.h>

class ScoreControl;


// arguments of the handler functions
#define handlerArgs const char* path, const char* types, lo_arg** argv, int argc, lo_message msg, void* user_data

class W_Osc
{

public:
    W_Osc( const char* port );
    ~W_Osc();

    int server_thread_start();
    int server_thread_stop();
    
    int       get_port();
    lo_server get_server();

    void set_control_methods( ScoreControl* ctl );

private:
    lo_server_thread stp;


    static void error( int num, const char* m, const char* path );
    static int reply_handler( handlerArgs );

    static int start_handler( handlerArgs );
    static int stop_handler ( handlerArgs );

    static int global_maxNoSources_handler( handlerArgs );
    static int source_activate_handler( handlerArgs );
    static int source_deactivate_handler( handlerArgs );
    static int source_name_handler( handlerArgs );
    static int source_type_handler( handlerArgs );
    static int source_angle_handler( handlerArgs );
    static int source_position_handler( handlerArgs );

    static int score_create_handler          ( handlerArgs );
    static int score_load_handler            ( handlerArgs );
    static int score_save_handler            ( handlerArgs );
    static int score_play_handler            ( handlerArgs );
    static int score_stop_handler            ( handlerArgs );
    static int score_newtime_handler         ( handlerArgs );
    static int score_offset_handler          ( handlerArgs );
    static int score_reset_handler           ( handlerArgs );
    static int score_setStartscenario_handler( handlerArgs );

    static int score_enable_record_handler       ( handlerArgs );
    static int score_enable_record_source_handler( handlerArgs );
    static int score_enable_read_handler         ( handlerArgs );
    static int score_enable_read_source_handler  ( handlerArgs );

    static int score_enable_mmc_handler      ( handlerArgs );
    static int score_enable_msrc_handler     ( handlerArgs );
    static int score_status_handler          ( handlerArgs );

    static int cwonder_ping_handler( handlerArgs );

    static int generic_handler( handlerArgs );   

}; // end of class W_Osc

#endif // W_OSC
