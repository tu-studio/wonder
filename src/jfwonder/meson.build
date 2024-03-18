if get_option('jfwonder')
	jfwonder_src = ['jfwonder.cpp',
			'jfwonder_config.cpp']
	executable('jfwonder' + install_postfix,
				jfwonder_src,
		   		include_directories: inc,
				dependencies: [jack_dep, thread_dep, liblo_dep, fmt_dep],
				link_with: libwonder,
				install: true)
	
	if get_option('versioned_installation')
	meson.add_install_script(
		'sh', '-c',
		'ln -s -f @0@ @1@'.format(
			get_option('prefix') / get_option('bindir') / 'jfwonder' + install_postfix,
			get_option('prefix') / get_option('bindir') / 'jfwonder')
	)
	endif

endif
