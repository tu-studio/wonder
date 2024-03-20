cwonder_src = ['cwonder.cpp',
	       'cwonder_config.cpp',
	       'events.cpp',
	       'main.cpp',
	       'oscinctrl.cpp',
	       'oscping.cpp',
	       'oscstream.cpp']

# Install true installs the executable to the /usr/local/bin directory
executable('cwonder' + install_postfix, 
            cwonder_src,
            include_directories: inc,
            install_rpath : lib_dir,
            dependencies: [xml_dep, thread_dep, liblo_dep, libsystemd_dep],
            cpp_args: defines,
            link_with: libwonder,
            install: true)

if get_option('versioned_install')
meson.add_install_script(
	'sh', '-c',
	'ln -s -f @0@ @1@'.format(
        get_option('prefix') / get_option('bindir') / 'cwonder' + install_postfix,
        get_option('prefix') / get_option('bindir') / 'cwonder')
)
endif