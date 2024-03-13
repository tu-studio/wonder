# configure_file takes a file and replaces the @variables@ with the values from the configuration data
# the configuration data is a dictionary of key value pairs

# the /usr/lib/systemd/user directory is used for user services but should only contain systemd files installed by the package manager of our distribution
# therefore we use the /etc/systemd/user directory which is used for user services that are installed by the user

conf_data = configuration_data()
conf_data.set('share_dir', get_option('prefix') / get_option('datadir') / 'wonder')
conf_data.set('bin_dir', get_option('prefix') / get_option('bindir'))

configure_file(input : 'twonder.service.in',
               output : 'twonder.service',
               configuration : conf_data,
               install_dir : '/etc/systemd/user')

configure_file(input : 'cwonder.service.in',
               output : 'cwonder.service',
               configuration : conf_data,
               install_dir : '/etc/systemd/user')
