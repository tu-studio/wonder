#!/bin/bash

# read config
SCRIPTDIR=`dirname $0`
source ${SCRIPTDIR}/../configs/cluster.conf

# For every node and every twonder instance running on it:
# - connect the inputs of the soundcard with the inputs (Source) of the twonder instance
# - connect the outputs (Speaker) of the twonder instance with the outputs of the soundcard
for node in $CLUSTER_NODES;
do
    rsh $USER_NAME@$node /opt/wonder/current/scripts/connections/connect-twonder.sh $node > /dev/null &
done
