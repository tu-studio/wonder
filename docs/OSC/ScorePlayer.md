# ScorePlayer

| Command|Parameters |content         >|...|...|...|...|...|
| -- | --  | -- | -- | -- | -- | --  | -- |
|/source/activate|i|id| | | | | |
|/source/deactivate|i|id| | | | | |
|/source/name|i s|id|name| | | | |
|/source/position|i f f f f|id|x coordinate [ meters ]|y coordinate [ meters ]|duration [ seconds ]|timestamp [ seconds ] | |
|/source/type|i i f |id|type [ 0=plane, 1=point ]|timestamp [ seconds ]| | | |
|/source/angle|i f f f|id|angle [ degrees ]|timestamp [ seconds ]|duration [ seconds ]| | |
|/score/source/enableRecord|i i|id|record  [ 0=off, 1= on ]| | | | |
|/score/source/enableRead|i i|id| | | | | |
| | | | | | | | |
|/score/enableRecord|i|record (0=off, 1=on)| | | | | |
|/score/enableRead|i|read    (0=off, 1=on)| | | | | |
| | | | | | | | |
|/score/create|s|scorename| | | | | |
|/score/save|NULL| | | | | | |
|/score/load|s|scorename| | | | | |
|/score/play|none| | | | | | |
|/score/stop|none| | | | | | |
| | | | | | | | |
|/score/reset|none| | | | | | |
|/score/setStartScenario|none| | | | | | |
|/score/enableMMC|i|0 = off, 1 = on| | | | | |
|/score/enableMSRC|i|0 = off, 1 = on| | | | | |
|/score/newtime|i i i i|hours|minutes|seconds |milliseconds| | |
| | | | | | | | |
|/global/maxNoSources|i|number of sources| | | | | |
| | | | | | | | |
|/stream/score/ping|i|pingcount| | | | | |
|/reply|s i s|reply to message|state|message| | | |
