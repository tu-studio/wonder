#!/bin/bash

NODE=$1

# Read config
SCRIPTDIR=`dirname $0`
source ${SCRIPTDIR}/../configs/cluster.conf
source ${SCRIPTDIR}/../configs/twonder_${NODE}.conf

i=1;

for NUM_CH in $CHANNELS;
do
    /opt/wonder/current/bin/twonder -c /opt/wonder/current/configs/twonder_config.xml -s /opt/wonder/current/configs/speakers/${NODE}/twonder_speakerarray$i.xml -i $CWONDER_IP -j twonder$i -o $((58200 + $i)) --negdelay $NEG_DELAY &
    i=$(($i+1));
    sleep 1;
done
