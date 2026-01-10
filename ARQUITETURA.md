# Arquitetura do Tropix OS

## Visao geral do sistema

Tropix e um sistema operacional tipo Unix para PCs x86 32-bit (486+). O kernel e escrito principalmente em C com assembly x86 para boot, interrupcoes e comutacao de contexto. O desenho e monolitico com sistemas de arquivos, rede e drivers no kernel.

- Arquitetura: x86 32-bit (CPUs 486+)
- Design: kernel preemptivo com recursos de escalonamento em tempo real; configurado para um unico CPU (NCPU=1)
- Enderecamento: memoria virtual paginada com regioes por processo TEXT/DATA/STACK
- Endereco de carga do kernel: 0x80000000

### Subsystems principais (kernel/kernel/)

- Gerenciamento de processos: `proc/` (escalonador, sinais, excecoes, clock)
- Gerenciamento de memoria: `mem/` (paginacao, regioes, MMU, alocadores)
- I/O de arquivos: `fio/`, `bio/` (camada de inode, resolucao de paths, buffer cache, block I/O)
- Syscalls: `sys/` (handlers e tabelas)
- Drivers de dispositivos: `dev/` (console, armazenamento, rede, I/O)
- TTY/Terminal: `tty/` (terminal handling)
- Rede: `itnet/` (Ethernet/ARP, IP/ICMP, TCP/UDP, DHCP)
- Sincronizacao/IPC: `sync/` (eventos, semaforos, IPC, spinlocks)
- Sistemas de arquivos: `t1fs/`, `v7fs/`, `ext2fs/`, `fatfs/`, `cdfs/`, `ntfs/`, `nfs/`, `pipefs/`, `invalfs/`
- Configuracao/init do kernel: `conf/`, `ctl/`, `etc/`

### Resumo do design do OS

O kernel segue o modelo UNIX de processos, um sistema de memoria virtual por regioes e uma interface de sistemas de arquivos definida por uma tabela de funcoes (fstab) que atua como VFS leve.

Modelo de processos
- Entradas da tabela de processos ficam em `struct uproc` (veja `kernel/kernel/h/uproc.h`) com estados RUN, SLEEP, SWAPRDY e ZOMBIE.
- O dispatcher em `kernel/kernel/proc/dispat.c` seleciona o proximo processo executavel da lista core-ready e realiza a troca de contexto.
- O escalonador em `kernel/kernel/proc/swap.c` roda manutencao periodica (alarmes, writeback, DNS, eventos do USB explorer).

Gerenciamento de memoria
- Cada processo possui regioes TEXT, DATA e STACK (`REGIONL`/`REGIONG` em `kernel/kernel/mem/region.c`), com bibliotecas e memoria compartilhada opcionais.
- A camada MMU (`kernel/kernel/mem/mmu.c`) constroi tabelas de paginas por processo e valida traducoes de endereco de usuario.
- Swap e gerenciado em `kernel/kernel/proc/swap.c` usando block I/O e restricoes de DMA.

Modelo de I/O e drivers
- Dispositivos de bloco usam a tabela `biotab` (block I/O por estrategia) e dispositivos de caractere usam `ciotab` (open/read/write/ioctl) em `kernel/kernel/conf/c.c`.
- O buffer cache e os caminhos de leitura/escrita de bloco ficam em `kernel/kernel/bio/`.
- A descoberta de dispositivos ocorre durante o init do kernel (PnP e PCI scan em `kernel/kernel/etc/main.c`).

Interface de sistemas de arquivos
- Filesystems registram na tabela `fstab` (`kernel/kernel/conf/c.c`) com ponteiros para mount, inode, diretorio e operacoes de leitura/escrita.
- A camada de inode (`kernel/kernel/fio/`) despacha para o filesystem selecionado pelo superblock.
- Nao ha /proc.

### Suporte a sistemas de arquivos

- T1FS (filesystem nativo, blocos de 4 KB)
- V7FS (Unix V7, blocos de 512 bytes)
- EXT2
- FAT
- CDFS (ISO 9660 com Rock Ridge)
- NTFS
- NFS (NFSv2)
- PipeFS (pipes)
- CHR/BLK pseudo-filesystems para nos de dispositivo
- InvalFS (handler placeholder/invalid)

### Drivers de dispositivos (kernel/kernel/dev/ e subsistemas relacionados)

