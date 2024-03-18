# Questions for Nils

1. Why are there system and user systemd services for twonder and cwonder? Which one should be used?
System services sind obsolet.
2. I believe we are using the user services for twonder and cwonder.
yes
3. The jack.service file has After=sound.target local-fs.target, but these targets do not exist on the systems. Why are they there?
Eventuell special targets for soundcards. I will check this.
4. Where is the include directory path for the config.h file defined?
weg
5. The config.h.in file is is not needed right?
yes
6. All those scripts in the scripts directory are not needed right? Except for the start-twonder.sh script.
yes
7. Where is the Dante pcie driver from?
install_low_latency_kernel.yml in the studio-ansible repository. Nils schickt neuen source code wenn er ihn findet.
8. Is pinging really needed for the twonder and cwonder services? And if not, cann't we just go without jfwonder?
Nils hatte es drin wegen restart und checken ob alle Renderer da sind.
Pings sind sinnvoll damit twonder weiß ob cwonder noch da ist und umgekehrt. Es muss nicht unbedingt über jfwonder gehen. Es kann auch asynchron sein.
9. Do you know what the twonder_config.xml file is for and the the Negative Delay in the cluster configuration?
This is the maximum position between a listener and a speaker. It is used as negativ delay in the cercle.
10. Seite ausschalten wenn Signal an einer Seite?
Früher war Listener dynamisch konnte sich bewegen. Jetzt ist er statisch beim Nullpunkt. Rendering total verbugt (Aussage von Nils).
11. Rendering Konzept
Rendering ist simples array von Lautsprecher. Im cercle ist es dann ebenso aber nur in der "richtigen Richtung".
