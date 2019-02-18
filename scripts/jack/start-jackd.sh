#!/bin/bash

# Read config
SCRIPTDIR=`dirname $0`
source ${SCRIPTDIR}/../configs/cluster.conf

jackd -R -P 70 -p 4096 -d alsa -d ${JACK_DEVICE} -r ${JACK_SAMPLERATE} -p ${JACK_BLOCKSIZE} -i ${JACK_INPUTS} -o ${JACK_OUTPUTS} &
