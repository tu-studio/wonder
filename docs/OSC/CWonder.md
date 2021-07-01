# CWonder

| Command|Parameters |content         >|...|...|...|...|...|
| -- | --  | -- | -- | -- | -- | --  | -- |
|/source/activate|i|id| | | | | |
|/source/deactivate|i|id| | | | | |
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
|/source/color|i i i i|id|red [ 0;255 ]|green [ 0;255 ]|blue [ 0; 255 ]| | |
|/source/groupId|i i|id|groupId| | | | |
|/source/rotationDirection|i i|id|inverted [ 0=false, 1=true]| | | | |
|/source/scalingDirection|i i|id|inverted [ 0=false, 1=true]| | | | |
|/source/dopplerEffect|i i|id|on [ 0 = false, 1 = true ]| | | | |

### Groups

| Command|Parameters |content         >|...|...|...|
| -- | --  | -- | -- | -- | -- |
|/group/activate|i|groupId| | | |
|/group/deactivate|i|groupId| | | |
|/group/position|i f f |groupId|x coordinate [ meters ]|y coordinate [ meters ]| |
|/group/color|i i i i|groupId|red [ 0;255 ]|green [ 0;255 ]|blue [ 0; 255 ]|

### Project

| Command|Parameters |content         >|...|
| -- | --  | -- | -- |
|/project/createWithScore|s|projectname| |
|/project/createWithoutScore|s|projectname| |
|/project/load|s|projectname| |
|/project/save| | | |
|/project/save|s|projectname| |
| | | | |
|/project/snapshot/take|i|snapshotId| |
|/project/snapshot/take|i s|snapshotId|name|
|/project/snapshot/recall|i f|snapshotId|duration [ seconds ]|
|/project/snapshot/delete|i|snapshotId| |
|/project/snapshot/rename|i s|snapshotId|name|
|/project/snapshot/copy|i i|snapshotId (from)|snapshotId (to)|


### Score

| Command|Parameters |content         >|...|
| -- | --  | -- | -- |
|/score/play|none| | |
|/score/stop|none| | |
|/score/setStartScenario|none| | |
|/score/enableRecord|i|0 = off, 1 = on| |
|/score/enableRead|i|0 = off, 1 = on| |
|/score/reset| | | |
|/score/newtime|f|time [ seconds ]| |
|/score/enableMMC|i|0 = off, 1 = on| |
| | | | |
|/score/source/enableRecord|i i|sourceId|0 = off, 1 = on|
|/score/source/enableRead|i i|sourceId|0 = off, 1 = on|
| | | | |

### JFWonder

| Command|Parameters |content |
| -- | --  | -- |
|/jfwonder/frametime|i |currenttime [ jackframe ]|
|/jfwonder/connect|none| |
|/jfwonder/error|s|errormessage|


### Stream
| Command|Parameters |content         >|...|...|
| -- | --  | -- | -- | -- |
|/stream/render/connect|s|name| | |
|/stream/render/connect|s s|host|port| |
|/stream/render/connect|none| | | |
|/stream/render/disconnect|none| | | |
|/stream/render/pong|i|count| | |
|/stream/render/send|NULL| | | |
| | | | | |
|/stream/score/connect|s|name| | |
|/stream/score/connect|s s|host|port| |
|/stream/score/connect|none| | | |
|/stream/score/disconnect|none| | | |
|/stream/score/pong|i|count| | |
|/stream/score/send|NULL| | | |
| | | | | |
|/stream/visual/connect|s|name| | |
|/stream/visual/connect|s s|host|port| |
|/stream/visual/connect|none| | | |
|/stream/visual/disconnect|none| | | |
|/stream/visual/pong|i|count| | |
|/stream/visual/send|NULL| | | |
| | | | | |
|/stream/timer/connect|s s|host|port| |
|/stream/timer/connect|none| | | |
|/stream/timer/pong|i|count| | |
| | | | | |
|/reply|s i s|reply to message|state|message|


