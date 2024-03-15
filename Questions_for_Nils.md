# Questions for Nils

1. Why are there system and user systemd services for twonder and cwonder? Which one should be used?
2. I believe we are using the user services for twonder and cwonder.
3. The jack.service file has After=sound.target local-fs.target, but these targets do not exist on the systems. Why are they there?
4. Where is the include directory path for the config.h file defined?
5. The config.h.in file is is not needed right?
6. All those scripts in the scripts directory are not needed right? Except for the start-twonder.sh script.
7. Where is the Dante pcie driver from?
8. Is pinging really needed for the twonder and cwonder services? And if not, cann't we just go without jfwonder?
9. Do you know what the twonder_config.xml file is for and the the Negative Delay in the cluster configuration?
10. Why shall we need jfwonder?
