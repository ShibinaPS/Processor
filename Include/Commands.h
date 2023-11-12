// Commands

DEF_CMD(HLT, CMD, 0,
{
      return 0;
})


DEF_CMD(PUSH, CMD, 1,
{
      DO_PUSH(*arg_ptr);
})


DEF_CMD(POP, CMD, 1,
{
      DO_POP(&arg_1);
      *arg_ptr = arg_1;
})


DEF_CMD(IN, CMD, 0, 
{
      printf("Enter a number in stack: ");
      scanf(ELEM_TYPE, &arg);
      DO_PUSH(arg);
})


DEF_CMD(OUT, CMD, 0,
{
      DO_POP(&arg_1);

      if(Compare(arg_1, 0) == EQUAL)
      {
            printf("SPU out: 0.\n");
      }
      else
      {
            printf("SPU out: " ELEM_TYPE "\n", arg_1);
      }
})


DEF_CMD(ADD, CMD, 0,
{
      DO_POP(&arg_1);
      DO_POP(&arg_2);
      DO_PUSH(arg_1 + arg_2);
})


DEF_CMD(SUB, CMD, 0,
{
      DO_POP(&arg_1);
      DO_POP(&arg_2);
      DO_PUSH(arg_2 - arg_1);
})


DEF_CMD(MUL, CMD, 0,
{
      DO_POP(&arg_1);
      DO_POP(&arg_2);
      DO_PUSH(arg_1 * arg_2);
})


DEF_CMD(DIV, CMD, 0,
{
      DO_POP(&arg_1);
      DO_POP(&arg_2);

      if(Compare(arg_1, 0) == EQUAL)
      {
            spu->error = ERROR_DIV_BY_ZERO;
            return ERROR_DIV_BY_ZERO;
      }

      DO_PUSH(arg_2 / arg_1);
})


DEF_CMD(JMP, JUMP, 1,
{
      spu->cur_n = (size_t)*arg_ptr - 1;
})


DEF_CMD(JE, JUMP, 1,
{
      DO_POP(&arg_1);
      DO_POP(&arg_2);

      if(Compare(arg_1, arg_2) == EQUAL)
      {
            spu->cur_n = (size_t)*arg_ptr - 1;
      }
})


DEF_CMD(JNE, JUMP, 1,
{
      DO_POP(&arg_1);
      DO_POP(&arg_2);

      if(Compare(arg_1, arg_2) != EQUAL)
      {
            spu->cur_n = (size_t)*arg_ptr - 1;
      }
})


DEF_CMD(JG, JUMP, 1,
{
      DO_POP(&arg_1);
      DO_POP(&arg_2);

      if(Compare(arg_1, arg_2) == FIRST)
      {
            spu->cur_n = (size_t)*arg_ptr - 1;
      }
})


DEF_CMD(JGE, JUMP, 1,
{
      DO_POP(&arg_1);
      DO_POP(&arg_2);

      if(Compare(arg_1, arg_2) != SECOND)
      {
            spu->cur_n = (size_t)*arg_ptr - 1;
      }
})


DEF_CMD(JL, JUMP, 1,
{
      DO_POP(&arg_1);
      DO_POP(&arg_2);

      if(Compare(arg_1, arg_2) == SECOND)
      {
            spu->cur_n = (size_t)*arg_ptr - 1;
      }
})


DEF_CMD(JLE, JUMP, 1,
{
      DO_POP(&arg_1);
      DO_POP(&arg_2);

      if(Compare(arg_1, arg_2) != FIRST)
      {
            spu->cur_n = (size_t)*arg_ptr - 1;
      }
})


DEF_CMD(CALL, JUMP, 1,
{
      DO_PUSH((elem_t)spu->cur_n + 1);

      spu->cur_n = (size_t)*arg_ptr - 1;
})

DEF_CMD(SQRT, CMD, 0,
{
      DO_POP(&arg_1);
      if(Compare(arg_1, 0) == SECOND)
      {
            printf("Descriminant is a negative number: %g.\n", arg_1);
            spu->error = ERROR_NEG_NUMBER;
            return ERROR_NEG_NUMBER;
      }
      DO_PUSH((elem_t)sqrt(arg_1));
})

DEF_CMD(MINUS, CMD, 0,
{
      DO_POP(&arg_1);
      DO_PUSH(-1*arg_1);
})

