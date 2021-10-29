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

#include "mtc_com.h"

#include "scorecontrol.h"
#include "scoreplayer_config.h"

#include "rtmidi/RtMidi.h"

#include <iostream>
#include <cstdlib>

using std::cout;
using std::cerr;
using std::endl;


MTC_Com::MTC_Com( ScoreControl* sc )
{    
/*****************************/    
// shared values by threads ( MTC_Com and ScoreControl )
    scoreControl          = sc;
    sc_play               = false;
    mmc_enabled           = true;
    msrc_enabled          = false;
    position_jump         = true;
    mtc_frame_completed   = false;

    firstQuarterFrameAfterTransportCommand = false;

    for( int i = 0; i < 8 ; ++i )    
        mtc_nibble[ i ] = 0;
    
    mtc_framerate   = 1;
    framerate_float = 25;

    mtc_time[ 0 ] = 0;
    mtc_time[ 1 ] = 0;
    mtc_time[ 2 ] = 0;
    mtc_time[ 3 ] = 0;
    mtc_time[ 4 ] = 0;


/*****************************/    

    // create RtMidiIn
    try 
    {
        midiin = new RtMidiIn();
    }
    catch( RtError& error ) 
    {
        cerr << "[ERROR-MTC_Com:MTC_COM()]: ";
        error.printMessage();
        cerr << " EXIT_FAILURE: " << EXIT_FAILURE << endl;
        exit( EXIT_FAILURE );
    }
    // create RtMidiOut 
    try 
    {
        midiout = new RtMidiOut();
    }
    catch( RtError& error ) 
    {
        cerr << "[ERROR-MTC_Com:MTC_COM()]: ";
        error.printMessage();
        cerr << " EXIT_FAILURE: " << EXIT_FAILURE << endl;
        exit( EXIT_FAILURE );
    }

    // Don't ignore sysex, timing, or active sensing messages.
    midiin->ignoreTypes( false, false, false );

    if( scoreplayerConf->verbose )
        cout << "[VERBOSE-MTC_Com:MTC_COM()]: creating virtual midi in port ..." << endl;
    
    // open inputport
    try
    {
        midiin->openVirtualPort( "scoreplayer_in" );
    }
    catch( RtError& error ) 
    {
        cerr << "[ERROR-MTC_Com:MTC_COM()]: ";
        error.printMessage();
        cerr << " EXIT_FAILURE: " << EXIT_FAILURE << endl;
        exit( EXIT_FAILURE );         
    }
    PortNameIn = "scoreplayer_in";
    
    // open outputport
    if( scoreplayerConf->verbose )
        cout << "[VERBOSE-MTC_Com:MTC_COM()]: creating virtual midi out port ..." << endl << endl;

    try
    {
        midiout->openVirtualPort( "scoreplayer_out" );
    }
    catch( RtError& error ) 
    {
        cerr << "[ERROR-MTC_Com:MTC_COM()]: ";
        error.printMessage();
        cerr << " EXIT_FAILURE: " << EXIT_FAILURE << endl;
        exit( EXIT_FAILURE );         
    }
    PortNameOut = "scoreplayer_out";              
    
    // Setting our callback function.  This should be done immediately after
    // opening the port to avoid having incoming messages written to the
    // queue.
    midiin->setCallback( &process_midi, this );    
}


MTC_Com::~MTC_Com()
{    
    if( scoreplayerConf->verbose )
        cout << "[VERBOSE-MTC_Com::shutdown_midiengine]: shuttin down midi..." << endl;

    if( midiin )    
    {
        delete midiin;
        midiin = NULL;
    }
    if( midiout )
    {
        delete midiout;
        midiout = NULL;
    }
}


