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

static void VerifyUnmodifiedFlagsFromLDA(
  const CPU& cpu,
  const CPU& CPUCopy
)
{ 
  EXPECT_EQ(cpu.C, CPUCopy.C);
  EXPECT_EQ(cpu.B, CPUCopy.B);
  EXPECT_EQ(cpu.D, CPUCopy.D);
  EXPECT_EQ(cpu.I, CPUCopy.I);
  EXPECT_EQ(cpu.V, CPUCopy.V);
}

TEST_F( M6502Test1, TheCpuDoesNothingWhenWeExecuteZeroCycles)
{
  //given:
  constexpr s32 NUM_CYCLES = 0;

  //when:
  s32 CyclesUsed = cpu.Execute(NUM_CYCLES,mem);

  //then:
  EXPECT_EQ(CyclesUsed, 0);
}


TEST_F(M6502Test1, CPUCanExecuteMoreCyclesThanRequired)
{
  // given: 
  mem[0xFFFC] = CPU::INS_LDA_IM;
  mem[0xFFFD] = 0x84;
  CPU CPUCopy = cpu; 
  constexpr s32 NUM_CYCLES = 0;

  // when:
  s32 CyclesUsed = cpu.Execute(1 , mem);

  // then:
  EXPECT_EQ(CyclesUsed, 2);
  printf("After execution, A register: %d\n", cpu.A);

}


TEST_F(M6502Test1, ExecutingABadInstructionDoesNotPutUsInAnInfiniteLoop)
{
  // given: 
  mem[0xFFFC] = 0x0; // invalid opcode
  mem[0xFFFD] = 0x0;
  CPU CPUCopy = cpu; 
  constexpr s32 NUM_CYCLES = 1;

  // when:
  s32 CyclesUsed = cpu.Execute(NUM_CYCLES , mem);

  // then:
  EXPECT_EQ(CyclesUsed, NUM_CYCLES);
  printf("After execution, A register: %d\n", cpu.A);

}


TEST_F(M6502Test1, LDAImmediateCanAffectZeroFlag)
{

  // given:
  cpu.A = 0x44;
  mem[0xFFFC] = CPU::INS_LDA_IM;
  mem[0xFFFD] = 0x0;
  // end - inline a little program
  
  // when:
  CPU CPUCopy = cpu; 
  s32 CyclesUsed = cpu.Execute(2 , mem);

  // then:
  EXPECT_EQ(cpu.A, 0x0);
  EXPECT_EQ(CyclesUsed, 2);
  EXPECT_TRUE(cpu.Z);
  EXPECT_FALSE(cpu.N);
  VerifyUnmodifiedFlagsFromLDA(cpu, CPUCopy); 
  
  printf("After execution, A register: %d\n", cpu.A);

}




TEST_F(M6502Test1, LDAImmediateCanLoadAValueIntoTheRegister)
{
  // start - inline a little program 
  mem[0xFFFC] = CPU::INS_LDA_IM;
  mem[0xFFFD] = 0x84;
  // end - inline a little program
  
  // when:
  CPU CPUCopy = cpu; 
  s32 CyclesUsed = cpu.Execute(2 , mem);

  // then:
  EXPECT_EQ(CyclesUsed, 2);
  
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
  CPU CPUCopy = cpu; 
  
  s32 CyclesUsed = cpu.Execute(3 , mem);

  // then:
  EXPECT_EQ(cpu.A, 0x37);
  EXPECT_EQ(CyclesUsed, 3);
  EXPECT_FALSE(cpu.Z);
  EXPECT_FALSE(cpu.N);
  VerifyUnmodifiedFlagsFromLDA(cpu, CPUCopy); 
  
  printf("After execution, A register: %d\n", cpu.A);

}

TEST_F(M6502Test1, LDAZeroPageXCanLoadAValueIntoTheRegister)
{
  // given:
  cpu.X = 5;
  // start - inline a little program 
  mem[0xFFFC] = CPU::INS_LDA_ZPX;
  mem[0xFFFD] = 0x42;
  mem[0x0047] = 0x37;
  // end - inline a little program
  
  // when:
  CPU CPUCopy = cpu; 
  s32 CyclesUsed = cpu.Execute(4 , mem);

  // then:
  EXPECT_EQ(cpu.A, 0x37);
  EXPECT_EQ(CyclesUsed, 4);
  EXPECT_FALSE(cpu.Z);
  EXPECT_FALSE(cpu.N);
  VerifyUnmodifiedFlagsFromLDA(cpu, CPUCopy); 

  printf("After execution, A register: %d\n", cpu.A);

}

