#
# Copyright (c) 2009-2010 Atmel Corporation. All rights reserved.
#
# edited by ezra buchla for monome-aleph project, 2012
#

# Path to top level ASF directory relative to this project directory.
# moved this to the top level Makefile cause it needs to know too
# PRJ_PATH = ../../avr/asf-3.3.0/

#ALEPH_AVR32 = ../../aleph/avr32
#ALEPH_COMMON = ../../aleph/common
#ALEPH_BEES = ../../aleph/bees

ALEPH_AVR32 = ../../avr32
ALEPH_COMMON = ../../common
ALEPH_BEES = ../../bees

# Target CPU architecture: ap, ucr1, ucr2 or ucr3
ARCH = ucr2

# Target part: none, ap7xxx or uc3xxxxx
PART = uc3a0512

# Target device flash memory details (used by the avr32program programming
# tool: [cfi|internal]@address
FLASH = internal@0x80000000

# Clock source to use when programming; xtal, extclk or int
PROG_CLOCK = int

# Applicat	ion target name. Given with suffix .a for library and .elf for a
# standalone application.
TARGET = aleph.elf

# List of C source files.
CSRCS = \
	avr32/drivers/ebi/smc/smc.c \
	avr32/drivers/flashc/flashc.c                      \
	avr32/drivers/gpio/gpio.c                          \
	avr32/drivers/intc/intc.c                          \
	avr32/drivers/pdca/pdca.c                          \
	avr32/drivers/pm/pm.c                              \
	avr32/drivers/pm/pm_conf_clocks.c                  \
	avr32/drivers/pm/power_clocks_lib.c                \
	avr32/drivers/spi/spi.c                            \
        avr32/drivers/tc/tc.c                          \
	avr32/drivers/usart/usart.c                        \
	avr32/utils/debug/print_funcs.c                    \
	avr32/components/memory/sd_mmc/sd_mmc_spi/sd_mmc_spi.c \
	avr32/components/memory/sd_mmc/sd_mmc_spi/sd_mmc_spi_mem.c \
	avr32/drivers/usbb/usbb_host.c                     \
        common/services/clock/uc3a0_a1/sysclk.c            \
	common/services/storage/ctrl_access/ctrl_access.c \
       common/services/usb/class/hid/host/mouse/uhi_hid_mouse.c \
       common/services/usb/uhc/uhc.c \
	$(ALEPH_AVR32)/src/adc.c \
	$(ALEPH_AVR32)/src/app_timers.c \
	$(ALEPH_AVR32)/src/bfin.c \
	$(ALEPH_AVR32)/src/events.c \
	$(ALEPH_AVR32)/src/encoders.c \
	$(ALEPH_AVR32)/src/files.c \
	$(ALEPH_AVR32)/src/filesystem.c \
	$(ALEPH_AVR32)/src/init.c	\
	$(ALEPH_AVR32)/src/interrupts.c \
	$(ALEPH_AVR32)/src/screen.c \
	$(ALEPH_AVR32)/src/scene.c \
	$(ALEPH_AVR32)/src/switches.c \
	$(ALEPH_AVR32)/src/timers.c \
	$(ALEPH_AVR32)/src/main.c \
	$(ALEPH_AVR32)/src/usb.c \
	$(ALEPH_BEES)/key_handler.c \
	$(ALEPH_BEES)/menu.c \
	$(ALEPH_BEES)/net.c \
	$(ALEPH_BEES)/op.c \
	$(ALEPH_BEES)/param.c \
	$(ALEPH_BEES)/preset.c \
	$(ALEPH_BEES)/redraw.c \
	$(ALEPH_BEES)/ops/op_add.c \
	$(ALEPH_BEES)/ops/op_enc.c \
	$(ALEPH_BEES)/ops/op_gate.c \
	$(ALEPH_BEES)/ops/op_mul.c \
	$(ALEPH_BEES)/ops/op_sw.c \
	$(ALEPH_COMMON)/font.c \
	$(ALEPH_COMMON)/fix.c \
	$(ALEPH_COMMON)/libfixmath/fix16.c \
	$(ALEPH_COMMON)/simple_string.c \
	$(ALEPH_COMMON)/fat_io_lib/fat_access.c \
	$(ALEPH_COMMON)/fat_io_lib/fat_cache.c \
	$(ALEPH_COMMON)/fat_io_lib/fat_filelib.c \
	$(ALEPH_COMMON)/fat_io_lib/fat_format.c \
	$(ALEPH_COMMON)/fat_io_lib/fat_misc.c \
	$(ALEPH_COMMON)/fat_io_lib/fat_string.c \
	$(ALEPH_COMMON)/fat_io_lib/fat_table.c \
	$(ALEPH_COMMON)/fat_io_lib/fat_write.c 