void process_midi( double deltatime, std::vector< unsigned char >* message, void* userData )
{    
    MTC_Com* mtc_com = ( MTC_Com* ) userData;

    const char* framerate = NULL;                    
    
    unsigned int nBytes = message->size();

    unsigned char status = 0;
    
    if( nBytes >= 1 )
        status = ( int ) message->at( 0 );
    
    if( status == 0xF1 ) // MTC quarterframe message
    {
        unsigned char data = 0;

        if( nBytes >= 2 )
            data  = ( int ) message->at( 1 );

        // split data byte into low and high nibble (contains id of message, can be 0-7 )
        unsigned char nibble    = data & 0x0F;
        unsigned char id        = data >> 4;

        mtc_com->mtc_nibble[ id ] = nibble;
        mtc_com->position_jump    = false;
        
        // eigth quarter frame message contains information about the framerate
        if( id == 7  &&  mtc_com->mtc_frame_completed )
        {
            switch( nibble >> 1 )
            {
                case 0: 
                    framerate = "24 fps"; 
                    mtc_com->framerate_float = 24.0;
                    break;
                case 1: 
                    framerate = "25 fps"; 
                    mtc_com->framerate_float = 25.0;
                    break;
                case 2: 
                    framerate = "30 fps (drop frame)";
                    mtc_com->framerate_float = 30.0;
                    break;
                case 3: 
                    framerate = "30 fps"; 
                    mtc_com->framerate_float = 30.0;
                    break;
                default: 
                    cout << "[MTC_COM]: can't calculate framerate of mtc ff-msg . result: " 
                         << ( nibble >> 1) << endl;
            }       
            
            // update mtc_time
            mtc_com->mtc_framerate = ( nibble >> 1 );

            //hours
            mtc_com->mtc_time[ 0 ] = ( ( mtc_com->mtc_nibble[ 7 ] & 0x01 ) << 3 ) | mtc_com->mtc_nibble[ 6 ] ;
            // minutes
            mtc_com->mtc_time[ 1 ] =  ( mtc_com->mtc_nibble[ 5 ] << 4 ) | mtc_com->mtc_nibble[ 4 ] ;
            // seconds
            mtc_com->mtc_time[ 2 ] =  ( mtc_com->mtc_nibble[ 3 ] << 4 ) | mtc_com->mtc_nibble[ 2 ] ;
            // frames
            mtc_com->mtc_time[ 3 ] =  ( mtc_com->mtc_nibble[ 1 ] << 4 ) | mtc_com->mtc_nibble[ 0 ] ;
            // quarterframes
            mtc_com->mtc_time[ 4 ] = 0;


            if( mtc_com->firstQuarterFrameAfterTransportCommand )
            { 
                // set score position accordingly 
                mtc_com->position_jump                           = true;
                mtc_com->firstQuarterFrameAfterTransportCommand  = false;
            }

            mtc_com->scoreControl->send_mtc_time();


            // output message data when in verbose mode
            if( scoreplayerConf->mtc_verbose  &&  scoreplayerConf->screendump_verbose )  
            {
                int* mtc = & ( mtc_com->mtc_time[ 0 ] );
                cout << "[VERBOSE-MTC_Com]: MTC qf message(" << framerate << "): " 
                     << std::dec << mtc[ 0 ] << ": "  
                     << std::dec << mtc[ 1 ] << ": "   
                     << std::dec << mtc[ 2 ] << ": "   
                     << std::dec << mtc[ 3 ] << endl;
            }
        }
        else if( id == 7  &&  ( ! mtc_com->mtc_frame_completed ) )
            mtc_com->mtc_frame_completed = true;     
        else if( id != 7 )
            mtc_com->mtc_time[ 4 ] += 1;

        //mtc_com->scoreControl->send_mtc_time();

        // if ScoreControl is in play mode: read from score
        if( mtc_com->sc_play ) 
            mtc_com->scoreControl->score_play_function();

    } // end of MTC quarter frame handling
    else if(  ( int ) message->at( 0 ) == 0xF0  &&  ( int ) message->at( 3 ) == 0x01  &&  nBytes == 10 )
    {
        // MTC full frame message:  F0 7F cc 01 01 hr mn sc fr F7
        /* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
           F0, 7F, nn, sub-ID, data, F7 - is sysex real time message
           
           nn = channel number, 00 to 7F; 7F = omni 
           sub-IDs: 
           01 = Long Form MTC 
           02 = MIDI Show Control 
           03 = Notation Information 
           04 = Device Control 
           05 = Real Time MTC Cueing 
           06 = MIDI Machine Control Command 
           07 = MIDI Machine Control Response 
           08 = Single Note Retune
           +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */

        mtc_com->position_jump = true;

        // parse message
        int local_framerate = ( int ) message->at( 5 ) >> 5;
        int hours           = ( int ) message->at( 5 ) & 0x1F;
        int minutes         = ( int ) message->at( 6 );
        int seconds         = ( int ) message->at( 7 );
        int frames          = ( int ) message->at( 8 );

        switch( local_framerate )
        {
            case 0: 
                framerate = "24 fps"; 
                mtc_com->framerate_float = 24.0;
                break;
            case 1: 
                framerate = "25 fps"; 
                mtc_com->framerate_float = 25.0;
                break;
            case 2: 
                framerate = "30 fps (drop frame)";
                mtc_com->framerate_float = 30.0;
                break;
            case 3: 
                framerate = "30 fps"; 
                mtc_com->framerate_float = 30.0;
                break;
            default: cout << "[ERROR-MTC_COM]: can't calculate framerate of mtc ff-msg . result: " 
                               << local_framerate << endl;
        }                    
        
        // just a shorthand for the current time
        int* mtc = & ( mtc_com->mtc_time[ 0 ] );

        if(    mtc[ 4 ] == 0 
            && mtc[ 3 ] == frames  
            && mtc[ 2 ] == seconds
            && mtc[ 1 ] == minutes
            && mtc[ 0 ] == hours 
            && mtc_com->mtc_framerate == local_framerate )
        {
                // do nothing because it is the same time
        }
        else
        {           
            mtc_com->mtc_framerate = local_framerate;
            mtc[ 4 ] = 0;
            mtc[ 3 ] = frames;
            mtc[ 2 ] = seconds;
            mtc[ 1 ] = minutes;
            mtc[ 0 ] = hours;
            
            // output message data if in verbose mode
            if( scoreplayerConf->mtc_verbose )  
            {
                cout << "[VERBOSE-MTC_Com]: received MTC ff message (" << framerate << "): " 
                     << std::dec << mtc[ 0 ] << ": "  
                     << std::dec << mtc[ 1 ] << ": "   
                     << std::dec << mtc[ 2 ] << ": "   
                     << std::dec << mtc[ 3 ] << endl;
            }
            
            mtc_com->scoreControl->score_play_function();
        }  
    } // end of MTC full frame handling
    else if( mtc_com->msrc_enabled ) // midi system realtime control messages (MSRC)
    {
        if( status ==  0xFA  ||  status == 0xFB ) // start or continue 
        {
            mtc_com->firstQuarterFrameAfterTransportCommand = true;
            mtc_com->sc_play = true;

            if( scoreplayerConf->osc_verbose )  
                cout << "[VERBOSE-MTC_Com]: sending transport status to OSC visual stream ..." << endl;

            if( lo_send( scoreplayerConf->cwonderAddr, "/WONDER/stream/visual/send", "s", "/WONDER/score/play" ) > -1 ) 
            {
                // do nothing
            }
            else if( scoreplayerConf->osc_verbose )  
                cerr << "[ERROR-MTC_Com]: sending transport status (play) to OSC visual stream failed" << endl;

            if( scoreplayerConf->verbose )
            {
                if( status == 0xFA )
                    cout << "     midi status command: start" << endl;
                else
                    cout << "     midi status command: continue" << endl;
            }
        }
        else if( status == 0xFC ) // stop 
        {
            if( scoreplayerConf->verbose )
                cout << "     midi status command  stop" << endl;

            if( mtc_com->sc_play )
            {
                mtc_com->scoreControl->record = false;
                mtc_com->sc_play              = false;
                
                if( mtc_com->scoreControl->score_recordbuffer.size() != 0 )
                {
                    mtc_com->scoreControl->playscore.insert( mtc_com->scoreControl->score_recordbuffer.begin(), 
                                                             mtc_com->scoreControl->score_recordbuffer.end() );
                    mtc_com->scoreControl->score_recordbuffer.clear();
                }
                
                if( scoreplayerConf->osc_verbose )  
                    cout << "[VERBOSE-MTC_Com]: sending transport status (stop) to OSC visual stream ..." << endl;                  
                
                if( lo_send( scoreplayerConf->cwonderAddr, "/WONDER/stream/visual/send", "s", "/WONDER/score/stop" ) > -1 ) 
                {
                    // do nothing
                }
                else if( scoreplayerConf->osc_verbose )  
                    cerr << "[ERROR-MTC_Com]: sending transport status to OSC visual stream failed" << endl;
            }
            mtc_com->firstQuarterFrameAfterTransportCommand = true;
        }
    } // end of midi system realtime control handling
    else if( mtc_com->mmc_enabled ) // MMC 
    {
        // MMC goto message
        if( nBytes == 13  
            && ( int ) message->at( 0 ) == 0xF0
            && ( int ) message->at( 1 ) == 0x7F 
            && ( int ) message->at( 3 ) == 0x06
            && ( int ) message->at( 4 ) == 0x44 ) 
        {
            // MMC Goto Message: F0 7F deviceID 06 44 06 01 hr mn sc fr ff F7

            mtc_com->position_jump = true; 

            // parse message
            int hours           = ( int ) message->at( 7 ) & 0x1F;
            int minutes         = ( int ) message->at( 8 );
            int seconds         = ( int ) message->at( 9 );
            int frames          = ( int ) message->at( 10 );
            int quarterframes   = ( int ) message->at( 11 );

            // just a shorthand for the current time
            int* mtc = & ( mtc_com->mtc_time[ 0 ] );

            if(    mtc[ 4 ] == 0
                && mtc[ 3 ] == frames
                && mtc[ 2 ] == seconds
                && mtc[ 1 ] == minutes
                && mtc[ 0 ] == hours  )
            {
                // do nothing if we are already at this time
            }
            else
            {
                mtc[ 4 ] = quarterframes;
                mtc[ 3 ] = frames; 
                mtc[ 2 ] = seconds;
                mtc[ 1 ] = minutes;
                mtc[ 0 ] = hours; 
                if( scoreplayerConf->verbose )
                {
                    cout << "[VERBOSE-MTC_Com]: received mmc goto message (hex): ";
                    for ( unsigned int i = 0; i < nBytes; ++i )
                        cout << std::hex << ( int ) message->at( i ) << " ";
                        cout << std::dec << " => time: "
                             << mtc[ 0 ] << ":"
                             << mtc[ 1 ] << ":" 
                             << mtc[ 2 ] << ":"
                             << mtc[ 3 ] << "." 
                             << mtc[ 4 ]
                             << " (hr:min:sec:fr)" << endl; 
                }           
                
                mtc_com->scoreControl->score_play_function();
            }
        } // end of MMC goto handling
        else if(   nBytes == 6  
                && ( int ) message->at( 0 ) == 0xF0
                && ( int ) message->at( 1 ) == 0x7F
                && ( int ) message->at( 3 ) == 0x06 ) // MMC transport commands
        { 
            // MMC Comand: F0 7F deviceID 06 command F7
            /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
               The fifth byte is the command:
               01 Stop 
               02 Play 
               03 Deferred Play 
               04 Fast Forward 
               05 Rewind 
               06 Record Strobe (Punch In) 
               07 Record Exit (Punch out)
               08 Record Ready 
               09 Pause
               ...
               ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
            
            if( scoreplayerConf->verbose )
                cout << "[VERBOSE-MTC_Com]: received mmc command." << endl;

            // if mmc is enabled a reply according to the received message will be sent
            std::vector< unsigned char > replyMessage = *message;
            
            // send mmc message respond according to received signal
            replyMessage.at( 3 ) = 7;
            mtc_com->midiout->sendMessage( &replyMessage ); 

            switch( ( int ) message->at( 4 ) )
            {
                case 1: // stop
                    if( scoreplayerConf->verbose )
                        cout << "     mmc command: stop" << endl;

                    if( mtc_com->sc_play )
                    {
                        mtc_com->scoreControl->record = false;
                        mtc_com->sc_play              = false;
                         
                        if( mtc_com->scoreControl->score_recordbuffer.size() != 0 )
                        {
                            mtc_com->scoreControl->playscore.insert( mtc_com->scoreControl->score_recordbuffer.begin(), 
                                                                     mtc_com->scoreControl->score_recordbuffer.end() );
                            mtc_com->scoreControl->score_recordbuffer.clear();
                        }
                        
                        if( scoreplayerConf->osc_verbose )  
                            cout << "[VERBOSE-MTC_Com]: sending transport status (stop) to OSC visual stream ..." << endl;                  
                        
                        if( lo_send( scoreplayerConf->cwonderAddr, "/WONDER/stream/visual/send", "s", "/WONDER/score/stop" ) > -1 ) 
                        {
                            //break;
                            //do nothing
                        }
                        else if( scoreplayerConf->osc_verbose )  
                            cerr << "[ERROR-MTC_Com]: sending transport status to OSC visual stream failed" << endl;
                    }
                    mtc_com->firstQuarterFrameAfterTransportCommand = true;
                    break;

                case 2: // play
                    if( scoreplayerConf->verbose )
                        cout << "     mmc command: play" << endl;

                    mtc_com->sc_play = true;

                    if( scoreplayerConf->osc_verbose )  
                        cout << "[VERBOSE-MTC_Com]: sending transport status to OSC visual stream ..." << endl;
                    
                    if( lo_send( scoreplayerConf->cwonderAddr, "/WONDER/stream/visual/send", "s", "/WONDER/score/play" ) > -1 ) 
                    {
                        //break;
                        //do nothing
                    }
                    else if( scoreplayerConf->osc_verbose )  
                        cerr << "[ERROR-MTC_Com]: sending transport status (play) to OSC visual stream failed" << endl;

                    mtc_com->firstQuarterFrameAfterTransportCommand = true;
                    break;

                case 3: // deferred play
                    if( scoreplayerConf->verbose )
                        cout << "     mmc command: deferred play" << endl;

                    mtc_com->sc_play = true;

                    if( scoreplayerConf->osc_verbose )  
                        cout << "[VERBOSE-MTC_Com]: sending transport status (play) to OSC visual stream ..." << endl;
                    
                    if( lo_send( scoreplayerConf->cwonderAddr, "/WONDER/stream/visual/send", "s", "/WONDER/score/play" ) > -1 ) 
                    {
                        //break;
                        //do nothing
                    }
                    else if( scoreplayerConf->osc_verbose )  
                        cerr << "[ERROR-MTC_Com]: sending transport status (play) to OSC visual stream failed" << endl;

                    mtc_com->firstQuarterFrameAfterTransportCommand = true;
                    break;

                case 4: // fast forward
                    if( scoreplayerConf->verbose )
                    {
                        cout << "     mmc command: fast forward" << endl;
                        cout << "     scoreplayer has no support for this command" << endl;
                    }
                    break;

                case 5: // rewind
                    if( scoreplayerConf->verbose )
                    {
                        cout << "     mmc command: rewind" << endl;
                        cout << "     scoreplayer has no support for this command" << endl;
                    }
                    break;

                case 6: // record strobe (punch in)
                    if( scoreplayerConf->verbose )
                        cout << "     mmc command: record strobe (punch in) (is rejected, because mmc record is not linked with wonder!)" << endl;
                    break;                  

                case 7: // record exit
                    if( scoreplayerConf->verbose )
                        cout << "     mmc command: record exit (is rejected, because mmc record is not linked with wonder!)" << endl;
                    break;

                case 8: // record
                    if( scoreplayerConf->verbose )
                        cout << "     mmc command: record (rejected, because mmc record is not linked with wonder!)" << endl;
                    break;
                case 9: // pause
                    if( scoreplayerConf->verbose )
                    {
                        cout << "     mmc command: pause" << endl;
                        cout << "     scoreplayer has no support for this command" << endl;
                    }
                    break;                  

                default: // unknown mmc command
                    if( scoreplayerConf->verbose )
                    {
                        cout << "     unknown mmc command: ";
                        for ( unsigned int i = 0; i < nBytes; ++i )
                            cout << std::hex << ( int ) message->at( i ) << " ";
                        cout << endl;             
                    }
                }
        } // end of MMC transport command handling
        else if(    ( int ) message->at( 0 ) == 0xF0 
                 && ( int ) message->at( 1 ) == 0x7E 
                 && ( int ) message->at( 3 ) == 0x06 
                 && ( int ) message->at( 4 ) == 0x01 ) // MMC Idendity request
        {
            // MMC Identity Request: F0 7E channel 06 01 F7 - is Sysex NonRealTime
            
            if( scoreplayerConf->verbose )
            {
                cout << "[VERBOSE-MTC_Com]: received mmc identity request: ";
                for ( unsigned int i = 0; i < nBytes; ++i )
                    cout << std::hex << ( int ) message->at( i ) << " ";
                cout << endl << "[VERBOSE-MTC_Com]: answering mmc identity request (sending reply) ..." << endl;
            }

            // generate mmc identity request reply:
            std::vector< unsigned char > mmc_ir_reply;
            mmc_ir_reply.push_back( 0xF0 ); //  SysEx
            mmc_ir_reply.push_back( 0x7E ); // Non Realtime
            mmc_ir_reply.push_back( 127  ); // channel
            mmc_ir_reply.push_back(   6  ); // sub id 1
            mmc_ir_reply.push_back(   2  ); // sub id 2
            mmc_ir_reply.push_back( 127  ); // 1 ID (of hard disc recorder?) but 0-127 possible
            mmc_ir_reply.push_back(   0  ); // fc1: device's family code
            mmc_ir_reply.push_back(   0  ); // fc2
            mmc_ir_reply.push_back(   0  ); // fn1: device's family number
            mmc_ir_reply.push_back(   0  ); // fn2
            mmc_ir_reply.push_back(   0  ); // version1
            mmc_ir_reply.push_back(   0  ); // version2
            mmc_ir_reply.push_back(   0  ); // version3
            mmc_ir_reply.push_back(   1  ); // version4
            mmc_ir_reply.push_back( 0xF7 ); // EOX

            mtc_com->midiout->sendMessage( &mmc_ir_reply );
        } // end of MMC Identiy request handling
        else if(    nBytes > 6
                 && ( int ) message->at( 0 ) == 0xF0 
                 && ( int ) message->at( 1 ) == 0x7E 
                 && ( int ) message->at( 3 ) == 0x06 
                 && ( int ) message->at( 4 ) == 0x02 ) // MMC Identiy reply
        {
            // MMC Identity Reply: F0, 7E, channel, 06 02 ID fc1 fc2 fn1 fn2 v1 v2 v3 v4 F7 - is Sysex NonRealTime 
            /* ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++                             parameters:
               
               ID - Device's ID 
               fc1 fc2  - Device's family code 
               fn1 fn2  - Device's family number 
               v1 v2 v3 v4 - Software Version 
               ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ */
            if( scoreplayerConf->verbose )
            {
                cout << "[VERBOSE-MTC_Com]: received an mmc identity reply: ";
                for ( unsigned int i = 0; i < nBytes; ++i )
                    cout << std::hex << ( int ) message->at( i ) << " " << endl;
                }
        } // end of MMC Identity reply handling
    } // end of MMC handling
    else if(    nBytes > 1 
             && ( int ) message->at( 0 ) == 0xF0 
             && ( int ) message->at( 1 ) != 0x7F 
             && ( int ) message->at( 1 ) != 0x7E ) // other manufacturer specific sysex
        {
        if( scoreplayerConf->verbose )
        {
            cout << "[VERBOSE-MTC_Com]: received manufacturer specific sysex message: ";
            for ( unsigned int i = 0; i < nBytes; ++i )
                cout << std::hex << ( int ) message->at( i ) << " ";
            cout << endl;
        }
    } 
    else if ( ( int ) message->at( 0 ) == 240 ) // every possible other sysex message
    {
        if( scoreplayerConf->verbose )
        {
            cout << "[VERBOSE-MTC_Com]: received unknown sysex message: ";
            for ( unsigned int i = 0; i < nBytes; ++i )
                cout << std::hex << ( int ) message->at( i ) << " ";
            cout << endl;
        }
    } 
    else // unknown midi messages
    {
        if( scoreplayerConf->verbose )
        {
            cout << "[VERBOSE-MTC_Com]: received unknown midi message format: ";
            for ( unsigned int i = 0; i < nBytes; ++i )
                cout << std::hex << ( int ) message->at( i ) << " ";
            cout << endl;
        }
    } 
}


