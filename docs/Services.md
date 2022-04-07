# SystemD Services

The startup of Wonder is handled by SystemD services. There are two possible
ways to do this. Because of JACK Wonder's services must be run as a specific user.

## User services (recommended)
The recommended way is to use user services. 


## Templated system services (alternative)

The system service solution is a templated service. Those services must be invoked
with the username:

```bash
sudo systemctl start cwonder@username
sudo systemctl stop cwonder@username
sudo systemctl enable cwonder@username
```


