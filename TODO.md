# Tropix TODO

## Network Tools

### Write simple wget
Implement basic HTTP client using XTI (libxti) to download files from web servers.

Requirements:
- Use t_open("/dev/itntcp") for TCP connection
- Parse HTTP/1.0 protocol
- Handle GET requests
- Write response to file

Reference: TROPIX-API-REFERENCE.md for XTI networking

### Investigate TCMPTO (rsync-like tool)

Explore remote tree comparison/synchronization tool.

Files to study:
- src/cmd/itnet/tnet/tcmpto_c/src/tcmpto_c.c (client)
- src/cmd/itnet/tnet/tcmpto_s/src/tcmpto_s.c (server)

Test operations:
- Compare trees between local/remote
- Sync files bidirectionally (-t/-f/-d flags)
- Test compression (-C flag)
- Pattern matching (-p/-P flags)
- Generate operation list (garfield file with -g)

Protocol details:
- Port 117
- Custom protocol: TCMPTO,3.0.5
- XTI-based (not BSD sockets)
- Requires tcmpto_s daemon on remote

Usage examples:
```bash
# Push local to remote
tcmpto -t -C -F -n remotebox -u user /path

# Pull remote to local
tcmpfrom -f -C -F -n remotebox -u user /path

# Direct copy
tcpto -F -n remotebox -u user /path
```
