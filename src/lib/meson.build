libwonder_sources = ['commandqueue.cpp',
		     'jackringbuffer.cpp',
		     'liblo_extended.cpp',
		     'oscin.cpp',
		     'project.cpp',
		     'rtcommandengine.cpp',
		     'speakersegment.cpp',
		     'timestamp.cpp',
		     'wonder_path.cpp'
		     ]
libwonder = static_library('wonder',
                        libwonder_sources,
                        include_directories : inc,
                        dependencies: [xml_dep, liblo_dep, jack_dep])
