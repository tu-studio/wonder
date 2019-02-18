#!/bin/bash

# Read config
SCRIPTDIR=`dirname $0`
source ${SCRIPTDIR}/../configs/cluster.conf

/opt/wonder/current/bin/cwonder -c /opt/wonder/current/configs/cwonder_config.xml -r $(($CWONDER_PING * $JACK_SAMPLERATE)) &
