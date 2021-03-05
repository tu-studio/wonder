conf_data = configuration_data()
conf_data.set('VERSION_STR', meson.project_version())
conf_data.set_quoted('INSTALL_PREFIX', get_option('prefix'))
configure_file(output: 'config.h',
               configuration: conf_data)
