# **WONDER Lite**
#### A stripped down version of the *WONDER* suite (https://github.com/dvzrv/wonder)

## Content
This package contains the following components of WONDER:
- libwonder
- cwonder
- twonder
- xwonder (needs some work)

## Installation of WONDER under Ubuntu Linux
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

sudo apt-get install meson pkg-config
```

#### 4. Install WONDER dependencies (libraries and header files) of libxml++2.6, libjack2, liblo, freeglut and Qt5:
```
sudo apt-get install libxml++3.0 libxml++3.0-dev libjack-jackd2-0 libjack-jackd2-dev liblo7 liblo-dev freeglut3 freeglut3-dev qt5-default
```

### BUILD

```
meson --buildtype release builddir
cd builddir
meson compile
```

### INSTALLATION

```
sudo meson install 
```

### CONFIGURATION

|Files|Location|
|---    |---    |
|The start-up scripts of the current version are usually located in:|*/usr/share/wonder/scripts*|
|The configuration files are located in:|*/etc/wonder*|

##### Application configuration files:
**cwonder_config.xml:** Adjust projects path, number of sources and room geometry.

**twonder_config.xml:** Adjust speaker distance and focus radius.

##### Speaker configuration files:
**twonder_speakerarray.xml**

or

**speakers/n101** and **speakers/n102**

##### Node configuration files:
**cluster.conf:** Adjust username, IP address, JACK device, negative delay, etc.
**twonder_n101.conf:** Speaker array configuration of node 1 (n101).
**twonder_n102.conf:** Speaker array configuration of node 2 (n102).

#### Important:
**Make shure to modify the configuration files (especially cluster.conf) properly! Otherwise the start-up scripts might fail!**

## Work from HAW Hamburg
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


