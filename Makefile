# Set the path to your toolchain here
CROSS_COMPILE = /mnt/devel/dafang/Main/toolchain/bin/mips-linux-gnu-
SDK_LIB_DIR =  /mnt/devel/dafang/Main/v4l2rtspserver-master/uclibc/
SDK_INC_DIR =  /mnt/devel/dafang/Main/v4l2rtspserver-master/inc/

CC = $(CROSS_COMPILE)gcc
CPLUSPLUS = $(CROSS_COMPILE)g++
LD = $(CROSS_COMPILE)ld
AR = $(CROSS_COMPILE)ar cr
STRIP = $(CROSS_COMPILE)strip

CFLAGS = $(INCLUDES) -O2 -Wall -march=mips32r2

CFLAGS += -muclibc
LDFLAG += -muclibc
LDFLAG += -Wl,-gc-sections

INCLUDES = -I$(SDK_INC_DIR)
LIBS=${SDK_LIB_DIR}libimp.so ${SDK_LIB_DIR}libalog.so
all: 	audioserver

audioserver: AudioServer.o
	$(CPLUSPLUS) $(LDFLAG) -o $@ $^ $(LIBS) -lpthread -lm -lrt
	$(STRIP) $@

%.o:%.c
	$(CC) -c $(CFLAGS) $< -o $@

clean:
	rm -f *.o *~

distclean: clean
	rm -f $(SAMPLES)