void MTC_Com::show_mididevices()
{
    // Check inputs.  Check outputs.
    unsigned int nPortsIn  = midiin ->getPortCount();
    unsigned int nPortsOut = midiout->getPortCount();
    std::string portNameIn;
    std::string portNameOut;

    cout << endl;
    cout << "[VERBOSE-MTC_Com::show_mididevices]: " << endl;
    cout << endl;

    for ( unsigned int i = 0; i < nPortsIn; ++i ) 
    {
        try 
        {
            portNameIn = midiin->getPortName( i );
            cout << "    midi input port: " << i << ": " << portNameIn << '\n';
        }
        catch( RtError& error ) 
        {
            error.printMessage();
            return;
        }
    }
    for ( unsigned int i = 0; i < nPortsOut; ++i )
    {
        try
        { 
            portNameOut = midiout->getPortName( i );
            cout << "    midi output port: " << i << ": " << portNameOut << '\n';
        }
        catch(RtError& error ) 
        {
            error.printMessage();
            return;
        }
        
    }
    cout << endl;
}


bool MTC_Com::set_midiin_dev( int id )
{
    unsigned int nPortsIn = midiin->getPortCount();

    if( id >= 0  &&  id < ( int ) nPortsIn )
    {
        if( scoreplayerConf->verbose )
            cout << "[VERBOSE-MTC_Com::set_midiin_dev]: closing current midi input port: "
                 << PortNameIn << endl;

        midiin->closePort();
        mtc_frame_completed = false;
        midiin->openPort( id );
        PortNameIn = midiin->getPortName(id);

        if( scoreplayerConf->verbose)
            cout << "[VERBOSE-MTC_Com::set_midiin_dev]: opening midi input port: "
                 << PortNameIn << endl;
    }
    else
    {
        cout << "[ERROR-MTC_Com::set_midiin_dev]: wrong midiport id." << endl;
        return false;
    }
    return true;
}


