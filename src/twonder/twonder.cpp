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

#include <jack/jack.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <algorithm>
#include <chrono>
#include <csignal>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <memory>
#include <ratio>
#include <sstream>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include "oscin.h"
#include "rtcommandengine.h"
#include "source.h"
#include "speaker.h"
#include "timestamp.h"
#include "twonder_config.h"

// global variables
jack_client_t* jackClient;
jack_port_t** jackInputs;
jack_port_t** jackOutputs;

SpkArray* speakers;
SourceArray* sources;
OSCServer* oscServer;
RTCommandEngine* realtimeCommandEngine;

void exitCleanupFunction() {
    // Close JACK client and free ports
    jack_deactivate(jackClient);
    jack_client_close(jackClient);
    std::free(jackInputs);
    std::free(jackOutputs);

    if (oscServer) {
        oscServer->stop();
        delete oscServer;
        oscServer = nullptr;
    }

    if (twonderConf) {
        delete twonderConf;
        twonderConf = nullptr;
    }

    if (speakers) {
        delete speakers;
        speakers = nullptr;
    }

    if (sources) {
        delete sources;
        sources = nullptr;
    }

    if (realtimeCommandEngine) {
        delete realtimeCommandEngine;
        realtimeCommandEngine = nullptr;
    }
}

// Shutdown callback process if the server shuts down or disconnects this jack client
void jack_shutdown(void* arg) {
    std::cerr << "[JACK]: JACK server was shut down or disconnected this JACK client - "
                 "exiting now!\n";
    std::exit(EXIT_FAILURE);
}

//-------------------------------Audio processing----------------------------//

int process(jack_nframes_t nframes, void* arg) {
    // process incoming commands
    realtimeCommandEngine->evaluateCommands((wonder_frames_t)10);

    // process audio

    // get the input buffers from the soundcard and put each of them into the delayline of
    // a source
    for (int i = 0; i < twonderConf->noSources; ++i) {
        float* input = static_cast<float*>(jack_port_get_buffer(jackInputs[i], nframes));
        (*sources)[i]->inputline->put(input, nframes);
    }

    // get the output buffers of the soundcard and mute them first
    for (unsigned int i = 0; i < speakers->array.size(); ++i) {
        speakers->array.data()[i]->outPtr =
            static_cast<float*>(jack_port_get_buffer(jackOutputs[i], nframes));
        std::memset(speakers->array.data()[i]->outPtr, 0, nframes * sizeof(jack_default_audio_sample_t));
    }

    for (auto* speaker: speakers->array) {
        // calculate the delay coefficients for every source-to-speaker-to-listener
        // combination
        for (int j = 0; j < twonderConf->noSources; ++j) {
            DelayCoeff c =
                (*sources)[j]->source->getDelayCoeff(*speaker);
            DelayCoeff c2 =
                (*sources)[j]->source->getTargetDelayCoeff(*speaker, nframes);

            // mute a source if it is not active
            if (!(*sources)[j]->active) {
                c.setFactor(0.0);
                c2.setFactor(0.0);
            }

            // interpolate the delayline with Doppler effect or use a fade-jump
            if ((*sources)[j]->source->hasDopplerEffect()) {
                (*sources)[j]->inputline->getInterp(c, c2, speaker->outPtr, nframes);
            } else {
                (*sources)[j]->inputline->getFadej(c, c2, speaker->outPtr, nframes,
                                                   DelayLine::dB3);
            }
        }
    }

    // interpolate all source positions
    for (int i = 0; i < twonderConf->noSources; i++) {
        (*sources)[i]->source->doInterpolationStep(nframes);
    }

    return EXIT_SUCCESS;
}

//---------------------------end of audio processing-------------------------//

//-----------------------------JACK initialization---------------------------//

