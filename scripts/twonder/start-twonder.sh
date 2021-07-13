#!/bin/bash

# Read config
source /etc/wonder/cluster.conf
source /etc/wonder/twonder.conf

i=1;

for NUM_CH in $CHANNELS;
do
    twonder ${TWONDER_ARGS} -c /etc/wonder/twonder_config.xml -s /etc/wonder/speakers/${NODE}/twonder_speakerarray$i.xml -i $CWONDER_IP -j twonder$i -o $((58200 + $i)) --negdelay $NEG_DELAY &
    i=$(($i+1));
    sleep 1;
done
