# **WONDER Lite**
#### A stripped down version of the *WONDER* suite (https://github.com/dvzrv/wonder)

### Content
This package contains the following components of WONDER:
- libwonder
- cwonder
- twonder
- xwonder

The speaker configuration files and start-up scripts are for the I²AudioLab (https://i2audiolab.de) at the HAW Hamburg (https://haw-hamburg.de).

Several bugfixes and changes have been applied to the code base of WONDER version 3.1.9 at the HAW.
For a better portability the WONDER logging functionalities have been removed.
Platform dependend code parts (suchs as POSIX *sleep*) have been replaced by platform independant versions from the C++ STL.
The Qt4 dependencies of xwonder have been updated to Qt5.

The code can be compiled on Linux and Windows.
**Linux:** Tested with Ubuntu 17.10.
**Windows:** Tested with Windows 10 64-bit, MSYS2 and MinGW-w64 GNU compiler toolchain 64.

The SConstruct script for the compilation has been edited and compiles xwonder with Qt5 instead of Qt4 dependencies.
**Note:** A compilation of xwonder with the SConstruct script on a Windows platforms is not implemented yet.

---

## Installation of WONDER under Ubuntu Linux 17.10
### PREPARATION
#### 1. Update the system, install the low-latency kernel and reboot:
```
sudo apt-get update
sudo apt-get dist-upgrade
sudo apt-get install linux-lowlatency
sudo reboot
```

#### 2. Install a compiler and further development tools:
```
sudo apt-get install build-essential
```

#### 3. Install SCons (software construction tool) and pkg-config to build WONDER:
```
sudo apt-get install scons pkg-config
```

#### 4. Install WONDER dependencies (libraries and header files) of libxml++2.6, libjack2, liblo, freeglut and Qt5:
```
sudo apt-get install libxml++2.6-2v5 libxml++2.6-dev libjack-jackd2-0 libjack-jackd2-dev liblo7 liblo-dev freeglut3 freeglut3-dev qt5-default
```

#### 5. [OPTIONAL] Install and adjust Qt5 theming styles to let Qt apps look more native under GTK:
```
sudo apt-get install qt5-style-plugins qt5-gtk-platformtheme
```

Open the file */etc/environment*:
```
sudo nano /etc/environment
```

Append the following new line at the bottom of the file:
```
export QT_QPA_PLATFORMTHEME=gtk2
```

Press **Ctrl+O** to save and **Ctrl+X** to exit the editor and reboot the system:
```
sudo reboot
```

Now xwonder should look like a native GTK application.


### INSTALLATION

#### 1. Create the destination folder:
```
sudo mkdir -p /opt/wonder/wonderWinter
```

#### 2. Open a terminal inside the WONDER folder containing the ***SConstruct*** file. Make shure to delete old builds in the ***bin*** and ***obj*** folders.

#### 3. Clean:
```
scons -c all=1
```

#### 4. Build:
```
scons all=1 installto=/opt/wonder/wonderWinter
```

#### 5. Install:
```
sudo scons install all=1 installto=/opt/wonder/wonderWinter
```

#### 6. Make the scripts executable:
```
find /opt/wonder/wonderWinter/scripts -type f -name "*.sh" -exec sudo chmod ugo+x -- {} +
```


#### 7. Create a symlink from *wonderWinter* to *current*. This way multiple builds are possible and the most recent one can always be found at */opt/wonder/current*:
```
cd /opt/wonder
sudo unlink current
sudo ln -s wonderWinter/ current
```

#### 8. Adjust the configuration files to your needs:
|Files|Location|
|---    |---    |
|Applications (cwonder, twonder, xwonder) are located in:|*/opt/wonder/current/bin*|
|The start-up scripts of the current version are located in:|*/opt/wonder/current/scripts*|
|The configuration files are located in:|*/opt/wonder/current/configs*|

##### Application configuration files:
**/opt/wonder/current/configs/cwonder_config.xml:** Adjust projects path, number of sources and room geometry.
**/opt/wonder/current/configs/twonder_config.xml:** Adjust speaker distance and focus radius.

##### Speaker configuration files:
**/opt/wonder/current/configs/twonder_speakerarray.xml**
or
**/opt/wonder/current/configs/speakers/n101** and **/opt/wonder/current/configs/speakers/n102**

##### Node configuration files:
**/opt/wonder/current/scripts/configs/cluster.conf:** Adjust username, IP address, JACK device, negative delay, etc.
**/opt/wonder/current/scripts/configs/twonder_n101.conf:** Speaker array configuration of node 1 (n101).
**/opt/wonder/current/scripts/configs/twonder_n102.conf:** Speaker array configuration of node 2 (n102).

#### Important:
**Make shure to modify the configuration files (especially cluster.conf) properly! Otherwise the start-up scripts might fail!**
