# NXMP Changelog
## [0.9.2]

### Added

- PDF file support

## Fixed

- fixed archive reading from USB
- playlist from local sdcard and USB drives
- settings language selection/save 

## [0.9.1]

### Added

- Added new network config syntax (fixes password with @ and add pubkey auth for sftp connections)
- UPNP (same code of 0.8.0 with minor fixes)
- HW Deinterlace
- File based translations (using json files, use the files on langs directory as reference)
- PKGBUILD for libnfs,libsmb2,sqlite3

## Fixed

- NFS entry duplication
- Software decoding (broken on 0.9.0 deko3d version)
- USB Browser fixed (no more duplicated devices)

# Removed

- Shadermania (now NXMP uses only deko3d, no more opengl)

### Changes

- Changed Rendering routine in a more sane way
- Now NXMP runs also in applet mode
- Updated all libs to last version
- Updated brasilian translation

## [0.9.0]

### Added

- Added Brazilian Portuguese language
- Added deko3d Backend capable of 4K HDR 60fps decoding (experimental)
- Light/Dark Theme (mimic the native HOS UI)
- Added a trivial image viewer instead of using mpv (feature zoom/pan)
- Added ZL/ZR to show prev/next image
- Added Archive browsing support using libarchive as a devoptab mount
- Added M3U parser as a devoptab mount

### Changes

- samba,nfs,sftp,ftp,libarchive are now implemented as devoptabs mounts

# Removed

- UPNP is disabled since need major rewrite of code
- Removed old themes class (will be replaced with new one when ready)
- Removed old ttf font used (now it uses the system default font)

## Fixed

- Ghosting was mitigated but still present in 0.8.2 now is fixed forever

### Knonw Issues

- deko3d Backend is experimental and can have issues or missing some features
- Shader deinterlace is not working on deko3d backend
- Shadermania is not working on deko3d backend
- Miniplayer is not working on deko3dbackend


## [0.8.2]

### Added

- Added initial i18n (More than 50% of text is i18n ready, be patient soon will be 100%)
- Italian-Spanish-Japanese-Deutsch-French-Chinese(Simplified)-Korean native translation
- Default file ordering can be changed in Settings menu
- Error messages on network share connection failure
- Loading splash screen
- Exit Choice between HB-Menu and Home

### Changes

- Update ImGui to last release
- Moved from SDL to GLFW for window context
- Theme ini structure changes
- SQLite vfs using native calls

### Fixed

- Video Ghosting Fixed
- SubFont Color fix
- Database Open/Close Fix
- Theme support is back

### Knonw Issues

- NXMP start loading times increased due to importing more font glyphs

### Release Notes

The config.ini file now has a config line for setting db location.
It will point to default location /switch/nxmp/nxmp.db

```
dbpath = /switch/nxmp/nxmp.db
```
You can set to any location

Special thanks goes to Djakku and Jorge E. García "nWo" for helping me a lot for this release


## [0.8.1]

### Fixed

- Audio File playback crash fixed
- Fixed a typo error in popups


## [0.8.0]

### Added

- ISO DVD VIDEO Support (both DVD and HD-DVD) (it will play the longest track as default)
- GUI Updated (Now show file date on FS that support it)
- Audren Audio Output (SDL still the default if not changed in settings)
- Media Probe (for Playlist duration parsing and in future for other cool things)
- Mini Player (while browsing NXMP contents)
- Battery % and Clock HH::MM on Menu
- Context Menu (For File Browsing and Playlist Menu)
- Sorting Files (Name,Date,Size)
- User extension list in config file (you can filter the file extensions you want)
- Disabled start if in applet mode (We need full RAM Access)
- Button description at the bottom of the screen for relevant functions


### Changes

- Config file for Network section changed (see README.md for details)
- Updated SQLite from 3.7.3 to 3.43.1 

### Fixed

- VP9 HW Decoder Fix
- MPEG2 Video now default to HW Decoding
- Layout on dock mode
- Small fixes in code

### Knonw Issues

- Tons of changes can lead to tons of bugs...
- I know code is a mess but works (i am slowly trying to make it better)

### Release Info

- Since the ini parser changed is recommended to start with a fresh ini
- The network share syntax changed see README
