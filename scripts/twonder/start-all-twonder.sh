#!/bin/bash

# Read config
SCRIPTDIR=`dirname $0`
source ${SCRIPTDIR}/../configs/cluster.conf

for node in $CLUSTER_NODES;
do
    rsh $USER_NAME@$node /opt/wonder/current/scripts/twonder/start-twonder.sh $node
done
