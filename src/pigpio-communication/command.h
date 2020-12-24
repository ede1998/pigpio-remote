/*
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org/>
*/

/*
This version is for pigpio version 70+
*/

#ifndef COMMAND_H
#define COMMAND_H

#include <stdio.h>
#include <string.h>

#define CMD_MAX_PARAM 512
#define CMD_MAX_EXTENSION (1<<16)

#define CMD_UNKNOWN_CMD   -1
#define CMD_BAD_PARAMETER -2
#define CMD_EXT_TOO_SMALL -3
#define CMD_NOT_IN_SCRIPT -4

#define CMD_P_ARR 10
#define CMD_V_ARR 10

#define CMD_NUMERIC 1
#define CMD_VAR     2
#define CMD_PAR     3

typedef struct
{
   uint32_t Cmd;
   uint32_t P1;
   uint32_t P2;
   union
   {
      uint32_t P3;
      uint32_t ExtLen;
      uint32_t Res;
   };
} cmdCmd_t;

typedef struct
{
   int    Eaten;
   int8_t Opt[4];
} cmdCtlParse_t;

typedef struct
{
   int   Cmd;  /* command number            */
   char *Name; /* command name              */
   int   Vt;   /* command verification type */
   int   Rv;   /* command return value type */
   int   Cvis; /* command valid in a script */
} cmdInfo_t;

typedef struct
{
   uint32_t Tag;
   int      Step;
} cmdTagStep_t;

typedef struct
{
   uintptr_t P[5]; //these are sometimes converted to pointers, so presumablly they sometimes have pointers stored in them, I haven't figured out where though. --plugwash
   int8_t Opt[4];
} cmdInstr_t;

typedef struct
{
   /*
     +-----------+---------+---------+----------------+
     | PARAMS... | VARS... | CMDS... | STRING AREA... |
     +-----------+---------+---------+----------------+
   */
   int *Par;
   int *Var;
   cmdInstr_t *Instr;
   int Instrs;
   char *StrArea;
   int StrAreaLen;
   int StrAreaPos;
} cmdScript_t;

extern cmdInfo_t cmd_info[];

extern char *cmd_usage;

int CmdParse(char *buf, uintptr_t *p, unsigned ext_len, char *ext, cmdCtlParse_t *ctl);

int CmdParseScript(char *script, cmdScript_t *s, int diags);

char *CmdErrStr(int error);

char *CmdStr(void);

#endif

