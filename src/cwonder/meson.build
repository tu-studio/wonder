cwonder_src = ['cwonder.cpp',
	       'cwonder_config.cpp',
	       'events.cpp',
	       'main.cpp',
	       'oscinctrl.cpp',
	       'oscping.cpp',
	       'oscstream.cpp']

executable('cwonder', cwonder_src,
           include_directories: inc,
           dependencies: [xml_dep, thread_dep, liblo_dep, libsystemd_dep],
           link_with: libwonder,
           install: true
          )
