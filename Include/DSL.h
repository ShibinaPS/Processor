#ifndef DSL_H
#define DSL_H
//===========================================================================================================

#define DO_PUSH(arg) stk_push(spu->stack, arg)
#define DO_POP(arg)  stk_pop(spu->stack, arg)
#define SPU_REG      spu->Reg
#define RAX          spu->Reg[0]
#define RBX          spu->Reg[1]
#define RCX          spu->Reg[2]
#define RDX          spu->Reg[3]
#define SPU_RAM      spu->Ram

//===========================================================================================================

#endif // DSL_H
