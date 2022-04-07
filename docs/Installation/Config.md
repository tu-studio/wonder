# Wonder Configuration

Wonder's configuration files must be put in `/etc/wonder`.

## Cluster configuration

`/etc/wonder/cluster.conf` sets environment variables for wonder. You should
set the IP address of your CWonder machine, the JACK setup information
and the negative delay length in meters for focused sources.

```bash
CWONDER_IP='8.8.8.8';

# Ping rate of cwonder in seconds (uses JACK_SAMPLERATE to calculate seconds)
CWONDER_PING=2;

NUM_SOURCES=64;

# Negative delay for focused sources (in meters)
NEG_DELAY=7.585;

# Execute this command to list all devices and insert the name in the square brackets behind the "hw:" part, e.g. 'hw:Intel': cat /proc/asound/cards
JACK_DEVICE='hw:DantePCIe';

JACK_BLOCKSIZE=512;
JACK_SAMPLERATE=48000;

JACK_INPUTS=128;
JACK_OUTPUTS=128;
```

## CWonder 

`/etc/wonder/cwonder_config.xml` sets the project path, number of sources and room geometry.

## TWonder

`twonder_config.xml` configures location specific rendering variables.
There are two variables for the rendering of focused sources.
Focus limit sets the maximum distance between virtual source and speaker.
Focus margin sets the area in which the focused source fades out.
Speaker distance sets the distance between two speakers in one speaker array.

### Node configuration

Every node (machine with twonder instance) needs a separate configuration.
`twonder.conf` sets the node name and a bash array that describes the speaker
setup. Following configures a node named riviera and a speaker setup with four
speaker arrays each having 16 channels.

```bash
NODE=riviera;
CHANNELS='16 16 16 16';
```

`twonder_speakerarray.xml` for one speaker array or numbered
`twonder_speakerarray1.xml`, `twonder_speakerarray2.xml`, etc. in `speakers/nodename/`
are describing the position and direction of speaker arrays.

normalx, normaly, normalz set the normal unit vector in the direction of the speakers.

```xml
<?xml version="1.0"?>

<!DOCTYPE speakerarray SYSTEM "twonder_speakerarray.dtd">

<speakerarray>
	<segment id="1" numspeak="8" winwidth="0" startx="0.055" starty="3.023" startz="1.400" endx="0.755" endy="3.023" endz="1.400" normalx="0.000" normaly="-1.000" normalz="0" />
	<segment id="2" numspeak="8" winwidth="0" startx="0.865" starty="3.023" startz="1.400" endx="1.565" endy="3.023" endz="1.400" normalx="0.000" normaly="-1.000" normalz="0" />
</speakerarray>
```

