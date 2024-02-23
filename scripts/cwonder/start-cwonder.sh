#!/bin/bash

# This script is outdated and is only necessary if we are not using systemd to manage cwonder.

# Read config
SCRIPTDIR=`dirname $0`
source ${SCRIPTDIR}/../configs/cluster.conf

/opt/wonder/current/bin/cwonder -c /opt/wonder/current/configs/cwonder_config.xml -r $(($CWONDER_PING * $JACK_SAMPLERATE)) &
