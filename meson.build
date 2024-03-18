project('wonder', 'cpp',
        version: '4.0.0',
        license: 'GPL-3.0-or-later',
        default_options: 'cpp_std=c++17')

defines = []

thread_dep = dependency('threads')
xml_dep = dependency('libxml++-3.0', fallback: ['libxml++-3.0', 'xmlxx_dep'])
jack_dep = dependency('jack')
liblo_dep = dependency('liblo')
fmt_dep = dependency('fmt')

subdir('src/include')
inc = include_directories('src/include')
lib_dir = get_option('prefix') / get_option('libdir')

if get_option('versioned_install')
  	install_postfix = '-' + meson.project_version()
else
	install_postfix = ''
endif

subdir('src/lib')
subdir('src/cwonder')
subdir('src/twonder')
if get_option('jfwonder')
  subdir('src/jfwonder')
endif

# The configuration directory for locally installed programs should be /usr/local/etc/wonder and not to /etc/wonder as before
# Configuration files examples are in config/examples and could be installed as follows:
# subdir('config/examples/EN325')
# For this to work the option 'location' and 'node' need to be set in the meson.options file

# With install_dir: install_dir: get_option('datadir') the files are installed in the usr/local/share/ directory
install_subdir('scripts', install_dir: get_option('datadir') / 'wonder' + install_postfix)
install_subdir('dtd', install_dir: get_option('datadir') / 'wonder' + install_postfix)
if get_option('versioned_install')
meson.add_install_script(
	'sh', '-c',
	'ln -s -f @0@ @1@'.format(
        get_option('prefix') / get_option('datadir') / 'wonder' + install_postfix + '/',
        get_option('prefix') / get_option('datadir') / 'wonder')
)
endif

summary({'CWonder': true,
		'TWonder': true,
		'JFWonder': get_option('jfwonder'),
		'Versioned Installation': get_option('versioned_install')},
		bool_yn: true, section: 'Options')