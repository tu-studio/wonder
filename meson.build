project('wonder', 'cpp',
        version: '3.1.9',
        license: 'GPL',
        default_options: 'cpp_std=c++11')

conf_data = configuration_data()
conf_data.set('VERSION_STR', '"3.1.9"')

qt5 = import('qt5')

qt5_dep = dependency('qt5', modules: ['Core', 'Gui', 'Widgets', 'OpenGL', 'Xml'])
thread_dep = dependency('threads')
xml_dep = dependency('libxml++-2.6')
jack_dep = dependency('jack')
liblo_dep = dependency('liblo')
# Xwonder dependencies
glu_dep = dependency('glu')

inc = include_directories('src/include')

subdir('src/lib')
subdir('src/cwonder')
subdir('src/twonder')
subdir('src/xwonder')

