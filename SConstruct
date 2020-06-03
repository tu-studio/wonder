#/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
# *                                                                                   *
# *  WONDER - Wave field synthesis Of New Dimensions of Electronic music in Realtime  *
# *  http://swonder.sourceforge.net                                                   *
# *                                                                                   *
# *                                                                                   *
# *  Technische Universitaet Berlin, Germany                                           *
# *  Audio Communication Group                                                        *
# *  www.ak.tu-berlin.de                                                              *
# *  Copyright 2006-2008                                                              *
# *                                                                                   *
# *                                                                                   *
# *  This program is free software; you can redistribute it and/or modify             *
# *  it under the terms of the GNU General Public License as published by             *
# *  the Free Software Foundation; either version 2 of the License, or                *
# *  (at your option) any later version.                                              *
# *                                                                                   *
# *  This program is distributed in the hope that it will be useful,                  *
# *  but WITHOUT ANY WARRANTY; without even the implied warranty of                   *
# *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the                    *
# *  GNU General Public License for more details.                                     *
# *                                                                                   *
# *  You should have received a copy of the GNU General Public License                *
# *  along with this program; if not, write to the Free Software                      *
# *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.       *
# *                                                                                   *
# * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

# ======================================================================
#
#  This is the global SConstruct file for WONDER.
#  Every module can be build from here.
#  Please see documentation/scons.txt for further instructions.
#
# ======================================================================



# ======================================================================
# SETUP
# ======================================================================

EnsureSConsVersion(0,96)
EnsurePythonVersion(2,3)
SConsignFile()



# ======================================================================
# IMPORTS
# ======================================================================

import sys
import os
import time
import platform



# ======================================================================
# CONSTANTS
# ======================================================================

PACKAGE = 'WONDER'
VERSION = '3.1.9'



# ======================================================================
# UTILITY FUNCTIONS
# ======================================================================

def createEnvironment(*keys):
    env = os.environ
    res = {}
    for key in keys:
        if key in env:
            res[key] = env[key]
    return res



def CheckPKGConfig(context, version):
    context.Message("pkg-config installed?\t\t\t")
    ret = context.TryAction('pkg-config --atleast-pkgconfig-version=%s' % version)[0]
    context.Result(ret)
    return ret



def LookForPackage(context, name):
    if len(name) > 4:
        context.Message("%s installed?\t\t\t" % name)
    else:
        context.Message("%s installed?\t\t\t\t" % name)

    ret = context.TryAction('pkg-config --exists \'%s\'' % name)[0]
    context.Result(ret)
    return ret



def print_config(msg, two_dee_iterable):
    print("\n {0} ".format(msg))
    for key, val in two_dee_iterable:
        print("    {0} {1} ".format(key, val))
    print("")



def build_config_h(target, source, env):
    if "MINGW" not in platform.system():
        config_h_defines = {
            "install_directory": env['installto'],
            "version_str": VERSION,
            "build": env['build']
        }
    else:
        config_h_defines = {
            "install_directory": "",
            "version_str": VERSION,
            "build": env['build']
        }

    print_config("Generating config.h with the following settings:", config_h_defines.items())

    for a_target, a_source in zip(target, source):
        config_h    = open(str(a_target), "w")
        config_h_in = open(str(a_source), "r")

    config_h.write(config_h_in.read() % config_h_defines)
    config_h_in.close()
    config_h.close()



# ======================================================================
# PRINT INFOS AND BUILD OPTIONS TO COMMANDLINE
# ======================================================================

print("\n==================== " + PACKAGE + " " + VERSION + " ====================")

opts = Variables()
opts.Add(BoolVariable('lib',       'Set to 1 to build the wonder library', 0))
opts.Add(BoolVariable('cwonder',   'Set to 1 to build cwonder',            0))
opts.Add(BoolVariable('twonder',   'Set to 1 to build twonder',            0))
opts.Add(BoolVariable('xwonder',   'Set to 1 to build xwonder',            0))
opts.Add(BoolVariable('all',       'Set to 1 to build all the targets',    0))
opts.Add(EnumVariable('build',     'Set the build version', 'release', allowed_values = ('debug', 'release'), ignorecase = 2))
opts.Add(PathVariable('installto', 'Set the installation directory', '/usr/local'))

