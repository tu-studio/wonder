# configure file takes a file and replaces the @variables@ with the values from the configuration data
# the configuration data is a dictionary of key value pairs

message('Systemd service files')
message('required arguments for the service files are: prefix, datadir, bindir')
message('prefix: the installation prefix: ' + get_option('prefix'))
message('datadir: the directory where the data files are installed: ' + get_option('datadir'))
message('bindir: the directory where the binary files are installed: ' + get_option('bindir'))

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

install_data('jack.service', install_dir : '/etc/systemd/user')

# everything after the @ is inserted into the service file, where the %i is
# in our case its the username of the user that the service is started for

# configure_file(input : 'twonder@.service.in',
#                output : 'twonder@.service',
#                configuration : conf_data,
#                install_dir : '/etc/systemd/system')

# configure_file(input : 'cwonder@.service.in',
#                output : 'cwonder@.service',
#                configuration : conf_data2,
#                install_dir : '/etc/systemd/system')

# configure_file(input : 'jfwonder.service.in',
#                output : 'jfwonder.service',
#                configuration : conf_data,
#                install_dir : '/etc/systemd/user')