TEST_F(M6502Test1, LDAZeroPageCanLoadAValueIntoTheRegisterWhenItWraps)
{
  // given:
  cpu.X = 0xFF;

  // start - inline a little program 
  mem[0xFFFC] = CPU::INS_LDA_ZPX;
  mem[0xFFFD] = 0x80;
  mem[0x007F] = 0x37;
  // end - inline a little program
  
  // when:
  CPU CPUCopy = cpu; 
  s32 CyclesUsed = cpu.Execute(4 , mem);

  // then:
  EXPECT_EQ(cpu.A, 0x37);
  EXPECT_EQ(CyclesUsed, 4);
  EXPECT_FALSE(cpu.Z);
  EXPECT_FALSE(cpu.N);
  VerifyUnmodifiedFlagsFromLDA(cpu, CPUCopy); 
  
  printf("After execution, A register: %d\n", cpu.A);

}


TEST_F(M6502Test1, LDAAbsoluteCanLoadAValueIntoTheRegister)
{
  // given:
  mem[0xFFFC] = CPU::INS_LDA_ABS;
  mem[0xFFFD] = 0x80;
  mem[0xFFFE] = 0x44; // 0x4480
  mem[0x4480] = 0x37;
  constexpr s32 Expected_Cycles = 4;
  
  
  // when:
  CPU CPUCopy = cpu; 
  s32 CyclesUsed = cpu.Execute(Expected_Cycles , mem);

  // then:
  EXPECT_EQ(cpu.A, 0x37);
  EXPECT_EQ(CyclesUsed, Expected_Cycles);
  EXPECT_FALSE(cpu.Z);
  EXPECT_FALSE(cpu.N);
  VerifyUnmodifiedFlagsFromLDA(cpu, CPUCopy); 
  
  printf("After execution, A register: %d\n", cpu.A);

}

TEST_F(M6502Test1, LDAAbsoluteXCanLoadAValueIntoTheRegister)
{
  // given:
  cpu.X = 1;
  mem[0xFFFC] = CPU::INS_LDA_ABSX;
  mem[0xFFFD] = 0x80;
  mem[0xFFFE] = 0x44; // 0x4480
  mem[0x4481] = 0x37;
  constexpr s32 Expected_Cycles = 4;
  
  
  // when:
  CPU CPUCopy = cpu; 
  s32 CyclesUsed = cpu.Execute(Expected_Cycles , mem);

  // then:
  EXPECT_EQ(cpu.A, 0x37);
  EXPECT_EQ(CyclesUsed, Expected_Cycles);
  EXPECT_FALSE(cpu.Z);
  EXPECT_FALSE(cpu.N);
  VerifyUnmodifiedFlagsFromLDA(cpu, CPUCopy); 
  
  printf("After execution, A register: %d\n", cpu.A);

}


TEST_F(M6502Test1, LDAAbsoluteXCanLoadAValueIntoTheRegisterWhenItCrosesAPageBoundary)
{
  // given:
  cpu.X = 0xFF;
  mem[0xFFFC] = CPU::INS_LDA_ABSX;
  mem[0xFFFD] = 0x02;
  mem[0xFFFE] = 0x44; // 0x4402
  mem[0x4501] = 0x37; // 0x4402 + 0xFF crosses page boundary!
  constexpr s32 Expected_Cycles = 5;
  
  
  // when:
  CPU CPUCopy = cpu; 
  s32 CyclesUsed = cpu.Execute(Expected_Cycles , mem);

  // then:
  EXPECT_EQ(cpu.A, 0x37);
  EXPECT_EQ(CyclesUsed, Expected_Cycles);
  EXPECT_FALSE(cpu.Z);
  EXPECT_FALSE(cpu.N);
  VerifyUnmodifiedFlagsFromLDA(cpu, CPUCopy); 
  
  printf("After execution, A register: %d\n", cpu.A);

}



TEST_F(M6502Test1, LDAAbsoluteYCanLoadAValueIntoTheRegister)
{
  // given:
  cpu.Y = 1;
  mem[0xFFFC] = CPU::INS_LDA_ABSY;
  mem[0xFFFD] = 0x80;
  mem[0xFFFE] = 0x44; // 0x4480
  mem[0x4481] = 0x37;
  constexpr s32 Expected_Cycles = 4;
  
  
  // when:
  CPU CPUCopy = cpu; 
  s32 CyclesUsed = cpu.Execute(Expected_Cycles , mem);

  // then:
  EXPECT_EQ(cpu.A, 0x37);
  EXPECT_EQ(CyclesUsed, Expected_Cycles);
  EXPECT_FALSE(cpu.Z);
  EXPECT_FALSE(cpu.N);
  VerifyUnmodifiedFlagsFromLDA(cpu, CPUCopy); 
  
  printf("After execution, A register: %d\n", cpu.A);

}


