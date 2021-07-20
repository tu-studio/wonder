if get_option('jfwonder')
	jfwonder_src = ['jfwonder.cpp',
			'jfwonder_config.cpp']
	executable('jfwonder', jfwonder_src,
		   include_directories: inc,
		   dependencies: [jack_dep, thread_dep, liblo_dep, fmt_dep],
		   link_with: libwonder,
		   install: true
		   )
endif
