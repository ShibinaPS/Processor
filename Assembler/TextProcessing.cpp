#include "TextProcessing.h"

//===========================================================================================================

#define STRCMP(val) strcmp(tp->tok_arr[tp->cur_n], val)
#define CHECK(flag) if(flag == true)  continue;
#define TOKEN       tp->tok_arr[tp->cur_n]  

//===========================================================================================================
void tp_ctor(struct TextProcessing* tp, struct FlagProcessing* fp, const char* filename)
{
      strcpy(tp->asmbl_filename, filename);

      file_open(tp, filename);
      if(tp->error != 0)
      {
            return;
      }

      num_of_chars(tp, filename);

      chars_buffer(tp);
      if(tp->error != 0)
      {
            return;
      }

      num_of_tok(tp);
      
      fill_tok_arr(tp);
      
      lexical_analysis(tp, fp);
      if(tp->error != 0)
      {
            printf("Lexical error in assembler code.\n");
            return;
      }

      tags_analysis(tp, fp);
      if(tp->error != 0)
      {
            printf("Flag error in assembler code.\n");
            return;
      }

      HLT_count(tp);
      if(tp->error != 0)
      {
            return;
      }

}
//===========================================================================================================

void file_open(struct TextProcessing* tp, const char* filename)
{
      tp->mainfile = fopen(filename, "r");

      if (tp->mainfile == nullptr)
      {
            printf("Error with file opening in Assembler!\n");
            tp->error = ERROR_MAINFILE_OPEN;
            return;
      }
}

//===========================================================================================================

void num_of_chars(struct TextProcessing* tp, const char* filename)
{
      struct stat buf{};
      stat(filename, &buf); 
      tp->chars_num = buf.st_size + 1;
}

//===========================================================================================================

void chars_buffer(struct TextProcessing* tp)
{
      tp->buffer_ptr = (char*)calloc(tp->chars_num, sizeof(char));

      fread(tp->buffer_ptr, sizeof(char), tp->chars_num - 1, tp->mainfile);

      tp->buffer_ptr[tp->chars_num - 1] = '\0';

      fclose(tp->mainfile);

      if((tp->buffer_ptr[0] == '\0'))
      {
            printf("File is empty!\n");
            tp->error = ERROR_EMPTY_MAINFILE;
            return;
      }
}

//===========================================================================================================

void num_of_tok(struct TextProcessing* tp)
{
      char* add_str = (char*)calloc(tp->chars_num, sizeof(char));

      strcpy(add_str, tp->buffer_ptr);

      const char* token = strtok(add_str, " \n\r");

      for(tp->tok_num; token != NULL; token = strtok(NULL, " \n\r"), tp->tok_num++)
      {
            ;
      }

      free(add_str);
}

//===========================================================================================================

void fill_tok_arr(struct TextProcessing* tp)
{
      tp->tok_arr = (char**)calloc(tp->chars_num, sizeof(char*));

      char* string = strtok(tp->buffer_ptr, " \n\r");

      for(size_t i = 0; string != NULL; string = strtok(NULL, " \n\r"), i++)
      {
            tp->tok_arr[i] = string;
      }

      /*for(size_t i = 0; i<tp->tok_num; i++)
      {
            printf("%s\n", tp->tok_arr[i]);
      }*/
      
}

//===========================================================================================================

void lexical_analysis(struct TextProcessing* tp, struct FlagProcessing* fp)
{
      for(; tp->cur_n < tp->tok_num; tp->cur_n++)
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
            size_t num_err  = 0;
            size_t dots_num = 0;

            for(size_t cur_ind = 0; cur_ind < strlen(TOKEN); cur_ind++)
            {
                  if((cur_ind == 0) && (TOKEN[0] == '-') && (isdigit(TOKEN[cur_ind + 1]) != 0))
                  {
                        continue;
                  }

                  if(isdigit(TOKEN[cur_ind]))
                  {
                        continue;
                  }

                  if((TOKEN[cur_ind] == '.') && (isdigit(TOKEN[cur_ind + 1]) != 0))
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
                  continue;
            }

            // Checking [number]
            if((TOKEN[0] == '[') && (TOKEN[strlen(TOKEN) - 1] == ']') && (TOKEN[2] != 'x') && (strlen(TOKEN) > 2))
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

                        if((TOKEN[i] == '.') && (isdigit(TOKEN[i + 1]) != 0))
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
                        continue;
                  }
            }

            //Checking [reg + num]
            dots_num              = 0;
            size_t min_num        = 0;
            size_t st_reg_num_err = 0;

            if((TOKEN[0] == '[') && (TOKEN[strlen(TOKEN) - 1] == ']') && 
            (TOKEN[1] == 'a' || TOKEN[1] == 'b' || TOKEN[1] == 'c' || TOKEN[1] == 'd') && (TOKEN[2] == 'x')
            && (strlen(TOKEN) > 5))
            {
                  for(size_t j = 3; j < strlen(TOKEN) - 1; j++)
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

                        if(isdigit(TOKEN[j]))
                        {
                              continue;
                        }

                        if(isdigit(TOKEN[j+1]) && (TOKEN[j] == '.'))
                        {
                              dots_num++;
                              continue;
                        }

                        if(isdigit(TOKEN[j]) == 0)
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
                  if(TOKEN[0] == ':')
                  {
                        printf("Error in lexeme '%s'\n", TOKEN);
                        tp->error = ERROR_FLAG;
                        return;
                  }

                  fp->tags_num++;
                  continue;
            }

            else if(TOKEN[0] == ':')
            {
                  if(TOKEN[strlen(TOKEN) - 1] == ':')
                  {
                        printf("Error in lexeme '%s'\n", TOKEN);
                        tp->error = ERROR_FLAG;
                        return;
                  }

                  fp->flags_num++;
                  continue;
            }

            printf("Error in %lu token '%s'!\n", tp->cur_n + 1, TOKEN);
            tp->error = ERROR_LEXICAL_ANALYSIS;
            return;
      }
}

