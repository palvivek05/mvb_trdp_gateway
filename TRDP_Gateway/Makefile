################################################################################
# TRDP Gateway Makefile - Clean version
# MD support enabled by default
################################################################################

#-----------------------
# Toolchain paths
#-----------------------
TOOLCHAIN_DIR := /home/bacancy/yocto-work/toolchain
SYSROOT_DIR   := $(TOOLCHAIN_DIR)/sysroots/cortexa7t2hf-neon-vfpv4-dey-linux-gnueabi
HOST_DIR      := $(TOOLCHAIN_DIR)/sysroots/x86_64-deysdk-linux

CROSS_PREFIX := $(HOST_DIR)/usr/bin/arm-dey-linux-gnueabi/arm-dey-linux-gnueabi-
CC           := $(CROSS_PREFIX)gcc
AR           := $(CROSS_PREFIX)ar
STRIP        := $(CROSS_PREFIX)strip

#-----------------------
# Project files
#-----------------------
TARGET    := UDP_Receiver
OBJ_DIR   := build
TRDP_LIB  := $(OBJ_DIR)/libtrdp.a

TARGET_VOS = posix
TARGET_OS  = LINUX
TRDP_BASE  := Middlewares/Trdp

#-----------------------
# Source and include paths
#-----------------------
INCPATH   += -I $(TRDP_BASE)/api
VOS_PATH  += -I $(TRDP_BASE)/vos/$(TARGET_VOS)
VOS_INCPATH += -I $(TRDP_BASE)/vos/api -I $(TRDP_BASE)/common

TRDP_SRC  := $(TRDP_BASE)/common \
             $(TRDP_BASE)/vos/common \
             $(TRDP_BASE)/vos/$(TARGET_VOS)

TRDP_INC  := $(TRDP_BASE)/api \
             $(TRDP_BASE)/vos/api \
             $(TRDP_BASE)/common \
             $(TRDP_BASE)/vos/common \
             $(TRDP_BASE)/vos/$(TARGET_VOS)

SRC_FILES := $(wildcard *.c) \
             $(wildcard Components/*/src/*.c)

#SRC_FILES := $(filter-out Components/Trdp_app/src/GW_trdp_md.c \
                          Components/Trdp_app/src/GW_trdp_pd.c, \
                          $(SRC_FILES))

TRDP_SRC_FILES := $(wildcard $(TRDP_BASE)/common/*.c) \
                  $(wildcard $(TRDP_BASE)/vos/common/*.c) \
                  $(filter-out %/vos_sockTSN.c, $(wildcard $(TRDP_BASE)/vos/$(TARGET_VOS)/*.c))

OBJ_FILES := $(patsubst %.c,$(OBJ_DIR)/%.o,$(notdir $(SRC_FILES)))
TRDP_OBJ_FILES := $(patsubst %.c,$(OBJ_DIR)/%.o,$(notdir $(TRDP_SRC_FILES)))

VPATH = . $(dir $(SRC_FILES)) $(TRDP_SRC)
vpath %.h $(TRDP_INC)

INC_DIRS := $(wildcard Components/*/inc)
INCLUDES := $(addprefix -I,$(INC_DIRS))

ALL_INCLUDES := $(INCPATH) $(VOS_INCPATH) $(VOS_PATH) $(INCLUDES)

#-----------------------
# Compiler flags
#-----------------------
ARCH_FLAGS := -march=armv7-a -mtune=cortex-a7 -mfpu=neon-vfpv4 -mfloat-abi=hard

CFLAGS := -Wall -O2 --sysroot=$(SYSROOT_DIR) \
          $(ALL_INCLUDES) \
          -I$(SYSROOT_DIR)/usr/include/libdigiapix \
          $(ARCH_FLAGS) \
          -DPOSIX \
          -D_GNU_SOURCE \
          -D_POSIX_C_SOURCE=200112L \
          -Wno-unknown-pragmas \
          -Wno-cpp \
          -Wno-stringop-truncation

LDFLAGS := --sysroot=$(SYSROOT_DIR) \
           -Wl,-rpath,/usr/lib -Wl,-rpath,/lib \
           -Wl,--dynamic-linker=/lib/ld-linux-armhf.so.3

LIBS := -L$(OBJ_DIR) -ltrdp -L$(SYSROOT_DIR)/usr/lib -ldigiapix -lm -lpthread

#-----------------------
# MD support default ON
#-----------------------
MD_SUPPORT ?= 1
ifeq ($(MD_SUPPORT),1)
    TRDP_OBJ_FILES += $(OBJ_DIR)/trdp_mdcom.o $(OBJ_DIR)/tlm_if.o
    CFLAGS += -DMD_SUPPORT=1
else
    CFLAGS += -DMD_SUPPORT=0
endif

#-----------------------
# Build types
#-----------------------
DEBUG ?= FALSE
ifeq ($(DEBUG),TRUE)
    CFLAGS += -g3 -O0 -DDEBUG
    LDFLAGS += -g3
else
    CFLAGS += -Os -DNO_DEBUG
endif

#-----------------------
# Build rules
#-----------------------
all: $(TARGET)

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	@echo "### Building $(notdir $<)"
	@$(CC) $(CFLAGS) -c $< -o $@

# Build TRDP static library
$(TRDP_LIB): $(TRDP_OBJ_FILES)
	@echo "### Building the lib libtrdp.a"
	@$(AR) rcs $@ $^
	@rm -f $(TRDP_OBJ_FILES)

# Link application
$(TARGET): $(OBJ_FILES) $(TRDP_LIB)
	@echo "### Building application $(TARGET)"
	@$(CC) $(LDFLAGS) -o $@ $(OBJ_FILES) $(LIBS)
	@$(STRIP) $@
	@echo "### Build complete: $(TARGET)"

clean:
	@rm -rf $(OBJ_DIR) $(TARGET)

.PHONY: all clean