# Multiuser Mode

Tropix init runs in three phases: single-user, `/etc/rc`, multiuser.

## Phases

1. MONOUSUARIO (single-user)
   - Console only.
   - Maintenance tasks.
   - Exit with CTRL+D.

2. `/etc/rc`
   - System initialization.
   - Mounts filesystems and starts services (including network).

3. MULTIUSUARIO (multiuser)
   - Logins spawned per `/etc/initab`.
   - 8 virtual terminals.
   - Network services active.

## Network

Network is started from `/etc/rc`, so it is not available in single-user mode.

## Boot directly into multiuser

At the kernel parameter prompt `Deseja modificar parametros? (n):`, do nothing.
When the timeout expires, the system skips single-user and proceeds to `/etc/rc`
and multiuser mode.

## Terminals

Switch terminals with `<^S><number><^Q>` (1-8).

## Key files

- `/etc/init`
- `/etc/rc`
- `/etc/initab`
- `/etc/itnetdev`
- `/etc/itnetserv`

## References

- `cmd/init/man.r`
- `cmd/itnet/daemon/itnet_daemon/man.r`
