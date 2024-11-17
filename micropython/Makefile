# To create LFS on NVUM board:
#     import vfs, pyb
#     vfs.VfsLfs1.mkfs(pyb.Flash(start=0))
# To check capacity:
#     import uos:
#     uos.statvfs('/flash')
# To enter bootloader:
#     import machine; machine.bootloader()

UMODBUS_LOC := ~/src/micropython-modbus/umodbus_pd_usb
UMODBUS_SRC := typing.py \
               common.py \
               functions.py \
               const.py \
               modbus.py \
               serial.py
UMODBUS_OBJ := $(addprefix mpy/, $(UMODBUS_SRC:.py=.mpy))

LFS_CMD   := ~/src/littlefs-fuse/lfs
#LFS_FLAGS := --block_size=512 --block_count=64 -o allow_other
LFS_DEV   := /dev/sdb1
LFS_MOUNT := /mnt/lfs

MPY_CROSS := ~/src/micropython/mpy-cross/build/mpy-cross

build: $(UMODBUS_OBJ)

%_with_version.py: %.py
	sed "s|__git_hash__|0x`git rev-parse --short HEAD`|" $< > $@

deploy: main_with_version.py build
	$(LFS_CMD) $(LFS_FLAGS) $(LFS_DEV) $(LFS_MOUNT)
	@mkdir -p $(LFS_MOUNT)/umodbus
	cp $(UMODBUS_OBJ) $(LFS_MOUNT)/umodbus
	cp main_with_version.py $(LFS_MOUNT)/main.py
	rm main_with_version.py
	cp servo.py $(LFS_MOUNT)
	df -h | grep $(LFS_MOUNT)
	umount $(LFS_MOUNT)

clean:
	rm -rf mpy

mpy/%.mpy: $(UMODBUS_LOC)/%.py
	@mkdir -p mpy
	$(MPY_CROSS) $< -o $@

.PHONY := build deploy clean
