#!/bin/zsh

# Tropix OS - QEMU launcher with BRIDGED networking
# 128MB RAM, 486 CPU, 512MB IDE disk
# Network bridged to real network (will get 192.168.0.x IP via DHCP)

# NOTE: On macOS, this requires running with sudo for vmnet access
# Usage: sudo ./run-tropix-bridged.zsh

qemu-system-i386 \
  -m 128 \
  -cpu 486 \
  -cdrom tropix.iso \
  -hda tropix-disk.qcow2 \
  -boot c \
  -netdev vmnet-bridged,id=net0,ifname=en7 \
  -device rtl8139,netdev=net0 \
  -audiodev coreaudio,id=audio0 \
  -device sb16,audiodev=audio0 \
  -rtc base=localtime
