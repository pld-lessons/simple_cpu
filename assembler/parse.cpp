#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "tiny_cpu.h"


// Check functions
void check_line ()
{
    if (strlen (g_line) == MAXLINE - 1)
    {
	printf ("Line: %d\n", g_line_no);
	printf ("%s\n", g_line);
	printf ("Line is too long.\n");
	exit (-3);
    }
}

void check_reg_number(int x)
{
    if (x<0 || x>7)
    {
	printf ("Line: %d\n", g_line_no);
	printf ("%s\n", g_line);
	printf ("Register number [%d] is invalid.\n", x);
	exit(-5);
    }
}

void check_literal(int x)
{
    if (x<0 || x>63)
    {
	printf ("Line: %d\n", g_line_no);
	printf ("%s\n", g_line);
	printf ("Literal value [%d] too large.\n", x);
	exit(-5);
    }
}

void check_comma (char *args)
{
    if (*args != ',') 
    {
	printf ("Line: %d\n", g_line_no);
	printf ("%s\n", g_line);
	printf ("Comma expected.\n");
	exit (-4);
    }
}


// Get functions
char *get_register (char *args, int *x)
{
    if (*args != 'r') 
    {
	printf ("Line: %d\n", g_line_no);
	printf ("%s\n", g_line);
	printf ("Register expected. [r0,r1,...,r7]\n");
	exit (-4);
    }

    args++;

    *x = *(args) - '0';
    check_reg_number(*x);

    args++;

    return args;
}

char *trim_line (char *line)
{
    char *start = line;
    char *end = NULL;

    // skip spaces at start
    while (*start && isspace(*start))
        ++start;

    char *c = start;
    // iterate over the rest remebering last non-whitespace
    while (*c)
    {
	if (!isspace(*c))
	    end = c + 1;

	if (*c == ';')
	{
	    end = c;
	    break;
	}

	c++;
    }

    // white the terminating zero after last non-whitespace
    if (end != NULL)
        *end = 0;

    return start;
}


// Argument variants
// -- (None) [NOP]
// -- r0 [PRINT, PUT]
// -- r0, r1 [READ, WRITE]
// -- r0, r1, r2 [ADD, SHIFT, XOR]

int parse_zero (char *args)
{
    if (*args) 
    {
	printf ("Line: %d\n", g_line_no);
	printf ("%s\n", g_line);
	printf ("Command contains excessive parameters.\n");
	exit (-4);
    }

    return 0;
}

int parse_one_reg (char *args)
{
    int x;

    args = get_register(args, &x);
    x = x << 6;

    args = trim_line (args);
    parse_zero(args);

    return x;
}

int parse_one_ltr (char *args)
{
    int x = atoi(args);
    check_literal(x);

    return x;
}

int parse_two_reg (char *args)
{
    int x;
    args = get_register(args, &x);
    x = x << 6;

    // check comma
    args = trim_line (args);
    check_comma(args);
    args++;

    args = trim_line (args);
    x |= (parse_one_reg(args) >> 3);

    return x;
}

int parse_two_reg_ltr (char *args)
{
    int x;
    args = get_register(args, &x);
    x = x << 6;

    // check comma
    args = trim_line (args);
    check_comma(args);
    args++;

    args = trim_line (args);
    x |= parse_one_ltr(args);

    return x;
}

int parse_three_reg (char *args)
{
    int x;
    args = get_register(args, &x);
    x = x << 6;

    // check comma
    args = trim_line (args);
    check_comma(args);
    args++;

    args = trim_line (args);
    x |= (parse_two_reg(args) >> 3);

    return x;
}
