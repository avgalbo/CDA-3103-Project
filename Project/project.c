// Anthony Galbo
// CDA 3103, Spring 2021
// an577845

#include "spimcore.h"

// Function that implements the operations on input paramaters A and B according
// to ALUControl and outputs them to ALUresult.
void ALU(unsigned A,unsigned B,char ALUControl,unsigned *ALUresult,char *Zero)
{
  int cmp = (int)ALUControl;

  // Case where cmp is equal to 000, if so add A and B.
  if (cmp == 0b000)
    *ALUresult = A + B;

  // Case where cmp is equal to 001, if so subtract A and B.
  else if (cmp == 0b001)
    *ALUresult = A - B;

  // Case where cmp is equal to 010, if so compare A and B, if A is less than B
  // set ALUresult to 1, otherwise set ALUresult to 0.
  else if (cmp == 0b010)
    *ALUresult = ((int)A < (int)B) ? 1 : 0;

  // Case where cmp is equal to 011, if so compare A and B, if A is less than B
  // set ALUresult to 1, otherwise set ALUresult to 0 where A and B are unsigned
  // integers.
  else if (cmp == 0b011)
    *ALUresult = (A < B) ? 1 : 0;

  // Case where cmp is equal to 100, if so preform bitwise AND operation on A
  // and B
  else if (cmp == 0b100)
    *ALUresult = A & B;

  // Case where cmp is equal to 101, if so preform bitwise OR operation on A
  // and B
  else if (cmp == 0b101)
    *ALUresult = A | B;

  // Case where cmp is equal to 110, if so preform bitwise LEFT SHIFT operation
  // on B by 16
  else if (cmp == 0b110)
    *ALUresult = B << 16;

  // Case where cmp is equal to 111, if so preform bitwise NOT operation on A
  else if (cmp == 0b111)
    *ALUresult = ~A;

  // Case where if ALUresult is equal to 0, set Zero variable to 1 otherwise 0.
  *Zero = (*ALUresult == 0) ? 1 : 0;
}

// Function simulates our instruction memory where Program Counter is
// retreving the 32 bit instruction from that location.
int instruction_fetch(unsigned PC,unsigned *Mem,unsigned *instruction)
{
  // Case where PC is not divisible by 4, halt condition occurs so return 1.
  if (PC % 4 != 0)
    return 1;

  // Otherwise fetch the instruction and preform a bitwise right shift on
  // Program Counter by 2.
  *instruction = Mem[PC >> 2];
  return 0;
}

// Function that takes in the instruction and sets up all of the other pieces
// that other parts of the data path might need.
void instruction_partition(unsigned instruction, unsigned *op, unsigned *r1,
                           unsigned *r2, unsigned *r3, unsigned *funct,
                           unsigned *offset, unsigned *jsec)
{
  // Instruction [31-26]
  *op = (instruction >> 26) & 0b00000000000000000000000000111111;

  // Instruction [25-21]
  *r1 = (instruction >> 21) & 0b00000000000000000000000000011111;

  // Instruction [20-16]
  *r2 = (instruction >> 16) & 0b00000000000000000000000000011111;

  // Instruction [15-11]
  *r3 = (instruction >> 11) & 0b00000000000000000000000000011111;

  // Instruction [5-0]
  *funct = (instruction >> 0) & 0b00000000000000000000000000111111;

  // Instruction [15-0]
  *offset = (instruction >> 0) & 0b00000000000000001111111111111111;

  // Instruction [25-0]
  *jsec = (instruction >> 0) & 0b00000011111111111111111111111111;
}

// Function that determines the opcode.
int instruction_decode(unsigned op,struct_controls *controls)
{
  // R-Type case where op is equal to 000000.
  if (op == 0b000000)
  {
    controls->RegDst = 1;
    controls->Jump = 0;
    controls->Branch = 0;
    controls->MemRead = 0;
    controls->MemtoReg = 0;
    controls->ALUOp = 7; // (further decoded by ALU)
    controls->MemWrite = 0;
    controls->ALUSrc = 0;
    controls->RegWrite = 1;
  }

  // J-Type case where op is equal to 000010
  else if (op == 0b000010)
  {
    controls->RegDst = 2;
    controls->Jump = 1;
    controls->Branch = 0;
    controls->MemRead = 0;
    controls->MemtoReg = 2;
    controls->ALUOp = 2; // (default add)
    controls->MemWrite = 0;
    controls->ALUSrc = 2;
    controls->RegWrite = 1;
  }

  // I-Type case where op is equal to 000100
  else if (op == 0b000100)
  {
    controls->RegDst = 2;
    controls->Jump = 0;
    controls->Branch = 1;
    controls->MemRead = 0;
    controls->MemtoReg = 2;
    controls->ALUOp = 1; // (subtract)
    controls->MemWrite = 0;
    controls->ALUSrc = 0;
    controls->RegWrite = 0;
  }

  // Case addi where op is equal to 001000
  else if (op == 0b001000)
  {
    controls->RegDst = 0;
    controls->Jump = 0;
    controls->Branch = 0;
    controls->MemRead = 0;
    controls->MemtoReg = 0;
    controls->ALUOp = 0;
    controls->MemWrite = 0;
    controls->ALUSrc = 1;
    controls->RegWrite = 1;
  }

  // Case slti where op is equal to 001010
  else if (op == 0b001010)
  {
		controls->RegDst = 0;
		controls->Jump = 0;
		controls->Branch = 0;
		controls->MemRead = 0;
		controls->MemtoReg = 0;
		controls->ALUOp = 2;
		controls->MemWrite = 0;
		controls->ALUSrc = 1;
		controls->RegWrite = 1;
	}

  // Case sltiu where op is equal to 001011
  else if (op == 0b001011)
  {
		controls->RegDst = 0;
		controls->Jump = 0;
		controls->Branch = 0;
		controls->MemRead = 0;
		controls->MemtoReg = 0;
		controls->ALUOp = 3;
		controls->MemWrite = 0;
		controls->ALUSrc = 1;
		controls->RegWrite = 1;
	}

  // Case lui where op is equal to 001111
  else if (op == 0b001111)
  {
		controls->RegDst = 0;
		controls->Jump = 0;
		controls->Branch = 0;
		controls->MemRead = 0;
		controls->MemtoReg = 0;
		controls->ALUOp = 6;
		controls->MemWrite = 0;
		controls->ALUSrc = 1;
		controls->RegWrite = 1;
	}

  // Case lw where op is equal to 100011
  else if (op == 0b100011)
  {
		controls->RegDst = 0;
		controls->Jump = 0;
		controls->Branch = 0;
		controls->MemRead = 1;
		controls->MemtoReg = 1;
		controls->ALUOp = 0;
		controls->MemWrite = 0;
		controls->ALUSrc = 1;
		controls->RegWrite = 1;
	}

  // Case sw where op is equal to 101011
  else if (op == 0b101011)
  {
		controls->RegDst = 2;
		controls->Jump = 0;
		controls->Branch = 0;
		controls->MemRead = 0;
		controls->MemtoReg = 2;
		controls->ALUOp = 0;
		controls->MemWrite = 1;
		controls->ALUSrc = 1;
		controls->RegWrite = 0;
	}

  // Case where halt condition occurs
  else
    return 1;

  // Otherwise return 0.
  return 0;
}

