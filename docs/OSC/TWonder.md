# TWonder
 
| Command|Parameters |content         >|...|...|...|...|...|
| -- | --  | -- | -- | -- | -- | --  | -- |
|/source/activate|i |id| | | | | |
|/source/deactivate|i |id| | | | | |
|/source/type|i i|id|type [ 0=plane, 1=point ]| | | | |
|/source/type|i i f|id|type [ 0=plane, 1=point ]|timestamp [ seconds ]| | | |
|/source/angle|i f|id|angle [ degrees ]| | | | |
|/source/angle|i f f |id|angle [ degrees ]|duration [ seconds ]| | | |
|/source/angle|i f f f|id|angle [ degrees ]|duration [ seconds ]|timestamp [ seconds ]| | |
|/source/position|i f f |id|x coordinate [ meters ]|y coordinate [ meters ]| | | |
|/source/position|i f f f |id|x coordinate [ meters ]|y coordinate [ meters ]|duration [ seconds ]| | |
|/source/position|i f f f f|id|x coordinate [ meters ]|y coordinate [ meters ]|duration [ seconds ]|timestamp [ seconds ]| |
|/source/position|i f f f f f|id|x coordinate [ meters ]|y coordinate [ meters ]|not used|timestamp [ seconds ]|duration [ seconds ]|
| | | | | | | | |
|/source/dopplerEffect|i i|id|on [ 0 = false, 1 = true ]| | | | |
|/source/dopplerEffect| i i f |id|on [ 0 = false, 1 = true ]|timestamp [ seconds ]| | | |
| | | | | | | | |
|/global/maxNoSources|i|number of sources| | | | | |
|/global/renderpolygon|s i (f f f)|roomname|number of vertices|V.1 x coord. [ meters ]|V.1 y coord. [ meters ]|V.1 z coord. [ meters ]| |
| | | | | | | | |
|/stream/render/ping|i|pingcount| | | | | |
|/reply|s i s|reply to message|state|message| | | |
