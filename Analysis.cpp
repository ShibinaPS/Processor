#include "FileProcessing.h"

//===========================================================================================================

const char* cmds[] = {"in", "out", "push", "pop", "hlt", "add", "sub", "mul", "div"};


const char* jmp[] = {"jmp", "ja", "jae", "jb", "jbe", "jc", "jcxz", "je", "jg", "jge", 
                  "jl", "jle", "jna", "jnae", "jnb", "jnbe", "jnc", "jne", "jng", "jnge", 
                  "jnl", "jnle", "jno", "jnp", "jns", "jnz", "jo", "jp", "jpe", "jpo", "js", "jz"};

const char* args[] = {"ax", "bx", "cx", "dx"};

//=========================================================================================================== 

void lexical_analysis(ProcDebug* proc)
{
      size_t hlt_count = 0;
      size_t i         = 0;

      while (i < proc->strings_num) 
      {
            size_t num_l = 1;   // Номер лексемы для понимания того, что мы должны получить: команду или 
                                // её аргумент.
            char* token;        // Лексема.
            char del[] = " ";   // Разделитель лексем.
            size_t pop_n = 0;

            for(token = strtok(proc->structs_arr[i].str_ptr, del); token != NULL; token = strtok(NULL, del), num_l++)
            { // При каждом вхождении в цикл рассматриваю отдельную лексему: команду или её аргумент.

                  printf("%d\n", num_l);
                  printf("%s\n", token);

                  size_t flag = 0;

                  if (num_l > 3)
                  {
                        printf("Error in %d string. You should write different command in the different lines!\n", i);
                        exit(ERROR_ONE_STRING);
                  }

                  if(num_l % 2 == 0) // Т.е. если это аргумент.
                  {
                        size_t r = 0;
                        
                        for(size_t m = 0; m < sizeof(cmds)/sizeof(cmds[0]); m++)
                        {
                              if (strcmp(cmds[m], token) == 0)
                              {
                                    printf("Error in %d string. You should write different command in the different lines!\n", i);
                                    exit(ERROR_ONE_STRING);
                              }
                        }

                        for(r = 0; r < sizeof(args)/sizeof(args[0]); r++) // Если регистр.
                        {
                              if (strcmp(args[r], token) == 0)
                              {
                                    flag = 1;
                              }
                        }

                        if(isdigit(*token)) // Положительные и отрицательные числа.
                        {
                              printf("%s\n", token);
                              if (pop_n == 1)
                              {
                                    printf("The command '%s' can not contain a number as an argument.\n", cmds[3]);
                                    exit(ERROR_POP_NUM);
                              }

                              flag = 1;
                        }

                        else // Ситауции типа [ax], [5], [ax+5], где прибавляемое число обязательно положительное.
                        {    // Выделила этот участок отдельно, хотя многое повторяется, но это было нужно
                             // из-за условия положительности чисел, прибавляемых к регистрам.

                              token = strtok(token, "[]"); // В полученном куске вновь выделяю лексемы.

                              for(r = 0; r < sizeof(args)/sizeof(args[0]); r++) // Если отдельно регистр, ex.: [ax].
                              {
                                    if (strcmp(args[r], token) == 0)
                                    {
                                          flag = 1;
                                    }
                              }

                              if (isdigit(*token) && (*token) > 0) // Если отдельно число, ex.: [5].
                              {
                                    if (pop_n == 1)
                                    {
                                          printf("The command '%s' can not contain a [number] as an argument.\n", cmds[3]);
                                          exit(ERROR_POP_NUM);
                                    }

                                    flag = 1;
                              }

                              if (strtok(NULL, "+") != NULL)
                              {
                                    for(token = strtok(token, "+"); token != NULL; token = strtok(NULL, "+"))
                                    { // К данному моменту остаются две ситуации: одна из них проверится в этом куске,
                                    // если, как положено, к регистру прибавляется положительное число; а вторая 
                                    // просто останется "нерассмотренной", за счёт чего flag не изменит своего исходного
                                    // занчения 0.

                                          for (r = 0; r < sizeof(args)/sizeof(args[0]); r++) // Проверяю первую часть токена на регистр.
                                          {
                                                if (strcmp(args[r], token) == 0)
                                                {
                                                      flag = 1;
                                                }
                                          }

                                          if (isdigit(*token)) // Проверяю вторую часть токена на положительное число.
                                          {
                                                flag = 1;
                                          }
                                    }
                              }                              
                        }
                  }

                  else // Т.е. если это команда
                  {
                        for(size_t m = 0; m < sizeof(cmds)/sizeof(cmds[0]); m++)
                        {
                              if (strcmp(cmds[m], token) == 0)
                              {
                                    if (m == 4)
                                    {
                                          hlt_count++;
                                    }

                                    if (m == 3)
                                    {
                                          pop_n = 1;
                                    }
                                    flag = 1;
                              }
                        }
                  }

                  if(flag == 0)
                  {
                        printf("Lexical error in %d string at the postion: %s\n", i, token);
                        exit(ERROR_LEXICAL_UNIT);
                  }
            }

            i++; // Перехожу на новую строку
      }

      if (hlt_count == 0)
      {
            printf("The command %s was not found.\n", cmds[4]);
            exit(ERROR_HLT_COUNT);
      }
}

//===========================================================================================================