print("Platform: " + platform.system() + "\n")



# ======================================================================
# BASIC ENVIRONMENT
# ======================================================================

# Location of headerfiles
includePath = 'src/include'

# Location of Qt5, distinguish between Linux and Windows platforms
qt5Path = '/usr/include/qt5'

# Use Qt5 build tools only on Linux platforms
if "MINGW" in platform.system():
    buildTools = ['default']
else:
    buildTools = ['default', 'qt5']

# Location of object files
VariantDir('obj', 'src', duplicate = 0)

# Prepare environment
env = Environment(variables = opts,
                  ENV       = createEnvironment('PATH', 'PKG_CONFIG_PATH'),
                  PACKAGE   = PACKAGE,
                  VERSION   = VERSION,
                  URL       = 'http://swonder.sourceforge.net',
                  CPPPATH   = includePath,
                  tools     = buildTools,
                  toolpath  = './site_scons/site_tools/qt5',
                  QT5DIR    = qt5Path)

# Set options if target "all" is selected, distinguish between Linux and Windows platforms
if env['all']:
    env['lib'] = 1
    env['cwonder'] = 1
    env['twonder'] = 1
    env['xwonder'] = 1

    if "MINGW" in platform.system():
        print("WARNING: Building xwonder is only supported on Linux platforms!\n")
        env['xwonder'] = 0

# Do not compile xwonder on Windows platforms with scons. Use "qmake" and "make" instead
if env['xwonder']:
    if "MINGW" in platform.system():
        print("WARNING: Building xwonder is only supported on Linux platforms!\n")
        print("Use \"qmake\" and \"make\" from inside the xwonder source directory instead:")
        print("Linux  : $ qmake ./xwonder.pro")
        print("Windows: $ /c/MSYS64/mingw64/qt5-static/bin/qmake.exe -spec win32-g++ ./xwonder.pro")
        print("MacOS X: $ qmake -spec macx-g++ ./xwonder.pro")
        print("\nAfterwards type \"make\" to build the xwonder executable.")
        Exit(1)

# Always build WONDER library for cwonder and twonder
if env['cwonder'] | env['twonder']:
    env['lib'] = 1

# Names for executables and libraries
executables = []
libs        = []

# Location for the WONDER library path on Linux platforms
if 'install' in COMMAND_LINE_TARGETS:
    rpath = env['installto'] + '/lib/'
else:
    rpath = env.Literal('\$$ORIGIN')

# Set debug flags if necessary
if env['build'] == 'debug':
    env.Append(CCFLAGS = '-g')

# Compiler arguments
env.Append(CCFLAGS = '-std=c++14 -Wall')

# Compiler optimizations
if env['build'] == 'release':
    env.Append(CCFLAGS = '-O3 -fPIC -march=native')
    env.Append(CPPDEFINES = 'NDEBUG')

# If using MinGW (MSYS2) on Windows platforms add the following libraries and defines
if "MINGW" in platform.system():
    env.Append(CPPDEFINES = ['WIN32', '_WIN32_WINNT=0x601', '_USE_MATH_DEFINES'])



# ======================================================================
# PRINT HELP
# ======================================================================

Help("""
Command Line options:
scons -h         (WONDER help)
scons -H         (SCons help)
""")

Help(opts.GenerateHelpText(env))



# ======================================================================
# CHECK DEPENDENCIES AND LIBRARIES
# ======================================================================

# Check for pkg-config and use it to configure the needed libraries and packages, but only on Linux platforms
conf = env.Configure(custom_tests = { 'CheckPKGConfig' : CheckPKGConfig, 'LookForPackage' : LookForPackage })

# Linux and MacOS platforms
if "MINGW" not in platform.system():

    if not conf.CheckPKGConfig('0'):
        print("pkg-config not found.")
        Exit(1)

    if not conf.LookForPackage('libxml++-2.6'):
        Exit(1)

    env.ParseConfig('pkg-config --cflags --libs libxml++-2.6')

    if not conf.LookForPackage('jack'):
        Exit(1)

    env.ParseConfig('pkg-config --cflags --libs jack')

    if not conf.LookForPackage('liblo'):
        Exit(1)

    env.ParseConfig('pkg-config --cflags --libs liblo')

    # xwonder additionally needs freeglut (glu)
    if env['xwonder']:

        if not conf.LookForPackage('glu'):
            Exit(1)

        env.ParseConfig('pkg-config --cflags --libs glu')

