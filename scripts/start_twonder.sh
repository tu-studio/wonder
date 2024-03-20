#!/bin/bash

# This is the script with which we start twonder instances, it is executed by the systemd service. Its variables are set in the systemd environment file.

i=1;

for file in /usr/local/etc/wonder/twonder/speaker-positions/twonder_*;
do
    # for debugging the --verbose flag can be added to the command line
    twonder -c /usr/local/etc/wonder/twonder/twonder_config.xml -s $file -i $CWONDER_IP -j twonder$i -o $((58200 + $i)) --negdelay $NEG_DELAY &
    i=$(($i+1));
done
