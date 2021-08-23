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

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <csignal>
#include <cstdlib>
#include <fmt/core.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>

#include <jack/jack.h>
#include <jack/transport.h>

#include <lo/lo.h>

#include "jackringbuffer.hpp"
#include "config.h"
#include "wonder_path.hpp"
#include "jfwonder_config.h"

using namespace std;

jack_client_t* jackClient;
jack_nframes_t lastFrame  = 0;
jack_nframes_t lastFrame2 = 0;

bool start = false;

// the control rate
// #define CONTROLRATE 1024
// now a command line option

// used when running daemonized
std::string path_pidfile;



// class to exchange data between the osc and the realtime thread

class FrameQueue : private JackRingbuffer
{

public:
    FrameQueue()
    {
        pthread_mutex_init( &mutex, NULL );
    }

    ~FrameQueue()
    {
        pthread_mutex_destroy( &mutex );
    }

    void put( jack_nframes_t frame )
    {
        write( reinterpret_cast< void * >( frame == 0 ? 1 : frame ) );
    }

    jack_nframes_t get()
    {
        jack_nframes_t retval;

        pthread_mutex_lock( &mutex );
        retval = ( jack_nframes_t ) reinterpret_cast< uint64_t >( read() );
        pthread_mutex_unlock( &mutex );

        return retval;
    }

private:
    pthread_mutex_t mutex;

};

FrameQueue* frameQueue;



int startDaemon()
{
    pid_t pid;

    pid = fork();

    if( pid  < 0 )
	exit( EXIT_FAILURE );

    if( pid > 0 )
	exit( EXIT_SUCCESS );

    if( setsid() < 0 )
        exit( EXIT_FAILURE  );

    if( ( chdir( "/" ) ) < 0 )
        exit( EXIT_FAILURE  );

    umask( 0 );

    for( int i = sysconf( _SC_OPEN_MAX ); i > 0; --i )
        close( i );


    // check if the directory for the pid file exists
    ifstream fin;
    string path = "/tmp/wonder";
    fin.open( path.c_str() , ios_base::in );

    if( !fin.is_open() )
    {
        // create /tmp/wonder directory if it does not exist yet
        // CAUTION: anyone is able to create files, but is not able to delete anyone elses files but their own
        int error = mkdir( path.c_str(), mode_t( S_IRWXU | S_IRWXG | S_IRWXO | S_ISVTX ) );
        if( error != 0 )
            return 1;
    }

    // open file stream to write the pid in
    string name;
    ostringstream spid;

    // get pid of parent process
    spid << getpid();

    if( jfConf->user == NULL )
        path_pidfile = path + "/jfwonder." + spid.str() + ".pid";
    else
        path_pidfile = path + "/jfwonder." + ( string ) jfConf->user + ".pid";

    ofstream pid_file( path_pidfile.c_str() );
    pid_file << spid.str();
    pid_file.close();

    // write stdin to /dev/null
    freopen("/dev/null", "r", stdin );

    // write stdout and stderr to the log file
    freopen( "/var/log/wonder.log", "w", stdout );
    freopen( "/var/log/wonder.log", "w", stderr );

    return 0;
}


int cleanDaemon()
{
    // delete the pid file
    if( jfConf->daemon )
    {
        if( remove( path_pidfile.c_str() ) != 0 )
        {
            fmt::print( "[E-Config] Error deleting pid file.\n" );
            return 1;
        }
        else
        {
            if( jfConf->verbose )
                fmt::print( "[V-Config] Deleted pid file.\n" );
        }
    }
    return 0;
}


int genericHandler( const char* path, const char* types, lo_arg** argv, int argc, lo_message data, void* user_data )
{
    int i;

    fmt::print(  ("[V-OSCServer] --- generic handler --- No handler implemented for message: "
                               + ( string ) path ).c_str()  );

    for( i = 0; i < argc; ++i )
        cout << "arg " << i << "'" << types[ i ] << "'" << endl;
    cout << endl;

    lo_address from = lo_message_get_source( data );
    lo_send( from, "/WONDER/reply", "sis", path, 1, "[genericHandler] No handler implemented for message" );
    return 0;
}


int connectHandler( const char* path, const char* types, lo_arg** argv, int argc, lo_message data, void* user_data )
{
    start = true;
    fmt::print( "[V-Wonder] Cwonder is up. Start sending the frametime.\n" );

    return 0;
}