bool MTC_Com::set_midiout_dev( int id )
{
    unsigned int nPortsOut = midiout->getPortCount();
    
    if( id >= 0  &&  id < ( int ) nPortsOut )
    {
        if( scoreplayerConf->verbose )
            cout << "[VERBOSE-MTC_Com::set_midiout_dev]: closing current midi output port: "
                 << PortNameOut << endl;

        midiout->closePort();
        midiout->openPort( id );
        PortNameOut = midiout->getPortName( id );

        if( scoreplayerConf->verbose )
            cout << "[VERBOSE-MTC_Com::set_midiout_dev]: opening midi output port: "
                 << PortNameOut << endl;
    }
    else
    {
        cout << "[ERROR-MTC_Com::set_midiout_dev]: wrong midiport id." << endl;
        return false;
    }
    return true;    
}


bool MTC_Com::send_mmc_identity_request( int chan )
{
   /// send MMC Identity Request. byteformat: F0 7E channel 06 01 F7;

    if( scoreplayerConf->verbose )
        cout << "[VERBOSE-MTC_Com]: sending mmc identity request ...  " << endl;

    std::vector<unsigned char> mmc_id_request;

    mmc_id_request.push_back( 0xF0 ); 
    mmc_id_request.push_back( 0x7E ); 
    mmc_id_request.push_back( 0x00 ); // on channel
    mmc_id_request.push_back( 0x06 ); 
    mmc_id_request.push_back( 0x01 ); 
    mmc_id_request.push_back( 0xF7 ); 
    
    try
    {
        midiout->sendMessage( &mmc_id_request );
    }
    catch( RtError& error )
    {
        error.printMessage();
        return false;
    }
    return true;
}