bool initialize_jack() {
    bool running          = false;  // Initialization status

    const char* client_name = twonderConf->jackName;  // Get name from twonderConfig
    const char* server_name = nullptr;  // Without a JACK server name the default server
                                        // from the .jackdrc file will be started.
    jack_options_t options = JackNullOption;  // No options needed, since we use the
                                              // options from the .jackdrc file.
    jack_status_t status;  // Holds the JACK status after the connection attempt for
                           // further verifications.

    // 1. Try to start the JACK client and connect to the JACK server.
    jackClient = jack_client_open(client_name, options, &status, server_name);
    running    = true;

    if (jackClient == nullptr) {
        running = false;
        std::cerr
            << "[JACK][ERROR]: Unable to open the JACK client! Failed with status: 0x"
            << std::hex << status << std::dec << '\n';

        if (status & JackServerFailed) {
            std::cerr << "[JACK][ERROR]: Unable to connect to the JACK server!\n";
        }
    }

    // 2. Verify the status of the JACK client.
    if (running) {
        // Print the name of the JACK server if it was not the default one from the
        // .jackdrc file.
        if (status & JackServerStarted) {
            std::cout << "\n[JACK]: Connection to the default JACK server established!"
                      << std::endl;
        }

        // Print the new JACK client name if it was not unique and has been been replaced
        // by the JACK server.
        if (status & JackNameNotUnique) {
            client_name = jack_get_client_name(jackClient);

            if (client_name == nullptr) {
                std::cout << "[JACK][WARNING]: JACK client name was not unique and has "
                             "been changed!"
                          << client_name << std::endl;
            } else {
                std::cout << "[JACK][WARNING]: JACK client name was not unique! New JACK "
                             "client name is: "
                          << client_name << std::endl;
            }
        }
    }

    // 3. Set the callback and shutdown functions to be called by the JACK server.
    if (running) {
        jack_set_process_callback(jackClient, process, nullptr);
        jack_on_shutdown(jackClient, jack_shutdown, nullptr);
    }

    // 4. Preallocate memory for JACK ports of Sources (inputs) and Speakers (outputs).
    if (running) {
        jackInputs = static_cast<jack_port_t**>(
            std::calloc(twonderConf->noSources, sizeof(jack_port_t*)));
        jackOutputs = static_cast<jack_port_t**>(
            std::calloc(speakers->array.size(), sizeof(jack_port_t*)));
    }

    // 5. Name the input ports of the JACK client (sources).
    if (running) {
        std::string port_name;

        for (int i = 0; i < twonderConf->noSources; ++i) {
            port_name = "input";
            port_name.append(std::to_string(i + 1));
            jackInputs[i] =
                jack_port_register(jackClient, port_name.data(), JACK_DEFAULT_AUDIO_TYPE,
                                   JackPortIsInput, 0);

            if (jackInputs[i] == nullptr) {
                running = false;
                std::cerr << "[JACK][ERROR]: Unable to register the input ports "
                             "(sources) of the JACK client!\n";
            }
        }
    }

    // 6. Name the output ports of the JACK client (speakers).
    if (running) {
        std::string port_name;

        for (int i = 0; i < static_cast<int>(speakers->array.size()); ++i) {
            port_name = "speaker";
            port_name.append(std::to_string(i + 1));
            jackOutputs[i] =
                jack_port_register(jackClient, port_name.data(), JACK_DEFAULT_AUDIO_TYPE,
                                   JackPortIsOutput, 0);

            if (jackOutputs[i] == nullptr) {
                running = false;
                std::cerr << "[JACK][ERROR]: Unable to register the output ports "
                             "(speakers) of the JACK client!\n";
            }
        }
    }

    // 7. Save the JACK client samplerate to twonderConfig and initialize TimeStamp and
    // DelayLine fade factors.
    twonderConf->sampleRate = jack_get_sample_rate(jackClient);
    TimeStamp::initSampleRate(twonderConf->sampleRate);
    DelayLine::initFadeBuffers(jack_get_buffer_size(jackClient));

    // 8. Activate the JACK client - this starts the real-time callback by the JACK server
    // and enables the interconnection of the ports of the JACK client.
    if (running) {
        if (jack_activate(jackClient)) {
            running = false;
            std::cerr << "[JACK][ERROR]: Unable to activate and start the JACK client!\n";
        }
    }

    // 9. and 10. removed. They connected hardware inputs and outputs.

    // 11. Everything is set up correctly, now print a final message with the parameters
    // of the JACK client.
    if (running) {
        std::cout << "\n" << std::endl;

        std::chrono::duration<float, std::milli> delay_msec(
            1.0 / jack_get_sample_rate(jackClient) * jack_get_buffer_size(jackClient)
            * 1000.0 * 2.0);

        std::cout << "\n\n"
                  << "[JACK]: JACK sample rate: " << jack_get_sample_rate(jackClient)
                  << " Hz\n"
                  << "[JACK]: JACK buffer size: " << jack_get_buffer_size(jackClient)
                  << " samples\n"
                  << "[JACK]: Round-Trip delay: " << delay_msec.count()
                  << " milliseconds\n"
                  << std::endl;
    }

    return running;
}