void showTime()
{
    jack_nframes_t frameTime = frameQueue->get();

    if( frameTime )
    {
        // We cast the unsigned int to a signed integer because OSC has no native support for unsigned integers.
        // When receiving we cast back to an uint.

        lo_send( jfConf->cwonderAddr, "/WONDER/jfwonder/frametime", "i", ( int32_t ) frameTime );

        if( jfConf->jfverbose )
            cout << "@ " << frameTime << " - " << lastFrame2 << " -> " << ( frameTime - lastFrame2 ) << endl;

        lastFrame2 = frameTime;
    }
}


void jackShutdown( void* arg )
{
    lo_send( jfConf->cwonderAddr, "/WONDER/jfwonder/error", "s", "the jack server has been shutdown." );

    fmt::print( "[E-Jack] the jack server has been shutdown, exiting ...\n" );

    cleanDaemon();

    exit( EXIT_FAILURE );
}


void signalHandler( int signal )
{
    jack_client_close( jackClient );

    lo_send( jfConf->cwonderAddr, "/WONDER/jfwonder/error", "s", "the program has been stopped." );

    fmt::print( "[E-Wonder] signal received, exiting ...\n" );

    cleanDaemon();

    exit( EXIT_SUCCESS );
}


int jackProcess( jack_nframes_t nframes, void* arg )
{
    jack_position_t pos;
    jack_nframes_t  frame;

    jack_transport_query( jackClient, &pos );

    frame = pos.frame;

    if( ( frame - lastFrame ) >= jfConf->controlRate )
    {
        lastFrame = pos.frame;
        frameQueue->put( frame );
    }

    return 0;
}


int main( int argc, char* argv[] )
{
    int cwonderTimeOut = 100;

    jfConf = new jfwonderConfig( argc, argv );

    fmt::print(  "[V] ----------start jfwonder--------\n" );
    fmt::print(  "[V-settings] cwonder host={}, port={}\n" , lo_address_get_hostname( jfConf->cwonderAddr ), lo_address_get_port( jfConf->cwonderAddr ) );

    // daemonize
    if( jfConf->daemon )
    {
        if( startDaemon() != 0 )
        {
            fmt::print( "[E-Config] mkdir /tmp/wonder directory error.\n" );
            cleanDaemon();
            return 1;
        }
    }

    if ( ( jackClient = jack_client_open( "showtime", JackNullOption, nullptr ) ) == 0 )
    {
        fmt::print( "[E-Jack] jack server not running?\n" );
        cerr << "[E-Jack] jack server not running? exiting now..." << endl;
        cleanDaemon();
        return 1;
    }

    frameQueue = new FrameQueue();

    signal( SIGQUIT, signalHandler );
    signal( SIGTERM, signalHandler );
    signal( SIGHUP,  signalHandler );
    signal( SIGINT,  signalHandler );

    jack_on_shutdown( jackClient, jackShutdown, 0 );

    jack_set_process_callback( jackClient, jackProcess, NULL );

    if( jack_activate( jackClient ) )
    {
        fmt::print( "[E-Jack] cannot activate client.\n" );
        if( frameQueue )
        {
            delete frameQueue;
            frameQueue = NULL;
        }

    cleanDaemon();
        return 1;
    }

    lo_server_thread loServer = lo_server_thread_new( jfConf->listeningPort, NULL );

    if( loServer )
    {
        // add the methods to the osc server
        lo_server_thread_add_method( loServer, "/WONDER/jfwonder/connect", "", connectHandler, NULL);
        lo_server_thread_add_method( loServer, NULL, NULL, genericHandler, NULL);

        // start the osc server
        lo_server_thread_start( loServer );

        // start the jack trannsport
        jack_transport_start( jackClient );

        // check if cwonder is up
        lo_send( jfConf->cwonderAddr, "/WONDER/jfwonder/connect", "" );

        bool keepRunning  = true;
        while( keepRunning )
        {
            usleep( 1000 );
            if( start )
            {
                showTime();
            }
            else
            {
                cwonderTimeOut -= 1;
                sleep( 1 );
                fmt::print( "[E-Wonder] Cwonder is not up. Retry.\n" );
                lo_send( jfConf->cwonderAddr, "/WONDER/jfwonder/connect", "" );

                if( cwonderTimeOut == 0 )
                {
                    fmt::print( "[E-Wonder] Cwonder is not up. Quit.\n" );
                    keepRunning = false;
                }
            }
        }

        jack_transport_stop( jackClient );

        if( jack_transport_locate( jackClient, jack_nframes_t( 0 ) ) == 0 )
            fmt::print( "[V-Jack] Set transport back.\n" );
        else
            fmt::print( "[E-Jack] Problem setting transport back.\n" );

        lo_server_thread_free( loServer );
    }
    else
        fmt::print( "[E-OSCServer] Could not start server.\n" );

    // cleanup
    cleanDaemon();
    delete frameQueue;
    delete jfConf;
}
