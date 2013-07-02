#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include <dos.h>

#include "tiny_cpu.h"


struct _cmd dictionary [] =
{
    { CMD_NOP, 0, op_nop, &parse_zero },
    { CMD_ADD, 3, op_add, &parse_three_reg },
    { CMD_SHIFT, 1, op_shift, &parse_three_reg },
    { CMD_XOR, 3, op_xor, &parse_three_reg },
    { CMD_READ, 2, op_read, &parse_two_reg },
    { CMD_WRITE, 2, op_write, &parse_two_reg },
    { CMD_PRINT, 1, op_print, &parse_one_reg },
    { CMD_PUT, 1, op_put, &parse_one_reg },
    { CMD_MOV, 2, op_mov, &parse_two_reg_ltr }
};

// Assemble the command
void assemble (char *line, char * cmd, op_id opcode, parse_func parser, FILE *outfile)
{
    // NOP
    char *args = trim_line (line + strlen(cmd));

    // calc opcode
    int op = (opcode << 9) | parser(args);
    fwrite(&op, sizeof(int), 1, outfile);
}


char  g_line[MAXLINE];
int   g_line_no = 0;


int main (int argc, char **argv)
{
    FILE *fr = NULL;
    FILE *fw = NULL;
    char *line = NULL;

    // check the input
    if (argc != 3)
    {
	printf ("Usage: %s infile outfile\n", argv[0]);
	exit (-1);
    }

    char *infile = argv[1];
    char *outfile = argv[2];


    // open the files
    if ((fr = fopen (infile, "rt")) == NULL)
    {
	printf ("Cannot open infile. infile: %s\n", infile);
	exit (-2);
    }

    if ((fw = fopen(outfile, "w+")) == NULL)
    {
	printf ("Cannot open infile. outfile: %s\n", outfile);
	exit (-2);
    }


    int i = 0;
    int len = sizeof(dictionary) / sizeof(dictionary[0]);

    // loop through the input
    while (fgets (g_line, MAXLINE, fr) != NULL)
    {
	// check the line
	g_line_no++;
	check_line();

	// skip empty lines
	line = trim_line(g_line);
	if (!(*line)) 
	    continue;

	// loop through the dictionary to find the command
	for (i = 0; i < len; i++)
	{
	    if (strncmp (line, dictionary[i].cmd, strlen (dictionary[i].cmd)) == 0)
	    {
		assemble (line, dictionary[i].cmd, dictionary[i].opcode, dictionary[i].parser, fw);
		break;
	    }
	}

	if (i == len)
	{
	    printf ("Line: %d\n", g_line_no);
	    printf ("%s\n", g_line);
	    printf ("UNKNOWN command.\n");
	    exit (-6);
	}
    }
    fclose(fr);
    fclose(fw);
}
