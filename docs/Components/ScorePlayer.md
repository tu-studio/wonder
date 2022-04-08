# SCOREPLAYER
Executable    file: scoreplayer3
Configuration file: scoreplayer_config.xml

REQUIREMENTS

- MIDI:
- scoreplayer is dependent from a MTC (MIDI Time Code) master like ardour and the MTC master should talk MMC (MIDI Machine Control), too.   
    - Both hast to enabled inside the chosen sequencer (maybe ardour).  
    - the MTC Master has to talk with scoreplayer's virtual MIDI ports (scoreplayer_in, scoreplayer_out). 
    - hint: these connections can be automised with the help of jack's patchbay functionality.

    - at startup scoreplayer builds a virtual midi input and output port. these ports are used for the MTC and MMC communication. 
      the MMC and MTC client has to connect to these ports
    - scoreplayer lstens to every incoming MIDI traffic on the scoreplayer_in port, the usage of more than one MTC/MMC client is not 
      tested so take care if you connect other sequencers.
    - MTC:	
        - choose as MTC framerate 25 frames per second, because there is currently a synch problem especially with ardour by using different framerates.
    - MMC:
        - MMC is used for the transports communication between scoreplayer and the chosen sequencer. scoreplayer is able to care for the transports of 
        connected MMC machines and/or to the "OSC visual-stream" connected clients (like xwonder)

- OSC: 
    - scoreplayer is connected to the swonder modules (cwonder, xwonder, etc.) per OSC (open sound control). 
     at startup time scoreplayer tries to connect to cwonder's visual stream.
    - scoreplayer reads cwonder's address from swonder's defaults.xml file but 
    - it is possible to set cwonder's address per command line (see scoreplayer -h)

DOCUMENTATION

scoreplayer's basic functionality is to record and play back incoming WFS control datas. these datas can be saved in a XML file which contains a complete description of a time changing 
scenario like a musical piece for instance.
this description, let's say the scorefile format, is devided into two parts: 
	1. the orchestra section 
	2. and a time dependent score section

this kind of representation is devided from XML3DAUDIO and csound.

orchestra: 
	the orchestra describes the start situation at time 0.0 sec and contains a complete description of every possible source.
score:
	the score contains the time based control datas. theses datas are not complete source descriptions they contain
        just the changing information of a source. 


at startuptime 
- scoreplayer receives swonders system state and uses it as startscene (the orchestra). after the system check scoreplayer receives the name of the 
currently used project file and tries to read it from the actual project path. inside the project file is saved the name of the project's scorefile. 
if scoreplayer is able to read this scorefile, it will load the scorefile. so the startscene is set with the orchestra contents and the score is set by the score contents of the file.
- then swonder, xwonder and every possible MMC and OSC Client are set to time 0.0sec and the transports stati are disabled (global rec off, source rec off, stop)
- now swonder is ready to start the play back of the "piece".

playback
- press play in xwonder, the MMC client (ardour) or send /WONDER/score/play (OSC)

recording
- enable record mode in xwonder, ardour or with "/WONDER/score/record 1"
- press play in xwonder, the MMC client (ardour) or send /WONDER/score/play (OSC)

save scorefile
- use xwonder, or send "/WONDER/score/save path"

read scorefile
- use xwonder, or send "/WONDER/score/load path"

ERRORS:
ERROR 1: a possible error is that the MTC Master is not connected to scoreplayer's MIDI port. 
