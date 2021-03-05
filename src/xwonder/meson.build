moc_h = ['XwonderMainWindow.h',
	'XwonderConfig.h',
	'Sources3DWidget.h',
	'SourcePositionDialog.h',
	'ChannelsWidget.h',
	'SourceWidget.h',
	'Source.h',
	'SourceGroup.h',
	'TimelineWidget.h',
	'TimeLCDNumber.h',
	'SnapshotSelector.h',
	'SnapshotSelectorButton.h',
	'SnapshotNameDialog.h',
	'OSCReceiver.h',
	'FilenameDialog.h',
	'AddChannelsDialog.h',
	'StreamClientWidget.h']
qrc = 'icons.qrc'
preproc_files = qt5.preprocess(moc_headers: moc_h,
	qresources: qrc)

conf_data = configuration_data()
conf_data.set('VERSION_STR', '"3.1.9"')

xwonder_src = ['Xwonder.cpp',
	'XwonderMainWindow.cpp',
	'XwonderConfig.cpp',
	'Sources3DWidget.cpp',
	'SourcePositionDialog.cpp',
	'ChannelsWidget.cpp',
	'SourceWidget.cpp',
	'Source.cpp',
	'SourceGroup.cpp',
	'TimelineWidget.cpp',
	'TimeLCDNumber.cpp',
	'SnapshotSelector.cpp',
	'SnapshotSelectorButton.cpp',
	'SnapshotNameDialog.cpp',
	'OSCReceiver.cpp',
	'FilenameDialog.cpp',
	'AddChannelsDialog.cpp',
	'StreamClientWidget.cpp']

executable('xwonder', xwonder_src, preproc_files,
           include_directories: inc,
           dependencies : [qt5_dep, xml_dep, jack_dep, liblo_dep, glu_dep],
           link_with: libwonder)
