#ifndef _TINY_ASSEMBLER_H
#define _TINY_ASSEMBLER_H

#define MAXLINE 100


#define CMD_NOP "NOP"
#define CMD_ADD "ADD"
#define CMD_SHIFT "SHIFT"
#define CMD_XOR "XOR"
#define CMD_READ "READ"
#define CMD_WRITE "WRITE"
#define CMD_PRINT "PRINT"
#define CMD_PUT "PUT"
#define CMD_MOV "MOV"

enum op_id
{
  op_nop = 0, // (do nothing)
  op_add,     // r[x] <- r[y] + r[z]
  op_shift,   // r[x] <- r[y] << r[z]
  op_xor,     // r[x] <- r[y] ^ r[z]
  
  op_read,    // r[x] <- readmem(r[y])
  op_write,   // writemem(r[y],r[x])
  op_print,   // printf("%d",r[x])
  op_put,     // putc(r[x])
  op_mov,     // r[x] <- yx
};

extern char  g_line[];
extern int   g_line_no;


typedef int  (*parse_func) (char *args);
typedef void (*print_func) (int op);
struct _cmd
{
    char *cmd;
    int arglen;
    op_id opcode;
    parse_func parser;
    print_func printer;
};


void check_line ();
char *trim_line (char *line);

int parse_zero (char *args);
int parse_one_reg (char *args);
int parse_two_reg (char *args);
int parse_three_reg (char *args);
int parse_two_reg_ltr (char *args);

/* 
    opcode format:
    x, y, z: register id (3 bits, 0 ~ 7)
  
    12 ~ 9 | 8~6 | 5~3 | 2~0
    op_id |  x  |  y  |  z
    op_id |  x  |    imm

    (op_mov<<9)   | (0<<6) | 3,               // mov r0, 3
    (op_mov<<9)   | (1<<6) | 5,               // mov r1, 5
    (op_add<<9)   | (0<<6) | (0<<3) | (1<<0), // add r0, r0, r1
    (op_print<<9) | (0<<6) ,                  // print r0
    (op_mov<<9)   | (1<<6) | '\n',            // mov r1, '\n'
    (op_put<<9)   | (1<<6) ,                  // put r1
*/

#endif