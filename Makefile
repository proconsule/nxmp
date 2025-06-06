#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------

ifeq ($(strip $(DEVKITPRO)),)
$(error "Please set DEVKITPRO in your environment. export DEVKITPRO=<path to>/devkitpro")
endif

TOPDIR ?= $(CURDIR)
include $(DEVKITPRO)/libnx/switch_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# DATA is a list of directories containing data files
# INCLUDES is a list of directories containing header files
# ROMFS is the directory containing data to be added to RomFS, relative to the Makefile (Optional)
#
# NO_ICON: if set to anything, do not use icon.
# NO_NACP: if set to anything, no .nacp file is generated.
# APP_TITLE is the name of the app stored in the .nacp file (Optional)
# APP_AUTHOR is the author of the app stored in the .nacp file (Optional)
# APP_VERSION is the version of the app stored in the .nacp file (Optional)
# APP_TITLEID is the titleID of the app stored in the .nacp file (Optional)
# ICON is the filename of the icon (.jpg), relative to the project folder.
#   If not set, it attempts to use one of the following (in this order):
#     - <Project name>.jpg
#     - icon.jpg
#     - <libnx folder>/default_icon.jpg
#
# CONFIG_JSON is the filename of the NPDM config file (.json), relative to the project folder.
#   If not set, it attempts to use one of the following (in this order):
#     - <Project name>.json
#     - config.json
#   If a JSON file is provided or autodetected, an ExeFS PFS0 (.nsp) is built instead
#   of a homebrew executable (.nro). This is intended to be used for sysmodules.
#   NACP building is skipped as well.
#---------------------------------------------------------------------------------
TARGET		:=	$(notdir $(CURDIR))
BUILD		:=	build
SOURCES		:=	source/dvdnav source/pdfloader source/i18n libs/nxmp-devoptabs/devoptabsfs/nfsfs/ libs/nxmp-devoptabs/devoptabsfs/smb2fs libs/nxmp-devoptabs/devoptabsfs/sshfs libs/nxmp-devoptabs/devoptabsfs/m3u8fs libs/nxmp-devoptabs/devoptabsfs/ftpfs libs/nxmp-devoptabs/devoptabsfs/libarchivefs source/backends/native-deko3d source/backends source/iniparser source/imgloader source/networkshareclass source/UI/textscroller source/mediaprobe libs/imgui_toggle source/fileBrowser source/logger source/stats libs/imgui libs/imgui/misc/freetype source source/updater source/curldownloader source/touchcontrols source/playlist source/shadermania source/eqpreset source/database source/UI source/remotefs/UPNP source/remotefs/nfsDir source/remotefs/smb2 source/remotefs/sshDir source/remotefs/Enigma2 source/localfs source/localfs/usb source/remotefs/ftplib source/remotefs/HTTPDir  source/libmpv 
DATA		:=	data
INCLUDES	:=	source/dvdnav source/pdfloader libs/nxmp-devoptabs/devoptabsfs/nfsfs/ libs/nxmp-devoptabs/devoptabsfs/smb2fs libs/nxmp-devoptabs/devoptabsfs/sshfs libs/nxmp-devoptabs/devoptabsfs/m3u8fs libs/nxmp-devoptabs/devoptabsfs/ftpfs libs/nxmp-devoptabs/devoptabsfs/libarchivefs source/backends/native-deko3d source/i18n source/backends source/iniparser source/imgloader source/networkshareclass source/UI/textscroller source/mediaprobe libs/imgui_toggle source/fileBrowser source/logger source/stats libs/simpleini libs/imgui include source/curldownloader source/updater source/touchcontrols source/playlist source/shadermania source/eqpreset source/database source/remotefs/UPNP source/remotefs/nfsDir source/remotefs/smb2 source/remotefs/sshDir source/remotefs/Enigma2 source/localfs source/localfs/usb source/remotefs/ftplib source/remotefs/HTTPDir source/libmpv
ROMFS		:=	romfs

OUT_SHADERS	:=	shaders

GITREV:= -D'GITREV="$(shell git rev-parse --short HEAD)"'

VERSION_MAJOR := 0
VERSION_MINOR := 9
VERSION_MICRO := 2

