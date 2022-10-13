#---------------------------------------------------------------------------------
# Clear the implicit built in rules
#---------------------------------------------------------------------------------
.SUFFIXES:
#---------------------------------------------------------------------------------
ifeq ($(strip $(DEVKITPPC)),)
$(error "Please set DEVKITPPC in your environment. export DEVKITPPC=<path to>devkitPPC")
endif

include $(DEVKITPPC)/wii_rules

#---------------------------------------------------------------------------------
# TARGET is the name of the output
# BUILD is the directory where object files & intermediate files will be placed
# SOURCES is a list of directories containing source code
# INCLUDES is a list of directories containing extra header files
#---------------------------------------------------------------------------------
TARGET		:=	$(notdir $(CURDIR))
BUILD		:=	build
SOURCES		:=	src/core src source source/gfx
RESOURCES	:=	res/textures res/meshes
ASSETS		:=	assets
SD_ASSET_PATH	:=	sd/apps/darts/assets
INCLUDES	:=
#$(shell python ./build-tools/buildStaticTextures.py)
#$(shell ./scripts/MeshBuilder.exe)
#---------------------------------------------------------------------------------
# options for code generation
#---------------------------------------------------------------------------------

CFLAGS	= -g -O2 -Wall $(MACHDEP) $(INCLUDE)
CXXFLAGS	=	$(CFLAGS)

#LDFLAGS	=	-g $(MACHDEP) -Wl,-Map,$(notdir $@).map
LDFLAGS = -g $(MACHDEP) -Wl,-Map,$(notdir $@).map -Wl,--section-start,.init=0x81000000

#---------------------------------------------------------------------------------
# any extra libraries we wish to link with the project
#---------------------------------------------------------------------------------
LIBS	:=	-lgrrlib -lfreetype -lbz2 -lpngu -lpng -ljpeg -lz -lfat -lwiiuse -lbte -lasnd -logc -lm 

#---------------------------------------------------------------------------------
# list of directories containing libraries, this must be the top level containing
# include and lib
#---------------------------------------------------------------------------------
LIBDIRS	:= $(CURDIR)/$(GRRLIB) $(PORTLIBS)

#---------------------------------------------------------------------------------
# no real need to edit anything past this point unless you need to add additional
# rules for different file extensions
#---------------------------------------------------------------------------------
ifneq ($(BUILD),$(notdir $(CURDIR)))
#---------------------------------------------------------------------------------

export OUTPUT	:=	$(CURDIR)/$(TARGET)

export VPATH	:=	$(foreach dir,$(SOURCES),$(CURDIR)/$(dir)) \
					$(foreach dir,$(RESOURCES),$(CURDIR)/$(dir))

export DEPSDIR	:=	$(CURDIR)/$(BUILD)

