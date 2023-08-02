#include "FileProcessing.h"

//===========================================================================================================

const char* cmds[] = {"in", "out", "push", "pop", "hlt", "add", "sub", "mul", "div"};

const char* jmp[] = {"jmp", "ja", "jae", "jb", "jbe", "jc", "jcxz", "je", "jg", "jge", 
                  "jl", "jle", "jna", "jnae", "jnb", "jnbe", "jnc", "jne", "jng", "jnge", 
                  "jnl", "jnle", "jno", "jnp", "jns", "jnz", "jo", "jp", "jpe", "jpo", "js", "jz"};

const char* args[] = {"ax", "bx", "cx", "dx"};

//=========================================================================================================== 

void lexical_analysis(Processor* proc)
{
      size_t hlt_count = 0;
      size_t i         = 0; // Номер строки

      while (i < proc->strings_num) 
      {
            size_t pop_n = 0; // Наличие pop, потому что на аргументы данной команды есть некоторые ограничения

            const char* token = strtok(proc->structs_arr[i].str_ptr, " "); // Выделила первую лексему в строке

            for(size_t num_l = 1; token != NULL; token = strtok(NULL, " "), num_l++)
            { // При каждом вхождении в цикл рассматриваю отдельную лексему: команду или её аргумент.
                  
                  // printf("%d\n", num_l);
                  // printf("%s\n", token);

                  size_t flag = 0; // Значение 1 будет говорить об успешно считанн(ом) аргументе или (ой) команде

                  if(num_l % 2 != 0) // Т.е. если это команда или метка
                  {
                        for (size_t j = 0; j < (proc->strings_num)/2; j++) // Если метка
                        {
                              if (strcmp(proc->tags_arr[j].str_ptr, token) == 0)
                              {
                                    flag = 1;
                              }
                        }

                        for (size_t m = 0; m < sizeof(cmds)/sizeof(cmds[0]); m++) // Если команда
                        {
                              if (strcmp(cmds[m], token) == 0)
                              {
                                    if(num_l == 3) // Запись кода в одну строку
                                    {
                                          printf("Error in %d string. You should write different command in the different lines!\n", i+1);
                                          exit(ERROR_ONE_STRING);
                                    }

                                    if (m == 4)
                                    {
                                          hlt_count++;
                                    }

                                    else if (m == 3)
                                    {
                                          pop_n = 1;
                                    }

                                    flag = 1;
                              }
                        }

                        if(flag == 0) // Чтобы не заходить сюда, если уже нашли команду
                        {
                              for(size_t s = 0; s < sizeof(jmp)/sizeof(jmp[0]); s++) // Если это jmp
                              {
                                    if(strcmp(jmp[s], token) == 0)
                                    {
                                          token = strtok(NULL, " :"); // Считываю следующий за jmp элемент, т.е. метку
                                          
                                          flag = tag_analysis(proc, token, i);
                                    }
                              }
                        }
                  }

                  else // Т.е. если это аргумент.
                  {                        
                        for(size_t m = 0; m < sizeof(cmds)/sizeof(cmds[0]); m++) // К примеру, если после in идёт pop
                        {
                              if (strcmp(cmds[m], token) == 0)
                              {
                                    printf("Error in %d string. You should write different commands in the different lines!\n", i+1);
                                    exit(ERROR_ONE_STRING);
                              }
                        }

                        for(size_t r = 0; r < sizeof(args)/sizeof(args[0]); r++) // Если регистр.
                        {
                              if (strcmp(args[r], token) == 0)
                              {
                                    flag = 1;
                              }
                        }
                        
                        if(isdigit(*token)) // Положительные и отрицательные числа.
                        {
                              if (pop_n == 1)
                              {
                                    printf("The command '%s' can not contain a number as an argument.\n", cmds[3]);
                                    exit(ERROR_POP_NUM);
                              }

                              flag = 1;
                        }

                        else if(flag == 0)// Ситауции типа: [ax], [5], [ax+5], где прибавляемое число обязательно положительное.
                        {    // Выделила этот участок отдельно из-за условия положительности чисел, прибавляемых к регистрам.

                              char* arg = (char*)malloc(sizeof(char)*(strlen(token) - 2));
                              // Здесь уже работаю с копией, потому что иначе функция strtok перестраивается
                              // и становится невозможным считать 3-ий элемент строки, если такой имеется
                              strncpy(arg, token + 1, strlen(token) - 2);
                              arg[strlen(arg) - 1] = '\0';

                              for(size_t r = 0; r < sizeof(args)/sizeof(args[0]); r++) // Если отдельно регистр, ex.: [ax].
                              {
                                    if (strcmp(args[r], arg) == 0)
                                    {
                                          flag = 1;
                                    }
                              }

                              if (isdigit(*arg) && (arg) > 0) // Если отдельно число, ex.: [5].
                              {
                                    if (pop_n == 1)
                                    {
                                          printf("The command '%s' can not contain a [number] as an argument.\n", cmds[3]);
                                          exit(ERROR_POP_NUM);
                                    }

                                    flag = 1;
                              }

                              else // Рассматриваю выражения, ex.: [ax+5]
                              {
                                    //=====================================================================

                                    char* reg = (char*)malloc(sizeof(char)*2); // Отдельно регистр 
                                    strncpy(reg, arg, 2);
                                    reg[strlen(reg) - 1] = '\0'; // Чтобы убрать ']'

                                    for(size_t r = 0; r < sizeof(args)/sizeof(args[0]); r++)
                                    {
                                          if (strcmp(args[r], reg) == 0)
                                          {
                                                flag = 1;
                                          }
                                    }
                                    
                                    if(flag == 0)
                                    {
                                          printf("Lexical error in %d string in position '%s'", i+1, reg);
                                          exit(ERROR_REG_PART);
                                    }

                                    flag = 0;

                                    //=====================================================================

                                    char* sign = (char*)malloc(sizeof(char)); // Отдельно знак
                                    strncpy(sign, arg + 2, 1);
                                    sign[strlen(sign)-2] = '\0';

                                    if(strcmp(sign, "-") == 0)
                                    {
                                          printf("Error in %d string! The sign '-' is unacceptable in the expression %s contained inside []!", i+1, arg);
                                          exit(ERROR_SIGN_PART);
                                    }

                                    else if(strcmp(sign, "+") == 0)
                                    {
                                          flag = 1;
                                    }

                                    else
                                    {
                                          printf("Unknown sign '%s' in %d string in the expression '%s' contained inside []! Be more attentive.", sign, i+1, arg);
                                          exit(ERROR_UNKNOWN_SIGN);
                                    }

                                    flag = 0;

                                    //=====================================================================

                                    char* num = (char*)malloc(sizeof(char)*(strlen(arg) - 3)); // Отдельно число
                                    strncpy(num, arg + 3, strlen(arg) - 3);
                                    num[strlen(num)-2] = '\0';

                                    if(isdigit(*num))
                                    {
                                          flag = 1;
                                    }

                                    else
                                    {
                                          printf("Error in %d string! You should write a number in the expression '%s' contained inside [] after the sign '%s'.", i+1, arg, sign);
                                          exit(ERROR_NUM_PART);
                                    }

                                    //=====================================================================
                              }                           
                        }
                  }

                  if(flag == 0)
                  {
                        printf("Lexical error in %d string at the position: %s\n", i+1, token);
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

int tag_analysis(Processor* proc, const char* token, size_t string)
{
      char* copy_token = (char*)malloc(sizeof(char)*(strlen(token))); // Создаю копию, так как в tags_arr метки хранятся
      strncpy(copy_token, token, strlen(token) + 1);                      // с ':' в конце
      strcat(copy_token, ":");

      for (size_t j = 0; j < (proc->strings_num)/2; j++) 
      {
            if (proc->tags_arr[j].num_tag > 1) // Метка должна встречаться один раз
            {
                  printf("Tag error in %d string! The tag '%s' is already in use.\n", string+1, proc->tags_arr[j].str_ptr);
                  exit(ERROR_TAG_AGAIN);
            }

            if (strcmp(proc->tags_arr[j].str_ptr, copy_token) == 0) // Проверка, что метка из данного jmp есть в программе
            {
                  return 1;
            }
      }

      printf("Tag error in %d string! There is no tag '%s'.\n", string+1, copy_token);
      exit(ERROR_TAG);
}

//===========================================================================================================