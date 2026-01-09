# Changes for Tropix Build

This directory contains Lua 1.1 adjusted to build on Tropix.

Build system
- Makefiles use `cc`, Tropix-style `-I`/`-D` spacing, and explicit `.o` rules.
- Static libraries are built as `mklib` directory libraries: `lib/lua` and `lib/lualib`.
- Shared library targets removed.
- Linking uses direct library paths instead of `-L` (Tropix `ld` has no `-L`).
- Added `.virtual` targets where needed.

Source changes
- `include/mm.h` now has a header guard (was empty).
- `src/hash.c` and `src/hash.h` rename the `nhash` parameter to `n_hash` to avoid a macro name collision; `src/hash.c` also avoids `new` as a variable name for Tropix `cc`.
- `src/opcode.c` dropped redundant `strtod` declaration.
- `clients/lib/iolib.c` uses `stat(2)` with `<stat.h>` for file existence checks.
