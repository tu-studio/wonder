# CWONDER

Wonder is a "distributed application" that communicates over OSC. cwonder is the
glue part that each of the other parts communicates with. The only thing the
applications need to know is the address of cwonder which can be retrieved from
the config file. The address can either be a dotted-decimal IP address or a
hostname which than is converted to an IP address by the underlying osc library
liblo. The project path is the standard path cwonder will store its projects.
When you use the "/WONDER/project/*" commands to handle your project work
cwonder will use this path to store and read the projects when no absolute path
is specified. The maximum number of sources is an integer value which determines
the maximum number of sources available in a project. Here is an example how a
wonder config file looks like.

```xml
<?xml version="1.0"?>
<!DOCTYPE wonderconfig PUBLIC "" "wonderconfig.dtd">

<wonderconfig>
	<cwaddr host="127.0.0.1" port="58100"/> 
    	<defaults projectpath="/home/wfsuser/wonder/projects" maxsources="64" />
</wonderconfig>
```
	
To read in the wonder config file from within your aplication you can use the
SwonderConfig class which use is described in wonder_config.h. Or have a look at
a usecase in gwonder or cwonder.

## control cwonder 

These are commands for debugging use. A source may change its position quite
often that is why these events are not logged by cwonder by default. In some
cases you may want to check if cwonder is receiving messages already. You can
toggle logging of these messages (inclede are type and angle as well) on or off
with the following command "/CWONDER/enable/posout". To debug the timing
information you receive by jfwonder you can toggle the logging of this
informmation by the following command "/CWONDER/enable/timerout". Do not forget
to turn these commmands off again when not in use anymore since they may result
in bad performance.

## start cwonder from the command line

Usually cwonder is started from a systemd service. To see the command line
options for cwonder start cwonder with the -h option.

```bash
cwonder -h
``` 

If you start cwonder without any options the default settings are used. They are
set in cwonder_globalconfig.cpp.

