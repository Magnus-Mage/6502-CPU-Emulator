#include <stdio.h>
#include <stdlib.h>

// Some helpful things


using Byte = unsigned char;
using Word = unsigned short;
  
using u32  = unsigned int;

// Memory logic


struct Mem
{
  static constexpr u32 MAX_MEM = 1024 * 64;
  Byte Data[MAX_MEM];
  
  // Clear memory
  void Initialise()
  {
    for (u32 i = 0; i < MAX_MEM; i++)
    {
      Data[i] = 0;
    }
  }
  
  // read 1 Byte
  Byte operator[](u32 Address) const
  {

    // asert here Address is < MAX_MEM
    return Data[Address];
  }

  // write 1 byte into the memory
  Byte& operator[](u32 Address)
  {
    // asert here address is < MAX_MEM
    return Data[Address];
  }
};

// CPU logic

struct CPU
{
  Word PC;  // program counter
  Word SP;  // stack pointer for address to the stackn  

  Word A, X, Y; // registers
  
  Byte C : 1; //status flags
  Byte Z : 1;
  Byte I : 1;
  Byte D : 1;
  Byte B : 1;
  Byte V : 1;
  Byte N : 1;

  // Reset or restart the cpu
  void Reset(Mem& memory)
  {
    PC = 0xFFFC;
    SP = 0x00FF;
    C = Z = I = B = V = N = D = 0;
    A = X = Y = 0;
    memory.Initialise();
  }
  
  //Fetch the latest instruction from memory
  Byte FetchByte(u32& Cycles, Mem& memory)
  {
    Byte Data = memory[PC];
    PC++;
    Cycles--;
    return Data;
  }

  Byte ReadByte(u32& Cycles, Byte Address, Mem& memory)
  {
    Byte Data = memory[Address];
    Cycles--;
    return Data;    
  }

  // operation codes 
  // thsi one specifically to get address mode instruction
  static constexpr Byte
    INS_LDA_IM = 0xA9,
    INS_LDA_ZP = 0xA5;
 
  void LDASetStatus()
  {
    Z = (A == 0);
    N = (A & 0b10000000) > 0;
  }


  void Execute (u32 Cycles, Mem& memory)
  {
    while( Cycles > 0)
    {
      Byte Ins = FetchByte(Cycles, memory);
      switch(Ins)
      {
        case INS_LDA_IM:
          {
            Byte Value = FetchByte(Cycles, memory);
            A = Value;
            Z = (A == 0);
            N = (A & 0b10000000) > 0;
          }
        break;
        case INS_LDA_ZP:
          {
            Byte ZeroPageAddr = 
              FetchByte (Cycles, memory);
            A = ReadByte(Cycles, ZeroPageAddr, memory);
            LDASetStatus();
          }
        break;
        default:
          {
            printf("Instruction not handled %d", Ins); 
          }
        break;
      }
    }
  }
};

int main () 
{
  
  Mem mem;
  
  CPU cpu;
  
  

  cpu.Reset(mem);
  
  // start - inline a little program 
  mem[0xFFFC] = CPU::INS_LDA_ZP;
  mem[0xFFFC] = 0x42;
  mem[0x0042] = 0x84;
  // end - inline a little program
  cpu.Execute(2 , mem);

  return 0;
}
