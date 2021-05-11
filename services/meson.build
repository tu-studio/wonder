conf_data = configuration_data()
conf_data.set('share_dir', get_option('prefix') / get_option('datadir') / 'wonder')
configure_file(input : 'twonder@.service.in',
               output : 'twonder@.service',
               configuration : conf_data,
               install_dir : '/usr/lib/systemd/system')

install_data('cwonder@.service', install_dir: '/usr/lib/systemd/system')
