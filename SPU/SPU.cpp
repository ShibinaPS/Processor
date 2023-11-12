#include "SPU.h"
#include "../Include/DSL.h"

//=========================================================================================================

int SPU_ctor(struct SPU* spu, const char* spu_filename)
{
      struct Stack stk = {};

      if(stk_ctor(&stk) != 0)
      {
            printf("Error in SPU. Error code: %d.\n", ERROR_STK_CREATION);
            spu->error = ERROR_STK_CREATION;
            return ERROR_STK_CREATION;
      }

      spu->stack = &stk;

      spu->bin_file = create_filename(spu, spu_filename);
      if(spu->bin_file == nullptr)
      {
            printf("Error in SPU. Error code: %d.\n", ERROR_BIN_FILE_OPEN);
            spu->error = ERROR_BIN_FILE_OPEN;
            return ERROR_BIN_FILE_OPEN;
      }

      if(file_reading(spu) != 0)
      {
            printf("Error in SPU. Error code: %d.\n", spu->error);
            return spu->error;
      }

      if(processor(spu, &stk) != 0)
      {
            printf("Error in SPU. Error code: %d.\n", spu->error);
            return spu->error;
      }

      return 0;
}

//=========================================================================================================

FILE* create_filename(struct SPU* spu, const char* spu_filename)
{
      char new_filename[MAX_TEXT_LEN] = "../Assembler/";

      strcat(new_filename, spu_filename);

      FILE* bin_file = fopen(new_filename, "rb");

      if(bin_file == nullptr)
      {
            printf("Error in SPU. Error code: %d.\n", ERROR_BIN_FILE_NULLPTR);
            spu->error = ERROR_BIN_FILE_NULLPTR;
            return bin_file;
      }
      return bin_file;
}

//=========================================================================================================

int file_reading(struct SPU* spu)
{
      elem_t* file_buffer = (elem_t*)calloc(2, sizeof(elem_t)); // Для сигнатуры и количества
      
      if(file_buffer == nullptr)
      {
            return ERROR_FILE_BUFFER_NULLPTR;
      }

      fread(file_buffer, sizeof(size_t), 2, spu->bin_file);

      spu->signature = (int)file_buffer[0];
      spu->quantity  = (int)file_buffer[1];

      // printf("%d %d\n", spu->signature, spu->quantity);


      spu->code_buffer = (elem_t*)calloc(spu->quantity, sizeof(elem_t)); // Буффер с кодом

      for(int i = 2; i < spu->quantity + 2; i++)
      {
            fread(spu->code_buffer - 2 + i, sizeof(int), 1, spu->bin_file);
      }

      // for(int i = 0; i < spu->quantity; i++)
      // {
      //       printf("CODE_BUFFER[%d] == " ELEM_TYPE "\n", i, spu->code_buffer[i]);
      // }

      free(file_buffer);
      fclose(spu->bin_file);

      return 0;
}

//=========================================================================================================

#define DEF_CMD(name, cmd_type, arg, code)                  \
      case(name):                                           \
      {                                                     \
            if(arg == 1)                                    \
            {                                               \
                  if(cmd_type == CMD)                       \
                  {                                         \
                        arg_check(spu, &arg_ptr);           \
                  }                                         \
                                                            \
                  else if(cmd_type == JUMP)                 \
                  {                                         \
                        label_check(spu, &arg_ptr);         \
                  }                                         \
            }                                               \
                                                            \
            code                                            \
                                                            \
            break;                                          \
      }                                                     \


int processor(struct SPU* spu, struct Stack* stk)
{
      elem_t arg = 0;
      elem_t* arg_ptr = &arg;

      elem_t arg_1 = 0;
      elem_t arg_2 = 0;

      elem_t* code = spu->code_buffer;

      while(Compare(HLT, code[spu->cur_n]) != EQUAL)
      {
            // printf("%g\n", code[spu->cur_n]);
            switch(GETARG(code[spu->cur_n]))
            {
                  #include "../Include/Commands.h"

                  default:
                  {
                        printf("%f, %d\n", code[spu->cur_n], spu->cur_n);
                        printf("Error in SPU processor!\n");
                        break;
                  }
            }
                  
            arg = 0;
            arg_ptr = &arg;
            spu->cur_n++;
      }

      return 0;
}

#undef DEF_CMD

//=========================================================================================================

void arg_check(struct SPU* spu, elem_t** arg)
{
      int cmd = (int)spu->code_buffer[spu->cur_n];
      **arg = 0;

      if(cmd & NUM_MASK)
      {
            spu->cur_n++;
            **arg += spu->code_buffer[spu->cur_n];
      }

      if(cmd & REG_MASK)
      {
            spu->cur_n++;
            
            if(cmd & RAM_MASK)
            {
                  **arg += spu->Reg[(int)spu->code_buffer[spu->cur_n] - REG_CODE];
            }

            else
            {
                  *arg = &spu->Reg[(int)spu->code_buffer[spu->cur_n] - REG_CODE];
                  return;
            }
      }

      if(cmd & RAM_MASK)
      {
            *arg = &spu->Ram[(int)**arg];
            return;
      }
}

//=========================================================================================================

void label_check(struct SPU* spu, elem_t** arg)
{
      spu->cur_n++;
      **arg += spu->code_buffer[spu->cur_n];
}

//=========================================================================================================

int Compare(const double x, const double y)
{
    if (fabs(x - y) <= EPS)
        return EQUAL;
    
    else
        return ((x - y) > EPS) ? FIRST : SECOND;
}

//=========================================================================================================