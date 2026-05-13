#!/bin/bash

# This is the script with which we start twonder instances, it is executed by the systemd service. Its variables are set in the systemd environment file.

if [ -z "$1" ]; then 
    echo "No argument provided, should be an index for this twonder"; 
    exit -1
fi

i=$1;

# get nth file from the array
filename_array=( '/usr/local/etc/wonder/twonder/speaker-positions/twonder_'* )
file=${filename_array[$i]}

if [[ -z "${file:-}" ]]; then
    echo "twonder config file with index $i does not exist";
    exit -1
fi

# setup args
args=(
    -c /usr/local/etc/wonder/twonder/twonder_config.xml
    -s $file
    -j "twonder$((1 + $i))"
    -o "$((58200 + $i))"
)

# optional args based on set environment variables
if [[ -v CWONDER_IP ]]; then
    args+=(-i "$CWONDER_IP")

fi

if [[ -v MULTICAST_GROUP ]]; then
    args+=(-g "$MULTICAST_GROUP")
fi

if [[ -v MULTICAST_PORT ]]; then
    args+=(-t "$MULTICAST_PORT")
fi

if [[ -v CWONDER_PORT ]]; then
    args+=(-p "$CWONDER_PORT")
fi

if [[ -v NEG_DELAY ]]; then
    args+=(--negdelay "$NEG_DELAY")
fi

if [[ -v VERBOSE ]]; then
    args+=(--verbose)
fi

echo "executing twonder with args ${args[@]}"
# exec so systemd notify properly works
exec twonder  "${args[@]}"
