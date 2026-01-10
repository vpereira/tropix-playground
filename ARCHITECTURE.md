# Tropix OS Architecture

## System Overview

Tropix is a Unix-like operating system for 32-bit x86 PCs (486+). The kernel is written primarily in C with x86 assembly for low-level boot, interrupt, and context switching paths. The design is monolithic with in-kernel filesystems, networking, and device drivers.

- Architecture: x86 32-bit (486+ CPUs)
- Design: Preemptive kernel with real-time scheduling features; configured for a single CPU (NCPU=1)
- Addressing: paged virtual memory with per-process TEXT/DATA/STACK regions
- Kernel load address: 0x80000000

### Core Subsystems (kernel/kernel/)

- Process Management: `proc/` (scheduler, signals, exceptions, clock)
- Memory Management: `mem/` (paging, regions, MMU, allocators)
- File I/O: `fio/`, `bio/` (inode layer, path lookup, buffer cache, block I/O)
- System Calls: `sys/` (syscall handlers and tables)
- Device Drivers: `dev/` (console, storage, network, I/O)
- TTY/Terminal: `tty/` (terminal handling)
- Networking: `itnet/` (Ethernet/ARP, IP/ICMP, TCP/UDP, DHCP)
- Synchronization/IPC: `sync/` (events, semaphores, IPC, spinlocks)
- Filesystems: `t1fs/`, `v7fs/`, `ext2fs/`, `fatfs/`, `cdfs/`, `ntfs/`, `nfs/`, `pipefs/`, `invalfs/`
- Configuration/Kernel Init: `conf/`, `ctl/`, `etc/`

### OS Design Summary

The kernel is organized around a traditional UNIX-style process model, a region-based virtual memory system, and a filesystem interface defined by a function table (fstab) that acts as a lightweight VFS layer.

Process model
- Process table entries live in `struct uproc` (see `kernel/kernel/h/uproc.h`) with states like RUN, SLEEP, SWAPRDY, and ZOMBIE.
- The dispatcher in `kernel/kernel/proc/dispat.c` selects the next runnable process from the core-ready list and performs context switches.
- The scheduler in `kernel/kernel/proc/swap.c` runs periodic maintenance (alarms, block writeback, DNS, USB explorer events).

Memory management
- Each process owns TEXT, DATA, and STACK regions (`REGIONL`/`REGIONG` in `kernel/kernel/mem/region.c`), plus optional shared libraries and shared memory regions.
- The MMU layer (`kernel/kernel/mem/mmu.c`) builds per-process page tables and validates user address translations.
- Swap is managed in `kernel/kernel/proc/swap.c` using block I/O buffers and DMA constraints.

I/O and driver model
- Block devices use the `biotab` table (strategy-based block I/O) and character devices use `ciotab` (open/read/write/ioctl) in `kernel/kernel/conf/c.c`.
- The buffer cache and block read/write paths live in `kernel/kernel/bio/`.
- Device discovery happens during kernel init (PnP and PCI scan in `kernel/kernel/etc/main.c`).

Filesystem interface
- Filesystems register in the `fstab` table (`kernel/kernel/conf/c.c`) with function pointers for mount, inode, directory, and read/write operations.
- The inode layer (`kernel/kernel/fio/`) dispatches to the filesystem implementation selected by the superblock.
- No /proc filesystem.

### Filesystem Support

- T1FS (Tropix native filesystem, 4 KB blocks)
- V7FS (Unix Version 7 filesystem, 512-byte blocks)
- EXT2
- FAT
- CDFS (ISO 9660 with Rock Ridge)
- NTFS
- NFS (NFSv2)
- PipeFS (pipes)
- CHR/BLK pseudo-filesystems for device nodes
- InvalFS (placeholder/invalid filesystem handler)

### Device Drivers (kernel/kernel/dev/ and related subsystems)

**Storage:**
- ATA/ATAPI with PCI chipset support (ata/)
- AIC7xxx SCSI (aic/)
- Adaptec 1542 SCSI (sd.1542.c)
- Floppy disk (fd)
- CD-ROM (cdfs)
- ZIP drive
- RAM disk

