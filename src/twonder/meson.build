twonder_src = ['angle.cpp',
               'delaycoeff.cpp',
               'delayline.cpp',
               'osc.cpp',
               'source.cpp',
               'speaker.cpp',
               'twonder.cpp',
               'twonder_config.cpp'
              ]
executable('twonder', twonder_src,
           include_directories: inc,
           dependencies: [jack_dep, thread_dep, xml_dep, liblo_dep],
           link_with: libwonder,
           install: true
           )
