project('wonder', 'cpp',
        version: '3.1.9',
        license: 'GPL-3.0-or-later',
        default_options: 'cpp_std=c++17')

qt5 = import('qt5')

qt5_dep = dependency('qt5', modules: ['Core', 'Gui', 'Widgets', 'OpenGL', 'Xml'], include_type: 'system')
thread_dep = dependency('threads')
xml_dep = dependency('libxml++-3.0')
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

fs = import('fs')
config_dir = '/etc/wonder'
if not fs.exists(config_dir)
	install_subdir('configs', strip_directory: true, install_dir : config_dir )
else
	message('/etc/wonder already exists. Remove directory if you want an example configuration!')
endif
install_subdir('scripts', install_dir: get_option('datadir') / 'wonder')
install_subdir('dtd', install_dir: get_option('datadir') / 'wonder')
