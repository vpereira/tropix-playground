# Tropix OS Bootable CDROM Build Guide

STILL UNTESTED

## Source Code Structure

- `cmd/` - 140+ Unix commands and utilities (ls, cp, gcc, vi, etc.)
- `lib/` - System libraries (libc, libm, libcurses, libxti)
- `kernel/` - Kernel sources with boot loader components
- `sxwin/` - X Window System components

## Build Requirements

**You MUST have a running Tropix system to build from source.** The build process uses Tropix-specific tools and assumes a Tropix filesystem layout.

## Build Process


### 1. Transfer Source Code

Mount/copy your source code into the running Tropix system:
- Use QEMU shared folders
- Network transfer (FTP/SCP)
- Or mount host filesystem if accessible

### 2. Extract Source Archives

From the running Tropix system:

```bash
# Extract kernel sources
gunzip kernel.tgz
gar -ixv kernel.tar

# Extract library sources
gunzip lib.tgz
gar -ixv lib.tar

# Extract command sources
gunzip cmd.tgz
gar -ixv cmd.tar
```

### 3. Build Kernel Components

```bash
# Build boot loader stages
cd kernel/boot
make

# Build individual boot components
cd boot0 && make
cd ../boot1 && make
cd ../boot2 && make

# Build main kernel
cd ../kernel && make
```

### 4. Build Libraries

```bash
cd lib
make  # Builds all libraries: libc, libm, libcurses, libxti, etc.
```

### 5. Build Commands

```bash
cd cmd
make  # Builds all 140+ utilities
```

Key make targets:
- `make` - Build all components
- `make lint` - Code quality checks
- `make clean` - Remove build artifacts
- `make cmp` - Compare with reference versions

### 6. Create Bootable Filesystem

Use Tropix filesystem tools to create a bootable system:

```bash
# Create T1FS filesystem image (example: 45MB)
mkfst1 -fr tropix.img 45M TROPIX cdrom

# Mount and populate
mount tropix.img /mnt

# Copy kernel, libraries, and commands
cp kernel-image /mnt/tropix
cp -r lib/* /mnt/lib/
cp -r cmd-binaries/* /mnt/bin/
cp -r cmd-binaries/* /mnt/usr/bin/

# Add system configuration files
# (fstab, inittab, network config, etc.)

umount /mnt
```

### 7. Generate Bootable ISO

```bash
# Use mkisofs or genisoimage with El Torito boot support
mkisofs -b boot/loader -c boot/catalog -o tropix-new.iso tropix.img
```

## Key Build Files

- `kernel/boot/boot0/Makefile` - Master boot record loader
- `kernel/boot/boot1/Makefile` - First stage boot loader
- `kernel/boot/boot2/Makefile` - Second stage with kernel loading
- `kernel/kernel/Makefile` - Main kernel build
- `lib/Makefile` - Library compilation
- `cmd/Makefile` - Command compilation (140+ targets)
