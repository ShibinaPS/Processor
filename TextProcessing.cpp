#include "TextProcessing.h"
//===========================================================================================================

#define STRCMP(val) strcmp(tp->str_arr[tp->cur_tok], val)
#define CHECK(flag) if(flag == true)  continue;
#define TOKEN       tp->str_arr[tp->cur_tok]  

//===========================================================================================================
int tp_ctor(struct TextProcessing* tp, struct FlagProcessing* fp, const char* filename)
{
      file_open(tp, filename);
      num_of_chars(tp, filename);
      chars_buffer(tp);
      num_of_tok(tp);
      fill_str_arr(tp);
      lexical_analysis(tp, fp);
      tags_analysis(tp, fp);

      // сначала лексика (есть ли слово в словаре, мб число), потом токенизация, 
      // потом проверка синтаксиса (не проверка текста, а проверка порядка слов в тексте),
      // потом проверка меток, 
      // ТОЛЬКО ПОТОМ БАЙТ КОД

      return 0;
}

// вид структуры из массива структур (по-русски --- токенов)
// char text[MAX_WORD_LEN] = {0};
// assemly_code
// CPU_code
// error

//===========================================================================================================

void file_open(struct TextProcessing* tp, const char* filename)
{
      tp->mainfile = fopen(filename, "r");

      if (tp->mainfile == nullptr)
      {
            printf("Error with file opening in Assembler!\n");
            exit(ERROR_MAINFILE_OPEN);
      }
}

//===========================================================================================================

void num_of_chars(struct TextProcessing* tp, const char* filename)
{
      struct stat buf{};
      stat(filename, &buf); 

      if(buf.st_size <= 0)
      {
            printf("Error in stat size!\n");
            exit(ERROR_STAT_SIZE);
      }

      tp->chars_num = buf.st_size + 1;
      return;
}

//===========================================================================================================

void chars_buffer(struct TextProcessing* tp)
{
      tp->buffer_ptr = (char*)calloc(tp->chars_num, sizeof(char));

      if (tp->buffer_ptr == nullptr)
      {
            printf("Error buffer is nullptr!\n");
            exit(ERROR_BUFFER_NULLPTR);
      }

      fread(tp->buffer_ptr, sizeof(char), tp->chars_num - 1, tp->mainfile);

      tp->buffer_ptr[tp->chars_num - 1] = '\0';

      fclose(tp->mainfile);

      return;
}

//===========================================================================================================

void num_of_tok(struct TextProcessing* tp)
{
      char* add_str = (char*)calloc(tp->chars_num, sizeof(char));

      strcpy(add_str, tp->buffer_ptr);

      const char* token = strtok(add_str, " \n\r");

      for(tp->tok_num; token != NULL; token = strtok(NULL, " "), tp->tok_num++)
      {
            ;
      }

      free(add_str);
}

//===========================================================================================================

int fill_str_arr(struct TextProcessing* tp)
{
      tp->str_arr = (char**)calloc(tp->chars_num, sizeof(char*));

      char* string = strtok(tp->buffer_ptr, " \n\r");

      for(size_t i = 0; string != NULL; string = strtok(NULL, " \n\r"), i++)
      {
            tp->str_arr[i] = string;
      }

      /*for(size_t i = 0; i<tp->tok_num; i++)
      {
            printf("%s\n", tp->str_arr[i]);
      }
      */
}

//===========================================================================================================

const char* cmd[] = {"in", "out", "push", "pop", "hlt", 
                     "add", "sub", "mul", "div"};

const char* reg[] = {"ax", "bx", "cx", "dx"};

const char* st_reg[] = {"[ax]", "[bx]", "[cx]", "[dx]"};

const char* jmp[] = {"jmp", "je", "jge", "call"};