//--------------------------end of JACK initialization-----------------------//

//--------------------------------MoveCommand---------------------------------//

class MoveCommand : public Command
{
  public:
    MoveCommand(Vector3D& v, int id = 0, TimeStamp timestamp = (float)0,
                int durationInSamples = 10240)
        : Command(timestamp) {
        destination = v;
        sourceId    = id;
        duration    = durationInSamples;
    }

    void execute();

  private:
    Vector3D destination;
    unsigned int sourceId;
    unsigned int duration;  // in samples
};

void MoveCommand::execute() {
    if (sourceId < sources->size()) {
        PositionSource* positionSource =
            dynamic_cast<PositionSource*>(sources->at(sourceId)->source);

        if (positionSource) {
            if (positionSource->getType() == 0) {  // is a plane wave
                positionSource->position.setCurrentValue(destination);
            } else {  // is a point source
                positionSource->position.setTargetValue(destination, duration);
            }
        }
    }
}

//----------------------------end of MoveCommand------------------------------//

//--------------------------------AngleCommand--------------------------------//

class AngleCommand : public Command
{
  public:
    AngleCommand(float af, int id = 0, TimeStamp timestamp = (float)0,
                 int durationInSamples = 10240)
        : Command(timestamp) {
        angle    = af;
        sourceId = id;
        duration = durationInSamples;
    }

    void execute();

  private:
    float angle;
    unsigned int sourceId;
    unsigned int duration;  // in samples
};

void AngleCommand::execute() {
    if (sourceId < sources->size()) {
        PlaneWave* planeWave = dynamic_cast<PlaneWave*>(sources->at(sourceId)->source);

        if (planeWave) {
            planeWave->angle.setTargetValue(angle * M_PI * 2 / 360.0, duration);
        }
    }
}

//----------------------------end of AngleCommand-----------------------------//

//--------------------------------TypeChangeCommand---------------------------//

class TypeChangeCommand : public Command
{
  public:
    TypeChangeCommand(int id, int type, TimeStamp ts = (float)0);
    ~TypeChangeCommand();

    void execute();

  private:
    unsigned int sourceId;
    unsigned int type;
    Source* srcPtr;
};

TypeChangeCommand::TypeChangeCommand(int id, int t, TimeStamp timestamp)
    : Command(timestamp) {
    sourceId = id;
    type     = t;

    PositionSource* temp = dynamic_cast<PositionSource*>(sources->at(sourceId)->source);

    if (type == 1) {
        srcPtr = new PointSource(temp->position.getCurrentValue());
        srcPtr->setDopplerEffect(temp->hasDopplerEffect());
    } else if (type == 0) {
        srcPtr = new PlaneWave(temp->position.getCurrentValue(),
                               sources->at(sourceId)->angle * M_PI * 2 / 360.0);
        srcPtr->setDopplerEffect(temp->hasDopplerEffect());
    } else {
        srcPtr = nullptr;
    }
}

void TypeChangeCommand::execute() {
    if (sourceId < sources->size()) {
        Source* tmp = sources->at(sourceId)->source;

        sources->at(sourceId)->source = srcPtr;

        srcPtr = tmp;
    }
}

TypeChangeCommand::~TypeChangeCommand() {
    if (srcPtr) {
        delete srcPtr;
        srcPtr = nullptr;
    }
}

//----------------------------end of TypeChangeCommand------------------------//

//------------------------------DopplerChangeCommand--------------------------//

class DopplerChangeCommand : public Command
{
  public:
    DopplerChangeCommand(int id, bool dopplerOn, TimeStamp ts = (float)0);

    void execute();

  private:
    unsigned int sourceId;
    bool useDoppler;
};

DopplerChangeCommand::DopplerChangeCommand(int id, bool dopplerOn, TimeStamp timestamp)
    : Command(timestamp) {
    sourceId   = id;
    useDoppler = dopplerOn;
}

void DopplerChangeCommand::execute() {
    if (sourceId < sources->size()) {
        sources->at(sourceId)->source->setDopplerEffect(useDoppler);
    }
}

//----------------------------end of DopplerChangeCommand------------------------//

//----------------------------------OSC-handler-------------------------------//

// arguments of the handler functions
#define handlerArgs                                                               \
    const char *path, const char *types, lo_arg **argv, int argc, lo_message msg, \
        void *user_data

