#!/bin/bash

# Read config
SCRIPTDIR=`dirname $0`
source ${SCRIPTDIR}/configs/cluster.conf

echo "Stopping twonder on all nodes..."
${SCRIPTDIR}/twonder/stop-all-twonder.sh

sleep 1;

echo "Stopping JACK-Server on all nodes..."
${SCRIPTDIR}/jack/stop-all-jackd.sh

sleep 1;

echo "Stopping cwonder on local machine..."
${SCRIPTDIR}/cwonder/stop-cwonder.sh
