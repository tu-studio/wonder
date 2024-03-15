cwonder_src = ['cwonder.cpp',
	       'cwonder_config.cpp',
	       'events.cpp',
	       'main.cpp',
	       'oscinctrl.cpp',
	       'oscping.cpp',
	       'oscstream.cpp']

# Install true installs the executable to the /usr/local/bin directory
executable('cwonder', cwonder_src,
           include_directories: inc,
           install_rpath : lib_dir,
           dependencies: [xml_dep, thread_dep, liblo_dep, libsystemd_dep],
           cpp_args: defines,
           link_with: libwonder,
           install: true
          )