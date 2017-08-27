**CogWheel FTP Server**
***

CogWheel is a Qt based personal FTP server initially built and run on Linux and should work on other Qt based target platforms. It supports all of the [ rfc 959](https://tools.ietf.org/html/rfc959)  based commands but is not strictly standard compliant in that it only supports stream tranfers and ignores ASCII mode amongst some of its divergences.

At present it supports muiltiple plain or  TLS (explicit) FTP connections  with each connection running in its own separate thread. It has a companion program CogWheelManger (yet to be released) that can be used to modify server based parameters and add/remove users and their related information (password, root directory etc).

The data channel can also be encrypted using TLS with support for PROT and PBSZ commands.


**To Do List**
***
- FTP commands MLSx.
- Release CogWheel Manager.
- Try out on other Qt platforms (Windows/MasOS).
- Add centralised logging class.
- Setting Server Private Key/Certicate file using CogWheel Manager.