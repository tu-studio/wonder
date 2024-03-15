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

## Clone Repository

Clone the wonder git repository in your chosen directory (e.g. Development). This creates
a subdirectory called wonder.

```bash
cd Development
git clone https://github.com/ntonnaett/wonder.git
```

## Setup

First the build directory must be set up. Change to the wonder directory
and set buildtype to release. You can change the build directory configuration
afterwards. 

```
cd wonder
meson --buildtype release builddir
```

## Configuration

There are several build options. Type `meson configure` in your build
directory to show all options.
The last options are project specific:

    Project options     Current Value         Possible Values           Description
    ---------------     -------------         ---------------           -----------
    jfwonder            true                  [true, false]             jfwonder
    location            none                  [none, EN325, HUFO]       Set location
    scoreplayer         true                  [true, false]             scoreplayer
    systemd             auto                  [enabled, disabled, auto] systemd
                                                                 
    xwonder             false                 [true, false]             xwonder

Installing the EN325 configuration can be set with `meson configure -Dlocation=EN325` for example.

## Build and install

Building and installing Wonder is done from the build directory. Meson has
multiple build backends, but _ninja_ is preferred.

```bash
ninja
sudo ninja install 
```

`ninja install` automatically builds changes. So running `sudo ninja install`
can change the owner of files in your build directory to root. Running
`ninja` beforehand mitigates this.

## Installation Locations

|Files|Location|
|---    |---    |
|Startup Scripts |*/usr/local/share/wonder/scripts*|
|XML DTD |*/usr/local/share/wonder/dtd*|
|Configuration files|*/usr/local/etc/wonder*|
