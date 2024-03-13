# The speaker configurations need to have the same folder structure for the twonder start script to work.
# Also the speaker configurations need to start with twonder.

if get_option('node') == 'none'
    error('Please specify location with -Dnode=...')
elif get_option('node') == 'riviera'
    message('Installing configuration for node riviera.')
    install_subdir('cwonder', install_dir : get_option('sysconfdir') / 'wonder')
    install_subdir('twonder/riviera', strip_directory : true, install_dir : get_option('sysconfdir') / 'wonder' / 'twonder')
elif get_option('node') == 'wintermute'
    message('Installing configuration for node wintermute.')
    install_subdir('twonder/wintermute', strip_directory : true, install_dir : get_option('sysconfdir') / 'wonder' / 'twonder')
else
    error('Unknown node specified.')
endif

# Install the remaining files if the node is correct, no error is thrown.
install_data('cluster.conf', install_dir : get_option('sysconfdir') / 'wonder')
install_data('twonder/twonder_config.xml', install_dir : get_option('sysconfdir') / 'wonder' / 'twonder')