**Armazenamento:**
- ATA/ATAPI com suporte a chipset PCI (ata/)
- AIC7xxx SCSI (aic/)
- Adaptec 1542 SCSI (sd.1542.c)
- Disquete (fd)
- CD-ROM (cdfs)
- ZIP drive
- RAM disk

**Rede:**
- 3Com 3c503 e NE1000/NE2000 (ed)
- RTL8139 (rtl)
- PPP/SLIP via serial (ppp.sio, slip.sio)
- Compressao VJ (ppp.vj)

**I/O:**
- Portas seriais (sio, tty)
- Porta paralela (lp)
- Teclado (key)
- Mouse PS/2 (ps2m)
- Console/video (ncon, xcon, video)
- Suporte PCI/PnP (pci, pnp)

**Multimidia:**
- Sound Blaster (sb)
- EMU10K1/Live! (live)

**USB:**
- Stack USB com controladores UHCI/OHCI/EHCI, hubs, HID, mouse (usb/)

**Outros:**
- Controlador DMA
- I/O fisico
- Dispositivo de memoria
- Screensaver

## Processo de boot

O caminho de boot e a inicializacao do kernel sao definidos em `kernel/boot/` e `kernel/kernel/etc/`:

1. **BIOS** le o primeiro bloco de 512 bytes do disco de boot e transfere o controle.
2. **boot0** carrega a si mesmo em modo 16-bit via INT 13h, detecta particoes ativas, mostra menu com indice default/timeout configuraveis e encadeia outro SO ou carrega o boot1 da particao escolhida. Prefere extensoes de INT 13h quando disponiveis para suportar >8 GB e cilindros >1024.
3. **boot1** permanece 16-bit, pede o arquivo do boot2 (default `/boot`), escolhe o modo de video (SVGA default com fallback VGA e overrides de CLI), le o boot2 do filesystem (V7FS ou T1FS) e transfere o controle.
4. **boot2** muda para 32-bit, usa drivers embutidos (disquete, IDE, Adaptec 1542, AIC7xxx), seleciona a imagem do kernel (default `/tropix`) e o dispositivo (default: primeira particao Tropix encontrada), e oferece comandos de manutencao (tabela de particoes, listagem de diretorio, dumps hex, carga de RAM disk). Defaults sao configuraveis via `edboot`.

O boot2 carrega a imagem do kernel e passa um Boot Control Block (BCB). O init do kernel (`kernel/kernel/etc/kinit.c`) consome o BCB para construir o SCB, tabela de discos, estado de video e bookkeeping de memoria. `kernel/kernel/etc/main.c` finaliza a inicializacao, monta o filesystem raiz, cria nos em `/dev` e inicia o `init`.

## Utilitarios de comando (140 comandos)

**Operacoes de arquivo:** cat, cp, mv, rm, ls, ln, chmod, chown
**Processamento de texto:** sed, grep, sort, awk, vi, ed
**Compilacao:** cc, as, ld, gcc, bison, flex
**Administracao do sistema:** mount, umount, fsck, mkfs, fdisk, df, du
**Rede:** ftp, telnet, ping, ifconfig
**X Window:** fvwm, xterm, xedit, xclock, xpaint
**Especializados:** cdplay, xfm, xcpu

## X Window System (sxwin/)

**Window Manager:** fvwm
**Aplicacoes:** xterm, xedit, xclock, xpaint, xsetroot, xtset
**Bibliotecas:** X11, Xaw, Xt, Xext, Xau, ICE, SM
**Suporte:** zlib, JPEG, fonts, xtrans

## Configuracao QEMU

### Requisitos de hardware
- CPU: Intel 486/Pentium
- RAM: 8MB em modo texto, 16MB para X11, testado com 32MB
- Disco: IDE/ATA/SATA
- Rede: NE2000, 3Com 3c503, RTL8129/8139
- SCSI: Adaptec 1542 (ISA), 2940/29160 (PCI)
- Audio: Sound Blaster 16
- USB: Suportado

### Setup atual do QEMU
```bash
qemu-system-i386 \
  -m 128 \
  -cpu i486 \
  -cdrom tropix.iso \
  -hda tropix-disk.qcow2 \
  -boot d \
  # se quiser compartilhar a rede do host, veja HACKING.md
  -netdev user,id=net0 \
  -device rtl8139,netdev=net0 \
  -audiodev coreaudio,id=audio0 \
  -device sb16,audiodev=audio0 \
  -rtc base=localtime \
```
