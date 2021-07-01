# XWonder

| Command|Parameters |content         >|...|...|...|...|...|
| -- | --  | -- | -- | -- | -- | --  | -- |
|/source/activate|i|id| | | | | |
|/source/deactivate|i|id| | | | | |
|/source/position|i f f|id|x coordinate [ meters ]|y coordinate [ meters ]| | | |
|/source/angle|i f|id|angle [ degrees ]| | | | |
|/source/type|i i|id|type [ 0=plane, 1=point ]| | | | |
|/source/name|i s|id|name| | | | |
|/source/color|i i i i|id|red [ 0;255 ]|green [ 0;255 ]|blue [ 0; 255 ]| | |
|/source/groupId|i i|id|groupId| | | | |
|/source/rotationDirection|i i|id|inverted [ 0=false, 1=true]| | | | |
|/source/scalingDirection|i i|id|inverted [ 0=false, 1=true]| | | | |
|/score/source/enableRecord|i i|id|0 = off, 1 = on| | | | |
|/score/source/enableRead|i i|id|0 = off, 1 = on| | | | |
|/source/dopplerEffect|i i|id|on [ 0 = false, 1 = true ]| | | | |
| | | | | | | | |
|/group/activate|i|groupId| | | | | |
|/group/deactivate|i|groupId| | | | | |
|/group/position|i f f |groupId|x coordinate [ meters ]|y coordinate [ meters ]| | | |
|/group/color|i i i i|groupId|red [ 0;255 ]|green [ 0;255 ]|blue [ 0; 255 ]| | |
| | | | | | | | |
|/mtctime|i i i i|hour|minute|second|millisecond| | |
|/score/stop|none| | | | | | |
|/score/play|none| | | | | | |
|/score/enableRecord|i|0 = off, 1 = on| | | | | |
|/score/enableRead|i|0 = off, 1 = on| | | | | |
|/score/enableMMC|i|0 = off, 1 = on| | | | | |
|/score/status| NULL ( i )|scoreplayerPlayMode|scoreplayerRecordMode|scoreplayerReadMode|sourceRecordMode|sourceReadMode| |
| | | | | | | | |
|/global/maxNoSources|i|number of sources| | | | | |
|/global/renderpolygon|s i (f f f)|roomname|number of vertices|V.1 x coord. [ meters ]|V.1 y coord. [ meters ]|V.1 z coord. [ meters ]| |
|/project/xmlDump|i s|errorflag ( 1 = error )|project in xml  (see dtd)| | | | |
| | | | | | | | |
|/stream/visual/ping|i|pingcount| | | | | |
|/reply|s i s|reply to message|state|message| | | |
