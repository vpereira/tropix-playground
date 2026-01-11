# Tropix OS

Revival and preservation of Tropix - Brazilian Unix-like operating system developed at NCE/UFRJ from 1988 to 2008.

## About

Tropix is a real-time multiprocessor Unix-like kernel for x86 (Intel 486/Pentium) developed in Brazil.

Official website: http://www.tropix.nce.ufrj.br/

Please check the Wiki maintained by us: https://github.com/vpereira/tropix-playground/wiki

## Getting Started

**Quick start:**
```bash
./scripts/run-tropix.zsh
```

See `HACKING.md` for complete setup, installation, troubleshooting, and network configuration.

See `install.txt` for detailed installation procedure.

## Features

- Real-time multiprocessor kernel
- C compiler, make, ld, as
- Lua interpreter
- Standard Unix utilities (awk, sed, grep, vi, etc.)
- Network stack with telnet, FTP, custom tools
- XTI networking (not BSD sockets)
- Filesystems: T1FS, V7FS, EXT2, FAT, CDFS

## Development

See `TODO.md` for current development tasks.

See `hacking/TROPIX-API-REFERENCE.md` for C API documentation.

Source code from NCE/UFRJ, I just had to modify the libc/fmt/licenca (licença), git didnt like it

- `lib/` - System libraries
- `kernel/` - Kernel sources
- `cmd/` - Command utilities
- `sxwin` - X-Window Graphical Interface

## Documentation

- `HACKING.md` - QEMU setup and running guide
- `install.txt` - Complete installation guide

## License

Copyright NCE/UFRJ - See source files for licensing details.