// Funciton that reads two values from the register file at the specified
// registers.
void read_register(unsigned r1,unsigned r2,unsigned *Reg,unsigned *data1,
                   unsigned *data2)
{
  // Assign data1 to register sub r1 and data2 to register sub r2
  *data1 = Reg[r1];
	*data2 = Reg[r2];
}

// Function that takes in the 16-bit immediate value and extend it to a
// 32-bit value.
void sign_extend(unsigned offset, unsigned *extended_value)
{
  int negConstant = 0b11111111111111110000000000000000;
  int posConstant = 0b00000000000000001111111111111111;

  // Case of a negative offset, if so, fill with 0's.
  if ((offset >> 15))
    *extended_value = offset | negConstant;

  // Otheriwise, fill with 1's if constant is positive
  else
    *extended_value = offset & posConstant;
}

// Function to set up for the Arithmetic Logic Unit function.
int ALU_operations(unsigned data1,unsigned data2,unsigned extended_value,
                   unsigned funct,char ALUOp,char ALUSrc,unsigned *ALUresult,
                   char *Zero)
{
  char ALUControl;
  int cmp = funct;

  // Case where ALUSrc evaluates to true, then set extended_value to data2.
  if (ALUSrc)
    data2 = extended_value;

  // Case R-Type instruction
  if (ALUOp == 7)
  {
    // Add case
    if (cmp == 0b100000)
      ALUOp = 0b000;

    // Subtract case
    else if (cmp == 0b100010)
      ALUOp = 0b001;

    // And case
    else if (cmp == 0b100100)
      ALUOp = 0b100;

    // OR case
    else if (cmp == 0b100101)
      ALUOp = 0b101;

    // Slt case
    else if (cmp == 0b101010)
      ALUOp = 0b010;

    // Sltu case
    else if (cmp == 0b101011)
      ALUOp = 0b011;

    // Halt Condition
    else
      return 1;

  }

  // Call ALU function.
  ALU(data1, data2, ALUOp, ALUresult, Zero);
  return 0;
}

// Function that simulates the data memory.
int rw_memory(unsigned ALUresult,unsigned data2,char MemWrite,char MemRead,
              unsigned *memdata,unsigned *Mem)
{
  // Case where MemRead and Memwrite evaluates to true and ALUresult mod 4
  // evaluates to true. Halt condition occurs so return 1.
  if (MemRead && MemWrite && (ALUresult % 4))
    return 1;

  // Case where MemWrite = 1, check word alignment and write into memory.
  if (MemWrite)
    Mem[ALUresult >> 2] = data2;

  // Case where MemRead = 1, check word alignment and read from memory.
  if (MemRead)
    *memdata = Mem[ALUresult >> 2];

  return 0;
}

// Function to update the register file with the information to be written
// into it.
void write_register(unsigned r2,unsigned r3,unsigned memdata,unsigned ALUresult,
                    char RegWrite,char RegDst,char MemtoReg,unsigned *Reg)
{
  // Case where data is coming from memory
  if (RegWrite && MemtoReg)
  {
    if (!RegDst)
      Reg[r2] = memdata;
    else
      Reg[r3] = memdata;
  }

  // Case where data is coming from ALUresult
  if (RegWrite && !MemtoReg)
  {
    if (!RegDst)
      Reg[r2] = ALUresult;
    else
      Reg[r3] = ALUresult;
  }
}

// Function that updates the program counter (PC).
void PC_update(unsigned jsec,unsigned extended_value,char Branch,char Jump,
               char Zero,unsigned *PC)
{
  int upper_4_bits = 0b11110000000000000000000000000000;

  // First, increment PC by 4
  *PC += 4;

  // Case where branch is taken or not, if so preform a bitwise left shift on
  // extended_value by 2 and add the result to Program Counter.
  if (Zero && Branch)
    *PC += (extended_value << 2);

  // Jump case: shift bits of jsec by 2 and use upper 4 bits of Program Counter.
  if (Jump)
    *PC = (jsec << 2) | (*PC & upper_4_bits);
}
