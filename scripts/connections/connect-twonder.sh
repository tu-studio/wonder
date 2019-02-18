#!/bin/bash

NODE=$1

# Read config
SCRIPTDIR=`dirname $0`
source ${SCRIPTDIR}/../configs/cluster.conf
source ${SCRIPTDIR}/../configs/twonder_${NODE}.conf

i=1;
playback=1;

for NUM_CH in $CHANNELS;
do
    # Connect the inputs of the soundcard with the inputs (Source) of a twonder instance
    for j in $(seq 1 1 $NUM_SOURCES);
    do
        jack_connect system:capture_${j} twonder${i}:input$[${j}] &> /dev/null
    done

    # Connect the outputs (Speaker) of a twonder instance with the outputs of the soundcard
    for k in $(seq 1 1 $NUM_CH);
    do
        jack_connect twonder${i}:speaker$[${k}] system:playback_${playback} &> /dev/null
        playback=$(($playback+1));
    done

    i=$(($i+1));
done
