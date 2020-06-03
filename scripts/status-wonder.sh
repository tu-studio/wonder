#!/bin/bash

# Read config
SCRIPTDIR=`dirname $0`
source ${SCRIPTDIR}/configs/cluster.conf

# Print JACK settings used by all nodes
echo ""
echo ".::| NODE SETTGINGS |::."
echo "JACK Device    :" $JACK_DEVICE
echo "JACK Samplerate: $JACK_SAMPLERATE Hz"
echo "JACK Blocksize : $JACK_BLOCKSIZE Samples"
echo "JACK Inputs    :" $JACK_INPUTS
echo "JACK Outputs   :" $JACK_OUTPUTS
echo "Pre-Delay      : $NEG_DELAY meters"

## Scan control computer
echo ""
echo ".::| WFS CONTROL |::."
ssh $CWONDER_IP pstree | egrep "(jackd|cwonder|twonder|xwonder)"
echo ""

# Scan nodes
for node in $CLUSTER_NODES;
do
    echo $node;
    ssh $USER_NAME@$node pstree | egrep "(jackd|cwonder|twonder|xwonder)"
    echo ""
done
echo ""
