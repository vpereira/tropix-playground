# Tropix Development Guide

Running Tropix OS 4.9.0 in QEMU on modern systems.

## Prerequisites

- QEMU (macOS: `brew install qemu`)
- Tropix ISO (`tropix.iso`)
- For bridged networking on macOS: QEMU with vmnet support

## Quick Start

### Create disk image

```bash
qemu-img create -f qcow2 tropix-disk.qcow2 512M
```

### Boot from CD

```bash
qemu-system-i386 \
  -m 128 \
  -cpu 486 \
  -cdrom tropix.iso \
  -hda tropix-disk.qcow2 \
  -boot d \
  -netdev user,id=net0 \
  -device rtl8139,netdev=net0 \
  -rtc base=localtime
```

Key parameters:
- `-m 128` - 128MB RAM required (32MB causes boot hang at 6%)
- `-cpu 486` - Use 486, not Pentium (timing issues)
- `-boot d` - Boot from CD-ROM
- `-device rtl8139` - Network card (best QEMU support)

### Installation

Boot process and partitioning instructions are in `install.txt`. Quick summary:

1. At boot prompt: `boot> -i` (decompresses filesystem to RAM)
2. Login: root/tropix
3. Partition with fdisk: `boot> -f` (see install.txt for details)
4. Run installer: `cd /usr/etc/install && install`
5. Mark root partition bootable in fdisk
6. Change to `-boot c` and reboot
7. Enable multi-user model with CTRL+D. You should see a login prompt.

Refer to `install.txt` for complete installation procedure.

## Network Configuration

### Supported Cards

RTL8139 (Realtek) works reliably. 

### QEMU Network Modes

**User networking (NAT):**
```bash
-netdev user,id=net0 -device rtl8139,netdev=net0
```
VM gets 10.0.2.15, isolated from host network.

**Bridged networking (macOS):**
```bash
-netdev vmnet-bridged,id=net0,ifname=en7 -device rtl8139,netdev=net0
```
Requires sudo. Replace `en7` with your active interface (`ifconfig`).

### Tropix Network Setup

Edit `/etc/itnetdev`:

Static IP configuration:
```
name_server 192.168.0.1 8.8.8.8
/dev/rtl0 192.168.0.11
netmask=255.255.255.0 default
```

Format: device + IP on one line, netmask on next. The `default` keyword creates gateway route (network+1).

DHCP may work but static IP is more reliable:
```
/dev/rtl0 dhcp
```

Reboot or kill the itnet daemon:

```
kill $PID # ps aux is your friend
```

Verify:
```bash
editscb -t    # routing table
ping 8.8.8.8  # connectivity
ping google.com # name resolution
```

![Network connectivity test](images/ping.png)

## Remote Access

Telnet to VM once network is configured:
```bash
telnet <tropix-ip> 23
```

Make sure you have the correct encoding (Latin-1) enabled on your terminal. Set your TERM to vt100

![Remote telnet access](images/telnet.png)

Default credentials: root/tropix

Available services:
- Telnet (port 23)
- FTP (port 21)
- SMTP (port 25)
- FINGER (port 79)
- HTTP (port 80)
- POP3 (port 110)
- RSYNC-Like (port 117)
- RLOGIN (port 513)

## Development Tools

Tropix includes:
- C compiler (`cc`)
- Make, ld, as
- Text editors (vi, ed)
- Lua interpreter
- Standard Unix utilities (awk, sed, grep, etc.)
- Dont use tar in tropix, instead gar. There is some incompatibility with the tar from mac. 
- `rm -rf` does not delete directories; use `rmtree -f` for non-empty directories and `rmdir -f` for empty ones.

![Lua interpreter](images/lua.png)


## Troubleshooting

**vmnet errors on macOS:** Run with sudo if you want to access the network on your host. 
Check QEMU has vmnet support: `qemu-system-i386 -netdev help | grep vmnet`

**Partition not bootable:** Use fdisk (`boot> -f`), activate with `a` command, write with `w`.

## Files

- `install.txt` - Complete installation guide
- `scripts/run-tropix.zsh` - QEMU launcher script
- `lib/`, `kernel/`, `cmd/` - Source code


Happy hacking and Have a lot of fun!
