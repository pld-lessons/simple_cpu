#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


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


void print_zero (int op)
{
}
void print_one_reg (int op)
{
    printf("r%d\n", ((op>>6) & 7));
}
void print_two_reg (int op) 
{
    printf("r%d, r%d\n", ((op>>6) & 7), ((op>>3) & 7));
}
void print_three_reg (int op)
{
    printf("r%d, r%d,r%d\n", ((op>>6) & 7), ((op>>3) & 7), ((op>>0) & 7));
}
void print_two_reg_ltr (int op)
{
    printf("r%d, %d\n", ((op>>6) & 7), (op & 63));
}


struct _cmd dictionary [] =
{
    { CMD_NOP, 0, op_nop, NULL, &print_zero },
    { CMD_ADD, 3, op_add, NULL, &print_three_reg },
    { CMD_SHIFT, 1, op_shift, NULL, &print_three_reg },
    { CMD_XOR, 3, op_xor, NULL, &print_three_reg },
    { CMD_READ, 2, op_read, NULL, &print_two_reg },
    { CMD_WRITE, 2, op_write, NULL, &print_two_reg },
    { CMD_PRINT, 1, op_print, NULL, &print_one_reg },
    { CMD_PUT, 1, op_put, NULL, &print_one_reg },
    { CMD_MOV, 2, op_mov, NULL, &print_two_reg_ltr }
};

// Disssemble the command
void disassemble (int op, struct _cmd cmd)
{
    printf ("%s ", cmd.cmd);
    cmd.printer(op);
}



int main (int argc, char **argv)
{
    FILE *fr = NULL;
    char *line = NULL;

    // check the input
    if (argc != 2)
    {
	printf ("Usage: %s infile\n", argv[0]);
	exit (-1);
    }

    char *infile = argv[1];

    // open the files
    if ((fr = fopen (infile, "r")) == NULL)
    {
	printf ("Cannot open infile. infile: %s\n", infile);
	exit (-2);
    }

    int i = 0;
    int len = sizeof(dictionary) / sizeof(dictionary[0]);

    int opcode;
    int op;

    // loop through the input
    while (fread (&opcode, sizeof(opcode), 1, fr) != NULL)
    {
	op = opcode >> 9;

	// loop through the dictionary to find the command
	for (i = 0; i < len; i++)
	{
	    if (dictionary[i].opcode == op)
	    {
		disassemble (opcode, dictionary[i]);
		break;
	    }
	}

	if (i == len)
	{
	    printf ("UNKNOWN command [0x%x].\n", opcode);
	    exit (-6);
	}
    }
    fclose(fr);
}