APP_TITLE     := NXMP
APP_AUTHOR    := proconsule and darkxex
APP_VERSION   := ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_MICRO}


#---------------------------------------------------------------------------------	
# options for code generation
#---------------------------------------------------------------------------------
ARCH	:=	-march=armv8-a+crc+crypto -mtune=cortex-a57 -mtp=soft -fPIE 

CFLAGS	:=	-g -ggdb -Wall -Wno-sign-compare -O2 -ffunction-sections -DCXXOPTS_NO_RTTI\
			$(ARCH) $(DEFINES) \
                $(GITREV)
CFLAGS  +=  `freetype-config --cflags` -I${PORTLIBS}/include/upnp/

CFLAGS	+=	$(INCLUDE) -D__SWITCH__ -DDEKO3D_BACKEND=1

CFLAGS  +=      -DVERSION_MAJOR=$(VERSION_MAJOR) -DVERSION_MINOR=$(VERSION_MINOR) -DVERSION_MICRO=$(VERSION_MICRO) 

CXXFLAGS	:= $(CFLAGS) -std=gnu++20 -fno-rtti -fexceptions -fpermissive -DIMGUI_IMPL_OPENGL_LOADER_CUSTOM \
		  -DIMGUI_IMPL_OPENGL_LOADER_CUSTOM 

ASFLAGS	:=	-g $(ARCH) 
LDFLAGS	=	-specs=$(DEVKITPRO)/libnx/switch.specs -g $(ARCH) -Wl,-Map,$(notdir $*.map)


LIBS	:=  -lmpv `curl-config --libs` `freetype-config --libs` -ldvdnav -ldvdread  `aarch64-none-elf-pkg-config --libs libavformat libavcodec libavutil libavfilter libswscale libpostproc` -llzma -lopus -lvpx -lass -lharfbuzz -lfreetype -lfribidi -lmupdf -lmupdf-third -lfreetype  -lstdc++ -ldav1d -lpng -lbz2 -ltinyxml2 -lturbojpeg -llua5.1 -lssh2 -lmbedcrypto -lmbedx509 -lmbedtls -lmbedcrypto -lmbedx509 -lmbedtls -lsqlite3 -lsmb2  -lnfs -ljansson -lusbhsfs -lntfs-3g -llwext4 -lgme -lopenmpt -ldeko3d -luam -larchive -lexpat -llzma -lzstd -llz4 -lbz2 -lz -lnx -lc -lz



#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= $(PORTLIBS) $(LIBNX)