//===========================================================================================================
void lexical_analysis(struct TextProcessing* tp, struct FlagProcessing* fp)
{
      for(tp->cur_tok; tp->cur_tok < tp->tok_num; tp->cur_tok++)
      {
            bool flag = false;
            // Checking cmd, reg, [reg], jmp

            // Checking commands
            for(size_t j = 0; j < sizeof(cmd)/sizeof(cmd[0]); j++) 
            {
                  if(STRCMP(cmd[j]) == 0)
                  {
                        flag = true;
                        break;
                  }
            }
            CHECK(flag);

            // Checking registers
            for(size_t j = 0; j < sizeof(reg)/sizeof(reg[0]); j++)
            {
                  if(STRCMP(reg[j]) == 0)
                  {
                        flag = true;
                        break;
                  }
            }
            CHECK(flag);

            // Checking [registers]
            for(size_t j = 0; j < sizeof(st_reg)/sizeof(st_reg[0]); j++) 
            {
                  if(STRCMP(st_reg[j]) == 0)
                  {
                        flag = true;
                        break;
                  }
            }
            CHECK(flag);

            // Checking jmps
            for(size_t j = 0; j < sizeof(jmp)/sizeof(jmp[0]); j++) 
            {
                  if(STRCMP(jmp[j]) == 0)
                  {
                        fp->jmps_num++;
                        flag = true;
                        break;
                  }
            }
            CHECK(flag);

            // Checking numbers (+ && -)
            size_t num_err   = 0;
            size_t dots_num  = 0;

            for(size_t cur_ind = 0; cur_ind < strlen(TOKEN); cur_ind++)
            {
                  if((cur_ind == 0) && (TOKEN[0] == '-'))
                  {
                        continue;
                  }

                  if(isdigit(TOKEN[cur_ind]))
                  {
                        continue;
                  }

                  if((TOKEN[cur_ind] == '.'))
                  {
                        dots_num++;
                        continue;
                  }

                  if(isdigit(TOKEN[cur_ind]) == 0)
                  {
                        num_err++;
                        break;
                  }
            }

            if(num_err == 0 && dots_num < 2) // It means that we have met a number
            {
                  // flag = true;
                  continue;
            }

            // Checking [number]
            if((TOKEN[0] == '[') && (TOKEN[strlen(TOKEN) - 1] == ']') && (TOKEN[2] != 'x'))
            {
                  dots_num          = 0;
                  size_t minus_num  = 0;
                  size_t st_num_err = 0;
                  for(size_t i = 1; i < strlen(TOKEN)-1; i++)
                  {
                        if(isdigit(TOKEN[i]))
                        {
                              continue;
                        }

                        if((TOKEN[i] == '.'))
                        {
                              dots_num++;
                              continue;
                        }

                        if((i == 1) && (TOKEN[i] == '-'))
                        {
                              minus_num++;
                              break;
                        }

                        if(isdigit(TOKEN[i]) == 0)
                        {
                              st_num_err++;
                              break;
                        }
                  }

                  if(minus_num == 0 && st_num_err == 0 && dots_num < 2)
                  {
                        // flag = true;
                        continue;
                  }
            }

            //Checking [reg + num]
            dots_num              = 0;
            size_t min_num        = 0;
            size_t st_reg_num_err = 0;

            if((TOKEN[0] == '[') && (TOKEN[strlen(TOKEN) - 1] == ']') && 
            (TOKEN[1] == 'a' || TOKEN[1] == 'b' || TOKEN[1] == 'c' || TOKEN[1] == 'd') && (TOKEN[2] == 'x'))
            {
                  for(size_t j = 1; j < strlen(TOKEN) - 1; j++)
                  {
                        if((j == 3) && (TOKEN[j] == '+'))
                        {
                              continue;
                        }

                        if((j == 3) && (TOKEN[j] == '-'))
                        {
                              min_num++;
                              break;
                        }

                        if((j > 3) && (isdigit(TOKEN[j])))
                        {
                              continue;
                        }

                        if((j > 3) && (isdigit(TOKEN[j])) && (TOKEN[j] == '.'))
                        {
                              dots_num++;
                              continue;
                        }

                        if((j > 3) && (isdigit(TOKEN[j]) == 0))
                        {
                              st_reg_num_err++;
                              break;
                        }                    
                  }

                  if(min_num == 0 && dots_num < 2 && st_reg_num_err == 0)
                  {
                        continue;
                  }
            }

            // Checking tags
            if(TOKEN[strlen(TOKEN) - 1] == ':')
            {
                  fp->tags_num++;
                  continue;
            }

            if(TOKEN[0] == ':')
            {
                  fp->flags_num++;
                  continue;
            }

            printf("Error in %d token '%s'!\n", tp->cur_tok + 1, TOKEN);
            exit(ERROR_LEXICAL_ANALYSIS);
      }
}