#---------------------------------------------------------------------------------
# automatically build a list of object files for our project
#---------------------------------------------------------------------------------
CFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.c)))
CPPFILES	:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.cpp)))
sFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.s)))
SFILES		:=	$(foreach dir,$(SOURCES),$(notdir $(wildcard $(dir)/*.S)))
BINFILES	:=	$(foreach dir,$(RESOURCES),$(notdir $(wildcard $(dir)/*.*)))

#---------------------------------------------------------------------------------
# use CXX for linking C++ projects, CC for standard C
#---------------------------------------------------------------------------------
ifeq ($(strip $(CPPFILES)),)
	export LD	:=	$(CC)
else
	export LD	:=	$(CXX)
endif

export OFILES_BIN	:=	$(addsuffix .o,$(BINFILES))
export OFILES_SOURCES := $(CPPFILES:.cpp=.o) $(CFILES:.c=.o) $(sFILES:.s=.o) $(SFILES:.S=.o)
export OFILES := $(OFILES_BIN) $(OFILES_SOURCES)
export HFILES := $(shell echo $(addsuffix .h,$(subst .,_,$(BINFILES))) | sed s/_png/_RGBA8/g | sed s/_obj/_mesh/g)

#---------------------------------------------------------------------------------
# build a list of include paths
#---------------------------------------------------------------------------------
export INCLUDE	:=	$(foreach dir,$(INCLUDES), -iquote $(CURDIR)/$(dir)) \
					$(foreach dir,$(LIBDIRS),-I$(dir)/include) \
					-I$(CURDIR)/$(BUILD) \
					-I$(LIBOGC_INC)

#---------------------------------------------------------------------------------
# build a list of library paths
#---------------------------------------------------------------------------------
export LIBPATHS	:= -L$(LIBOGC_LIB) $(foreach dir,$(LIBDIRS),-L$(dir)/lib)

export OUTPUT	:=	$(CURDIR)/$(TARGET)
.PHONY: $(BUILD) clean

#---------------------------------------------------------------------------------
$(BUILD):
	@[ -d $@ ] || mkdir -p $@
	@$(MAKE) -C $(BUILD) -f $(CURDIR)/Makefile

#---------------------------------------------------------------------------------
clean:
	@echo clean ...
	@rm -fr $(BUILD) $(OUTPUT).elf $(OUTPUT).dol *.img $(ASSETS)/*.RGBA8 *.mesh $(ASSETS)/*.mesh $(SD_ASSET_PATH)/*.mesh $(SD_ASSET_PATH)/*.RGBA8

#---------------------------------------------------------------------------------
run:
	wiiload $(TARGET).dol


#---------------------------------------------------------------------------------
else
DEPENDS	:=	$(OFILES:.o=.d)

#---------------------------------------------------------------------------------
# main targets
#---------------------------------------------------------------------------------
$(OUTPUT).dol: $(OUTPUT).elf
$(OUTPUT).elf:	$(shell echo $(OFILES) | sed s/.png/.RGBA8/g | sed s/.obj/.mesh/g)

$(OFILES_SOURCES) : $(HFILES)
#---------------------------------------------------------------------------------
# This rule links in binary data with the .obj extension
#---------------------------------------------------------------------------------
%.mesh.o	:	%.obj
#---------------------------------------------------------------------------------
	@echo $(shell python $(CURDIR)/../build-tools/convertObj.py $(notdir $<) $(RESOURCES) $(ASSETS))
	@cp ../$(ASSETS)/$(shell echo $(<F) | sed s/.obj/.mesh/) ../$(SD_ASSET_PATH)/
	$(SILENTCMD)bin2s -a 32 -H `(echo $(<F) | sed s/.obj/_mesh/)`.h `(shell python $(CURDIR)/../build-tools/replacePath.py $< $(RESOURCES) $(ASSETS) | sed s/.obj/.mesh/)` | $(AS) -o `(echo $(<F) | sed s/.obj/.mesh/)`.o
#---------------------------------------------------------------------------------
# This rule links in binary data with the .png extension
#---------------------------------------------------------------------------------
%.RGBA8.o	:	%.png
#---------------------------------------------------------------------------------
	@echo $(shell python $(CURDIR)/../build-tools/convertImg.py $(notdir $<) $(RESOURCES) $(ASSETS))
	@cp ../$(ASSETS)/$(shell echo $(<F) | sed s/.png/.RGBA8/) ../$(SD_ASSET_PATH)/
	$(SILENTCMD)bin2s -a 32 -H `(echo $(<F) | sed s/.png/_RGBA8/)`.h `(shell python $(CURDIR)/../build-tools/replacePath.py $< $(RESOURCES) $(ASSETS) | sed s/.png/.RGBA8/)` | $(AS) -o `(echo $(<F) | sed s/.png/.RGBA8/)`.o
#---------------------------------------------------------------------------------
# This rule links in binary data with the .ttf extension
#---------------------------------------------------------------------------------
%.ttf.o	:	%.ttf
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	$(bin2o)
#---------------------------------------------------------------------------------
# This rule links in binary data with the .ogg extension
#---------------------------------------------------------------------------------
%.ogg.o	%_ogg.h :	%.ogg
#---------------------------------------------------------------------------------
	@echo $(notdir $<)
	$(bin2o)

-include $(DEPENDS)

#---------------------------------------------------------------------------------
endif
#---------------------------------------------------------------------------------
