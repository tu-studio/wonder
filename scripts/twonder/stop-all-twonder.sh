#!/bin/bash

# Read config
SCRIPTDIR=`dirname $0`
source ${SCRIPTDIR}/../configs/cluster.conf

for node in $CLUSTER_NODES;
do
    ssh $USER_NAME@$node "killall twonder"
done
