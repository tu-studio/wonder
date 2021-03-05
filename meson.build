project('wonder', 'cpp',
        version: '3.1.9',
        license: 'GPL-3.0-or-later',
        default_options: 'cpp_std=c++11')

qt5 = import('qt5')

qt5_dep = dependency('qt5', modules: ['Core', 'Gui', 'Widgets', 'OpenGL', 'Xml'], include_type: 'system')
thread_dep = dependency('threads')
xml_dep = dependency('libxml++-2.6')
jack_dep = dependency('jack')
liblo_dep = dependency('liblo')
# Xwonder dependencies
glu_dep = dependency('glu')

subdir('src/include')
inc = include_directories('src/include')

subdir('src/lib')
subdir('src/cwonder')
subdir('src/twonder')
subdir('src/xwonder')

install_subdir('configs/.', install_dir : '/etc/wonder')
install_subdir('scripts', install_dir: get_option('datadir') / 'wonder')