int oscSrcPositionHandler(handlerArgs) {
    if ((argv[0]->i >= twonderConf->noSources) || (argv[0]->i < 0)) { return -1; }

    if (!sources->at(argv[0]->i)->active) { return 0; }

    int sourceId   = argv[0]->i;
    float newX     = argv[1]->f;
    float newY     = argv[2]->f;
    float newZ     = 2.0;
    float time     = 0.0;
    float duration = 0.0;

    if (argc == 5) {
        duration = argv[3]->f;
        time     = argv[4]->f;
    } else if (argc == 4) {
        duration = argv[3]->f;
    }

    if (twonderConf->verbose) {
        std::cout << "osc-position: src=" << sourceId << " x=" << newX << " y=" << newY
                  << " ts=" << time << " dur=" << duration << std::endl;
    }

    Vector3D newPos(newX, newY, newZ);

    MoveCommand* moveCmd = new MoveCommand(
        newPos, sourceId,
        TimeStamp((wonder_frames_t)(time * (float)twonderConf->sampleRate)),
        (int)(duration * twonderConf->sampleRate));
    realtimeCommandEngine->put(moveCmd);

    return 0;
}

int oscSrcPositionHandler3D(handlerArgs) {
    if ((argv[0]->i >= twonderConf->noSources) || (argv[0]->i < 0)) { return -1; }

    if (!sources->at(argv[0]->i)->active) { return 0; }

    int sourceId   = argv[0]->i;
    float newX     = argv[1]->f;
    float newY     = argv[2]->f;
    float newZ     = argv[3]->f;
    float time     = 0.0;
    float duration = 0.0;

    if (argc == 6) {
        duration = argv[4]->f;
        time     = argv[5]->f;
    } else if (argc == 5) {
        duration = argv[4]->f;
    }

    if (twonderConf->verbose) {
        std::cout << "osc-position: src=" << sourceId << " x=" << newX << " y=" << newY
                  << " z=" << newZ << " ts=" << time << " dur=" << duration << std::endl;
    }

    Vector3D newPos(newX, newY, newZ);

    MoveCommand* moveCmd = new MoveCommand(
        newPos, sourceId,
        TimeStamp((wonder_frames_t)(time * (float)twonderConf->sampleRate)),
        (int)(duration * twonderConf->sampleRate));
    realtimeCommandEngine->put(moveCmd);

    return 0;
}

int oscSrcAngleHandler(handlerArgs) {
    if ((argv[0]->i >= twonderConf->noSources) || (argv[0]->i < 0)) { return -1; }

    if (!sources->at(argv[0]->i)->active) { return 0; }

    int sourceId   = argv[0]->i;
    float newAngle = argv[1]->f;
    float time     = 0.0;
    float duration = 0.0;

    if (argc == 4) {
        duration = argv[2]->f;
        time     = argv[3]->f;
    } else if (argc == 3) {
        duration = argv[2]->f;
    }

    if (twonderConf->verbose) {
        std::cout << "osc-angle: src=" << sourceId << " a=" << newAngle << " ts=" << time
                  << " dur=" << duration << std::endl;
    }

    AngleCommand* angleCmd = new AngleCommand(newAngle, sourceId, TimeStamp(time),
                                              (int)(duration * twonderConf->sampleRate));
    realtimeCommandEngine->put(angleCmd);

    // preserve angle information for type switching of sources if source is of type
    // pointsource
    if (sources->at(sourceId)->source->getType() == 0) {
        sources->at(sourceId)->angle = newAngle;
    }

    return 0;
}

int oscSrcTypeHandler(handlerArgs) {
    if ((argv[0]->i >= twonderConf->noSources) || (argv[0]->i < 0) || (argv[1]->i < 0)
        || (argv[1]->i > 1)) {
        return -1;
    }

    if (!sources->at(argv[0]->i)->active) { return 0; }

    int sourceId = argv[0]->i;
    int newType  = argv[1]->i;
    float time   = 0.0;

    // if targeted source is already of the requested type, then nothing has to be done
    if (argv[1]->i == sources->at(sourceId)->source->getType()) { return 0; }

    if (argc == 3) { time = argv[2]->f; }

    if (twonderConf->verbose) {
        std::cout << "osc-type-change: src=" << sourceId << " type=" << newType
                  << " ts=" << time << std::endl;
    }

    TypeChangeCommand* typeCmd = new TypeChangeCommand(sourceId, newType, time);
    realtimeCommandEngine->put(typeCmd);

    return 0;
}

