## CONFIGURATION

|Files|Location|
|---    |---    |
|Starup Scripts |*/usr/share/wonder/scripts*|
|Configuration files|*/etc/wonder*|

### Application configuration files
*cwonder_config.xml:* Adjust projects path, number of sources and room geometry.

*twonder_config.xml:* Adjust speaker distance and focus radius.

### Speaker configuration files:
*twonder_speakerarray.xml*

or

*speakers/n101* and *speakers/n102*

### Node configuration files:
*cluster.conf:* Adjust username, IP address, JACK device, negative delay, etc.

*twonder_n101.conf:* Speaker array configuration of node 1 (n101).

*twonder_n102.conf:* Speaker array configuration of node 2 (n102).

### Important!!
Make shure to modify the configuration files (especially cluster.conf) properly! Otherwise the start-up scripts might fail!
