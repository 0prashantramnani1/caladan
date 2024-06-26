ROOT_PATH=.
include $(ROOT_PATH)/build/shared.mk

DPDK_PATH = dpdk
CHECKFLAGS = -D__CHECKER__ -Waddress-space

ifneq ($(TCP_RX_STATS),)
CFLAGS += -DTCP_RX_STATS
endif

CFLAGS += -DNO_LIBNUMA

# libbase.a - the base library
base_src = $(wildcard base/*.c)
base_obj = $(base_src:.c=.o)

#libnet.a - a packet/networking utility library
net_src = $(wildcard net/*.c)
net_obj = $(net_src:.c=.o)

# iokernel - a soft-NIC service
iokernel_src = $(wildcard iokernel/*.c)
iokernel_obj = $(iokernel_src:.c=.o)
$(iokernel_obj): INC += -I$(DPDK_PATH)/build/include

# runtime - a user-level threading and networking library
runtime_src = $(wildcard runtime/*.c) $(wildcard runtime/net/*.c)
runtime_src += $(wildcard runtime/net/directpath/*.c)
runtime_src += $(wildcard runtime/net/directpath/mlx5/*.c)
runtime_src += $(wildcard runtime/rpc/*.c)
runtime_asm = $(wildcard runtime/*.S)
runtime_obj = $(runtime_src:.c=.o) $(runtime_asm:.S=.o)

# test cases
test_src = $(wildcard tests/*.c)
test_obj = $(test_src:.c=.o)
test_targets = $(basename $(test_src))

lib := \
	neper/check_all_options.o \
	neper/coef.o \
	neper/common.o \
	neper/control_plane.o \
	neper/cpuinfo.o \
	neper/define_all_flags.o \
	neper/flags.o \
	neper/flow.o \
	neper/hexdump.o \
	neper/histo.o \
	neper/logging.o \
	neper/loop.o \
	neper/numlist.o \
	neper/or_die.o \
	neper/parse.o \
	neper/percentiles.o \
	neper/pq.o \
	neper/print.o \
	neper/rusage.o \
	neper/snaps.o \
	neper/socket.o \
	neper/stats.o \
	neper/thread.o \
	neper/version.o

#neper
neper_src = $(wildcard neper/tcp_rr_main.c)
#neper_obj = $(neper_src:.c=.o)
neper_obj = neper/tcp_stream_main.o neper/tcp_stream.o neper/stream.o $(lib)
neper_targets = neper/tcp_stream

# pcm lib
PCM_DEPS = $(ROOT_PATH)/deps/pcm/build/src/libpcm.a
PCM_LIBS = -lm -lstdc++

# dpdk libs
DPDK_LIBS=$(shell PKG_CONFIG_PATH=$(PKG_CONFIG_PATH) pkg-config --libs --static libdpdk)

# additional libs for running with Mellanox NICs
ifeq ($(CONFIG_MLX5),y)
$(iokernel_obj): INC += $(MLX5_INC)
endif

# must be first
all: libbase.a libnet.a libruntime.a iokerneld $(test_targets) $(neper_targets)

libbase.a: $(base_obj)
	$(AR) rcs $@ $^

libnet.a: $(net_obj)
	$(AR) rcs $@ $^

libruntime.a: $(runtime_obj)
	$(AR) rcs $@ $^

iokerneld: $(iokernel_obj) libbase.a libnet.a base/base.ld $(PCM_DEPS)
	$(LD) $(LDFLAGS) -o $@ $(iokernel_obj) libbase.a libnet.a $(DPDK_LIBS) \
	$(PCM_DEPS) $(PCM_LIBS) -lpthread -lnuma -ldl

$(test_targets): $(test_obj) libbase.a libruntime.a libnet.a base/base.ld
	$(LD) $(LDFLAGS) -o $@ $@.o $(RUNTIME_LIBS)

$(neper_targets): $(neper_obj)
	$(LD) $(LDFLAGS) -o $@ $^ $(RUNTIME_LIBS) -lm -lrt


# general build rules for all targets
src = $(base_src) $(net_src) $(runtime_src) $(iokernel_src) $(test_src)
asm = $(runtime_asm)
obj = $(src:.c=.o) $(asm:.S=.o)
dep = $(obj:.o=.d)

ifneq ($(MAKECMDGOALS),clean)
-include $(dep)   # include all dep files in the makefile
endif

# rule to generate a dep file by using the C preprocessor
# (see man cpp for details on the -MM and -MT options)
%.d: %.c
	@$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
%.d: %.S
	@$(CC) $(CFLAGS) $< -MM -MT $(@:.d=.o) >$@
%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

# prints sparse checker tool output
sparse: $(src)
	$(foreach f,$^,$(SPARSE) $(filter-out -std=gnu11, $(CFLAGS)) $(CHECKFLAGS) $(f);)

.PHONY: submodules
submodules:
	$(ROOT_PATH)/build/init_submodules.sh

.PHONY: submodules-clean
submodules-clean:
	$(ROOT_PATH)/build/init_submodules.sh clean

.PHONY: clean
clean:
	rm -f $(obj) $(dep) libbase.a libnet.a libruntime.a \
	iokerneld $(test_targets) neper/*.o neper/tcp_stream 

