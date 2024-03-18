twonder_src = ['angle.cpp',
               'delaycoeff.cpp',
               'delayline.cpp',
               'osc.cpp',
               'source.cpp',
               'speaker.cpp',
               'twonder.cpp',
               'twonder_config.cpp'
              ]
executable('twonder' + install_postfix,
            twonder_src,
            include_directories: inc,
            install_rpath: lib_dir,
            dependencies: [jack_dep, thread_dep, xml_dep, liblo_dep],
            link_with: libwonder,
            install: true)

if get_option('versioned_installation')
meson.add_install_script(
	'sh', '-c',
	'ln -s -f @0@ @1@'.format(
        get_option('prefix') / get_option('bindir') / 'twonder' + install_postfix,
        get_option('prefix') / get_option('bindir') / 'twonder')
)
endif