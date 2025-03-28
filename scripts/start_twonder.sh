#!/bin/bash

# This is the script with which we start twonder instances, it is executed by the systemd service. Its variables are set in the systemd environment file.

i=1;


for file in /home/leto/data/projects/arbeit/akt/seamless-configs/EN325/wonder/twonder/wintermute/speaker-positions/twonder_*;
# for file in /usr/local/etc/wonder/twonder/speaker-positions/twonder_*;
do
    # setup args
    args=(
        -c /usr/local/etc/wonder/twonder/twonder_config.xml
        -s $file
        -i "$CWONDER_IP"
        -j "twonder$i"
        -o "$((58200 + $i))"
        --negdelay "$NEG_DELAY"
    )

    # optional args based on set environment variables
    if [[ -v MULTICAST_GROUP ]]; then
        args+=(-g "$MULTICAST_GROUP")
    fi

    if [[ -v MULTICAST_PORT ]]; then
        args+=(-t "$MULTICAST_PORT")
    fi

    if [[ -v CWONDER_PORT ]]; then
        args+=(-p "$CWONDER_PORT")
    fi
    echo "${args[@]}"
    # for debugging the --verbose flag can be added to the command line
    twonder  "${args[@]}" &
    i=$(($i+1));
done
