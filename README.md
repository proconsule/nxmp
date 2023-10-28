NXMP
======

![Banner](docs/banner.jpg)
NXMP is a video player for Nintendo Switch based on [MPV](https://mpv.io/).
NXMP support most popular video format , it support HTTP,FTP,SFTP,Samba,NFS streaming and it also support Enigma2 decoders.
Starting from 0.7.0 Version it features HW Decoding for H264,HEVC,VP8,VP9,MPEG2 up to 4K HDR@30fps

NXMP is coded by proconsule and darkxex

Installation 
----
Edit config.ini
```
[Main]
startpath=/switch/nxmp

[Network]

[Enigma2]
```

Network have syntax (you can add as many source as you like) up to version 0.7.0

```
Network]
source=TESTFtp|ftp://user:pass@192.168.1.10:21/path/
source=TestHTTP|http://192.168.1.10/path/
source=TestSFTp|sftp://192.168.1.10/path/ (without path it default to home)
source=TestSamba|smb://user:pass@192.168.1.10/share/ (share is a share name not a path)
source=TestNFS|nfs://192.168.1.10/export/
```

Network have syntax (you can add as many source as you like) from version 0.8.0

```
[Network]
TESTFtp = ftp://user:pass@192.168.1.10:21/path/
TestHTTP = http://192.168.1.10/path/
TestSFTp = sftp://user:pass@192.168.1.10/path/ (without path it default to home)
TestSamba = smb://user:pass@192.168.1.10/share/ (share is a share name not a path)
TestNFS = nfs://192.168.1.10/export/
```

Enigma2 have syntax
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
- \- Exit NXMP
- R Stick Button Toggle Masterlock (during playback, only A button will work)
- Dpad Right (during playback show/hide right menu)
- Dpad Down (during playback show/hide player UI)
- R Stick Up/Down Fast Scroll on file list (Volume Control During Playback)
- \+ Show file selection during playback


FAQ
-----
**Question:** My 4K file dont work? Why?

**Answer** ~~Keep in mind that currently Homebrew apps can't access the HW Decoder, so all is done with CPU, 4K file is too heavy to be decoded~~ **From version 0.7.0 NXMP have HW Decoding for format H264,HEVC,VP9,MPEG2 it can decode up to 4K HDR@30fps**


**Q:** What is Enigma2?

**A:** Enigma2 is an application used in Linux-based Digital Video Broadcasting (DVB-S, DVB-C, DVB-T) receivers or TV set-top boxes and Internet Protocol television receivers.

**Q:** What is ShaderMania?

**A:** ShaderMania is an useless but fun feature of NXMP, it uses the ability of MPV to load custom shader into the video output chain. I adopt some shaders from https://www.shadertoy.com/ to make shaders effects to the video. Some like grayscale may be usefull for someone others (like the sea shaders) is only for fun. Users can also make their custom shaders and place in mpv/shaders directory.


Translations
-----

If you want to have NXMP translated to your native language use this
https://github.com/proconsule/nxmp/blob/master/source/i18n/nxmp-en.h
as reference,create a file for your lang and send it to me.

- Spanish thanks to Jorge E. García "nWo"
- French thanks to Zoria, Pharuxtan, Gum
- Deutsch thanks to Extreme
- Italian thanks to proconsule 
- Japanese thanks to mossan0101
- Chinese thanks to dragonflybee
- Korean thanks to Asadayot




Thanks to
-----
- Cpasjuste for pPlay https://github.com/Cpasjuste/pplay some code was taken here (mpv and Overclock part)
- DarkMatterCore for libusbhsfs https://github.com/DarkMatterCore/libusbhsfs (this gives NXMP USB support)
- Averne for HW Decoding on Tegra X1 https://github.com/averne/FFmpeg and for all the hints and help (thanks a lot!)
- xfangfang for https://github.com/xfangfang/wiliwili (for Audren audio mpv patch)
- Jorge E. García "nWo" for beta testing and suggestions
- Djakku @ GBATemp for beta testing and suggestions
- spkatsi @ GBATemp for splash screen, themes and more
- Chocola @ GBATemp forum for usage guide and beta testing
- sashka69 @ GBATemp forum for beta testing
- bodyXY @ GBATemp forum for banner and icons
- Godorowski @ GBATemp forum for player UI design and icons
- docgold @ GBATemp forum for Enigma2 samples and support on decoders
- tataniko @ GBATemp forum for suggestions and bug hunting
- All guys on GBAtemp
