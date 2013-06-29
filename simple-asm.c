/*
 * simple-asm.c
 *
 *  Created on: Jun 28, 2013
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>

#define SASM_ISC 1 //is character postion in parsed string
#define SASM_ISSPCL 2 //is special character position
#define SASM_MEMSIZE 1024

int isComment(char* s){
	//just check string and
	//returns 0 if its first char is comment (c-style)

	if(*s == '/'){
		return 0;
	}else{
		return -1;
	}
}

int trimComments(char *s,char *substring){
	//returns 0 and a pointer to a clean substring
	//in case : ADD r0,r1; //Lorem Ipsum
	//or -1 if the string is comment and the copy of the first string

	if(isComment(s) == 0){
		return -1;
	}

	int index = 0;
	index = strcspn(s, "//");

	//memory alloc from strcpy returns new string
	//strcspn returns the string until the character position
	//if the character we are looking for isnt found returns full strings length

	if( index < strlen(s)){
		strncpy(substring, s, index);
		return 0;
	}else{
		strcpy(substring,s);
	}

	return -1;
}

//finally


int getRegId(int s[],int l,int op_bit){

	//takes an array with the pointer "addresses"
	//the length of the array and the biggest bit for register position



	int bits = 3;

	op_bit -= bits;

	//returns registers values or -1 on error

	int reg_id = 0;
	int reg = 0;

	for(int i = 0; i < l; i++){
		reg = s[i];

		if(i == 0){
			reg_id = ( reg << op_bit);
			op_bit -= bits;
			continue;
		}

		reg_id |= (reg << op_bit);
		op_bit -= bits;
		if(op_bit < 0 ){
			break;
		}

	}

	return reg_id;
}

int getOpId(char * s){

	//gets a comment free string as a parameter from trim comments

	//returns command op code

	//as an idea, is a good case to
	//declare prototype in a header
	//and each command as a pointed function ??
	//like semi psedocode : if(..add){ function = (*add(s))}
	//						if(..mov){ function = (*mov(s))}
	//						function(s);



	//op_id bit
	int op_bit = 9;

	//all op_codes
	int op_add = 1;
	int op_mov = 8;


	//reg index
	int index = 0;

	int l = 3; //literal command length
	char cmd[3];

	int op_code = -1;

	//get command

	if(s != '\0'){
		char c;

		for(int i = 0; i < l;i++){
			c = s[i];

			if(c == ' '){
				cmd[i] = '\0';
				break;
			}
			index++;
			cmd[i] = s[i];
		}


		//compares and definitions
		//here we can put all command definitions

		int reg_id;

		if(strcasecmp("add",cmd) == 0){

			op_code = op_add<<9;

			//parse options

			int options_needed = 3;

			int options[options_needed];

			int opts_index = 0;

			//iterates until we fill the options array
			for(int i = index+1;opts_index < options_needed; i++){
				c = s[i];
				if(c == ';'){
					break;
				}
				if(c == 'r'){
					options[opts_index] = atoi(&s[i+1]);
					opts_index++;
				}

			}

			if(opts_index < options_needed){
				//if optios is ADD r0,r1 converts to r0,r0,r1
				//swap values , converting from r0,r1 to r0,r0,r1
				options[2] = options[1];
				options[1] = options[0];
			}

			//get the register values ids
			reg_id = getRegId(options,options_needed,op_bit);
			op_code |= reg_id;

		}

		//mov command
		//more or less same as above
		if(strcasecmp("mov",cmd) == 0){

			op_code = op_mov<<9;

			int options_needed = 2;

			int options[options_needed];

			int opts_index = 0;

			for(int i = index+1;opts_index < options_needed; i++){
				c = s[i]; //curent character

				if(c == ';'){
					break;
				}

				if(c == 'r'){
					i++;
					c = s[i];
					options[opts_index] = atoi(&c);
					opts_index++;
					continue; //prevent matching to the next general case
				}

				if((opts_index > 0) & (c !=',') ){

					//check special char and chars

					if(c == '\''){

						c = s[i+SASM_ISC]; //plus one position of buffer s from "'"

						if(c == ';'){
							break;
						}

						//if string contains \,asume is special
						if(c == '\\'){
							c = s[i+SASM_ISSPCL]; //plus one

							//lookup table :(

							switch(c)
							{
							case 'n':
								c = '\n';
								break;
							case 't':
								c = '\t';
								break;
							}

						}

						options[opts_index] = c;
						break;
					}else{
						//assume is number
						options[opts_index] = atoi(&s[i]);
						break;
					}
				}

			}

			options_needed = 1;

			//for this case we pass only the register from the array
			//we use the literal inside this function

			reg_id = getRegId(options,options_needed,op_bit);

			//here we pass the literal
			op_code |= reg_id | options[1];
		}

	}

	return op_code;
}



int wbuf[SASM_MEMSIZE];
char buf[SASM_MEMSIZE];


char *cmd, *arg;
FILE *inputfile;
FILE *outfile;

int main(int args, char* argv[]) {
	char string1[20];
	char *filename;

	if( (args < 2) | (args > 4)){
		return 0;
	}

	if( args == 3){
		filename = argv[2];
	}else{
		filename = "simple_cpu.bin"; //if we dont have a 3 argument use this name as default
	}


	inputfile = fopen(argv[1],"r");

	outfile = fopen(filename,"w");

	int op_code;
	int z = 0;

	memset(wbuf,0,sizeof(wbuf)); //maybe good practice to fill buffer with zeros

	while(fgets(buf,sizeof buf,inputfile)){

		trimComments(buf,string1);
		op_code = getOpId(string1);
		if(op_code != -1){

			//maybe we could also parse one time with trimcomments and count line num as size
			//buffers could be replaced with malloc

			wbuf[z] = op_code;

			if(z >= (SASM_MEMSIZE-1)){
				break; //just in case
			}

			z++; //number of opcodes
		}


	}

	fwrite(wbuf,sizeof(int),z,outfile); //write buffer to binary file
	fclose(inputfile);
	fclose(outfile);
	return 0;
}

