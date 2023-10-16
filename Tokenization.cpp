#include "TextProcessing.h"
#include "Tokenization.h"

//===========================================================================================================

#define STRCMP(pos) strcmp(tokp->tok_arr[tokp->cur_n].text, pos)

//===========================================================================================================

int tok_ctor(struct TokProcessing* tokp, struct TextProcessing* tp, struct FlagProcessing* fp)
{
      tokp->tok_num = tp->tok_num;
      tp->cur_tok   = 0;

      tokenization(tokp, tp, fp);

      return 0;
}

//===========================================================================================================

FILE* open_file(const char* filename)
{
      FILE* file = fopen(filename, "w");

      if(file == nullptr)
      {
            printf("Error! File is nullptr!\n");
            return nullptr;
      }

      return file;
}

//===========================================================================================================

void tokenization(struct TokProcessing* tokp, struct TextProcessing* tp, struct FlagProcessing* fp)
{
      for(tokp->cur_n; tokp->cur_n < tokp->tok_num, tp->cur_tok < tp->tok_num; tp->cur_tok++)
      {
            if(tp->str_arr[tp->cur_tok][strlen(tp->str_arr[tp->cur_tok]) - 1] != ':')
            {
                  strcpy(tokp->tok_arr[tokp->cur_n].text, tp->str_arr[tp->cur_tok]);

                  arg_check(tokp, tp);
            }
      }
}

//===========================================================================================================

int cmd_check(struct TokProcessing* tokp)
{
      // If the token is a command
      for(size_t i; i < sizeof(cmd)/sizeof(cmd[0]); i++)
      {
            if(STRCMP(cmd[i]) == 0)
            {
                  tokp->tok_arr[tokp->cur_n].tok_type = CMD;
                  return i;
            }
      }

      // If the token is a jump
      for(size_t i; i < sizeof(jmp)/sizeof(jmp[0]); i++)
      {
            if(STRCMP(jmp[i]) == 0)
            {
                  tokp->tok_arr[tokp->cur_n].tok_type = JUMP;
                  return (i+20);
            }
      }
}

//===========================================================================================================