bool MTC_Com::send_mmc_goto( int hours, int minutes, int seconds, int milliSeconds )
{
    if( ! mmc_enabled )
        return true;

    /// send MMC goto message: byteformat: MMC Goto Message: F0 7F deviceID 06 44 06 01 hr mn sc fr ff F7
    if( scoreplayerConf->verbose )
        cout << "[VERBOSE-MTC_Com]: sending mmc goto message ...";

    float mtc_fr = 0.0;
    switch( mtc_framerate )
    {
        case 0:
            mtc_fr = 24.0;
            break;
        case 1:
            mtc_fr = 25.0;
            break;
        case 2:
            mtc_fr = 30.0;
            break;
        case 3:
            mtc_fr = 30.0;
            break;
    }    

    int frames = ( int ) ( ( ( double ) milliSeconds / 1000.0 ) * mtc_fr );

    if( scoreplayerConf->verbose )
        cout << "    " << hours << ":" << minutes << ":" << seconds << ":" << frames << endl;

    std::vector< unsigned char > mmc_goto;

    mmc_goto.push_back( 0xF0 );
    mmc_goto.push_back( 0x7F );
    mmc_goto.push_back( 0x7F ); 
    mmc_goto.push_back( 0x06 );  
    mmc_goto.push_back( 0x44 );  
    mmc_goto.push_back( 0x06 );  
    mmc_goto.push_back( 0x01 );  
    mmc_goto.push_back( hours | ( mtc_framerate << 5 ) );  
    mmc_goto.push_back( minutes );  
    mmc_goto.push_back( seconds );  
    mmc_goto.push_back( frames );  
    mmc_goto.push_back(   0 ); // no subframes supported  
    mmc_goto.push_back( 0xF7 );

    try
    {
        midiout->sendMessage( &mmc_goto );
    }
    catch( RtError& error )
    {
        error.printMessage();
        return false;
    }
    return true;
}


