# NXMP Changelog

## [0.8.2]

### Added

- Added initial i18n (partial)
- Error messages on network share connection failure

### Changes

- Update ImGui to last release
- Moved from SDL to GLFW for window context

### Fixed

- SubFont Color fix

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