#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)
export TOPDIR	:=	$(CURDIR)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
			$(foreach dir,$(DATA),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp))) 
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
GLSLFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.glsl)))
BINFILES	:=	$(foreach dir,$(DATA),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
#---------------------------------------------------------------------------------
	export LD	:=	$(CC)
#---------------------------------------------------------------------------------
else
#---------------------------------------------------------------------------------
	export LD	:=	$(CXX)
#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------

export OFILES_BIN	:=	$(addsuffix .o,$(BINFILES))
export OFILES_SRC	:=	$(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(SFILES:.s=.o)
export OFILES 	:=	$(OFILES_BIN) $(OFILES_SRC)
export HFILES_BIN	:=	$(addsuffix .h,$(subst .,_,$(BINFILES)))

ifneq ($(strip $(ROMFS)),)
	ROMFS_TARGETS :=
	ROMFS_FOLDERS :=
	ifneq ($(strip $(OUT_SHADERS)),)
		ROMFS_SHADERS := $(ROMFS)/$(OUT_SHADERS)
		ROMFS_TARGETS += $(patsubst %.glsl, $(ROMFS_SHADERS)/%.dksh, $(GLSLFILES))
		ROMFS_FOLDERS += $(ROMFS_SHADERS)
	endif

	export ROMFS_DEPS := $(foreach file,$(ROMFS_TARGETS),$(CURDIR)/$(file))
endif

export INCLUDE	:=	$(foreach dir,$(INCLUDES),-I$(CURDIR)/$(dir)) \
			$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
			-I$(CURDIR)/$(BUILD)

export LIBPATHS	:=	$(foreach dir,$(LIBDIRS),-L$(dir)/lib)

ifeq ($(strip $(CONFIG_JSON)),)
	jsons := $(wildcard *.json)
	ifneq (,$(findstring $(TARGET).json,$(jsons)))
		export APP_JSON := $(TOPDIR)/$(TARGET).json
	else
		ifneq (,$(findstring config.json,$(jsons)))
			export APP_JSON := $(TOPDIR)/config.json
		endif
	endif
else
	export APP_JSON := $(TOPDIR)/$(CONFIG_JSON)
endif

ifeq ($(strip $(ICON)),)
	icons := $(wildcard *.jpg)
	ifneq (,$(findstring $(TARGET).jpg,$(icons)))
		export APP_ICON := $(TOPDIR)/$(TARGET).jpg
	else
		ifneq (,$(findstring icon.jpg,$(icons)))
			export APP_ICON := $(TOPDIR)/icon.jpg
		endif
	endif
else
	export APP_ICON := $(TOPDIR)/$(ICON)
endif

ifeq ($(strip $(NO_ICON)),)
	export NROFLAGS += --icon=$(APP_ICON)
endif

ifeq ($(strip $(NO_NACP)),)
	export NROFLAGS += --nacp=$(CURDIR)/$(TARGET).nacp
endif

ifneq ($(APP_TITLEID),)
	export NACPFLAGS += --titleid=$(APP_TITLEID)
endif

ifneq ($(ROMFS),)
	export NROFLAGS += --romfsdir=$(CURDIR)/$(ROMFS)
endif

.PHONY: $(BUILD) clean all

#---------------------------------------------------------------------------------

all: $(BUILD)

$(BUILD): $(ROMFS_TARGETS)
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) --no-print-directory -C $(BUILD) -f $(CURDIR)/$(firstword $(MAKEFILE_LIST)) 

$(ROMFS_TARGETS): | $(ROMFS_FOLDERS)

$(ROMFS_FOLDERS):
	@mkdir -p $@

$(ROMFS_SHADERS)/%_vsh.dksh: %_vsh.glsl
	@echo {vert} $(notdir $<)
	@uam -s vert -o $@ $<

$(ROMFS_SHADERS)/%_tcsh.dksh: %_tcsh.glsl
	@echo {tess_ctrl} $(notdir $<)
	@uam -s tess_ctrl -o $@ $<

$(ROMFS_SHADERS)/%_tesh.dksh: %_tesh.glsl
	@echo {tess_eval} $(notdir $<)
	@uam -s tess_eval -o $@ $<

$(ROMFS_SHADERS)/%_gsh.dksh: %_gsh.glsl
	@echo {geom} $(notdir $<)
	@uam -s geom -o $@ $<

$(ROMFS_SHADERS)/%_fsh.dksh: %_fsh.glsl
	@echo {frag} $(notdir $<)
	@uam -s frag -o $@ $<

$(ROMFS_SHADERS)/%.dksh: %.glsl
	@echo {comp} $(notdir $<)
	@uam -s comp -o $@ $<	

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
ifeq ($(strip $(APP_JSON)),)
	@rm -fr $(BUILD) $(TARGET).nro $(TARGET).nacp $(TARGET).elf
else
	@rm -fr $(BUILD) $(TARGET).nsp $(TARGET).nso $(TARGET).npdm $(TARGET).elf
endif


#---------------------------------------------------------------------------------
else
.PHONY:	all

DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
ifeq ($(strip $(APP_JSON)),)


all	:	$(OUTPUT).nro

ifeq ($(strip $(NO_NACP)),)
$(OUTPUT).nro	:	$(OUTPUT).elf $(OUTPUT).nacp
else
$(OUTPUT).nro	:	$(OUTPUT).elf
endif

else

all	:	$(OUTPUT).nsp

$(OUTPUT).nsp	:	$(OUTPUT).nso $(OUTPUT).npdm

$(OUTPUT).nso	:	$(OUTPUT).elf

endif

$(OUTPUT).elf	:	$(OFILES)

$(OFILES_SRC)	: $(HFILES_BIN)

#---------------------------------------------------------------------------------
# you need a rule like this for each extension you use as binary data
#---------------------------------------------------------------------------------
%.bin.o	%_bin.h :	%.bin
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	@$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------------