//===========================================================================================================

void tags_analysis(struct TextProcessing* tp, struct FlagProcessing* fp)
{
      // Checking an equality of jumps and flags ||
      // absence of jums or flags
      if((fp->jmps_num != fp->flags_num) || 
         (fp->flags_num == 0 && fp->jmps_num != 0) || 
         (fp->flags_num != 0 && fp->jmps_num == 0))
      {
            printf("Error! Different number of jumps and flags!\n");
            exit(ERROR_JMP_FLG_NUM);
      }

      // Orginizing all tags and flags in tags_arr and flags_arr
      fp->tags_arr = (Flag*)calloc(fp->tags_num, sizeof(Flag*));
      fp->flags_arr = (Flag*)calloc(fp->flags_num, sizeof(Flag*));

      for(size_t i = 0; i < tp->tok_num; i++)
      {
            if(tp->str_arr[i][strlen(tp->str_arr[i])-1] == ':')
            {
                  // printf("%s\n", TOKEN);
                  strcpy(fp->tags_arr[fp->cur_tag].word, tp->str_arr[i]);
                  // printf("%s\n", fp->tags_arr[fp->cur_tag].word);
                  fp->cur_tag++;
                  // printf("%d\n", fp->cur_tag);
            }

            else if(tp->str_arr[i][0] == ':')
            {
                  strcpy(fp->flags_arr[fp->cur_flag].word, tp->str_arr[i]);
                  fp->cur_flag++;
            }
      }

      /*for(size_t m = 0; m < fp->flags_num; m++)
      {
            printf("%s\n", fp->flags_arr[m].word);
      }*/

      /*for(size_t m = 0; m < fp->tags_num; m++)
      {
            printf("%s\n", fp->tags_arr[m].word);
      }*/


      // Checking twice using tags
      for(size_t i = 0; i < fp->tags_num; i++)
      {
            for(size_t j = 0; j < fp->tags_num; j++)
            {
                  if((strcmp(fp->tags_arr[i].word, fp->tags_arr[j].word) == 0) && (i != j))
                  {
                        printf("Error twice using tag '%s'\n", fp->tags_arr[i].word);
                        exit(ERROR_NUM_TAGS);
                  }
            }
      }

      //Checking the existence of tag for flag
      for(size_t i = 0; i < fp->flags_num; i++)
      {     
            size_t n = 0;

            char flag[MAX_WORD_LEN] = {0};
            strcpy(flag, fp->flags_arr[i].word + 1);
            // printf("%s\n", flag);

            for(size_t j = 0; j < fp->tags_num; j++)
            {
                  char tag[MAX_WORD_LEN] = {0};
                  strncpy(tag, fp->tags_arr[j].word, strlen(fp->tags_arr[j].word)-1);
                  // printf("%s\n", tag);

                  // printf("%d\n", strcmp(flag, tag));

                  if(strcmp(flag, tag) == 0)
                  {
                        n++;
                  }
            }

            if(n == 0)
            {
                  printf("Error in flag '%s'. Such tag does not exist!\n", flag);
                  exit(ERROR_FLAG);
            }
      }
}

//===========================================================================================================
void tp_dtor(struct TextProcessing* tp, struct FlagProcessing* fp)
{
      free(tp->buffer_ptr);
      free(tp->str_arr);
      free(fp->tags_arr);
      free(fp->flags_arr);
}

//===========================================================================================================