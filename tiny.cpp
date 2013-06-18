#include <stdio.h>
#include <stdlib.h>

#define MEM_SIZE 8192
#define MEM_MASK (MEM_SIZE-1)

/*
op_id:
r <- register r[0] -> register 0
*/
enum op_id
{
  op_nop = 0, // (do nothing)
  op_add, //   r[x] <- r[y] + r[z]
  op_shift, // r[x] <- r[y] << r[z]
  op_xor, //   r[x] <- r[y] ^ r[z]
  
  op_read, //  r[x] <- readmem(r[y])
  op_write, // writemem(r[y],r[x])
  op_print, // printf("%d",r[x])
  op_put,   // putc(r[x])
  op_mov, 	// r[x] <- yx
};
/* 
  opcode format:
  x, y, z: register id (3 bits, 0 ~ 7)
  
  12 ~ 9 | 8~6 | 5~3 | 2~0
   op_id |  x  |  y  |  z
   op_id |  x  |    imm
*/

//mov x, 0 -> 

int memory[MEM_SIZE] = { 
	//(op_mov<<9)   | (0<<6) | 3, //mov r0, 3
	(op_mov<<9)   | (1<<6) | 5, //mov r1, 5

	(op_add<<9)   | (0<<6) | (0<<3) | (1<<0), //add r0, r0,r1

	(op_print<<9) | (0<<6) , //print r0



	(op_mov<<9)   | (1<<6) | '\n', //mov r1, '\n'

	(op_put<<9) | (1<<6) , //op_put r1


};

//these are the same address on memory

//0x1000_0000
//0x2000_0000
//0x3000_0000

int readmem(int where)
{
	return memory[where & MEM_MASK]; // mask memory to avoid buffer overflow
}

void writemem(int where, int what)
{
  memory[where & MEM_MASK]=what; // mask memory to avoid buffer overflow
}

int reg[8];
int pc;

bool running = true;

void put(const char* s) { puts(s); }
void put(int ch) { putc(ch, stdout); }

int main(int argc, const char** argv)
{

  while(running)
  {
    //fetch
    int op = readmem(pc);
    pc++;
    
    //decode
    int op_id = op >> 9;
    int x = (op >> 6) & 7; //shift by 6, and take 3 bits (2^3-1 = 7)
                            //x & 0b000000111 (masking)
                            //0x -> hex
                            //0x0005_1234 <- nice formating
    
	int y = (op >> 3) & 7;	//bits 5 to 3
    int z = (op >> 0) & 7;	//bits 2 to 0
    int imm = (op >> 0) & 63; //bits 5 to 0

    switch(op_id)
    {
      case op_nop: //do nothing
        
        break;
        
      case op_add: //   x <- y + z
        reg[x] = reg[y] + reg[z];
        break;

      case op_shift: // x <- y << z
      reg[x] = reg[y] << reg[z];
        break;
      
      case op_xor: //   x <- y ^ z
        reg[x] = reg[y] ^ reg[z];
        break;
        
      case op_read: //  x <- readmem(y)
        if ( z !=0 )
          put("ERROR");
        else
          reg[x] = readmem(reg[y]);
        break;
      
      case op_write: // writemem(y,x)
        if ( z !=0 )
          put("ERROR");
        else
          writemem(reg[y],reg[x]);
        break;      
      
      case op_print: //printf("%d",r[x])
        if ( y !=0 || z !=0 )
          put("ERROR");
        else
          printf("%d",reg[x]);
        break;

	case op_put: // putc(x)
        if ( y !=0 || z !=0 )
          put("ERROR");
        else
          put(reg[x]);
        break;

	  case op_mov:
		  reg[x] = imm;
		  break;
    }
  }
}