
```
meson --buildtype release builddir
cd builddir
meson compile
sudo meson install 
```


## Dependencies on Ubuntu Linux

Install compiler and further development tools:
```
sudo apt-get install build-essential
```

Install Meson build system:
```
sudo apt-get install meson pkg-config
```

Install WONDER dependencies (libraries and header files) of libxml++2.6, libjack2, liblo, freeglut and Qt5:
```
sudo apt-get install libxml++3.0 libxml++3.0-dev libjack-jackd2-0 libjack-jackd2-dev liblo7 liblo-dev qt5-default
```


