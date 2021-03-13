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