bool MTC_Com::send_stop()
{
    firstQuarterFrameAfterTransportCommand = true;


    if( mmc_enabled )
    {
        std::vector< unsigned char > mmc_command;

        mmc_command.push_back( 0xF0 );  
        mmc_command.push_back( 0x7F ); 
        mmc_command.push_back( 0x7F ); // on channel
        mmc_command.push_back( 0x06 );  
        mmc_command.push_back( 0x01 ); 
        mmc_command.push_back( 0xF7 );  

        if( scoreplayerConf->verbose )
            cout << "[VERBOSE-MTC_Com]: sending mmc command: stop" << endl;

        // send message
        try
        {
            midiout->sendMessage( &mmc_command );
        }
        catch( RtError& error )
        {
            error.printMessage();
            return false;
        }
    }

    if( msrc_enabled )
    {
        std::vector< unsigned char > msrc_command;
        msrc_command.push_back( 0xFC ); 
        
        // send message
        try
        {
            midiout->sendMessage( &msrc_command );
        }
        catch( RtError& error )
        {
            error.printMessage();
            return false;
        }
    }

    return true;
}


bool MTC_Com::send_play()
{
    firstQuarterFrameAfterTransportCommand = true;

    if( mmc_enabled )
    {
        std::vector< unsigned char > mmc_command;

        mmc_command.push_back( 0xF0 ); 
        mmc_command.push_back( 0x7F );
        mmc_command.push_back( 0x7F ); // on channel
        mmc_command.push_back( 0x06 ); 
        mmc_command.push_back( 0x02 ); 
        mmc_command.push_back( 0xF7 ); 

        if( scoreplayerConf->verbose )
            cout << "[VERBOSE-MTC_Com]: sending mmc command: play" << endl;
        try    
        {
            midiout->sendMessage( &mmc_command );
        }
        catch( RtError& error )
        {
            error.printMessage();
            return false;
        }
    }
    else if( msrc_enabled )
    {
        std::vector< unsigned char > msrc_command;
        msrc_command.push_back( 0xFB ); 

        if( scoreplayerConf->verbose )
            cout << "[VERBOSE-MTC_Com]: sending msrc command: continue" << endl;

        try    
        {
            midiout->sendMessage( &msrc_command );
        }
        catch( RtError& error )
        {
            error.printMessage();
            return false;
        }
    }

    return true;
}
