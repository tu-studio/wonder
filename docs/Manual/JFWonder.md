===== JFWONDER =====
Executable file: jfwonder3

jfwonder is a helper application for cwonder.
Its purpose is to provide the current timing information of the render engines which is used to synchronize the control commands and it's chain of execution.
jfwonder is a jack client that sends the actual frame rate to cwonder every 1024 samples by default.
This event makes cwonder 'tick'.
Commands gets processed each 1024 samples - the control rate is 1024 samples.
This is true for commands with a timestamp > 0.
The realtime commands (timestamp 0) gets processed immediatly (they are not put on the event stack) and distributed to the render engines.
So even when running the jack server at a sampling rate of for example 128 the control rate will be 1024 by default when not sending 'realtime commands'.
The 'tick' is also used to provide the timing basis for sending the ping commands. If jfwonder is not connected to cwonder the ping commands will not work because of the lack of a time basis.