void arg_check(struct TokProcessing* tokp, struct TextProcessing* tp)
{
      switch(cmd_check(tokp))
      {
            case(PUSH):
            {
                  tokp->tok_arr[tokp->cur_n].asm_code = PUSH;
                  tp->cur_tok++;

                  // If the token is a num
                  size_t dots_num     = 0;
                  size_t err_num_asm  = 0;
                  for(size_t i; i < strlen(tp->str_arr[tp->cur_tok]); i++)
                  {
                        if(tp->str_arr[tp->cur_tok][i] == '.')
                        {
                              dots_num++;
                              continue;
                        }
                        if((tp->str_arr[tp->cur_tok][i] == '-') && (i == 0))
                        {
                              continue;
                        }
                        if(!isdigit(tp->str_arr[tp->cur_tok][i]))
                        {
                              err_num_asm++;
                              break;
                        }
                  }

                  if((err_num_asm == 0) && (dots_num < 2))
                  {
                        tokp->tok_arr[tokp->cur_n].asm_code += NUM_MASK;
                        tokp->cur_n++;
                        strcpy(tokp->tok_arr[tokp->cur_n].text, tp->str_arr[tp->cur_tok]);
                        tokp->tok_arr[tokp->cur_n].tok_type = NUM;
                        tokp->tok_arr[tokp->cur_n].num = (float)strtod(tp->str_arr[tp->cur_tok], NULL);
                        tokp->cur_n++;

                        return;
                  }

                  // If the token is a [num]
                  if((tp->str_arr[tp->cur_tok][0] == '[') && (tp->str_arr[tp->cur_tok][strlen(tp->str_arr[tp->cur_tok]) - 1] == ']')
                        && (tp->str_arr[tp->cur_tok][2]) != 'x')
                  {
                        size_t st_num_err = 0;
                        for(size_t i = 0; i < strlen(tp->str_arr[tp->cur_tok] - 1); i++)
                        {                                    
                              if(isdigit(tp->str_arr[tp->cur_tok][i] == 0) && (tp->str_arr[tp->cur_tok][i] != '.'))
                              {
                                    st_num_err++;
                                    break;
                              }
                        }

                        if(st_num_err == 0)
                        {
                              tokp->tok_arr[tokp->cur_n].asm_code += (NUM_MASK + RAM_MASK);
                              tokp->cur_n++;
                              strcpy(tokp->tok_arr[tokp->cur_n].text, tp->str_arr[tp->cur_tok]);
                              tokp->tok_arr[tokp->cur_n].tok_type = NUM;
                              tokp->tok_arr[tokp->cur_n].num = (float)strtod(tp->str_arr[tp->cur_tok], NULL);
                              tokp->cur_n++;
                              return;
                        }
                  }

                  // If the token is a register
                  for(size_t i = 0; i < sizeof(reg)/sizeof(reg[0]); i++)
                  {
                        if(strcmp(tp->str_arr[tp->cur_tok], reg[i]) == 0)
                        {
                              tokp->tok_arr[tokp->cur_n].asm_code += REG_MASK;
                              tokp->cur_n++;
                              tokp->tok_arr[tokp->cur_n].tok_type = REG;
                                                                  
                              if((i + 10) == AX)
                              {
                                    strcpy(tokp->tok_arr[tokp->cur_n].text, tp->str_arr[tp->cur_tok]);
                                    tokp->tok_arr[tokp->cur_n].asm_code = AX;
                              }

                              else if((i+10) == BX)
                              {
                                    strcpy(tokp->tok_arr[tokp->cur_n].text, tp->str_arr[tp->cur_tok]);
                                    tokp->tok_arr[tokp->cur_n].asm_code = BX;
                              }

                              else if((i+10) == CX)
                              {
                                    strcpy(tokp->tok_arr[tokp->cur_n].text, tp->str_arr[tp->cur_tok]);
                                    tokp->tok_arr[tokp->cur_n].asm_code = CX;
                              }

                              else if((i+10) == DX)
                              {
                                    strcpy(tokp->tok_arr[tokp->cur_n].text, tp->str_arr[tp->cur_tok]);
                                    tokp->tok_arr[tokp->cur_n].asm_code = DX;
                              }

                              tokp->cur_n++;
                              return;
                        }
                  }

                  // If the token is a [register]
                  for(size_t i = 0; i < sizeof(st_reg)/sizeof(st_reg[0]); i++) 
                  {
                        if(strcmp(tp->str_arr[tp->cur_tok], st_reg[i]) == 0)
                        {
                              tokp->tok_arr[tokp->cur_n].asm_code += (REG_MASK + RAM_MASK);
                              tokp->tok_arr[tokp->cur_n].tok_type = REG;
                              tokp->cur_n++;
                              strcpy(tokp->tok_arr[tokp->cur_n].text, tp->str_arr[tp->cur_tok]);
                              
                              if((i+14) == ST_AX)
                              {
                                    tokp->tok_arr[tokp->cur_n].asm_code = ST_AX;
                              }

                              else if((i+14) == ST_BX)
                              {
                                    tokp->tok_arr[tokp->cur_n].asm_code = ST_BX;
                              }

                              else if((i+14) == ST_CX)
                              {
                                    tokp->tok_arr[tokp->cur_n].asm_code = ST_CX;
                              }

                              else if((i+14) == ST_DX)
                              {
                                    tokp->tok_arr[tokp->cur_n].asm_code = ST_DX;
                              }

                              tokp->cur_n++;
                              return;
                        }
                  }

                  // If the token is [reg + num]

                  if((tp->str_arr[tp->cur_tok][0] == '[') && (tp->str_arr[tp->cur_tok][strlen(tp->str_arr[tp->cur_tok]) - 1] == ']') && 
                        (tp->str_arr[tp->cur_tok][1] == 'a' || tp->str_arr[tp->cur_tok][1] == 'b' || tp->str_arr[tp->cur_tok][1] == 'c' || tp->str_arr[tp->cur_tok][1] == 'd') 
                        && (tp->str_arr[tp->cur_tok][2] == 'x'))
                  {
                        tokp->tok_arr[tokp->cur_n].asm_code += (NUM_MASK + REG_MASK + RAM_MASK);
                        tokp->cur_n++;

                        char reg_part[MAX_WORD_LEN] = {0};
                        strncpy(reg_part, tp->str_arr[tp->cur_tok] + 1, 2);

                        for(size_t i = 0; i < sizeof(reg)/sizeof(reg[0]); i++)
                        {
                              if(strcmp(reg_part, reg[i]) == 0)
                              {
                                    strcpy(tokp->tok_arr[tokp->cur_n].text, reg_part);
                                    tokp->tok_arr[tokp->cur_n].tok_type = REG;

                                    if((i+10) == AX)
                                    {
                                          tokp->tok_arr[tokp->cur_n].asm_code = AX;
                                    }

                                    else if((i+10) == BX)
                                    {
                                          tokp->tok_arr[tokp->cur_n].asm_code = BX;
                                    }

                                    else if((i+10) == CX)
                                    {
                                          tokp->tok_arr[tokp->cur_n].asm_code = CX;
                                    }

                                    else if((i+10) == DX)
                                    {
                                          tokp->tok_arr[tokp->cur_n].asm_code = DX;
                                    }

                                    tokp->cur_n++;
                              }
                        }

                        char num_part[MAX_WORD_LEN] = {0};
                        size_t error_part = 0;
                        strncpy(num_part, tp->str_arr[tp->cur_tok] + 4, strlen(tp->str_arr[tp->cur_tok]) - 5);

                        for(size_t j = 0; j < strlen(tp->str_arr[tp->cur_tok]) - 4; j++)
                        {
                              if(isdigit(tp->str_arr[tp->cur_tok][j]) == 0)
                              {
                                    error_part++;
                                    break;
                              }
                        }

                        if(error_part == 0)
                        {
                              strcpy(tokp->tok_arr[tokp->cur_n].text, num_part);
                              tokp->tok_arr[tokp->cur_n].tok_type = NUM;
                              tokp->tok_arr[tokp->cur_n].num = (float)strtod(num_part, NULL);
                              tokp->cur_n++;
                              return;
                        }
                        tokp->tok_arr[tokp->cur_n].error = ERROR_CMD_ARG_ERROR;
                        tokp->cur_n++;
                        return;
                  }
            }

            case(POP) : 
            {
                  tokp->tok_arr[tokp->cur_n].asm_code = POP;
                  tp->cur_tok++;

                  // If the token is a num
                  size_t err_num_asm  = 0;
                  for(size_t i; i < strlen(tp->str_arr[tp->cur_tok]); i++)
                  {
                        if(isdigit(tp->str_arr[tp->cur_tok][i]) == 0 && (tp->str_arr[tp->cur_tok][i] != '.'))
                        {
                              err_num_asm++;
                              break;
                        }
                  }

                  if(err_num_asm == 0)
                  {
                        tokp->tok_arr[tokp->cur_n].asm_code += NUM_MASK;
                        tokp->cur_n++;
                        strcpy(tokp->tok_arr[tokp->cur_n].text, tp->str_arr[tp->cur_tok]);
                        tokp->tok_arr[tokp->cur_n].tok_type = NUM;
                        tokp->tok_arr[tokp->cur_n].num = (float)strtod(tp->str_arr[tp->cur_tok], NULL);
                        tokp->cur_n++;

                        return;
                  }

                  // If the token is a [num]
                  if((tp->str_arr[tp->cur_tok][0] == '[') && (tp->str_arr[tp->cur_tok][strlen(tp->str_arr[tp->cur_tok]) - 1] == ']')
                        && (tp->str_arr[tp->cur_tok][2]) != 'x')
                  {
                        size_t st_num_err = 0;
                        for(size_t i = 0; i < strlen(tp->str_arr[tp->cur_tok] - 1); i++)
                        {                                    
                              if(isdigit(tp->str_arr[tp->cur_tok][i] != 0))
                              {
                                    st_num_err++;
                                    break;
                              }
                        }

                        if(st_num_err == 0)
                        {
                              tokp->tok_arr[tokp->cur_n].asm_code += (NUM_MASK + RAM_MASK);
                              tokp->cur_n++;
                              strcpy(tokp->tok_arr[tokp->cur_n].text, tp->str_arr[tp->cur_tok]);
                              tokp->tok_arr[tokp->cur_n].tok_type = NUM;
                              tokp->tok_arr[tokp->cur_n].num = (float)strtod(tp->str_arr[tp->cur_tok], NULL);
                              tokp->cur_n++;
                              return;
                        }
                        tokp->tok_arr[tokp->cur_n].error = ERROR_NUM_POP;
                        tokp->cur_n++;
                        return;
                  }

                  // If the token is a register
                  for(size_t i = 0; i < sizeof(reg)/sizeof(reg[0]); i++)
                  {
                        if(strcmp(tp->str_arr[tp->cur_tok], reg[i]) == 0)
                        {
                              tokp->tok_arr[tokp->cur_n].asm_code += REG_MASK;
                              tokp->cur_n++;
                              tokp->tok_arr[tokp->cur_n].tok_type = REG;
                                                                  
                              if((i + 10) == AX)
                              {
                                    strcpy(tokp->tok_arr[tokp->cur_n].text, tp->str_arr[tp->cur_tok]);
                                    tokp->tok_arr[tokp->cur_n].asm_code = AX;
                              }

                              else if((i+10) == BX)
                              {
                                    strcpy(tokp->tok_arr[tokp->cur_n].text, tp->str_arr[tp->cur_tok]);
                                    tokp->tok_arr[tokp->cur_n].asm_code = BX;
                              }

                              else if((i+10) == CX)
                              {
                                    strcpy(tokp->tok_arr[tokp->cur_n].text, tp->str_arr[tp->cur_tok]);
                                    tokp->tok_arr[tokp->cur_n].asm_code = CX;
                              }

                              else if((i+10) == DX)
                              {
                                    strcpy(tokp->tok_arr[tokp->cur_n].text, tp->str_arr[tp->cur_tok]);
                                    tokp->tok_arr[tokp->cur_n].asm_code = DX;
                              }

                              tokp->cur_n++;
                              return;
                        }
                  }

                  // If the token is a [register]
                  for(size_t i = 0; i < sizeof(st_reg)/sizeof(st_reg[0]); i++) 
                  {
                        if(strcmp(tp->str_arr[tp->cur_tok], st_reg[i]) == 0)
                        {
                              tokp->tok_arr[tokp->cur_n].asm_code += (REG_MASK + RAM_MASK);
                              tokp->tok_arr[tokp->cur_n].tok_type = REG;
                              tokp->cur_n++;
                              strcpy(tokp->tok_arr[tokp->cur_n].text, tp->str_arr[tp->cur_tok]);
                              
                              if((i+14) == ST_AX)
                              {
                                    tokp->tok_arr[tokp->cur_n].asm_code = ST_AX;
                              }

                              else if((i+14) == ST_BX)
                              {
                                    tokp->tok_arr[tokp->cur_n].asm_code = ST_BX;
                              }

                              else if((i+14) == ST_CX)
                              {
                                    tokp->tok_arr[tokp->cur_n].asm_code = ST_CX;
                              }

                              else if((i+14) == ST_DX)
                              {
                                    tokp->tok_arr[tokp->cur_n].asm_code = ST_DX;
                              }

                              tokp->cur_n++;
                              return;
                        }
                  }

                  // If the token is [reg + num]
                  size_t st_reg_num_err = 0;

                  if((tp->str_arr[tp->cur_tok][0] == '[') && (tp->str_arr[tp->cur_tok][strlen(tp->str_arr[tp->cur_tok]) - 1] == ']') && 
                        (tp->str_arr[tp->cur_tok][1] == 'a' || tp->str_arr[tp->cur_tok][1] == 'b' || tp->str_arr[tp->cur_tok][1] == 'c' || tp->str_arr[tp->cur_tok][1] == 'd') 
                        && (tp->str_arr[tp->cur_tok][2] == 'x'))
                  {
                        tokp->tok_arr[tokp->cur_n].asm_code += (NUM_MASK + REG_MASK + RAM_MASK);
                        tokp->cur_n++;

                        char reg_part[MAX_WORD_LEN] = {0};
                        strncpy(reg_part, tp->str_arr[tp->cur_tok] + 1, 2);

                        for(size_t i = 0; i < sizeof(reg)/sizeof(reg[0]); i++)
                        {
                              if(strcmp(reg_part, reg[i]) == 0)
                              {
                                    strcpy(tokp->tok_arr[tokp->cur_n].text, reg_part);
                                    tokp->tok_arr[tokp->cur_n].tok_type = REG;

                                    if((i+10) == AX)
                                    {
                                          tokp->tok_arr[tokp->cur_n].asm_code = AX;
                                    }

                                    else if((i+10) == BX)
                                    {
                                          tokp->tok_arr[tokp->cur_n].asm_code = BX;
                                    }

                                    else if((i+10) == CX)
                                    {
                                          tokp->tok_arr[tokp->cur_n].asm_code = CX;
                                    }

                                    else if((i+10) == DX)
                                    {
                                          tokp->tok_arr[tokp->cur_n].asm_code = DX;
                                    }

                                    tokp->cur_n++;
                              }
                        }

                        char num_part[MAX_WORD_LEN] = {0};
                        size_t error_part = 0;
                        strncpy(num_part, tp->str_arr[tp->cur_tok] + 4, strlen(tp->str_arr[tp->cur_tok]) - 5);

                        for(size_t j = 0; j < strlen(tp->str_arr[tp->cur_tok]) - 4; j++)
                        {
                              if(isdigit(tp->str_arr[tp->cur_tok][j]) == 0)
                              {
                                    error_part++;
                                    break;
                              }
                        }

                        if(error_part == 0)
                        {
                              strcpy(tokp->tok_arr[tokp->cur_n].text, num_part);
                              tokp->tok_arr[tokp->cur_n].tok_type = NUM;
                              tokp->tok_arr[tokp->cur_n].num = (float)strtod(num_part, NULL);
                              tokp->cur_n++;
                              return;
                        }
                        tokp->tok_arr[tokp->cur_n].error = ERROR_CMD_ARG_ERROR;
                        tokp->cur_n++;
                        return;
                  }
            }

            case(HLT) : tokp->tok_arr[tokp->cur_n].asm_code = HLT;
                        tokp->cur_n++;
                        return;

            case(IN)  : tokp->tok_arr[tokp->cur_n].asm_code = IN;
                        tokp->cur_n++;                        
                        return;

            case(OUT) : tokp->tok_arr[tokp->cur_n].asm_code = OUT;
                        tokp->cur_n++;                        
                        return;

            case(ADD) : tokp->tok_arr[tokp->cur_n].asm_code = ADD;
                        tokp->cur_n++;
                        return;

            case(SUB) : tokp->tok_arr[tokp->cur_n].asm_code = SUB;
                        tokp->cur_n++;
                        return;

            case(MUL) : tokp->tok_arr[tokp->cur_n].asm_code = MUL;
                        tokp->cur_n++;
                        return;

            case(DIV) : tokp->tok_arr[tokp->cur_n].asm_code = DIV;
                        tokp->cur_n++;
                        return;

            case(JMP) : tokp->tok_arr[tokp->cur_n].asm_code = JMP;
                        tokp->cur_n++;
                        tp->cur_tok++;

                        tokp->tok_arr[tokp->cur_n].tok_type = FLAG;
                        strcpy(tokp->tok_arr[tokp->cur_n].text, tp->str_arr[tp->cur_tok]);

                        tokp->cur_n++;
                        return;

            case(JE)  : tokp->tok_arr[tokp->cur_n].asm_code = JE;
                        tokp->cur_n++;
                        tp->cur_tok++;
                        tokp->tok_arr[tokp->cur_n].tok_type = FLAG;
                        strcpy(tokp->tok_arr[tokp->cur_n].text, tp->str_arr[tp->cur_tok]);

                        tokp->cur_n++;
                        return;

            case(JGE) : tokp->tok_arr[tokp->cur_n].asm_code = JGE;
                        tokp->cur_n++;
                        tp->cur_tok++;

                        tokp->tok_arr[tokp->cur_n].tok_type = FLAG;
                        strcpy(tokp->tok_arr[tokp->cur_n].text, tp->str_arr[tp->cur_tok]);

                        tokp->cur_n++;
                        return;

            case(JNE) : tokp->tok_arr[tokp->cur_n].asm_code = JNE;
                        tokp->cur_n++;
                        tp->cur_tok++;
                        tokp->tok_arr[tokp->cur_n].tok_type = FLAG;
                        strcpy(tokp->tok_arr[tokp->cur_n].text, tp->str_arr[tp->cur_tok]);

                        tokp->cur_n++;
                        return;

            case(JG)  : tokp->tok_arr[tokp->cur_n].asm_code = JG;
                        tokp->cur_n++;
                        tp->cur_tok++;

                        tokp->tok_arr[tokp->cur_n].tok_type = FLAG;
                        strcpy(tokp->tok_arr[tokp->cur_n].text, tp->str_arr[tp->cur_tok]);

                        tokp->cur_n++;
                        return;

            case(JL)  : tokp->tok_arr[tokp->cur_n].asm_code = JL;
                        tokp->cur_n++;
                        tp->cur_tok++;

                        tokp->tok_arr[tokp->cur_n].tok_type = FLAG;
                        strcpy(tokp->tok_arr[tokp->cur_n].text, tp->str_arr[tp->cur_tok]);

                        tokp->cur_n++;
                        return;

            case(JLE) : tokp->tok_arr[tokp->cur_n].asm_code = JLE;
                        tokp->cur_n++;
                        tp->cur_tok++;

                        tokp->tok_arr[tokp->cur_n].tok_type = FLAG;
                        strcpy(tokp->tok_arr[tokp->cur_n].text, tp->str_arr[tp->cur_tok]);

                        tokp->cur_n++;
                        return;

            case(CALL): tokp->tok_arr[tokp->cur_n].asm_code = CALL;
                        tokp->cur_n++;
                        tp->cur_tok++;

                        tokp->tok_arr[tokp->cur_n].tok_type = FLAG;
                        strcpy(tokp->tok_arr[tokp->cur_n].text, tp->str_arr[tp->cur_tok]);

                        tokp->cur_n++;
                        return;

            default   : tokp->tok_arr[tokp->cur_n].error = ERROR_TOKEN;
                        printf("Error token!\n");
                        return;
      }
}

//===========================================================================================================



//===========================================================================================================

void tok_dtor(struct TokProcessing* tokp, struct TextProcessing* tp)
{

      free(tokp->tok_arr);
}
