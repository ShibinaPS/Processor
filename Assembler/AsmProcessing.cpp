#include "AsmProcessing.h"
#include "../Include/DSL.h"

//===========================================================================================================

#define STRCMP(pos) strcmp(asmbl->tok_arr[asmbl->cur_n].text, pos)

//===========================================================================================================

void asm_ctor(struct AsmblProcessing* asmbl, struct TextProcessing* tp, struct FlagProcessing* fp)
{
      asmbl->lst_file = new_extension(tp->asmbl_filename, ".lst");

      if(asmbl->lst_file == nullptr)
      {
            asmbl->error = ERROR_LST_FILE_CREATE;
      }

      asmbl->tok_num = tp->tok_num;
      tp->cur_n   = 0;

      asmbl->tok_arr = (Token*)calloc(asmbl->tok_num, sizeof(Token));
      if(asmbl->tok_arr == nullptr)
      {
            asmbl->error = ERROR_TOK_ARR_CALLOC;
      }

      if(asmbl->tok_arr == nullptr)
      {
            asmbl->error = ERROR_CALLOC_TOK_ARR;
      }

      tokenization(asmbl, tp, fp);

      arr_jmp_with_ind(asmbl, fp);

      listing(asmbl, fp);

      bin_file(asmbl, tp, fp);
}

//===========================================================================================================

FILE* new_extension(char* filename, const char* extn)
{
      char* dot_ptr = strchr(filename, '.');

      char new_ext[MAX_WORD_LEN] = {0};

      strncpy(new_ext, filename, dot_ptr - filename);

      strcat(new_ext, extn);

      FILE* new_file = fopen(new_ext, "w");

      if(new_file == nullptr)
      {
            printf("Error! File is nullptr!\n");
            return nullptr;
      }

      return new_file;
}

//===========================================================================================================

