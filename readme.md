**CogWheel FTP Server**
***

CogWheel is a Qt based personal FTP server initially built and run on Linux although it should work on other Qt based target platforms. It supports all of the [ rfc 959](https://tools.ietf.org/html/rfc959)  based commands but is not strictly standard compliant in that it only performs stream tranfers and ignores ASCII mode amongst some of its divergences.

At present it allows multiple plain or  TLS (explicit) FTP connections  with each connection running in its own separate thread; the data channel can also be encrypted using TLS with support for both the PROT and PBSZ extended commands. 

The server comes with a companion program **CogWheelManger**  that can be used to modify server based parameters and add/remove users and their related information (password, root directory etc). The Manager program also has the ability to start/stop the server and also kill/launch the server process. Logging is also provided in the form of a window that can display redirected server logging. Logging to a specified file is also provided along with being able to set the logging level via the server settings in the config file (no UI is currently provided for the latter two).

Also only one instance of the server and manager me be run at a time with a new invocation of the manager bringing the window of the currently running manager to the foregroud.

**To Do List**
***
- Try out on other Qt platforms (Windows/MasOS).
 