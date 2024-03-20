project('wonder', 'cpp',
        version: run_command('git', 'describe', '--dirty', check: true).stdout().strip(),
        license: 'GPL-3.0-or-later',
        default_options: 'cpp_std=c++17')
	
message('Building Wonder version ' + meson.project_version())

defines = []

thread_dep = dependency('threads')
xml_dep = dependency('libxml++-3.0', fallback: ['libxml++-3.0', 'xmlxx_dep'])
jack_dep = dependency('jack')
liblo_dep = dependency('liblo')
if get_option('jfwonder')
	fmt_dep = dependency('fmt')
endif

subdir('src/include')
inc = include_directories('src/include')
lib_dir = get_option('prefix') / get_option('libdir')

if get_option('versioned_install')
  	install_postfix = '-' + meson.project_version()
else
	install_postfix = ''
endif

subdir('src/lib')
if get_option('cwonder')
	subdir('src/cwonder')
endif
if get_option('twonder')
	subdir('src/twonder')
endif
if get_option('jfwonder')
	subdir('src/jfwonder')
endif
subdir('systemd')

# The configuration directory for locally installed programs should be /usr/local/etc/wonder and not to /etc/wonder as before
# Configuration files examples are in config/examples and could be installed as follows:
# subdir('config/examples/EN325')
# For this to work the option 'location' and 'node' need to be set in the meson.options file

# With install_dir: install_dir: get_option('datadir') the files are installed in the usr/local/share/ directory
if get_option('twonder')
	install_data('scripts/start_twonder.sh', install_dir: get_option('datadir') / 'wonder' + install_postfix + '/scripts')
	install_data(['dtd/twonder_config.dtd', 'dtd/twonder_speakerarray.dtd'], install_dir: get_option('datadir') / 'wonder' + install_postfix + '/dtd')
endif
if get_option('cwonder')
	install_data('dtd/cwonder_config.dtd', install_dir: get_option('datadir') / 'wonder' + install_postfix + '/dtd')
endif

# -n option is used to overwrite symlinks to directories without dereferencing them
if get_option('versioned_install')
meson.add_install_script(
	'sh', '-c',
	'ln -s -f -n @0@ @1@'.format(
        get_option('prefix') / get_option('datadir') / 'wonder' + install_postfix + '/',
        get_option('prefix') / get_option('datadir') / 'wonder')
)
endif

summary({'TWonder': get_option('twonder'),
		 'CWonder': get_option('cwonder'),
		'JFWonder': get_option('jfwonder'),
		'Versioned Installation': get_option('versioned_install')},
		bool_yn: true, section: 'Options')