#define DEF_CMD(name, cmd_type, arg, code)                        \
      else if(strcasecmp(tp->tok_arr[tp->cur_n], #name) == 0)     \
      {                                                           \
            asmbl->tok_arr[asmbl->cur_n].tok_type = cmd_type;     \
            asmbl->tok_arr[asmbl->cur_n].asm_code = name;         \
                                                                  \
            if(arg)                                               \
            {                                                     \
                  tp->cur_n++;                                    \
                  arg_check(asmbl, tp, name, fp);                 \
            }                                                     \
                                                                  \
            else                                                  \
            {                                                     \
                  asmbl->cur_n++;                                 \
            }                                                     \
            continue;                                             \
      }                                                           \
      
//===========================================================================================================

void tokenization(struct AsmblProcessing* asmbl, struct TextProcessing* tp, struct FlagProcessing* fp)
{
      for(; asmbl->cur_n < asmbl->tok_num, tp->cur_n < tp->tok_num; tp->cur_n++)
      {
            if(tp->tok_arr[tp->cur_n][strlen(tp->tok_arr[tp->cur_n]) - 1] != ':')
            {
                  strcpy(asmbl->tok_arr[asmbl->cur_n].text, tp->tok_arr[tp->cur_n]);
                  // printf("%d\n", asmbl->cur_n);
                  // printf("%s\n", asmbl->tok_arr[asmbl->cur_n].text);

                  if(0) {}

                  #include "../Include/Commands.h"

                  /*else*/
                  asmbl->tok_arr[asmbl->cur_n].error = ERROR_CMD_ERROR;
                  printf("Error command '%s'!\n", asmbl->tok_arr[asmbl->cur_n].text);
                  asmbl->cur_n++;
            }
      }
}

#undef DEF_CMD

//===========================================================================================================

#define FLAGS_ARG                                                                         \
      {                                                                                   \
            asmbl->cur_n++;                                                               \
                                                                                          \
            asmbl->tok_arr[asmbl->cur_n].tok_type = FLAG;                                 \
            strcpy(asmbl->tok_arr[asmbl->cur_n].text, tp->tok_arr[tp->cur_n]);            \
            for(size_t i = 0; i < fp->tags_num; i++)                                      \
            {                                                                             \
                  char new_word[MAX_WORD_LEN] = {0};                                      \
                  strcpy(new_word, asmbl->tok_arr[asmbl->cur_n].text + 1);                \
                  new_word[strlen(new_word)] = ':';                                       \
                  if(strcmp(fp->tags_arr[i].word, new_word) == 0)                         \
                  {                                                                       \
                        asmbl->tok_arr[asmbl->cur_n].num = (float)fp->tags_arr[i].index;  \
                  }                                                                       \
            }                                                                             \
                                                                                          \
            asmbl->cur_n++;                                                               \
            return;                                                                       \
      }                                                                                   \

#define OTHER_ARG             \
      {                       \
            asmbl->cur_n++;   \
            return;           \
      }                       \

void arg_check(struct AsmblProcessing* asmbl, struct TextProcessing* tp, int asm_code, struct FlagProcessing* fp)
{
      switch(asm_code)
      {
            case(PUSH):
            {
                  asmbl->tok_arr[asmbl->cur_n].asm_code = PUSH;
                  // printf("%s\n", tp->tok_arr[tp->cur_n]);

                  // If the token is a num
                  size_t dots_num     = 0;
                  size_t err_num_asm  = 0;
                  for(size_t i = 0; i < strlen(tp->tok_arr[tp->cur_n]); i++)
                  {

                        if(tp->tok_arr[tp->cur_n][i] == '.')
                        {
                              dots_num++;
                              continue;
                        }
                        if(tp->tok_arr[tp->cur_n][i] == '-')
                        {
                              continue;
                        }
                        if(isdigit(tp->tok_arr[tp->cur_n][i]) == 0)
                        {
                              err_num_asm++;
                              break;
                        }
                  }


                  if((err_num_asm == 0) && (dots_num < 2))
                  {
                        asmbl->tok_arr[asmbl->cur_n].asm_code += NUM_MASK;
                        asmbl->cur_n++;
                        strcpy(asmbl->tok_arr[asmbl->cur_n].text, tp->tok_arr[tp->cur_n]);
                        asmbl->tok_arr[asmbl->cur_n].tok_type = NUM;
                        asmbl->tok_arr[asmbl->cur_n].num = (float)strtod(tp->tok_arr[tp->cur_n], NULL);
                        asmbl->cur_n++;
                        return;
                  }


                  // If the token is a [num]
                  if((tp->tok_arr[tp->cur_n][0] == '[') && (tp->tok_arr[tp->cur_n][strlen(tp->tok_arr[tp->cur_n]) - 1] == ']')
                        && (tp->tok_arr[tp->cur_n][2]) != 'x')
                  {
                        char text[MAX_WORD_LEN] = {0};
                        strncpy(text, tp->tok_arr[tp->cur_n] + 1, strlen(tp->tok_arr[tp->cur_n]) - 2);

                        size_t st_num_err = 0;
                        for(size_t i = 0; i < strlen(text); i++)
                        {                                    
                              if((isdigit(text[i]) == 0) && (text[i] != '.'))
                              {
                                    st_num_err++;
                                    break;
                              }
                        }

                        if(st_num_err == 0)
                        {
                              asmbl->tok_arr[asmbl->cur_n].asm_code += (NUM_MASK + RAM_MASK);
                              asmbl->cur_n++;
                              strcpy(asmbl->tok_arr[asmbl->cur_n].text, text);
                              asmbl->tok_arr[asmbl->cur_n].tok_type = NUM;
                              asmbl->tok_arr[asmbl->cur_n].num = (float)atof(text);
                              asmbl->cur_n++;
                              return;
                        }
                  }

                  // If the token is a register
                  for(size_t i = 0; i < sizeof(reg)/sizeof(reg[0]); i++)
                  {
                        if(strcmp(tp->tok_arr[tp->cur_n], reg[i]) == 0)
                        {
                              asmbl->tok_arr[asmbl->cur_n].asm_code += REG_MASK; // Добавляем к команде маску регистра
                              asmbl->cur_n++;
                              asmbl->tok_arr[asmbl->cur_n].tok_type = REG;
                                                                  
                              if((i + 20) == AX)
                              {
                                    strcpy(asmbl->tok_arr[asmbl->cur_n].text, tp->tok_arr[tp->cur_n]);
                                    asmbl->tok_arr[asmbl->cur_n].asm_code = AX;
                              }

                              else if((i+20) == BX)
                              {
                                    strcpy(asmbl->tok_arr[asmbl->cur_n].text, tp->tok_arr[tp->cur_n]);
                                    asmbl->tok_arr[asmbl->cur_n].asm_code = BX;
                              }

                              else if((i+20) == CX)
                              {
                                    strcpy(asmbl->tok_arr[asmbl->cur_n].text, tp->tok_arr[tp->cur_n]);
                                    asmbl->tok_arr[asmbl->cur_n].asm_code = CX;
                              }

                              else if((i+20) == DX)
                              {
                                    strcpy(asmbl->tok_arr[asmbl->cur_n].text, tp->tok_arr[tp->cur_n]);
                                    asmbl->tok_arr[asmbl->cur_n].asm_code = DX;
                              }

                              asmbl->cur_n++;
                              return;
                        }
                  }

                  // If the token is a [register]
                  for(size_t i = 0; i < sizeof(st_reg)/sizeof(st_reg[0]); i++) 
                  {
                        if(strcmp(tp->tok_arr[tp->cur_n], st_reg[i]) == 0)
                        {
                              asmbl->tok_arr[asmbl->cur_n].asm_code += (REG_MASK + RAM_MASK);
                              asmbl->cur_n++;
                              asmbl->tok_arr[asmbl->cur_n].tok_type = REG;
                              strcpy(asmbl->tok_arr[asmbl->cur_n].text, tp->tok_arr[tp->cur_n]);
                              
                              if((i+20) == AX)
                              {
                                    asmbl->tok_arr[asmbl->cur_n].asm_code = AX;
                              }

                              else if((i+20) == BX)
                              {
                                    asmbl->tok_arr[asmbl->cur_n].asm_code = BX;
                              }

                              else if((i+20) == CX)
                              {
                                    asmbl->tok_arr[asmbl->cur_n].asm_code = CX;
                              }

                              else if((i+20) == DX)
                              {
                                    asmbl->tok_arr[asmbl->cur_n].asm_code = DX;
                              }

                              asmbl->cur_n++;
                              return;
                        }
                  }

                  // If the token is [reg + num]

                  if((tp->tok_arr[tp->cur_n][0] == '[') && (tp->tok_arr[tp->cur_n][strlen(tp->tok_arr[tp->cur_n]) - 1] == ']') && 
                        (tp->tok_arr[tp->cur_n][1] == 'a' || tp->tok_arr[tp->cur_n][1] == 'b' || tp->tok_arr[tp->cur_n][1] == 'c' || tp->tok_arr[tp->cur_n][1] == 'd') 
                        && (tp->tok_arr[tp->cur_n][2] == 'x'))
                  {
                        asmbl->tok_arr[asmbl->cur_n].asm_code += (NUM_MASK + REG_MASK + RAM_MASK);
                        asmbl->cur_n++;

                        char reg_part[MAX_WORD_LEN] = {0};
                        strncpy(reg_part, tp->tok_arr[tp->cur_n] + 1, 2);

                        for(size_t i = 0; i < sizeof(reg)/sizeof(reg[0]); i++)
                        {
                              if(strcmp(reg_part, reg[i]) == 0)
                              {
                                    strcpy(asmbl->tok_arr[asmbl->cur_n].text, reg_part);
                                    asmbl->tok_arr[asmbl->cur_n].tok_type = REG;

                                    if((i+20) == AX)
                                    {
                                          asmbl->tok_arr[asmbl->cur_n].asm_code = AX;
                                    }

                                    else if((i+20) == BX)
                                    {
                                          asmbl->tok_arr[asmbl->cur_n].asm_code = BX;
                                    }

                                    else if((i+20) == CX)
                                    {
                                          asmbl->tok_arr[asmbl->cur_n].asm_code = CX;
                                    }

                                    else if((i+20) == DX)
                                    {
                                          asmbl->tok_arr[asmbl->cur_n].asm_code = DX;
                                    }

                                    asmbl->cur_n++;
                              }
                        }

                        char num_part[MAX_WORD_LEN] = {0};
                        strncpy(num_part, tp->tok_arr[tp->cur_n] + 4, strlen(tp->tok_arr[tp->cur_n]) - 5);
                              
                        strcpy(asmbl->tok_arr[asmbl->cur_n].text, num_part);
                        asmbl->tok_arr[asmbl->cur_n].tok_type = NUM;
                        asmbl->tok_arr[asmbl->cur_n].num = (float)atof(num_part);
                        asmbl->cur_n++;
                        return;
                  }
            }

            case(POP) : 
            {
                  asmbl->tok_arr[asmbl->cur_n].asm_code = POP;

                  // If the token is a num (+ || -) - it is a mistake in this case
                  size_t err_num_asm  = 0;
                  for(size_t i = 0; i < strlen(tp->tok_arr[tp->cur_n]); i++)
                  {
                        if((tp->tok_arr[tp->cur_n][i] == '.') || (tp->tok_arr[tp->cur_n][i] == '-') || isdigit(tp->tok_arr[tp->cur_n][i]))
                        {
                              err_num_asm++;
                              break;
                        }
                  }

                  if(err_num_asm != 0)
                  {
                        printf("Error in sequence of arguments! Pop can`t contains a num as an argument!\n");
                        asmbl->error = ERROR_SEQUENCE_OF_ARGS;
                        asmbl->tok_arr[asmbl->cur_n].asm_code += NUM_MASK;   //ИСПРАВИТЬ
                        asmbl->cur_n++;
                        asmbl->tok_arr[asmbl->cur_n].error = 1;
                        strcpy(asmbl->tok_arr[asmbl->cur_n].text, tp->tok_arr[tp->cur_n]);
                        asmbl->tok_arr[asmbl->cur_n].tok_type = NUM;
                        asmbl->tok_arr[asmbl->cur_n].num = (float)atof(tp->tok_arr[tp->cur_n]);
                        asmbl->cur_n++;
                        return;
                  }

                  // If the token is a [num]
                  if((tp->tok_arr[tp->cur_n][0] == '[') && (tp->tok_arr[tp->cur_n][strlen(tp->tok_arr[tp->cur_n]) - 1] == ']')
                        && (tp->tok_arr[tp->cur_n][2]) != 'x')
                  {
                        size_t st_num_err = 0;
                        for(size_t i = 0; i < strlen(tp->tok_arr[tp->cur_n] - 1); i++)
                        {                                    
                              if((tp->tok_arr[tp->cur_n][i] == '.') || (tp->tok_arr[tp->cur_n][i] == '-') || isdigit(tp->tok_arr[tp->cur_n][i]))
                              {
                                    st_num_err++;
                                    break;
                              }
                        }

                        if(st_num_err != 0)
                        {
                              printf("Error in sequence of arguments! Pop can`t contains a [num] as an argument!\n");
                              asmbl->error = ERROR_SEQUENCE_OF_ARGS;
                              asmbl->tok_arr[asmbl->cur_n].asm_code += (NUM_MASK + RAM_MASK); // НАВАЛИЛА КУЧУ КРИНЖА
                              asmbl->cur_n++;
                              asmbl->tok_arr[asmbl->cur_n].error = 1;
                              strcpy(asmbl->tok_arr[asmbl->cur_n].text, tp->tok_arr[tp->cur_n]);
                              asmbl->tok_arr[asmbl->cur_n].tok_type = NUM;
                              asmbl->tok_arr[asmbl->cur_n].num = (float)atof(tp->tok_arr[tp->cur_n]);
                              asmbl->cur_n++;
                              return;
                        }
                  }

                  // If the token is a register
                  for(size_t i = 0; i < sizeof(reg)/sizeof(reg[0]); i++)
                  {
                        if(strcmp(tp->tok_arr[tp->cur_n], reg[i]) == 0)
                        {
                              asmbl->tok_arr[asmbl->cur_n].asm_code += REG_MASK; // Добавляем к команде маску регистра
                              asmbl->cur_n++;
                              asmbl->tok_arr[asmbl->cur_n].tok_type = REG;
                              strcpy(asmbl->tok_arr[asmbl->cur_n].text, tp->tok_arr[tp->cur_n]);
                                                                  
                              if((i + 20) == AX)
                              {
                                    asmbl->tok_arr[asmbl->cur_n].asm_code = AX;
                                    // printf("%d\n", asmbl->tok_arr[asmbl->cur_n].asm_code);
                              }

                              else if((i+20) == BX)
                              {
                                    asmbl->tok_arr[asmbl->cur_n].asm_code = BX;
                              }

                              else if((i+20) == CX)
                              {
                                    asmbl->tok_arr[asmbl->cur_n].asm_code = CX;
                              }

                              else if((i+20) == DX)
                              {
                                    asmbl->tok_arr[asmbl->cur_n].asm_code = DX;
                              }

                              asmbl->cur_n++;
                              return;
                        }
                  }

                  // If the token is a [register]
                  for(size_t i = 0; i < sizeof(st_reg)/sizeof(st_reg[0]); i++) 
                  {
                        if(strcmp(tp->tok_arr[tp->cur_n], st_reg[i]) == 0)
                        {
                              asmbl->tok_arr[asmbl->cur_n].asm_code += (REG_MASK + RAM_MASK);
                              asmbl->cur_n++;
                              asmbl->tok_arr[asmbl->cur_n].tok_type = REG;
                              strcpy(asmbl->tok_arr[asmbl->cur_n].text, tp->tok_arr[tp->cur_n]);
                              
                              if((i+20) == AX)
                              {
                                    asmbl->tok_arr[asmbl->cur_n].asm_code = AX;
                              }

                              else if((i+20) == BX)
                              {
                                    asmbl->tok_arr[asmbl->cur_n].asm_code = BX;
                              }

                              else if((i+20) == CX)
                              {
                                    asmbl->tok_arr[asmbl->cur_n].asm_code = CX;
                              }

                              else if((i+20) == DX)
                              {
                                    asmbl->tok_arr[asmbl->cur_n].asm_code = DX;
                              }

                              asmbl->cur_n++;
                              return;
                        }
                  }

                  // If the token is [reg + num]

                  if((tp->tok_arr[tp->cur_n][0] == '[') && (tp->tok_arr[tp->cur_n][strlen(tp->tok_arr[tp->cur_n]) - 1] == ']') && 
                        (tp->tok_arr[tp->cur_n][1] == 'a' || tp->tok_arr[tp->cur_n][1] == 'b' || tp->tok_arr[tp->cur_n][1] == 'c' || tp->tok_arr[tp->cur_n][1] == 'd') 
                        && (tp->tok_arr[tp->cur_n][2] == 'x'))
                  {
                        asmbl->tok_arr[asmbl->cur_n].asm_code += (NUM_MASK + REG_MASK + RAM_MASK);
                        asmbl->cur_n++;

                        char reg_part[MAX_WORD_LEN] = {0};
                        strncpy(reg_part, tp->tok_arr[tp->cur_n] + 1, 2);

                        for(size_t i = 0; i < sizeof(reg)/sizeof(reg[0]); i++)
                        {
                              if(strcmp(reg_part, reg[i]) == 0)
                              {
                                    strcpy(asmbl->tok_arr[asmbl->cur_n].text, reg_part);
                                    asmbl->tok_arr[asmbl->cur_n].tok_type = REG;

                                    if((i+20) == AX)
                                    {
                                          asmbl->tok_arr[asmbl->cur_n].asm_code = AX;
                                    }

                                    else if((i+20) == BX)
                                    {
                                          asmbl->tok_arr[asmbl->cur_n].asm_code = BX;
                                    }

                                    else if((i+20) == CX)
                                    {
                                          asmbl->tok_arr[asmbl->cur_n].asm_code = CX;
                                    }

                                    else if((i+20) == DX)
                                    {
                                          asmbl->tok_arr[asmbl->cur_n].asm_code = DX;
                                    }

                                    asmbl->cur_n++;
                              }
                        }

                        char num_part[MAX_WORD_LEN] = {0};
                        strncpy(num_part, tp->tok_arr[tp->cur_n] + 4, strlen(tp->tok_arr[tp->cur_n]) - 5);
                              
                        strcpy(asmbl->tok_arr[asmbl->cur_n].text, num_part);
                        asmbl->tok_arr[asmbl->cur_n].tok_type = NUM;
                        asmbl->tok_arr[asmbl->cur_n].num = (float)atof(num_part);
                        asmbl->cur_n++;
                        return;
                  }
            }

            case(HLT) : asmbl->tok_arr[asmbl->cur_n].asm_code = HLT;
                        OTHER_ARG;

            case(IN)  : asmbl->tok_arr[asmbl->cur_n].asm_code = IN;
                        OTHER_ARG;

            case(OUT) : asmbl->tok_arr[asmbl->cur_n].asm_code = OUT;
                        OTHER_ARG;

            case(ADD) : asmbl->tok_arr[asmbl->cur_n].asm_code = ADD;
                        OTHER_ARG;

            case(SUB) : asmbl->tok_arr[asmbl->cur_n].asm_code = SUB;
                        OTHER_ARG;

            case(MUL) : asmbl->tok_arr[asmbl->cur_n].asm_code = MUL;
                        OTHER_ARG;

            case(DIV) : asmbl->tok_arr[asmbl->cur_n].asm_code = DIV;
                        OTHER_ARG;

            case(JMP) : asmbl->tok_arr[asmbl->cur_n].asm_code = JMP;
                        FLAGS_ARG;

            case(JE)  : asmbl->tok_arr[asmbl->cur_n].asm_code = JE;
                        FLAGS_ARG;

            case(JGE) : asmbl->tok_arr[asmbl->cur_n].asm_code = JGE;
                        FLAGS_ARG;

            case(JNE) : asmbl->tok_arr[asmbl->cur_n].asm_code = JNE;
                        FLAGS_ARG;

            case(JG)  : asmbl->tok_arr[asmbl->cur_n].asm_code = JG;
                        FLAGS_ARG;

            case(JL)  : asmbl->tok_arr[asmbl->cur_n].asm_code = JL;
                        FLAGS_ARG;

            case(JLE) : asmbl->tok_arr[asmbl->cur_n].asm_code = JLE;
                        FLAGS_ARG;

            case(CALL): asmbl->tok_arr[asmbl->cur_n].asm_code = CALL;
                        FLAGS_ARG;

            default   : asmbl->tok_arr[asmbl->cur_n].error = ERROR_TOKEN;
                        printf("Error token!\n");
                        return;
      }
}

#undef FLAGS_ARG
#undef OTHER_ARG

//===========================================================================================================
void listing(struct AsmblProcessing* asmbl, struct FlagProcessing* fp)
{
      fprintf(asmbl->lst_file, "Signature: %s\n", asmbl->signature);
      fprintf(asmbl->lst_file, "Quantity: %lu\n\n\n", asmbl->tok_num - fp->tags_num);
      fprintf(asmbl->lst_file, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
      fprintf(asmbl->lst_file, "  №  \t\tCommand\t\t Type \t\tAsmCode \t\tError\n");
      fprintf(asmbl->lst_file, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");

      for(size_t i = 0; i < (asmbl->tok_num - fp->tags_num); i++)
      {
            fprintf(asmbl->lst_file, " %03lu\t\t", i);
            fprintf(asmbl->lst_file, "%-9s\t\t", asmbl->tok_arr[i].text);

            switch(asmbl->tok_arr[i].tok_type)
            {
                  case(CMD):
                  {
                        fprintf(asmbl->lst_file, "%-10s\t\t", "cmd");
                        fprintf(asmbl->lst_file, "%-10lu\t\t", asmbl->tok_arr_with_jmp_ind[i].asm_code);

                        break;
                  }

                  case(NUM):
                  {
                        fprintf(asmbl->lst_file, "%-10s\t\t", "num");
                        fprintf(asmbl->lst_file, "%-10g\t\t", asmbl->tok_arr_with_jmp_ind[i].num);

                        break;
                  }

                  case(REG):
                  {

                        fprintf(asmbl->lst_file, "%-10s\t\t", "reg");
                        fprintf(asmbl->lst_file, "%-10lu\t\t", asmbl->tok_arr_with_jmp_ind[i].asm_code);

                        break;
                  }

                  case(JUMP):
                  {
                        fprintf(asmbl->lst_file, "%-10s\t\t", "jmp");
                        fprintf(asmbl->lst_file, "%-10lu\t\t", asmbl->tok_arr_with_jmp_ind[i].asm_code);

                        break;
                  }

                  case(FLAG):
                  {
                        fprintf(asmbl->lst_file, "%-10s\t\t", "flag");
                        fprintf(asmbl->lst_file, "%-10s\t\t", asmbl->tok_arr_with_jmp_ind[i].text);

                        break;
                  }

                  default:
                  {
                        printf("%s\n", asmbl->tok_arr[i].text);
                        fprintf(asmbl->lst_file, "%-10s\t\t", "unknown");
                        break;
                  }

            }

            fprintf(asmbl->lst_file, "%d\n", asmbl->tok_arr[i].error);
      }

      fprintf(asmbl->lst_file, ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
}

//===========================================================================================================

void arr_jmp_with_ind(struct AsmblProcessing* asmbl, struct FlagProcessing* fp)
{
      asmbl->tok_arr_with_jmp_ind = (Token*)calloc(asmbl->tok_num - fp->tags_num, sizeof(Token));
      if(asmbl->tok_arr_with_jmp_ind == nullptr)
      {
            asmbl->error = ERROR_TOK_ARR_WITH_JMP_IND_CALLOC;
            return;
      }

      size_t i = 0;
      while(i < (asmbl->tok_num - fp->tags_num))
      {
            if(!strcmp(asmbl->tok_arr[i].text, "jmp") || !strcmp(asmbl->tok_arr[i].text, "je") 
               || !strcmp(asmbl->tok_arr[i].text, "jge") || !strcmp(asmbl->tok_arr[i].text, "jne")
               || !strcmp(asmbl->tok_arr[i].text, "jg") || !strcmp(asmbl->tok_arr[i].text, "jl")
               || !strcmp(asmbl->tok_arr[i].text, "jle") || !strcmp(asmbl->tok_arr[i].text, "call"))
            {
                  strcpy(asmbl->tok_arr_with_jmp_ind[i].text, asmbl->tok_arr[i].text);
                  asmbl->tok_arr_with_jmp_ind[i].tok_type = asmbl->tok_arr[i].tok_type;
                  asmbl->tok_arr_with_jmp_ind[i].asm_code = asmbl->tok_arr[i].asm_code;
                  asmbl->tok_arr_with_jmp_ind[i].num      = asmbl->tok_arr[i].num;

                  i++;
                  char word[MAX_WORD_LEN] = {0};
                  strcpy(word, asmbl->tok_arr[i].text + 1);
                  word[strlen(word)] = ':';
                  for(size_t j = 0; j < fp->tags_num; j++)
                  {
                        if(strcmp(word, fp->new_tags_arr[j].word) == 0)
                        {
                              snprintf(asmbl->tok_arr_with_jmp_ind[i].text, MAX_WORD_LEN, "%lu", fp->new_tags_arr[j].index);
                              asmbl->tok_arr_with_jmp_ind[i].tok_type = asmbl->tok_arr[i].tok_type;
                              asmbl->tok_arr_with_jmp_ind[i].num = fp->new_tags_arr[j].index; 
                              i++;                             
                        }
                  }
            }

            strcpy(asmbl->tok_arr_with_jmp_ind[i].text, asmbl->tok_arr[i].text);
            asmbl->tok_arr_with_jmp_ind[i].tok_type = asmbl->tok_arr[i].tok_type;
            asmbl->tok_arr_with_jmp_ind[i].asm_code = asmbl->tok_arr[i].asm_code;
            asmbl->tok_arr_with_jmp_ind[i].num      = asmbl->tok_arr[i].num;
            i++;
      }

      // for(size_t i = 0; i < asmbl->tok_num - fp->tags_num; i++)
      // {
      //       printf("%s\n", asmbl->tok_arr_with_jmp_ind[i].text);
      // }
}

//===========================================================================================================

int signature(struct AsmblProcessing* asmbl)
{
      int res = 0;
      int n = 1;
      int step = 256;

      for(size_t i = 0; i < strlen(asmbl->signature); i++)
      {
            // printf("%c\n", asmbl->signature[i]);
            res += asmbl->signature[i] * n;
            n *= step;
      }
      return res;
}

//===========================================================================================================

void bin_file(struct AsmblProcessing* asmbl, struct TextProcessing* tp, struct FlagProcessing* fp)
{
      asmbl->bin_filename = new_extension(tp->asmbl_filename, ".bin");

      float* bin_file = (float*)calloc(asmbl->tok_num - fp->tags_num + 2, sizeof(float));
      bin_file[0] = (float)signature(asmbl);
      bin_file[1] = float(asmbl->tok_num - fp->tags_num);

      for(size_t i = 2; i < (asmbl->tok_num - fp->tags_num + 2); i++)
      {
            if(asmbl->tok_arr_with_jmp_ind[i - 2].tok_type == NUM || asmbl->tok_arr_with_jmp_ind[i - 2].tok_type == FLAG)
            {
                  bin_file[i] = asmbl->tok_arr_with_jmp_ind[i - 2].num;
                  continue;
            }
            bin_file[i] = (float)asmbl->tok_arr_with_jmp_ind[i - 2].asm_code;
      }

      /*for(size_t i = 0; i < (asmbl->tok_num - fp->tags_num + 2); i++)
      {
            printf("%g\n", bin_file[i]);
      }
      */

      fwrite(bin_file, sizeof(float), asmbl->tok_num - fp->tags_num + 2, asmbl->bin_filename);
      free(bin_file);
}

//===========================================================================================================

int Compare(const double x, const double y)
{
    if (fabs(x - y) <= EPS)
        return EQUAL;
    
    else
        return ((x - y) > EPS) ? FIRST : SECOND;
}

//===========================================================================================================

void asm_dtor(struct AsmblProcessing* asmbl, struct TextProcessing* tp)
{
      free(asmbl->tok_arr);
      free(asmbl->tok_arr_with_jmp_ind);

      fclose(asmbl->lst_file);
      fclose(asmbl->bin_filename);
}