# Windows platforms with MSYS2 and MinGW
else:

    if not conf.CheckPKGConfig('0'):
        print("pkg-config not found.")
        Exit(1)

    if not conf.LookForPackage('libxml++-2.6'):
        Exit(1)

    env.ParseConfig('pkg-config --cflags --libs libxml++-2.6')

# Finish environment configuration
env = conf.Finish()



# ======================================================================
# WRITE BUILD AND VERSION INFO
# ======================================================================

wonder_version_file = open('wonder.built.info', "w")
wonder_version_file.write('###\t ' + PACKAGE + ' build info                        ###\n')
wonder_version_file.write('### This file is automatically generated by scons ###\n\n')
wonder_version_file.write('### build info ###\n')
wonder_version_file.write('Version: \t' + PACKAGE + ' ' + VERSION + ' ( ' + env['build'] + ' )\n')
wonder_version_file.write('last change: \t' + time.asctime(time.localtime()) + '\n\n')

if env['lib']:
    wonder_version_file.write( 'library: \t' + time.asctime(time.localtime()) + '\n')
if env['cwonder']:
    wonder_version_file.write( 'cwonder: \t' + time.asctime(time.localtime()) + '\n')
if env['twonder']:
    wonder_version_file.write( 'twonder: \t' + time.asctime(time.localtime()) + '\n')
if env['xwonder']:
    wonder_version_file.write( 'xwonder: \t' + time.asctime(time.localtime()) + '\n')

wonder_version_file.close()

# Write config.h
env.Command(includePath + '/config.h', includePath + '/config.h.in', build_config_h)



# ======================================================================
# WONDER MODULES
# ======================================================================

# +++ WONDER LIBRARY +++
lib_srcs = Split('''
obj/lib/oscin.cpp
obj/lib/speakersegment.cpp
obj/lib/liblo_extended.cpp
obj/lib/timestamp.cpp
obj/lib/wonder_path.cpp
obj/lib/project.cpp
obj/lib/rtcommandengine.cpp
obj/lib/commandqueue.cpp
obj/lib/jackringbuffer.cpp
''')

wonderLibraryName = 'wonder'

if env['lib']:
    print("\nwonder library\t\t\t\tyes")
    libenv = env.Clone()

    if "MINGW" not in platform.system():
        libenv.Append(LIBS = ['lo', 'jack'], RPATH = rpath)
    else:
        libenv.Append(LIBS = ['lo', 'ws2_32', 'iphlpapi', 'jack64'])

    wonderlib = libenv.SharedLibrary(target = 'bin/' + wonderLibraryName, source = lib_srcs)
    libs.append(wonderlib)
else:
    print("\nWONDER library\t\t\t\tno")


# +++ CWONDER +++
cwonder_srcs = Split('''
obj/cwonder/main.cpp
obj/cwonder/cwonder.cpp
obj/cwonder/cwonder_config.cpp
obj/cwonder/oscinctrl.cpp
obj/cwonder/events.cpp
obj/cwonder/oscstream.cpp
obj/cwonder/oscping.cpp
''')

if env['cwonder']:
    print("cwonder\t\t\t\t\tyes")
    cwonderenv = env.Clone()

    if "MINGW" not in platform.system():
        cwonderenv.Append(LIBS = [wonderLibraryName, 'lo'], LIBPATH = '#bin/', RPATH = rpath)
    else:
        cwonderenv.Append(LIBS = [wonderLibraryName, 'lo', 'ws2_32', 'iphlpapi'], LIBPATH = '#bin/')

    cwonderprog = cwonderenv.Program(target = 'bin/cwonder', source = cwonder_srcs)
    executables.append(cwonderprog)
else:
    print("cwonder\t\t\t\t\tno")


# +++ TWONDER +++
twonder_srcs = Split('''
obj/twonder/twonder.cpp
obj/twonder/delayline.cpp
obj/twonder/delaycoeff.cpp
obj/twonder/speaker.cpp
obj/twonder/osc.cpp
obj/twonder/source.cpp
obj/twonder/angle.cpp
obj/twonder/twonder_config.cpp
obj/twonder/listener.cpp
obj/twonder/listener_array.cpp
''')