//===========================================================================================================

void tags_analysis(struct TextProcessing* tp, struct FlagProcessing* fp)
{
      // Checking an equality of jumps and flags ||
      // absence of jums or flags
      
      if((fp->jmps_num != fp->flags_num))
      {
            printf("Error! Different number of jumps and flags!\n");
            tp->error = ERROR_JMP_FLG_NUM;
            return;
      }

      // Если джампов нет в программе => прекращение работы функции
      if(fp->jmps_num == 0)
      {
            return;
      }

      // Orginizing all tags and flags in tags_arr and flags_arr
      fp->tags_arr  = (Flag*)calloc(fp->tags_num, sizeof(Flag));
      if (fp->tags_arr == nullptr) 
      {
            tp->error = ERROR_TAG_CALLOC;
            return;
      }

      fp->flags_arr = (Flag*)calloc(fp->flags_num, sizeof(Flag));
      if (fp->flags_arr == nullptr) 
      {
            tp->error = ERROR_FLAG_CALLOC;
            return;
      }

      // заполняю поля массива структур
      for(size_t i = 0; i < tp->tok_num; i++)
      {
            if(tp->tok_arr[i][strlen(tp->tok_arr[i])-1] == ':')
            {
                  strcpy(fp->tags_arr[fp->cur_tag].word, tp->tok_arr[i]);
                  fp->tags_arr[fp->cur_tag].index = i;
                  fp->cur_tag++;
            }

            else if(tp->tok_arr[i][0] == ':')
            {
                  strcpy(fp->flags_arr[fp->cur_flag].word, tp->tok_arr[i]);
                  fp->cur_flag++;
            }
      }
      // for(size_t m = 0; m < fp->flags_num; m++)
      // {
      //       printf("%s\n", fp->flags_arr[m].word);
      // }

      // for(size_t m = 0; m < fp->tags_num; m++)
      // {
      //       printf("%s ", fp->tags_arr[m].word);
      // }
      // printf("\n");
      // for(size_t m = 0; m < fp->tags_num; m++)
      // {
      //       printf("%d  ", fp->tags_arr[m].index);
      // }



      // Checking twice using tags
      for(size_t i = 0; i < fp->tags_num; i++)
      {
            for(size_t j = 0; j < fp->tags_num; j++)
            {
                  if((strcmp(fp->tags_arr[i].word, fp->tags_arr[j].word) == 0) && (i != j))
                  {
                        printf("Error twice using tag '%s'\n", fp->tags_arr[i].word);
                        tp->error = ERROR_NUM_TAGS;
                        return;
                  }
            }
      }

      //Checking the existance of tag for flag
      for(size_t i = 0; i < fp->flags_num; i++)
      {     
            size_t n = 0;

            char flag[MAX_WORD_LEN] = {0};
            strcpy(flag, fp->flags_arr[i].word + 1);

            for(size_t j = 0; j < fp->tags_num; j++)
            {
                  char tag[MAX_WORD_LEN] = {0};
                  strncpy(tag, fp->tags_arr[j].word, strlen(fp->tags_arr[j].word)-1);
                  // printf("%s\n", tag);

                  if(strcmp(flag, tag) == 0)
                  {
                        n++;
                  }
            }

            if(n == 0)
            {
                  printf("Error in flag '%s'. Such tag does not exist!\n", flag);
                  tp->error = ERROR_FLAG;
                  return;
            }
      }

      // Run the tags_arr and checking indexes to delete labels

      fp->new_tags_arr = (Flag*)calloc(fp->tags_num, sizeof(Flag));
      if(fp->new_tags_arr == nullptr)
      {
            tp->error = ERROR_NEW_TAG_CALLOC;
            return;
      }

      strcpy(fp->new_tags_arr[0].word, fp->tags_arr[0].word);
      fp->new_tags_arr[0].index = fp->tags_arr[0].index;

      for(size_t i = 0; i < fp->tags_num-1; i++)
      {
            if(fp->tags_arr[i+1].index - fp->tags_arr[i].index == 1)
            {
                  strcpy(fp->new_tags_arr[i+1].word, fp->tags_arr[i+1].word);
                  fp->new_tags_arr[i+1].index = fp->tags_arr[i+1].index;
            }

            else
            {
                  strcpy(fp->new_tags_arr[i+1].word, fp->tags_arr[i+1].word);
                  fp->new_tags_arr[i+1].index = fp->tags_arr[i+1].index - (i+1);
            }     
      }

      /*for(size_t i = 0; i < fp->tags_num; i++)
      {
            printf("%s ", fp->new_tags_arr[i].word);
      }
      printf("\n");
      for(size_t i = 0; i < fp->tags_num; i++)
      {
            printf("%d  ", fp->new_tags_arr[i].index);
      }*/
}

//===========================================================================================================

void HLT_count(struct TextProcessing* tp)
{
      for(size_t i = 0; i < tp->tok_num; i++)
      {
            if(strcmp(tp->tok_arr[i], "hlt") == 0)
            {
                  tp->hlt_count++;
            }
      }

      if(tp->hlt_count < 1)
      {
            printf("HLT error! There is no the end of the programm!\n");
            tp->error = ERROR_HLT_COUNT;
            return;
      }
}

//===========================================================================================================

void tp_dtor(struct TextProcessing* tp, struct FlagProcessing* fp)
{
      free(tp->buffer_ptr);
      free(tp->tok_arr);
      free(fp->tags_arr);
      free(fp->flags_arr);
      free(fp->new_tags_arr);
}

//===========================================================================================================

#undef STRCMP