int oscSrcDopplerHandler(handlerArgs) {
    if ((argv[0]->i >= twonderConf->noSources) || (argv[0]->i < 0)) { return -1; }

    if (!sources->at(argv[0]->i)->active) { return 0; }

    int sourceId    = argv[0]->i;
    bool useDoppler = (bool)argv[1]->i;
    float time      = 0.0;

    if (argc == 3) { time = argv[2]->f; }

    if (twonderConf->verbose) {
        std::cout << "osc-doppler-change: src=" << sourceId << " doppler=" << useDoppler
                  << " ts=" << time << std::endl;
    }

    DopplerChangeCommand* dopplerCmd =
        new DopplerChangeCommand(sourceId, useDoppler, time);
    realtimeCommandEngine->put(dopplerCmd);

    return 0;
}

int oscReplyHandler(handlerArgs) {
    if (twonderConf->verbose) {
        std::cout << "[V-OSCServer] reply to: " << &argv[0]->s << " state=" << argv[1]->i
                  << " msg=" << &argv[2]->s << std::endl;
    }

    return 0;
}

int oscSrcActivateHandler(handlerArgs) {
    if ((argv[0]->i >= twonderConf->noSources) || (argv[0]->i < 0)) { return 0; }

    if (twonderConf->verbose) { std::cout << "osc-activate " << argv[0]->i << std::endl; }

    sources->at(argv[0]->i)->active = true;

    return 0;
}

int oscSrcDeactivateHandler(handlerArgs) {
    if ((argv[0]->i >= twonderConf->noSources) || (argv[0]->i < 0)) { return 0; }

    if (twonderConf->verbose) {
        std::cout << "osc-deactivate " << argv[0]->i << std::endl;
    }

    sources->at(argv[0]->i)->active = false;
    sources->at(argv[0]->i)->reset();

    return 0;
}

int oscPingHandler(handlerArgs) {
    lo_send(twonderConf->cwonderAddr, "/WONDER/stream/render/pong", "i", argv[0]->i);

    return 0;
}

int oscNoSourcesHandler(handlerArgs) {
    // this handler is only allowed to be called once and only with a valid value at
    // startup, so ignore further messages
    static bool noSourcesIsSet = false;

    if (!noSourcesIsSet) {
        if (argv[0]->i > 0) {
            twonderConf->noSources = argv[0]->i;
            noSourcesIsSet         = true;
            sources = new SourceArray(twonderConf->noSources, twonderConf->negDelayInit);
        }
    }

    return 0;
}

int oscRenderPolygonHandler(handlerArgs) {
    // parse incoming points
    // argv[0] is the roomname, drop it, we don't need that
    // get number of points
    int noPoints = argv[1]->i;

    // iterate over all points and store them
    for (int i = 1; i <= noPoints; ++i) {
        twonderConf->renderPolygon.push_back(
            Vector3D(argv[(i * 3) - 1]->f, argv[i * 3]->f, argv[(i * 3) + 1]->f));
    }

    // TODO: calculate elevation of speakers
    // slope = (elevationZ2 - elevationZ1) / (elevationY2 - elevationY1);

    return 0;
}

int oscGenericHandler(handlerArgs) {
    if (twonderConf->verbose) {
        std::cout << "\n[twonder]: received unknown osc message: " << path << std::endl;
    }

    return 0;
}

//------------------------------end of OSC-Handler---------------------------//

