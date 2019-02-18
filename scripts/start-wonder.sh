#!/bin/bash

# Read config
SCRIPTDIR=`dirname $0`
source ${SCRIPTDIR}/configs/cluster.conf

echo "Starting JACK-Server on all nodes..."
${SCRIPTDIR}/jack/start-all-jackd.sh

echo "Starting cwonder on local machine..."
${SCRIPTDIR}/cwonder/start-cwonder.sh > /dev/null

sleep 1

echo "Starting twonder on all nodes..."
${SCRIPTDIR}/twonder/start-all-twonder.sh

sleep 1

echo "Connecting twonder with JACK-Server on all nodes..."
${SCRIPTDIR}/connections/connect-all-twonder.sh