TEST_F(M6502Test1, LDAAbsoluteYCanLoadAValueIntoTheRegisterWhenItCrosesAPageBoundary)
{
  // given:
  cpu.Y = 0xFF;
  mem[0xFFFC] = CPU::INS_LDA_ABSY;
  mem[0xFFFD] = 0x02;
  mem[0xFFFE] = 0x44; // 0x4402
  mem[0x4501] = 0x37; // 0x4402 + 0xFF crosses page boundary!
  constexpr s32 Expected_Cycles = 5;
  
  
  // when:
  CPU CPUCopy = cpu; 
  s32 CyclesUsed = cpu.Execute(Expected_Cycles , mem);

  // then:
  EXPECT_EQ(cpu.A, 0x37);
  EXPECT_EQ(CyclesUsed, Expected_Cycles);
  EXPECT_FALSE(cpu.Z);
  EXPECT_FALSE(cpu.N);
  VerifyUnmodifiedFlagsFromLDA(cpu, CPUCopy); 
  
  printf("After execution, A register: %d\n", cpu.A);

}



TEST_F(M6502Test1, LDAIndirectXCanLoadAValueIntoTheRegister)
{
  // given:
  cpu.X = 0x04;
  mem[0xFFFC] = CPU::INS_LDA_INDX;
  mem[0xFFFD] = 0x02;
  mem[0x0006] = 0x00; // 0x2 + 0x4
  mem[0x0007] = 0x80;
  mem[0x8000] = 0x37;
  constexpr s32 Expected_Cycles = 6;
  
  
  // when:
  CPU CPUCopy = cpu; 
  s32 CyclesUsed = cpu.Execute(Expected_Cycles , mem);

  // then:
  EXPECT_EQ(cpu.A, 0x37);
  EXPECT_EQ(CyclesUsed, Expected_Cycles);
  EXPECT_FALSE(cpu.Z);
  EXPECT_FALSE(cpu.N);
  VerifyUnmodifiedFlagsFromLDA(cpu, CPUCopy); 
  
  printf("After execution, A register: %d\n", cpu.A);

}




TEST_F(M6502Test1, LDAIndirectYCanLoadAValueIntoTheRegister)
{
  // given:
  cpu.Y = 0x04;
  mem[0xFFFC] = CPU::INS_LDA_INDY;
  mem[0xFFFD] = 0x02;
  mem[0x0002] = 0x00;
  mem[0x0003] = 0x80;
  mem[0x8004] = 0x37; //0x8000 + 0x4
  constexpr s32 Expected_Cycles = 5;
  
  
  // when:
  CPU CPUCopy = cpu; 
  s32 CyclesUsed = cpu.Execute(Expected_Cycles , mem);

  // then:
  EXPECT_EQ(cpu.A, 0x37);
  EXPECT_EQ(CyclesUsed, Expected_Cycles);
  EXPECT_FALSE(cpu.Z);
  EXPECT_FALSE(cpu.N);
  VerifyUnmodifiedFlagsFromLDA(cpu, CPUCopy); 
  
  printf("After execution, A register: %d\n", cpu.A);

}


TEST_F(M6502Test1, LDAIndirectYCanLoadAValueIntoTheRegisterWhenItCrossesThePageBoundary)
{
  // given:
  cpu.Y = 0x04;
  mem[0xFFFC] = CPU::INS_LDA_INDY;
  mem[0xFFFD] = 0x02;
  mem[0x0002] = 0x00;
  mem[0x0003] = 0x80;
  mem[0x8101] = 0x37; //0x8000 + 0x4
  constexpr s32 Expected_Cycles = 6;
  
  
  // when:
  CPU CPUCopy = cpu; 
  s32 CyclesUsed = cpu.Execute(Expected_Cycles , mem);

  // then:
  EXPECT_EQ(cpu.A, 0x37);
  EXPECT_EQ(CyclesUsed, Expected_Cycles);
  EXPECT_FALSE(cpu.Z);
  EXPECT_FALSE(cpu.N);
  VerifyUnmodifiedFlagsFromLDA(cpu, CPUCopy); 
  
  printf("After execution, A register: %d\n", cpu.A);

}

