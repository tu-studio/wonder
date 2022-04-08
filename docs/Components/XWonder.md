# XWONDER

xwonder is a graphical user interface for WONDER.

## Scenes, Scores, Projects

The aim of xwonder is to provide an interface, which facilitates the use of WONDERs
functions and establishes a stringent workflow for particular tasks.
Right now there are 4 tasks defined (for definitions of terms see section b):

1. Invoke a defined, static state of the room in which there are x sound
   sources, each of a certain type, with a fixed position (and orientation,
   if it's a planewave-source) and ID-number. (In short: "Load a Scene")
2. Create or edit a defined static state of the room and store it for future
   use. ("Create/edit an save a Scene")
3. Play a pre-recorded or -composed piece in which the room changes its state
   over time. ("Play a Score")
4. Create or edit a piece where the room changes its state over time and
   store it for future use. ("Create/edit and save a score")

So there are two different output entities with WONDER, a Scene or a Score. There is
a third intermediate entity called a Project.

In order to provide a consistent and meaningful terminology for users of WONDER the
following entities are defined.
(The terms here may differ slightly in meaning from similar terms used in other
"backend" softwaremodules of WONDER, which should not be visible to the users.)

`Scene`
:	A defined, static state of the room with a fixed number of soundsources,
	each with the parameters position (and orientation for planewave-sources),
	type, name and id#. A Scene always has a name.
	A Scene can be stored in an individual file.
	("backend"-wise this is a cwonder-projectfile consisting of a single scene)

`Score`
:	A piece containing movements and events of sources over time. A Score
	always has a name. A Score represents a finished piece of work, ready for
	playback.
	A Score can be stored in an individual file.
	("backend"-wise this is a scoreplayer-scorefile.)

`Project`
:	It's purpose is to create Scenes and/or Scores. A Project is a collection
	of Scenes.
	A Project can be stored in an individual file.
	(a cwonder-Projectfile)
	NOT IMPLEMENTED YET:
	A Project should have an associated Scorefile, which should be loaded
	along with the Project.

`Scene-Selector`
:   (Only available if working on a "Project" as opposed to a "Scene")
	A GUI-tool, which is a collection of pushbuttons. Each button is assigned
	an individual Scene. It's purpose is to compose a Score of several Scenes,
	either by navigating to a certain point in time(NOT IMPLEMENTED YET)
	and selecting a Scene or selecting a Scene when in scoreplayer-recordmode.
	This works much like your tv-remotecontrol, push a button and the room responds.
	To assign a Scene to a button just organise the sources in the room to your
	liking and assign this configuration to an existing button or add a new button
	which will be associated with this current configuration.

## Preliminary Presumptions

As of now these preliminary presumptions are made:

### Project already loaded

If at startup of xwonder there is already a project loaded in cwonder, the
user can choose whether to use this project or to discard it and reset all
sources. If he chooses to use the current project cwonder will be set to the
data transmitted by /WONDER/project/current (the scene with the lowest id
will be "selected", because the actual state of cwonder may differ from this data.

Further explanation:
at startup xwonder connects itself with the cwonder-messagestream
/WONDER/stream/visual, which replies to a connect with a /WONDER/source/posi-
tion and a /WONDER/source/type message for each source (even for disabled
sources) and in addition to that it sends an xml-string representation of the
currently loaded project in it's actual state. Problem is, that there is no
full information about the current state of cwonder (number of sources used,
names, mute) so in order to give a representations of what you will actu-
ally hear xwonder selects a scene.)

### Room Dimensions known to XWonder    

The dimensions of the room in which the WFS (Wavefield-Synthesis) system is
installed are known to xwonder (this version here is hardcoded for room H104
of the Technical Universitiy of Berlin (TU))

### CWonder, Scoreplayer and XWonder run on same machine

Cwonder (the controlling unit of WONDER) the scoreplayer and xwonder run on
the same computer, so that all files will be stored locally. (A SAMBA-solution
to this will be tested in the next few days, to allow a distriution of the
WONDER components to several computers.)