**Network:**
- 3Com 3c503 and NE1000/NE2000 (ed)
- RTL8139 (rtl)
- PPP/SLIP over serial (ppp.sio, slip.sio)
- VJ compression (ppp.vj)

**I/O:**
- Serial ports (sio, tty)
- Parallel port (lp)
- Keyboard (key)
- PS/2 mouse (ps2m)
- Console/video (ncon, xcon, video)
- PCI/PnP support (pci, pnp)

**Multimedia:**
- Sound Blaster (sb)
- EMU10K1/Live! (live)

**USB:**
- USB stack with UHCI/OHCI/EHCI host controllers, hubs, HID, mouse (usb/)

**Other:**
- DMA controller
- Physical I/O
- Memory device
- Screensaver

## Boot Process

Boot path and kernel bring-up are defined in `kernel/boot/` and `kernel/kernel/etc/`:

1. **BIOS** reads the first 512-byte block from the boot disk and transfers control.
2. **boot0** loads itself from disk in 16-bit mode via INT 13h, detects active partitions, shows a selectable menu with a configurable default/timeout, and either chainloads another OS or loads boot1 from the chosen partition. It prefers INT 13h extensions when available to support >8 GB and cylinders >1024.
3. **boot1** remains 16-bit, prompts for the boot2 file (default `/boot`), chooses video mode (SVGA default with VGA fallback and CLI overrides), reads boot2 from the filesystem (V7FS or T1FS), and transfers control.
4. **boot2** switches to 32-bit mode, uses built-in drivers (floppy, IDE, Adaptec 1542, AIC7xxx), selects the kernel image (default `/tropix`) and device (default: first Tropix partition found), and can run maintenance commands (partition table, directory listing, hex dumps, RAM disk load). Defaults are configurable via `edboot`.

boot2 loads the kernel image and passes a Boot Control Block (BCB). Kernel init (`kernel/kernel/etc/kinit.c`) consumes the BCB to build the SCB, disk table, video state, and memory bookkeeping. `kernel/kernel/etc/main.c` finishes initialization, mounts the root filesystem, creates `/dev` nodes, and starts `init`.

## Init Process

`/etc/init` coordinates system startup in three phases: single-user mode on the console for maintenance, execution of `/etc/rc` for system initialization, and multi-user mode where a login process is spawned per active terminal as defined in `/etc/initab`. Runtime state is tracked in `/etc/utmp` and `/etc/mtab`, with environment defaults in `/etc/globalenv`.

## Command Utilities (140 commands)

**File Operations:** cat, cp, mv, rm, ls, ln, chmod, chown
**Text Processing:** sed, grep, sort, awk, vi, ed
**Compilation:** cc, as, ld, gcc, bison, flex
**System Admin:** mount, umount, fsck, mkfs, fdisk, df, du
**Networking:** ftp, telnet, ping, ifconfig
**X Window:** fvwm, xterm, xedit, xclock, xpaint
**Specialized:** cdplay, xfm, xcpu

## X Window System (sxwin/)

**Window Manager:** fvwm
**Applications:** xterm, xedit, xclock, xpaint, xsetroot, xtset
**Libraries:** X11, Xaw, Xt, Xext, Xau, ICE, SM
**Support:** zlib, JPEG, fonts, xtrans

## QEMU Configuration

### Hardware Requirements
- CPU: Intel 486/Pentium
- RAM: 8MB text mode, 16MB X11, tested with 32MB
- Disk: IDE/ATA/SATA
- Network: NE2000, 3Com 3c503, RTL8129/8139
- SCSI: Adaptec 1542 (ISA), 2940/29160 (PCI)
- Audio: Sound Blaster 16
- USB: Supported

### Current QEMU Setup
```bash
qemu-system-i386 \
  -m 128 \
  -cpu i486 \
  -cdrom tropix.iso \
  -hda tropix-disk.qcow2 \
  -boot d \
  # if you want to shre your host network, please check HACKING.md
  -netdev user,id=net0 \
  -device rtl8139,netdev=net0 \
  -audiodev coreaudio,id=audio0 \
  -device sb16,audiodev=audio0 \
  -rtc base=localtime \
```
