#!/bin/bash

# This script is outdated and is only necessary if we are not using systemd to manage the twonder instances.

# Read config
SCRIPTDIR=`dirname $0`
source ${SCRIPTDIR}/../configs/cluster.conf

for node in $CLUSTER_NODES;
do
    ssh $USER_NAME@$node "killall twonder"
done
