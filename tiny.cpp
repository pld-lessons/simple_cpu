#include <stdio.h>
#include <stdlib.h>

#define MEM_SIZE 8192
#define MEM_MASK (MEM_SIZE-1)

int memory[MEM_SIZE];

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


/*
op_id:
*/
enum op_id
{
  op_nop = 0, // (do nothing)
  op_add, //   x <- y + z
  op_shift, // x <- y << z
  op_xor, //   x <- y ^ z
  
  op_read, //  x <- readmem(y)
  op_write, // writemem(y,x)
  op_print, // putc(x)
};
 /* 
  opcode format:
  x, y, z: register id (3 bits, 0 ~ 7)
  
  11 ~ 9 | 8~6 | 5~3 | 2~0
   op_id |  x  |  y  |  z
    
*/
void put(const char* s) { puts(s); }
void put(int ch) { putc(ch, stdout); }

int main()
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
    int y = (op >> 3) & 7;
    int z = (op >> 0) & 7;
    
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
      
      case op_print: // putc(x)
        if ( y !=0 || z !=0 )
          put("ERROR");
        else
          put(reg[x]);
        break;
    }
  }
}
