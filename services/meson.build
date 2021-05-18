conf_data = configuration_data()
conf_data.set('share_dir', get_option('prefix') / get_option('datadir') / 'wonder')
configure_file(input : 'twonder@.service.in',
               output : 'twonder@.service',
               configuration : conf_data,
               install_dir : '/usr/lib/systemd/system')

conf_data2 = configuration_data()
conf_data2.set('bin_dir', get_option('prefix') / get_option('bindir'))
configure_file(input : 'cwonder@.service.in',
               output : 'cwonder@.service',
               configuration : conf_data2,
               install_dir : '/usr/lib/systemd/system')
