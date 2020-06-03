project('wonder', 'cpp')
qt5 = import('qt5')

qt5dep = dependency('qt5', modules: ['Core', 'Gui', 'Widgets', 'OpenGL', 'Xml'])
xmldep = dependency('libxml++-2.6')
jackdep = dependency('jack')
lodep = dependency('liblo')

# Xwonder dependencies
gludep = dependency('glu')

inc = include_directories('src/include')

moc_files = qt5.preprocess(moc_headers: ['src/xwonder/XwonderMainWindow.h',
	'src/xwonder/XwonderConfig.h',
	'src/xwonder/Sources3DWidget.h',
	'src/xwonder/SourcePositionDialog.h',
	'src/xwonder/ChannelsWidget.h',
	'src/xwonder/SourceWidget.h',
	'src/xwonder/Source.h',
	'src/xwonder/SourceGroup.h',
	'src/xwonder/TimelineWidget.h',
	'src/xwonder/TimeLCDNumber.h',
	'src/xwonder/SnapshotSelector.h',
	'src/xwonder/SnapshotSelectorButton.h',
	'src/xwonder/SnapshotNameDialog.h',
	'src/xwonder/OSCReceiver.h',
	'src/xwonder/FilenameDialog.h',
	'src/xwonder/AddChannelsDialog.h',
	'src/xwonder/StreamClientWidget.h'],
                           moc_extra_arguments: ['-DMAKES_MY_MOC_HEADER_COMPILE'],
                           include_directories: inc,
                           dependencies: qt5dep)
conf_data = configuration_data()
conf_data.set('VERSION_STR', '"3.1.9"')

xwonder_src = ['src/xwonder/Xwonder.cpp',
	'src/xwonder/XwonderMainWindow.cpp',
	'src/xwonder/XwonderConfig.cpp',
	'src/xwonder/Sources3DWidget.cpp',
	'src/xwonder/SourcePositionDialog.cpp',
	'src/xwonder/ChannelsWidget.cpp',
	'src/xwonder/SourceWidget.cpp',
	'src/xwonder/Source.cpp',
	'src/xwonder/SourceGroup.cpp',
	'src/xwonder/TimelineWidget.cpp',
	'src/xwonder/TimeLCDNumber.cpp',
	'src/xwonder/SnapshotSelector.cpp',
	'src/xwonder/SnapshotSelectorButton.cpp',
	'src/xwonder/SnapshotNameDialog.cpp',
	'src/xwonder/OSCReceiver.cpp',
	'src/xwonder/FilenameDialog.cpp',
	'src/xwonder/AddChannelsDialog.cpp',
	'src/xwonder/StreamClientWidget.cpp',
	qt5.preprocess(qresources : 'src/xwonder/icons.qrc'),
	'src/lib/oscin.cpp']
	
executable('xwonder', xwonder_src, moc_files,
           include_directories: inc, 
           dependencies : [qt5dep, xmldep, jackdep, lodep, gludep])
