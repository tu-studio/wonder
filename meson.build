project('wonder', 'cpp',
        version: '3.90.0',
        license: 'GPL-3.0-or-later',
        default_options: 'cpp_std=c++17')

thread_dep = dependency('threads')
xml_dep = dependency('libxml++-3.0', fallback: ['libxml++-3.0', 'xmlxx_dep'])
jack_dep = dependency('jack')
liblo_dep = dependency('liblo')
systemd_dep = dependency('systemd', required: get_option('systemd'))
libsystemd_dep = dependency('libsystemd')
fmt_dep = dependency('fmt')

subdir('src/include')
inc = include_directories('src/include')

subdir('src/lib')
subdir('src/cwonder')
subdir('src/jfwonder')
subdir('src/score')
subdir('src/twonder')
if get_option('xwonder') == true
	qt5 = import('qt5')
	qt5_dep = dependency('qt5', modules: ['Core', 'Gui', 'Widgets', 'OpenGL', 'Xml'], include_type: 'system')
	glu_dep = dependency('glu')
	subdir('src/xwonder')
endif

if systemd_dep.found()
	subdir('services')
endif

config_dir = '/etc/wonder'
if get_option('location') == 'none'
	message('No location configuration selected.')
elif get_option('location') == 'EN325'
	install_subdir('configs/EN325', strip_directory: true, install_dir : config_dir )
elif get_option('location') == 'HUFO'
	install_subdir('configs/HuFo', strip_directory: true, install_dir : config_dir )
endif

install_subdir('scripts', install_dir: get_option('datadir') / 'wonder')
install_subdir('dtd', install_dir: get_option('datadir') / 'wonder')

summary({'CWonder': true,
	'TWonder': true,
	'JFWonder': get_option('jfwonder'),
	'XWonder': get_option('xwonder'),
	'ScorePlayer': get_option('scoreplayer')}, bool_yn: true, section: 'Components')
summary({'Location': get_option('location'), 'Directory': config_dir}, section: 'Configuration')
