# Build and Installation

## Install Dependencies

Wonder uses the [Meson build system](https://mesonbuild.com). You can either
install it via the system's package manager or with pip.
Wonder depends further on libxml++-3.0, JACK, liblo. XWonder depends on freeglut and Qt5.

=== "Ubuntu"

    ```bash
    sudo apt-get install build-essential meson pkg-config libxml++3.0 libxml++3.0-dev libjack-jackd2-0 libjack-jackd2-dev liblo7 liblo-dev qt5-default
    ```

=== "Fedora"

    ```bash
    sudo dnf install meson pkg-config libxml++3.0 libxml++3.0-dev liblo-dev qt5-default
    ```

Meson and Ninja can also be installed from pip, but it's necessary to install them system wide:

```bash
sudo pip install meson ninja
```
## Setup

First the build directory must be set up:
```
meson --buildtype release builddir
cd builddir
```

There are several build options:

    Project options     Current Value         Possible Values           Description
    ---------------     -------------         ---------------           -----------
    jfwonder            true                  [true, false]             jfwonder
    location            none                  [none, EN325, HUFO]       Set location
    scoreplayer         true                  [true, false]             scoreplayer
    systemd             auto                  [enabled, disabled, auto] systemd
                                                                 
    xwonder             false                 [true, false]             xwonder

Installing the EN325 configuration can be set with `meson -Dlocation=EN325` for example.

## Build and install

Building and installing Wonder is done from the build directory:

```bash
ninja
sudo ninja install 
```

## Installation Locations

|Files|Location|
|---    |---    |
|Startup Scripts |*/usr/share/wonder/scripts*|
|XML DTD |*/usr/share/wonder/dtd*|
|Configuration files|*/etc/wonder*|
