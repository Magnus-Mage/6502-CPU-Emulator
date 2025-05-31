#include <cstdio>
#include "cpu.h"
#include "gtest/gtest.h"

class M6502Test1 : public testing::Test
{
public:
  Mem mem;
  CPU cpu;

  virtual void SetUp()
  {
    cpu.Reset(mem);
  }

  virtual void TearDown()
  {
    
  }
};


TEST_F(M6502Test1, LDAImmediateCanLoadAValueIntoTheRegister)
{
  // start - inline a little program 
  mem[0xFFFC] = CPU::INS_LDA_IM;
  mem[0xFFFD] = 0x84;
  // end - inline a little program
  
  // when:
  cpu.Execute(2 , mem);

  // then:
  EXPECT_EQ(cpu.A, 0x84);
  
  
  printf("After execution, A register: %d\n", cpu.A);

}


TEST_F(M6502Test1, LDAZeroPageCanLoadAValueIntoTheRegister)
{
  // start - inline a little program 
  mem[0xFFFC] = CPU::INS_LDA_ZP;
  mem[0xFFFD] = 0x42;
  mem[0x0042] = 0x37;
  // end - inline a little program
  
  // when:
  cpu.Execute(3 , mem);

  // then:
  EXPECT_EQ(cpu.A, 0x37);
  
  
  printf("After execution, A register: %d\n", cpu.A);

}