void signalHandler(int signal) {
    std::cout << "[SIGNAL-HANDLER]: Received interrupt signal number " << signal
              << " - exiting now!" << std::endl;
    exitCleanupFunction();
    std::exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
    // read the twonder configuration file
    twonderConf = new TwonderConfig(argc, argv);
    int result  = twonderConf->readConfig();

    if (result != 0) {
        std::cerr << "Error parsing twonder configfile !!!" << std::endl;

        switch (result) {
        case 1:
            std::cerr << "configfile " << twonderConf->twonderConfigfile
                      << " does not exist. " << std::endl;
            break;

        case 2:
            std::cerr << "dtd file"
                      << " could not be found. " << std::endl;
            break;

        case 3:
            std::cerr << "configfile " << twonderConf->twonderConfigfile
                      << " is not well formed. " << std::endl;
            break;

        case 4:
            std::cerr << "libxml caused an exception while parsing "
                      << twonderConf->twonderConfigfile << std::endl;
            break;

        default:
            std::cerr << " an unkown error occurred, sorry. " << std::endl;
            break;
        }

        std::exit(EXIT_FAILURE);
    }

    // add signal handlers
#ifdef WIN32
    std::signal(SIGINT, signalHandler);
    std::signal(SIGABRT, signalHandler);
    std::signal(SIGTERM, signalHandler);
#else
    std::signal(SIGQUIT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    std::signal(SIGHUP, signalHandler);
    std::signal(SIGINT, signalHandler);
#endif

    // read the speaker configuration file
    try {
        if (twonderConf->verbose) {
            std::cout << "using speakers configfile: " << twonderConf->speakersConfigfile
                      << std::endl;
        }

        speakers = new SpkArray(twonderConf->speakersConfigfile);
    }
    catch (std::exception& e) {
        std::cerr << "could not read speaker configuration file "
                  << twonderConf->speakersConfigfile << ", exiting now... " << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // start the command engine
    realtimeCommandEngine = new RTCommandEngine();

    // start OSCServer and register messagehandler
    try {
        oscServer = new OSCServer(twonderConf->listeningPort);
    }
    catch (OSCServer::EServ) {
        std::cerr << "[twonder] Could not create server, maybe the server( using the "
                     "same port ) is already running?"
                  << std::endl;
        std::exit(EXIT_FAILURE);
    }

    // multiple entries for one message (e.g. type) are just for convenience and will use
    // a value of 0 for timestamp and/or duration
    oscServer->addMethod("/WONDER/reply", "sis", oscReplyHandler);
    oscServer->addMethod("/WONDER/source/activate", "i", oscSrcActivateHandler);
    oscServer->addMethod("/WONDER/source/deactivate", "i", oscSrcDeactivateHandler);
    oscServer->addMethod("/WONDER/source/position", "iff", oscSrcPositionHandler);
    oscServer->addMethod("/WONDER/source/position", "ifff", oscSrcPositionHandler);
    oscServer->addMethod("/WONDER/source/position", "ifff", oscSrcPositionHandler);
    oscServer->addMethod("/WONDER/source/position3D", "ifff", oscSrcPositionHandler3D);
    oscServer->addMethod("/WONDER/source/position3D", "iffff", oscSrcPositionHandler3D);
    oscServer->addMethod("/WONDER/source/position3D", "ifffff", oscSrcPositionHandler3D);
    oscServer->addMethod("/WONDER/source/type", "ii", oscSrcTypeHandler);
    oscServer->addMethod("/WONDER/source/type", "iif", oscSrcTypeHandler);
    oscServer->addMethod("/WONDER/source/angle", "if", oscSrcAngleHandler);
    oscServer->addMethod("/WONDER/source/angle", "iff", oscSrcAngleHandler);
    oscServer->addMethod("/WONDER/source/angle", "ifff", oscSrcAngleHandler);
    oscServer->addMethod("/WONDER/source/dopplerEffect", "ii", oscSrcDopplerHandler);
    oscServer->addMethod("/WONDER/source/dopplerEffect", "iif", oscSrcDopplerHandler);
    oscServer->addMethod("/WONDER/global/maxNoSources", "i", oscNoSourcesHandler);
    oscServer->addMethod("/WONDER/global/renderpolygon", nullptr,
                         oscRenderPolygonHandler);
    oscServer->addMethod("/WONDER/stream/render/ping", "i", oscPingHandler);
    oscServer->addMethod(nullptr, nullptr, oscGenericHandler);
    oscServer->start();

    // connect to cwonder
    lo_send(twonderConf->cwonderAddr, "/WONDER/stream/render/connect", "s",
            twonderConf->name.c_str());

    // wait for cwonder to send setup data
    // reconnect if it doesn't work
    while (twonderConf->noSources == 0) {
        static int timeoutCounter = 0;
        std::this_thread::sleep_for(std::chrono::seconds(1));

        if (timeoutCounter % 15 == 15) {
            std::cerr << "Cannot connect to cwonder...\n";
            lo_send(twonderConf->cwonderAddr, "/WONDER/stream/render/connect", "s",
            twonderConf->name.c_str());
        }
        timeoutCounter++;
    }

    std::cout << "Connection to cwonder established.";

    // Initialize JACK
    if (!initialize_jack()) {
        exitCleanupFunction();
        std::exit(EXIT_FAILURE);
    }

    // event loop
    while (true) { std::this_thread::sleep_for(std::chrono::seconds(10)); }

    // cleanup before exiting
    exitCleanupFunction();

    return 0;
}
