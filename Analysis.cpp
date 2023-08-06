#include "FileProcessing.h"

//=========================================================================================================== 

void lexical_analysis(Processor* proc)
{
      const char* cmd[] = {"ax", "bx", "cx", "dx", "in", "out", "push", "pop", "hlt", "add", "sub", "mul", "div", "call", "jmp", 
                        "je", "jge"};

      size_t hlt_counter = 0;
      size_t i           = 0;

      while (i < proc->strings_num) 
      {
            char* string = (char*)calloc(proc->structs_arr[i].str_len, sizeof(char));
            strcpy(string, proc->structs_arr[i].str_ptr);

            const char* token = strtok(string, " "); // Выделяю первую лексему в строке

            for(size_t num_l = 1; token != NULL; token = strtok(NULL, " "), num_l++) // При каждом вхождении в цикл рассматриваю отдельную лексему
            {
                  // printf("%d\n", num_l);
                  // printf("%s\n", token);
                  size_t flag = 0;

                  if(token[0] == '-')
                  {
                        for(size_t j = 1; j < strlen(token); j++)
                        {
                              if(!(isdigit(token[j])))
                              {
                                    printf("Lexical error in %d line in the lexeme part '%s'! There should be a number.", i+1, token);
                                    exit(ERROR_LEXICAL_PART);
                              }
                        }
                        continue; // Перехожу к другому токену
                  }

                  for(size_t j = 0; j < sizeof(cmd)/sizeof(cmd[0]); j++) 
                  {
                        if(strcmp(cmd[j], token) == 0)
                        {
                              if (j == 8)
                              {
                                    hlt_counter++;
                              }

                              flag = 1;
                              break; // Выхожу из цикла по массиву указателей
                        }
                  }

                  if(flag == 1)
                  {
                        continue; // Перехожу к новому токену
                  }

                  else if(token[0] == '[')
                  {
                        size_t n = 1;
                        char* intro = (char*)calloc(strlen(token) - 2, sizeof(char));
                        size_t m = 0;

                        while(token[n] != ']')
                        {
                              if (token[n] == '+')
                              {
                                    for(size_t j = 0; j < sizeof(cmd)/sizeof(cmd[0]); j++)
                                    {
                                          if(strcmp(intro, cmd[j]) == 0)
                                          {
                                                strncpy(intro, "", m);
                                                m = 0;
                                                n++;
                                                flag = 1;
                                                break; // Выхожу из цикла for, чтобы далее проверить вторую часть на число
                                          }
                                    }

                                    if (flag == 0)
                                    {
                                          printf("Lexical error in %d line in the lexeme part '%s' between []!", i+1, intro);
                                          exit(ERROR_LEXICAL_PART);
                                          
                                    }
                              }

                              else if (token[n] == '-') // Две ситуации: либо отр. число, либо ex.: [ax-5]
                              {
                                    for(size_t j = 0; j < sizeof(cmd)/sizeof(cmd[0]); j++)
                                    {
                                          if(strcmp(intro, cmd[j]) == 0)
                                          {
                                                flag = 1;
                                          }
                                    }

                                    if (flag == 1)
                                    {
                                          printf("Lexical error in %d line in the lexeme part '%c'! The token '%s' cannot contain a sign minus.", i+1, token[n], token);
                                          exit(ERROR_LEXICAL_PART);
                                    }

                                    printf("Lexical error in %d line in the %d token between []! A negative number is not allowed.", i+1, num_l);
                                    exit(ERROR_LEXICAL_PART);
                              }

                              else
                              {
                                    strncpy(intro + m, token + n, 1);
                                    n++;
                                    m++;
                              }
                        }

                        if(flag == 1) // Т.е. ситуация со сложным выражением внутри скобок => осталось проверить на число
                        {
                              for(size_t j = 0; j < strlen(intro); j++)
                              {
                                    if(!(isdigit(intro[j])))
                                    {
                                          printf("Lexical error in %d line in the lexeme part '%s'! There should be a number.", i+1, intro);
                                          exit(ERROR_LEXICAL_PART);
                                    }
                              }

                              if(token[n+1] == ']')
                              {
                                    printf("Lexical error in %d line! Too many '%c'.", i+1, token[n+1]);
                                    exit(ERROR_LEXICAL_PART);
                              }
                              continue; // Переход к новому токену
                        }

                        else // Внутри [] должно быть число (> 0) || регистр 
                        {
                              for(size_t j = 0; j < sizeof(cmd)/sizeof(cmd[0]); j++) // Если регистр
                              {
                                    if(strcmp(intro, cmd[j]) == 0)
                                    {
                                          flag = 1;
                                          break;
                                    }
                              }

                              if (flag == 1)
                              {
                                    if(token[n+1] == ']')
                                    {
                                          printf("Lexical error in %d line! Too many '%c'.", i+1, token[n+1]);
                                          exit(ERROR_LEXICAL_PART);
                                    }
                                    continue; // Переход к новому токену
                              }

                              else
                              {
                                    for(size_t j = 0; j < strlen(intro); j++)
                                    {
                                          if(!(isdigit(intro[j]))) // Все ли цифры
                                          {
                                                printf("Lexical error in %d line in the lexeme part '%s'!", i+1, intro);
                                                exit(ERROR_LEXICAL_PART);
                                          }
                                    }

                                    if(token[n+1] == ']')
                                    {
                                          printf("Lexical error in %d line! Too many '%c'.", i+1, token[n+1]);
                                          exit(ERROR_LEXICAL_PART);
                                    }  
                                    continue; // Переход к новому токену
                              }
                        }
                        continue;
                  }

                  else if(token[0] == ':' || token[strlen(token)-1] == ':' || strcmp(token, "]") == 0) // Т.е. это метка или учёт пробела в скобках
                  {
                        continue;
                  }

                  else // Остался вариант того, что это число
                  {
                        for(size_t j = 0; j < strlen(token); j++)
                        {
                              if(!(isdigit(token[j])))
                              {
                                    printf("Lexical error in %d line in the lexeme part '%s'!", i+1, token);
                                    exit(ERROR_LEXICAL_PART);
                              }
                              continue;
                        }
                  }
            }
      
            i++; // Переход на новую строку
      }

      if (hlt_counter == 0)
            {
                  printf("Lexical error! The programm should contain 'hlt'.");
                  exit(ERROR_HLT_COUNT);
            }
}

//===========================================================================================================
