# SystemD Services

The startup of Wonder is handled by SystemD services. There are two possible
ways to do this. Because of JACK Wonder's services must be run as a specific user.

## User services (recommended)

The recommended way is to use user services. CWonder, TWonder and JFWonder have
a service unit installed that can be started, stopped, enabled and disabled.

```bash
systemctl --user start cwonder.service
systemctl --user stop twonder.service
systemctl --user disable jfwonder.service
systemctl --user enable twonder.service
```

## Templated system services (alternative)

The system service solution is a templated service. Those services must be invoked
with the username:

```bash
sudo systemctl start cwonder@username
sudo systemctl stop cwonder@username
sudo systemctl enable cwonder@username
```

## Status and Logs

The current status of a systemd service can be obtained by:

```bash
systemctl --user status cwonder.service
```

If more log history is wanted, journalctl can be used:

```bash
journalctl --user -xeu cwonder.service
``` 

### Startup order

Originally Wonder depended on the startup order of its components. A lot of work
was invested getting Wonder more resilient to restarts of parts of the system.
TWonder can be restarted without the need of restarting CWonder. When CWonder
gets restarted it tries to reconnect all TWonder nodes from the last run.

The startup order of CWonder and JFWonder should look as follows:

1. JACK server
2. CWonder
    - a possible error is that you have cwonder already running and you can not start another osc server using the same port
3. JFWonder
	- will try and connect to the jack server 
		- a possible error is that the jackd is not running or as a differnt user and you do not have the priviligies to connect to 
	- will try to connect to cwonder
		- a possible error is that cwonder is not running, jfwonder will then wait for a second and retry to connect. After a certain number of retries (100) without beeing able to connect jfwonder will exit.

If CWonder gets restarted, JFwonder needs to be restarted as well in order to get the timing information.