if env['twonder']:
    print("twonder\t\t\t\t\tyes")
    twonderenv = env.Clone()

    if "MINGW" not in platform.system():
        twonderenv.Append(LIBS = [wonderLibraryName, 'lo', 'jack'], LIBPATH = '#bin/', RPATH = rpath)
    else:
        twonderenv.Append(LIBS = [wonderLibraryName, 'lo', 'ws2_32', 'iphlpapi', 'jack64'], LIBPATH = '#bin/')

    twonderprog = twonderenv.Program(target = 'bin/twonder', source = twonder_srcs)
    executables.append(twonderprog)
else:
    print("twonder\t\t\t\t\tno")


# +++ XWONDER +++
xwonder_files = Split('''
obj/xwonder/Xwonder.cpp
obj/xwonder/XwonderMainWindow.cpp
obj/xwonder/XwonderConfig.cpp
obj/xwonder/Sources3DWidget.cpp
obj/xwonder/SourcePositionDialog.cpp
obj/xwonder/ChannelsWidget.cpp
obj/xwonder/SourceWidget.cpp
obj/xwonder/Source.cpp
obj/xwonder/SourceGroup.cpp
obj/xwonder/TimelineWidget.cpp
obj/xwonder/TimeLCDNumber.cpp
obj/xwonder/SnapshotSelector.cpp
obj/xwonder/SnapshotSelectorButton.cpp
obj/xwonder/SnapshotNameDialog.cpp
obj/xwonder/OSCReceiver.cpp
obj/xwonder/FilenameDialog.cpp
obj/xwonder/AddChannelsDialog.cpp
obj/xwonder/StreamClientWidget.cpp
obj/lib/oscin.cpp
''')

if env['xwonder']:
    print("xwonder\t\t\t\t\tyes")
    xwonderenv = env.Clone()
    xwonderenv.EnableQt5Modules(['QtWidgets', 'QtOpenGL', 'QtXml'], debug = False)
    xwonderprog = xwonderenv.Program(target = ['bin/xwonder'], source = xwonder_files + Glob('obj/xwonder/icons.qrc'))
    executables.append(xwonderprog)
else:
    print("xwonder\t\t\t\t\tno")



# ======================================================================
# INSTALLATION
# ======================================================================

if 'install' in COMMAND_LINE_TARGETS and '-c' not in sys.argv:
    PREFIX = env['installto']

    # Installation directories, distinguished by Linux and Windows platforms
    if "MINGW" not in platform.system():
        BIN_DIR = PREFIX + "/bin"
        LIB_DIR = PREFIX + "/lib"
    else:
        BIN_DIR = PREFIX
        LIB_DIR = PREFIX

    CONF_DIR = PREFIX + "/configs"
    DTD_DIR  = PREFIX + "/configs/dtd"

    # Names for configuration and DTD files
    configs = []
    dtds    = []

    if env['cwonder']:
        dtds.append('configs/dtd/cwonder_project.dtd')
        dtds.append('configs/dtd/cwonder_config.dtd')
        configs.append('configs/cwonder_config.xml')

    if env['twonder']:
        dtds.append('configs/dtd/twonder_config.dtd')
        dtds.append('configs/dtd/twonder_speakerarray.dtd')
        configs.append('configs/twonder_config.xml')
        configs.append('configs/twonder_speakerarray.xml')
        configs.append('configs/speakers/')

    # Install all files
    env.Alias('install', env.Install(BIN_DIR, executables))
    env.Alias('install', env.Install(LIB_DIR, libs))
    env.Alias('install', env.Install(CONF_DIR, configs))
    env.Alias('install', env.Install(CONF_DIR, 'wonder.built.info'))
    env.Alias('install', env.Install(DTD_DIR, dtds))
    env.Alias('install', env.Install(PREFIX, 'scripts'))



# ======================================================================
# PRINT BUILD MODE AND INSTALLATION DIRECTORY
# ======================================================================

print("\nBuild mode:\t\t\t\t" + env['build'] + "\n")

if 'install' in COMMAND_LINE_TARGETS:
    print("Install to:\t\t\t\t" + env['installto'] + "\n")



# ======================================================================
# END OF SCONSTRUCT
# ======================================================================
