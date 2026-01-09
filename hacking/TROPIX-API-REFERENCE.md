# Tropix API Reference - C Programming Guide

Complete API documentation for porting C programs to Tropix OS.

## Table of Contents

1. [Standard C Library (libc)](#1-libc-standard-c-library)
2. [Network API (libxti)](#2-libxti-network-api)
3. [Terminal/Screen API (libcurses)](#3-libcurses-terminalscreen-api)
4. [Math Library (libm486)](#4-libm486-math-library)
5. [System Calls](#5-system-calls--kernel-interface)
6. [Tropix-Specific Features](#6-tropix-specific-features)
7. [Porting Guide](#7-porting-guide)

---

## 1. LIBC (Standard C Library)

### Memory Management (mem/)
```c
// Standard allocation
void *malloc(size_t size);
void *calloc(size_t nmemb, size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);
void *alloca(size_t size);

// Tropix-specific: stack-based allocation
void *smalloc(size_t size);
void sfree(void *ptr);

// Memory operations
void *memcpy(void *dest, const void *src, size_t n);
void *memmove(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
void *memchr(const void *s, int c, size_t n);

// Tropix extensions
int memtcmp(const void *s1, const void *s2, size_t n);  // Case-insensitive
int memttcmp(const void *s1, const void *s2, size_t n); // Case-insensitive
```

### String Functions (str/)
```c
// Standard
char *strcpy(char *dest, const char *src);
char *strcat(char *dest, const char *src);
int strcmp(const char *s1, const char *s2);
size_t strlen(const char *s);
char *strchr(const char *s, int c);
char *strstr(const char *haystack, const char *needle);
char *strtok(char *str, const char *delim);

// Tropix-specific
int streq(const char *s1, const char *s2);           // Equality check
int stricmp(const char *s1, const char *s2);         // Case-insensitive
int strtcmp(const char *s1, const char *s2);         // Tab-aware
int strttcmp(const char *s1, const char *s2);        // Tab-aware case-insensitive
char *strdup(const char *s);
char *strscpy(char *dest, const char *src, size_t n); // Safe copy
unsigned strhash(const char *s);                     // Hash function
```

### File I/O (stdio/)
```c
// Standard streams
FILE *fopen(const char *path, const char *mode);
int fclose(FILE *stream);
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream);
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream);
int fprintf(FILE *stream, const char *format, ...);
int fscanf(FILE *stream, const char *format, ...);
int fseek(FILE *stream, long offset, int whence);
long ftell(FILE *stream);

// Tropix extensions
char *fngets(char *s, int size, FILE *stream);  // Safe gets
int fnputs(const char *s, FILE *stream);        // Safe puts
int snprintf(char *str, size_t size, const char *format, ...);
int vsnprintf(char *str, size_t size, const char *format, va_list ap);
```

### Character Classification (ctype/)
```c
int isalpha(int c), isdigit(int c), isalnum(int c);
int isupper(int c), islower(int c);
int isspace(int c), ispunct(int c), iscntrl(int c);
int isprint(int c), isgraph(int c);
int isxdigit(int c), isascii(int c);
int isiso(int c);  // Tropix: ISO-8859-1 check

int toupper(int c), tolower(int c);
int toascii(int c), toiso(int c);  // Tropix: ISO conversion
```

### Time/Date Functions
```c
time_t time(time_t *t);
char *ctime(const time_t *timep);
struct tm *localtime(const time_t *timep);
time_t mktime(struct tm *tm);
size_t strftime(char *s, size_t max, const char *format, const struct tm *tm);
time_t strtotime(const char *s);  // Tropix: Parse time string
int sleep(unsigned int seconds);
clock_t clock(void);
```

### User/Group Management
```c
struct passwd *getpwuid(uid_t uid);
struct passwd *getpwnam(const char *name);
struct passwd *getpwent(void);
void setpwent(void), endpwent(void);

struct group *getgrgid(gid_t gid);
struct group *getgrnam(const char *name);
struct group *getgrent(void);
void setgrent(void), endgrent(void);

char *getlogin(void);
char *getpass(const char *prompt);
```

### System Utilities
```c
void abort(void);
int atexit(void (*function)(void));
void exit(int status);
int system(const char *command);
char *getenv(const char *name);
int putenv(char *string);

void qsort(void *base, size_t nmemb, size_t size,
           int (*compar)(const void *, const void *));
void *bsearch(const void *key, const void *base, size_t nmemb,
              size_t size, int (*compar)(const void *, const void *));

int rand(void);
void srand(unsigned int seed);

// Tropix-specific
unsigned short crc16(const char *buf, int count);   // CRC calculation
int patmatch(const char *str, const char *pattern); // Pattern matching
```

---

## 2. LIBXTI (Network API)

Tropix uses **XTI (X/Open Transport Interface)** instead of BSD sockets.

### Network Device Endpoints
```c
"/dev/itntcp"  // TCP (connection-oriented)
"/dev/itnudp"  // UDP (connectionless)
"/dev/itnraw"  // Raw IP (requires superuser)
```

### Core Functions

#### Initialization
```c
#include <xti.h>

int t_open(const char *name, int oflag, T_INFO *info);
// Returns: file descriptor on success, -1 on error
// Example: fd = t_open("/dev/itntcp", O_RDWR, NULL);

int t_bind(int fd, INADDR *addr, int qlen);
// Bind address and set listen queue length
// qlen: 0 for client, >0 for server

int t_close(int fd);
// Close endpoint

int t_getinfo(int fd, T_INFO *info);
// Get protocol capabilities

int t_getaddr(int fd, INADDR *localaddr, INADDR *remoteaddr);
// Get local and remote addresses
```

#### Connection-Oriented (TCP)
```c
// Client side
int t_connect(int fd, INADDR *addr, T_CALL *call);
// Initiate connection to server

int t_rcvconnect(int fd, T_CALL *call);
// Receive connection confirmation (async mode)

// Server side
int t_listen(int fd, T_CALL *call);
// Wait for incoming connection

int t_accept(int fd, int resfd, T_CALL *call);
// Accept connection (resfd can equal fd)
```

#### Data Transfer
```c
// Connection-oriented (TCP)
int t_snd(int fd, char *buf, unsigned nbytes, int flags);
// flags: T_MORE (more data), T_PUSH (flush), T_URGENT (OOB)

int t_rcv(int fd, char *buf, unsigned nbytes, int *flags);
// Returns: bytes received, sets flags

// Connectionless (UDP)
int t_sndudata(int fd, T_UNITDATA *unitdata);
int t_rcvudata(int fd, T_UNITDATA *unitdata, int *flags);

// Raw IP
int t_sndraw(int fd, T_UNITDATA *unitdata);
int t_rcvraw(int fd, T_UNITDATA *unitdata, int *flags);

// Check available data
int t_nread(int fd);  // Returns bytes available
```

#### Disconnection
```c
// Abortive disconnect
int t_snddis(int fd, T_CALL *call);
int t_rcvdis(int fd, T_DISCON *discon);

// Orderly release (TCP only)
int t_sndrel(int fd);
int t_rcvrel(int fd);
```

#### Event Management
```c
int t_look(int fd);
// Returns pending event:
// T_LISTEN, T_CONNECT, T_DATA, T_EXDATA, T_DISCONNECT, T_ORDREL, etc.

int t_push(int fd);  // Force data transmission (non-standard)
```

#### Address Translation
```c
char *t_addr_to_name(int fd, INADDR *addr);
// Convert address to hostname

int t_node_to_addr(int fd, const char *nodename, INADDR *addr);
// Convert hostname to address

char *t_addr_to_str(int fd, INADDR *addr, char *string);
// Convert address to "ip:port" string

int t_str_to_addr(int fd, const char *string, INADDR *addr);
// Parse "ip:port" string

char *t_addr_to_node(int fd, INADDR *addr);
// Get node name from address
```

#### Error Handling
```c
extern int t_errno;  // Error code (unified with errno)

void t_error(const char *label);
// Print error message (like perror)
```

### Data Structures
```c
typedef struct {
    ulong a_addr;      // IP address (network byte order)
    ushort a_port;     // Port number (or protocol for raw)
    ushort a_family;   // Address family
} INADDR;

typedef struct {
    long addr;         // Max address size
    long options;      // Max options size
    long tsdu;         // Max TSDU size
    long etsdu;        // Max expedited TSDU size
    long connect;      // Max connect data size
    long discon;       // Max disconnect data size
    int servtype;      // Service type (T_COTS, T_CLTS, T_COTS_ORD)
} T_INFO;

typedef struct {
    INADDR addr;       // Address
    INADDR opt;        // Options
    char *udata;       // User data
    char *uopt;        // User options
    long udata_len;    // Data length
    long uopt_len;     // Options length
} T_UNITDATA;

typedef struct {
    INADDR addr;       // Address
    INADDR opt;        // Options
    char *udata;       // User data
    long sequence;     // Connection sequence number
} T_CALL;

typedef struct {
    char *udata;       // User data
    int reason;        // Disconnect reason
    int sequence;      // Connection sequence number
} T_DISCON;
```

### Example: TCP Client
```c
#include <xti.h>
#include <fcntl.h>

int fd;
INADDR addr;
char buf[512];

// Open TCP endpoint
fd = t_open("/dev/itntcp", O_RDWR, NULL);

// Bind (qlen=0 for client)
t_bind(fd, NULL, 0);

// Connect to server
t_str_to_addr(fd, "192.168.0.1:80", &addr);
t_connect(fd, &addr, NULL);

// Send/receive data
t_snd(fd, "GET / HTTP/1.0\r\n\r\n", 18, 0);
t_rcv(fd, buf, sizeof(buf), NULL);

// Close
t_close(fd);
```

### Example: TCP Server
```c
int listen_fd, conn_fd;
INADDR local_addr;
T_CALL call;

// Open and bind
listen_fd = t_open("/dev/itntcp", O_RDWR, NULL);
t_str_to_addr(listen_fd, "0.0.0.0:8080", &local_addr);
t_bind(listen_fd, &local_addr, 5);  // qlen=5

// Accept connections
while (1) {
    t_listen(listen_fd, &call);
    conn_fd = t_open("/dev/itntcp", O_RDWR, NULL);
    t_bind(conn_fd, NULL, 0);
    t_accept(listen_fd, conn_fd, &call);

    // Handle connection in conn_fd
    // ...

    t_close(conn_fd);
}
```

---

## 3. LIBCURSES (Terminal/Screen API)

### Initialization
```c
#include <curses.h>

TERM *newterm(const char *type, FILE *outfp, FILE *infp);
// Initialize terminal
// type: NULL for TERM environment variable

int setterm(int fd);
// Set active terminal

int incurses(const char *devname);
// Enter curses mode (simpler initialization)

void outcurses(void);
// Exit curses mode
```

### Window Management
```c
WINDOW *newwin(int nrows, int ncols, int begy, int begx);
// Create window

int delwin(WINDOW *win);
// Delete window

int mvwin(WINDOW *win, int y, int x);
// Move window

int touchwin(WINDOW *win);
// Mark window as changed

int wrefresh(WINDOW *win);
// Update physical screen from window

extern WINDOW *stdwin;  // Standard window (full screen)
extern int LINES, COLS; // Screen dimensions
```

### Output
```c
int waddch(WINDOW *win, chtype ch);
int waddstr(WINDOW *win, const char *str);
int wprintw(WINDOW *win, const char *fmt, ...);

int wmove(WINDOW *win, int y, int x);
// Move cursor

int wclrtoeol(WINDOW *win);
int wclrtobot(WINDOW *win);
int wclear(WINDOW *win);
int werase(WINDOW *win);

// Convenience macros for stdwin
#define addch(ch)        waddch(stdwin, ch)
#define addstr(str)      waddstr(stdwin, str)
#define printw(fmt, ...) wprintw(stdwin, fmt, __VA_ARGS__)
#define move(y, x)       wmove(stdwin, y, x)
#define clear()          wclear(stdwin)
#define refresh()        wrefresh(stdwin)
```

### Input
```c
int wgetch(WINDOW *win);
// Get character (handles special keys)
// Returns: character or KEY_* constant

int wgetstr(WINDOW *win, char *str);
int wscanw(WINDOW *win, const char *fmt, ...);

// Special keys
#define KEY_DOWN   0x0102
#define KEY_UP     0x0103
#define KEY_LEFT   0x0104
#define KEY_RIGHT  0x0105
#define KEY_HOME   0x0101
#define KEY_END    0x0100
#define KEY_NPAGE  0x0106  // Page down
#define KEY_PPAGE  0x0107  // Page up
#define KEY_F(n)   (0x0200 + (n))  // Function keys F0-F9
```

### Attributes
```c
// Attribute flags
#define A_NORMAL     0x0000
#define A_STANDOUT   0x0100
#define A_UNDERLINE  0x0200
#define A_REVERSE    0x0400
#define A_BLINK      0x0800
#define A_DIM        0x1000
#define A_BOLD       0x2000
#define A_ALTCHARSET 0x4000

int wattrset(WINDOW *win, chtype attrs);
int wattron(WINDOW *win, chtype attrs);
int wattroff(WINDOW *win, chtype attrs);
```

### Box Drawing
```c
int wbox(WINDOW *win, const char *text, int height, int width);
// Draw box with title

// Box drawing characters
#define ACS_ULCORNER  0x0100  // ┌
#define ACS_LLCORNER  0x0101  // └
#define ACS_URCORNER  0x0102  // ┐
#define ACS_LRCORNER  0x0103  // ┘
#define ACS_HLINE     0x0104  // ─
#define ACS_VLINE     0x0105  // │
#define ACS_PLUS      0x0106  // ┼
```

### Scrolling
```c
int wscroll(WINDOW *win, int n);
// Scroll window n lines (positive=up, negative=down)

int winsertln(WINDOW *win);
int wdeleteln(WINDOW *win);
```

### Mode Control
```c
// Output modes (waddset)
#define A_WRAP   0x01  // Wrap at edge
#define A_SCROLL 0x02  // Enable scrolling
#define A_PAGE   0x04  // Pagination
#define A_REAL   0x08  // Keep control chars

int waddset(WINDOW *win, int flags);

// Input modes (wgetset)
#define G_ECHO     0x0001  // Echo input
#define G_EDIT     0x0002  // Line editing
#define G_KEYPAD   0x0004  // Special key processing
#define G_NODELAY  0x0008  // Non-blocking input

int wgetset(WINDOW *win, int flags);
```

### Example
```c
#include <curses.h>

int main(void) {
    int ch;

    // Initialize
    incurses("/dev/video");

    // Draw interface
    clear();
    box(stdwin, "My Program", 0, 0);
    mvaddstr(2, 2, "Press any key (q to quit)");
    refresh();

    // Input loop
    while ((ch = getch()) != 'q') {
        mvprintw(4, 2, "You pressed: %c (0x%02X)", ch, ch);
        refresh();
    }

    // Cleanup
    outcurses();
    return 0;
}
```

---

## 4. LIBM486 (Math Library)

### Trigonometric
```c
#include <math.h>

double sin(double x), cos(double x), tan(double x);
double asin(double x), acos(double x), atan(double x);
double atan2(double y, double x);
double sinh(double x), cosh(double x), tanh(double x);
```

### Exponential & Logarithmic
```c
double exp(double x);
double exp2(double x);      // 2^x
double exp10(double x);     // 10^x
double log(double x);       // Natural log
double log2(double x);      // Log base 2
double log10(double x);     // Log base 10
double pow(double x, double y);  // x^y
double sqrt(double x);
double hypot(double x, double y);  // sqrt(x^2 + y^2)
```

### Rounding & Manipulation
```c
double ceil(double x);      // Round up
double floor(double x);     // Round down
double fabs(double x);      // Absolute value
double fmod(double x, double y);     // Floating modulo
double modf(double x, double *iptr); // Split integer/fraction
double frexp(double x, int *exp);    // Extract mantissa/exponent
double ldexp(double x, int exp);     // x * 2^exp
```

### FPU Control (i486-specific)
```c
int fexcep(int mask);
// Enable/disable FPU exceptions
// mask: FP_INVALOPER, FP_DENORMAL, FP_ZERODIV,
//       FP_OVERFLOW, FP_UNDERFLOW, FP_PRECISION

double fhuge(void);  // Return infinity (HUGE_VAL)

// Exception types for matherr()
#define DOMAIN    1  // Argument domain error
#define SING      2  // Singularity
#define OVERFLOW  3  // Overflow
#define UNDERFLOW 4  // Underflow
#define TLOSS     5  // Total loss of precision
#define PLOSS     6  // Partial loss of precision

int matherr(EXCEPTION *exc);
// Custom math error handler
```

### Constants
```c
#define M_PI       3.14159265358979323846
#define M_E        2.71828182845904523536
#define M_LOG2E    1.44269504088896340736
#define M_LOG10E   0.43429448190325182765
#define M_LN2      0.69314718055994530942
#define M_LN10     2.30258509299404568402
#define M_SQRT2    1.41421356237309504880
#define HUGE_VAL   (fhuge())  // Infinity
```

---

## 5. System Calls & Kernel Interface

### Process Management
```c
#include <unistd.h>
#include <sys/types.h>

pid_t fork(void);
// Create child process

int thread(void);
// Create thread (Tropix-specific, lightweight process)

void exit(int status);
pid_t wait(int *status);
pid_t waitpid(pid_t pid, int *status, int options);

int execl(const char *path, const char *arg, ...);
int execv(const char *path, char *const argv[]);
int execvp(const char *file, char *const argv[]);

pid_t getpid(void);
pid_t getppid(void);
uid_t getuid(void);
gid_t getgid(void);
```

### File Operations
```c
#include <fcntl.h>
#include <sys/stat.h>

int open(const char *pathname, int flags, mode_t mode);
// flags: O_RDONLY, O_WRONLY, O_RDWR, O_CREAT, O_TRUNC, O_APPEND
//        O_LOCK (Tropix: exclusive access)
//        O_PHYS (Tropix: physical addressing)

int close(int fd);
ssize_t read(int fd, void *buf, size_t count);
ssize_t write(int fd, const void *buf, size_t count);
off_t lseek(int fd, off_t offset, int whence);

int stat(const char *path, struct stat *buf);
int fstat(int fd, struct stat *buf);
int chmod(const char *path, mode_t mode);
int chown(const char *path, uid_t owner, gid_t group);

int mkdir(const char *pathname, mode_t mode);
int rmdir(const char *pathname);
int unlink(const char *pathname);
int link(const char *oldpath, const char *newpath);
int rename(const char *oldpath, const char *newpath);
```

### Signal Handling
```c
#include <signal.h>

typedef void (*sighandler_t)(int);

sighandler_t signal(int signum, sighandler_t handler);
int kill(pid_t pid, int sig);
int raise(int sig);
unsigned int alarm(unsigned int seconds);
int pause(void);

// Standard signals
#define SIGHUP   1   // Hangup
#define SIGINT   2   // Interrupt (Ctrl-C)
#define SIGQUIT  3   // Quit
#define SIGILL   4   // Illegal instruction
#define SIGTRAP  5   // Trace trap
#define SIGABRT  6   // Abort
#define SIGBUS   7   // Bus error
#define SIGFPE   8   // Floating point exception
#define SIGKILL  9   // Kill (uncatchable)
#define SIGUSR1  10  // User-defined 1
#define SIGSEGV  11  // Segmentation violation
#define SIGUSR2  12  // User-defined 2
#define SIGPIPE  13  // Broken pipe
#define SIGALRM  14  // Alarm clock
#define SIGTERM  15  // Termination
```

### Process Synchronization (Tropix-specific)
```c
#include <sys/sema.h>

// Semaphore operations
int sema(int cmd, int arg1, int arg2);
```

### Memory Management
```c
void *sbrk(intptr_t increment);
int brk(void *addr);

// Tropix-specific shared memory
void *shmem(key_t key, size_t size, int flags);
```

### I/O Control
```c
#include <sys/ioctl.h>

int ioctl(int fd, unsigned long request, ...);

// Common requests
#define TCINTERNET  // Mark as internet device
#define TC_IS_ETHERNET  // Check if ethernet
#define TC_GET_ETHADDR  // Get MAC address
#define TCGETS      // Get termios
#define TCSETS      // Set termios
```

---

## 6. Tropix-Specific Features

### Real-Time Priority Control
```c
#include <sys/pcntl.h>

// Set process priority
pcntl(P_SETPRI, pid, priority);

// Make real-time process
pcntl(P_SETRTPROC, pid, 0);

// Get/set stack size
pcntl(P_GETSTKRGSZ, pid, 0);
pcntl(P_SETSTKRGSZ, pid, new_size);
```

### Physical Memory Access (requires superuser)
```c
#include <sys/pcntl.h>

// Get physical address
ulong phys_addr = pcntl(P_GET_PHYS_ADDR, virtual_addr, 0);

// Direct physical I/O
phys(fd, mode, addr, count);

// Enable/disable I/O port access
pcntl(P_ENABLE_USER_IO, 0, 0);
pcntl(P_DISABLE_USER_IO, 0, 0);
```

### Threading Model
```c
// Lightweight thread creation
int tid = thread();
if (tid == 0) {
    // Child thread
    // ...
    exit(0);
}
// Parent continues
```

### Identity Types
```c
#include <sys/id.h>

// ID type (short identifier)
typedef struct { char id[8]; } ID;
#define IDCAN(id)   // Check if valid
#define IDCPY(d,s)  // Copy
#define IDEQ(a,b)   // Compare equality

// LID type (long identifier)
typedef struct { char lid[16]; } LID;
// Similar macros: LIDCAN, LIDCPY, LIDEQ

// SID type (security identifier)
typedef struct { char sid[32]; } SID;
// Similar macros: SIDCAN, SIDCPY, SIDEQ
```

---

## 7. Porting Guide

### From BSD Sockets to XTI

**BSD Socket Code:**
```c
int sock = socket(AF_INET, SOCK_STREAM, 0);
struct sockaddr_in addr;
addr.sin_family = AF_INET;
addr.sin_port = htons(80);
inet_aton("192.168.0.1", &addr.sin_addr);
connect(sock, (struct sockaddr*)&addr, sizeof(addr));
send(sock, buf, len, 0);
recv(sock, buf, len, 0);
close(sock);
```

**Tropix XTI Equivalent:**
```c
int fd = t_open("/dev/itntcp", O_RDWR, NULL);
INADDR addr;
t_bind(fd, NULL, 0);
t_str_to_addr(fd, "192.168.0.1:80", &addr);
t_connect(fd, &addr, NULL);
t_snd(fd, buf, len, 0);
t_rcv(fd, buf, len, NULL);
t_close(fd);
```

### Threading Considerations

**POSIX Threads:**
```c
pthread_t tid;
pthread_create(&tid, NULL, func, arg);
pthread_join(tid, NULL);
```

**Tropix Threads:**
```c
int tid = thread();
if (tid == 0) {
    func(arg);
    exit(0);
}
wait(NULL);
```

### Key Differences from Standard Unix

1. **No BSD sockets** - Use XTI (libxti)
2. **thread() instead of pthread_create()** - Simpler threading
3. **Real-time extensions** - P_SETRTPROC, priority control
4. **Identity types** - ID, LID, SID for tracking
5. **Physical I/O** - Direct hardware access via phys()
6. **Case-insensitive strings** - stricmp, strtcmp variants
7. **Stack allocation** - smalloc() for temporary allocations

### Compilation

```bash
cc -o program program.c           # Basic compilation
cc -o program program.c -lxti     # Link with networking
cc -o program program.c -lcurses  # Link with terminal library
cc -o program program.c -lm486    # Link with math library
```

### Common Porting Issues

1. **Network code** - Replace socket API with XTI
2. **Threading** - Use thread() instead of pthreads
3. **Terminal I/O** - termios similar but use curses for full-screen
4. **Signals** - Mostly compatible, but integrate with Tropix events
5. **File I/O** - Compatible, but note O_LOCK, O_PHYS extensions

---

## Quick Reference Card

### Headers
```c
#include <stdio.h>      // Standard I/O
#include <stdlib.h>     // Memory, utilities
#include <string.h>     // String functions
#include <unistd.h>     // System calls
#include <fcntl.h>      // File control
#include <signal.h>     // Signals
#include <xti.h>        // Networking (XTI)
#include <curses.h>     // Terminal/screen
#include <math.h>       // Math functions
#include <sys/types.h>  // Type definitions
#include <sys/stat.h>   // File status
#include <sys/pcntl.h>  // Process control (Tropix)
#include <sys/sema.h>   // Semaphores (Tropix)
```

### Compilation
```bash
cc program.c                    # Compile
cc -o name program.c            # Specify output
cc program.c -lxti              # Network library
cc program.c -lcurses           # Terminal library
cc program.c -lm486             # Math library
cc -O program.c                 # Optimize
cc -g program.c                 # Debug symbols
```

### Networking Quick Start
```c
// TCP client
fd = t_open("/dev/itntcp", O_RDWR, NULL);
t_bind(fd, NULL, 0);
t_str_to_addr(fd, "host:port", &addr);
t_connect(fd, &addr, NULL);
t_snd(fd, data, len, 0);
t_rcv(fd, data, len, NULL);
t_close(fd);

// UDP
fd = t_open("/dev/itnudp", O_RDWR, NULL);
t_bind(fd, &local_addr, 0);
t_sndudata(fd, &unitdata);
t_rcvudata(fd, &unitdata, NULL);
t_close(fd);
```

---

## Resources

- System headers: `/usr/include/`
- Man pages: `man <function>`
- Source code: `/usr/src/lib/`
- Examples: `/usr/src/cmd/` (command sources)

For detailed documentation on specific functions, use:
```bash
man <function_name>
man xti          # XTI overview
man curses       # Curses overview
man intro        # System introduction
```
