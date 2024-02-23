project('wonder', 'cpp',
        version: '3.90.0',
        license: 'GPL-3.0-or-later',
        default_options: 'cpp_std=c++17')

defines = []

thread_dep = dependency('threads')
xml_dep = dependency('libxml++-3.0', fallback: ['libxml++-3.0', 'xmlxx_dep'])
jack_dep = dependency('jack')
liblo_dep = dependency('liblo')
systemd_dep = dependency('systemd', required: get_option('systemd'))
libsystemd_dep = dependency('libsystemd', required: get_option('systemd'))
fmt_dep = dependency('fmt')

if libsystemd_dep.found()
	defines += '-DWONDER_SYSTEMD' # the defines are passed to the excecutables as cppargs
endif

subdir('src/include')
inc = include_directories('src/include')
lib_dir = get_option('prefix') / get_option('libdir')
message('Library directory: ' + lib_dir)

subdir('src/lib')
subdir('src/cwonder')
# subdir('src/jfwonder')
# subdir('src/score')
subdir('src/twonder')
# if get_option('xwonder') == true
# 	qt5 = import('qt5')
# 	qt5_dep = dependency('qt5', modules: ['Core', 'Gui', 'Widgets', 'OpenGL', 'Xml'], include_type: 'system')
# 	glu_dep = dependency('glu')
# 	subdir('src/xwonder')
# endif

if systemd_dep.found()
	subdir('services')
endif

# The configuration directory for locally installed programs should be /usr/local/etc/wonder and not to /etc/wonder as before
if get_option('location') == 'none'
	message('No location configuration selected.')
elif get_option('location') == 'EN325'
	install_subdir('configs/EN325', strip_directory: true, install_dir : get_option('sysconfdir') / 'wonder' )
elif get_option('location') == 'HUFO'
	install_subdir('configs/HuFo', strip_directory: true, install_dir : get_option('sysconfdir') / 'wonder' )
endif

# With install_dir: install_dir: get_option('datadir') the files are installed in the usr/local/share/ directory
install_subdir('scripts', install_dir: get_option('datadir') / 'wonder')
install_subdir('dtd', install_dir: get_option('datadir') / 'wonder')
message('Data directory: ' + get_option('datadir'))

summary({'CWonder': true,
	'TWonder': true,
	'JFWonder': get_option('jfwonder'),
	'XWonder': get_option('xwonder'),
	'ScorePlayer': get_option('scoreplayer')}, bool_yn: true, section: 'Components')
summary({'Location': get_option('location'), 'Directory': get_option('prefix') / get_option('sysconfdir') / 'wonder'}, section: 'Configuration')
