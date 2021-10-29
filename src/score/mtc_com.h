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


#ifndef MTC_COM_H
#define MTC_COM_H

#include <string>
#include <vector>

class RtMidi;
class RtMidiIn;
class RtMidiOut;

class ScoreControl;


// this global RTMIDI callback function is executed for every incoming RTMIDI event
void process_midi( double deltatime, std::vector<unsigned char>* message, void* userData );

// sends and receives midi time code (mtc)
// sends and receives midi machine control (mmc)
class MTC_Com 
{
public: 
    RtMidiIn*  midiin;
    RtMidiOut* midiout;

    unsigned int numPortsIn;
    unsigned int numPortsOut;

    std::string PortNameIn;
    std::string PortNameOut;

    // variables used by rtmidi callback:
    // the rtmidi callback is running in a different thread. so pay attention to threadsaveness

    // is set true when a mtc a timeframe is complete 
    // happens after a minimum of eight quarter frame messages
    bool mtc_frame_completed;

    // containes the mtc nibbles
    int mtc_nibble[ 8 ];

    // containes current mtc time hr:min:sec:frame:quarterframe 
    int mtc_time[ 5 ];

    // containes the current framerate ( as index )
    int mtc_framerate;

    // framerate as float
    float framerate_float;

    // flag shows if mmc is enabled (default is enabled, i.e. true)
    bool mmc_enabled;  

    // flag shows if midi system realtime control is enabled (default is disabled, i.e. false)
    // this is an alternative to mmc
    bool msrc_enabled;  

    // flag shows scorecontrol if a jump in time happened
    bool position_jump;

    // flag shows mtccom that scorecontrol  is in playmode    
    bool sc_play;     

    // needed for compatibility with sequencers who do not implement MMC/MTC like Ardour ( for which this scoreplayer
    // was planned to work with ... )
    bool firstQuarterFrameAfterTransportCommand;

    // scorecontrol context pointer 
    ScoreControl* scoreControl; 

    // Constructor with scorecontrol context pointer
    MTC_Com( ScoreControl* sc );
    ~MTC_Com();
    
    // sets the MIDI input device by it's id
    bool set_midiin_dev( int id );

    // sets the MIDI output device by it's id
    bool set_midiout_dev( int id );

    // function for sending an mmc identity request. chan = machine id
    bool send_mmc_identity_request( int chan );

    // sends a mmc goto message  
    bool send_mmc_goto( int hours, int minutes, int seconds, int milliSeconds );

    // sends a mmc and/or msrc stop message on all midi channels
    bool send_stop();

    // sends a mmc and/or msrc play message on all midi channels
    bool send_play();
    
    // prints available mididevices to console
    void show_mididevices();

    
}; // end of class MTC_Com

#endif // MTC_COM_H 