#	avr32/services/fs/fat/fat.c                        \
#	avr32/services/fs/fat/fat_unusual.c                \
#	avr32/services/fs/fat/file.c                       \
#	avr32/services/fs/fat/navigation.c                 \

# List of assembler source files.
ASSRCS = \
       avr32/drivers/intc/exception.S                     \
	$(ALEPH_AVR32)/startup/startup.S \
	$(ALEPH_AVR32)/startup/trampoline.S

# List of include paths.
INC_PATH = \
       avr32/boards                                       \
       avr32/boards/user_board                               \
	avr32/components/memory/sdram			 \
	avr32/drivers/cpu/cycle_counter/                 \
	avr32/drivers/ebi/smc/                            \
	avr32/drivers/eic/                            \
       avr32/drivers/flashc                               \
       avr32/drivers/gpio                                 \
       avr32/drivers/intc                                 \
       avr32/drivers/pdca                                   \
       avr32/drivers/pm                                   \
       avr32/drivers/spi                                  \
       avr32/drivers/tc                                 \
       avr32/drivers/twi                                 \
       avr32/drivers/usart                                \
       avr32/drivers/usbb                                 \
       avr32/services/delay                               \
       avr32/services/fs/fat                              \
       avr32/utils                                        \
       avr32/utils/debug                                  \
       avr32/utils/preprocessor                           \
       common/boards                                      \
	avr32/components/memory/sd_mmc/sd_mmc_spi \
       common/services/storage/ctrl_access                \
       common/services/clock                              \
	common/services/delay \
       common/services/usb                                \
       common/services/usb/class/hid                      \
       common/services/usb/class/hid/host/mouse           \
       common/services/usb/uhc                            \
       common/utils         \
	$(ALEPH_AVR32)	\
	$(ALEPH_AVR32)/src	\
	$(ALEPH_AVR32)/conf	\
	$(ALEPH_AVR32)/startup	\
	$(ALEPH_BEES)	\
	$(ALEPH_COMMON)	\
	$(ALEPH_COMMON)/libfixmath \
	$(ALEPH_COMMON)/fat_io_lib
#	../../aleph/avr32/     \
#	../../aleph/avr32/src   \
#	../../aleph/avr32/conf  \
#	../../aleph/bees        \
#	../../aleph/common      \
#	../../aleph/common/libfixmath

# Additional search paths for libraries.
LIB_PATH = 

# List of libraries to use during linking.
LIBS = 

# Path relative to top level directory pointing to a linker script.
# LINKER_SCRIPT = avr32/utils/linker_scripts/at32uc3a/0512/gcc/link_uc3a0512.lds
LINKER_SCRIPT = $(ALEPH_AVR32)/aleph.lds

# Additional options for debugging. By default the common Makefile.in will
# add -g3.
DBGFLAGS = 

# Application optimization used during compilation and linking:
# -O0, -O1, -O2, -O3 or -Os
OPTIMIZATION = -O2
# OPTIMIZATION = -Os
# OPTIMIZATION = -O3
# OPTIMIZATION = -O0 -fno-inline

# Extra flags to use when archiving.
ARFLAGS = 

# Extra flags to use when assembling.
ASFLAGS = 

# Extra flags to use when compiling.
CFLAGS += -g3
#CFLAGS += --verbose

# Extra flags to use when preprocessing.
#
# Preprocessor symbol definitions
#   To add a definition use the format "-D name[=definition]".
#   To cancel a definition use the format "-U name".
#
# The most relevant symbols to define for the preprocessor are:
#   BOARD      Target board in use, see boards/board.h for a list.
#   EXT_BOARD  Optional extension board in use, see boards/board.h for a list.
CPPFLAGS = \
       -D BOARD=USER_BOARD -D ARCH_AVR32=1

# Extra flags to use when linking
# LDFLAGS = -Wl,--gc-sections,-e,_trampoline -Wl,--defsym,__heap_size__=0x00080000 -nostartfiles	
LDFLAGS = -Wl,--gc-sections,-e,_trampoline -Wl,--defsym,__heap_size__=0x00001000 -nostartfiles	