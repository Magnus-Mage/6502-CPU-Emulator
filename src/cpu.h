#include <stdio.h>
#include <stdlib.h>

// Some helpful things


using Byte = unsigned char;
using Word = unsigned short;
  
using u32  = unsigned int;
using s32  = signed int;
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
  
  // Read the least significant byte off the memory
  // Write two bytes
  void WriteWord(Word Value, u32 Address, u32& Cycles)
  {
    Data[Address]     = Value & 0xFF;
    Data[Address + 1] = (Value >> 8);
    Cycles -= 2;
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

  Word FetchWord(u32& Cycles, Mem& memory)
  {
    // 6502 is little endian
    Word Data = memory[PC];
    PC++;

    Data |= (memory[PC] << 8);
    PC++;
    Cycles -= 2;

    // if you want to handle endian
    // you would have to swap bytes here 
    // if (PLATFORM_BIG_ENDIAN)
    // SwapByteInWord(Data)

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
    INS_LDA_IM  = 0xA9,
    INS_LDA_ZP  = 0xA5,
    INS_LDA_ZPX = 0xB5,
    INS_JSR     = 0x20;
 
  void LDASetStatus()
  {
    Z = (A == 0);
    N = (A & 0b10000000) > 0;
  }
  
  /** @return the number of cycles that were used */
  s32 Execute (u32 Cycles, Mem& memory)
  {
    const u32 CyclesRequested = Cycles;
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
        case INS_LDA_ZPX:
          {
            Byte ZeroPageAddrX = 
              FetchByte(Cycles, memory);
            ZeroPageAddrX += X;
            Cycles--;
            A = ReadByte(Cycles, ZeroPageAddrX, memory);
            LDASetStatus();
          }
        break;
        case INS_JSR:
          {
            Word SubAddr = FetchWord(Cycles, memory);
            Word ReturnAddr = PC - 1;

            // Push high byte
            memory[0x0100 + SP] = (ReturnAddr >> 8);
            SP--;
            Cycles--;

            // Push low byte
            memory[0x0100 + SP] = ReturnAddr & 0xFF;
            SP--;
            Cycles--;

            PC = SubAddr;
            Cycles--;
          }
        break;
        default:
          {
            printf("Instruction not handled %d", Ins); 
          }
        break;
      }
    }
    return CyclesRequested - Cycles;
  }
};

/*
int main () 
{
  
  Mem mem;
  
  CPU cpu;
  
  

  cpu.Reset(mem);
  
  // start - inline a little program 
  mem[0xFFFC] = CPU::INS_JSR;
  mem[0xFFFD] = 0x42;
  mem[0xFFFE] = 0x42;
  mem[0x4242] = CPU::INS_LDA_IM;
  mem[0x4243] = 0x84;
  // end - inline a little program
  printf("Memory at 0xFFFC: %d (JSR opcode)\n", mem[0xFFFC]);
  printf("Memory at 0xFFFD: %d (low byte of address)\n", mem[0xFFFD]);
  printf("Memory at 0xFFFE: %d (high byte of address)\n", mem[0xFFFE]);
  printf("Memory at 0x4242: %d (LDA opcode)\n", mem[0x4242]);
  printf("Memory at 0x4243: %d (immediate value)\n", mem[0x4243]);
  

  cpu.Execute(8 , mem);

  printf("After execution, A register: %d\n", cpu.A);

  return 0;
}
*/
