conf_data = configuration_data()
conf_data.set('VERSION_STR', meson.project_version())
conf_data.set_quoted('CONFIG_DIR', '/etc/wonder')
conf_data.set_quoted('DATA_DIR', get_option('prefix') / get_option('datadir') / 'wonder')
configure_file(output: 'config.h',
               configuration: conf_data)
