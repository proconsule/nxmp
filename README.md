NXMP
======

![Banner](docs/banner.jpg)
NXMP is a video player for Nintendo Switch based on [MPV](https://mpv.io/).
NXMP support most popular video format , it support HTTP and FTP streaming and it also support Enigma2 decoders.

Installation 
----
Edit config.ini
```
[Main]

[Network]

[Enigma2]
```

Netowrok have syntax

```
[Network]
source=TESTFtp|ftp://user:pass@192.168.1.10:21/path/
source=TestHTTP|http://192.168.1.10/path/
```

Eningma2 have syntax
```
[Enigma2]
e2address=http://192.168.1.10/
```

Copy "nxmp" folder to switch sdcard (/switch/nxmp)

Usage
-----
Buttons Mapping
- A select (play/pause during playback)
- B back (stop during playback)
- Y Main Menu
- X show OSD (during playback)
- R L ZR ZL (seek +/-  during playback)
- + Exit NXMP
- R Stick Button Toggle Masterlock (during playback, only A button will work)


FAQ
-----
**Question:** My 4K file dont work? Why?

**Answer** Keep in mind that currently Homebrew apps can't access the HW Decoder, so all is done with CPU, 4K file is too heavy to be decoded

Thanks to
-----
- bodyXY @ GBATemp forum for banner and icons
- docgold @ GBATemp forum for Enigma2 samples and support on decoders
- tataniko @ GBATemp forum for suggestions and bug hunting
- All guys on GBAtemp
