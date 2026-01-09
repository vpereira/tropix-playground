# libxti

X/Open Transport Interface (XTI) library for Tropix. It provides the t_*
transport APIs (connection oriented, connectionless, and raw) plus helper
functions for addressing, state, options, and error handling.

Contents

Each subdirectory corresponds to one t_* function and contains a Makefile,
source file(s), and a man page (man.r). The t_intro directory contains the
overall XTI introduction.

Build

From the repository root:

```
cd lib/libxti
make
```

Other targets:
- make n (dry run)
- make lint
- make cp (install objects into /lib/libxti and run mklib on Tropix)
- make man (builds the t_intro manual page)

Linking

Programs link against libxti with -lxti.
