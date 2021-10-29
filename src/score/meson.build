if get_option('scoreplayer')

	rtmidi_dep = dependency('rtmidi', required: true)
	score_src = ['file_io.cpp',
		     'main.cpp',
		     'mtc_com.cpp',
		     'scorecontrol.cpp',
		     'score_line.cpp',
		     'scoreplayer_config.cpp',
		     'scoresaxparser.cpp',
		     'w_osc.cpp']

	executable('score', score_src,
		   include_directories: inc,
		   dependencies: [thread_dep, xml_dep, liblo_dep, rtmidi_dep],
		   link_with: libwonder,
		   install: true
		   )
endif
