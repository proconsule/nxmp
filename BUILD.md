# NXMP BUILD 

Building NXMP is not that hard (but not that easy also)

first you need a working devkitPro with switch-dev installed

(if you are lost at this point..... don't go further)

you have to install this packages

switch-curl, switch-libass, switch-libvpx, switch-glm, switch-libpng, switch-ntfs-3g, switch-libfribidi, switch-bzip2, switch-libarchive, switch-mbedtls, switch-pkg-config, switch-harfbuzz, switch-libjpeg-turbo, switch-dav1d, switch-freetype, switch-libwebp, switch-libssh2, switch-lwext4


https://github.com/DarkMatterCore/libusbhsfs
Compile and build libusbhsfs


build and install libsmb2,libnfs,sqlite3 from libs directory (with dkp-makepkg)

https://github.com/averne/FFmpeg/tree/nvtegra
build averne FFMpeg with this

```
source /opt/devkitpro/switchvars.sh
./configure --prefix=$PORTLIBS_PREFIX --enable-gpl --disable-shared --enable-static \
    --cross-prefix=aarch64-none-elf- --enable-cross-compile \
    --arch=aarch64 --cpu=cortex-a57 --target-os=horizon --enable-pic \
    --extra-cflags='-D__SWITCH__ -D_GNU_SOURCE -D_POSIX_VERSION=200809L -Dtimegm=mktime -O2 -march=armv8-a -mtune=cortex-a57 -mtp=soft -fPIC -ftls-model=local-exec' \
    --extra-cxxflags='-D__SWITCH__ -D_GNU_SOURCE -D_POSIX_VERSION=200809L -Dtimegm=mktime -O2 -march=armv8-a -mtune=cortex-a57 -mtp=soft -fPIC -ftls-model=local-exec' \
    --extra-ldflags='-fPIE -L${PORTLIBS_PREFIX}/lib -L${DEVKITPRO}/libnx/lib' \
    --disable-runtime-cpudetect --disable-programs --disable-debug --disable-doc --disable-autodetect \
    --enable-asm --enable-neon \
    --disable-avdevice --disable-encoders --disable-muxers \
    --enable-swscale --enable-swresample --enable-network  \
    --disable-protocols \
    --enable-protocol='file,crypto,http,ftp,tcp,udp,rtmp,hls,https,tls' --enable-demuxer=hls \
    --enable-zlib --enable-bzlib --enable-libass --enable-libvpx --enable-libfreetype --enable-libfribidi --enable-libdav1d --enable-libwebp \
    --enable-nvtegra --enable-mbedtls --enable-version3

make -j10  
make install
```

https://github.com/averne/libuam
build averne libuam 



https://github.com/averne/mpv
build averne mpv with

```
./bootstrap.py
CFLAGS="$CFLAGS -D_POSIX_VERSION=200809L -I./osdep/switch" TARGET=aarch64-none-elf ./waf configure --prefix=/opt/devkitpro/portlibs/switch --enable-libmpv-static --disable-libmpv-shared --disable-manpage-build \
                            --disable-cplayer --disable-iconv --disable-lua \
                                                        --disable-sdl2 --disable-gl --disable-plain-gl --enable-hos-audio --enable-deko3d
        sed -i 's/#define HAVE_POSIX 1/#define HAVE_POSIX 0/' build/config.h

./waf install
```

Now on the top NXMP dir 
make

(it should produce a working nxmp.nro)


 