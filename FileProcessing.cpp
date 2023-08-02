#include "FileProcessing.h"

char PSN[] = {"POISON"};

//===========================================================================================================

int proc_ctor(Processor* proc, const char* filename)
{
      file_open(proc, filename);
      
      num_of_chars(proc, filename);
      
      chars_buffer(proc, proc->mainfile);

      fill_structs_arr(proc);

      lexical_analysis(proc);

      return 0;
}

//===========================================================================================================

void file_open(Processor* proc, const char* filename)
{
      proc->mainfile = fopen(filename, "r+");

      if (proc->mainfile == nullptr)
      {
            printf("Error code: %d\n", ERROR_MAINFILE_OPEN);
            exit(ERROR_MAINFILE_OPEN);
      }
}

//===========================================================================================================

void chars_buffer(Processor* proc, FILE* filename)
{
      proc->buffer_ptr = (char*)calloc(proc->chars_num, sizeof(char));

      if (proc->buffer_ptr == nullptr)
      {
            printf("Error code: %d\n", ERROR_BUFFER_NULLPTR);
            exit(ERROR_BUFFER_NULLPTR);
      }

      size_t fread_count = fread(proc->buffer_ptr, sizeof(char), proc->chars_num - 1, filename);

      num_of_strings(proc);

      if(fread_count != (proc->chars_num - proc->strings_num))
      {
            printf("Error code: %d\n", ERROR_FREAD_COUNT);
            exit(ERROR_FREAD_COUNT);
      }

      proc->buffer_ptr[proc->chars_num - 1] = '\0';

}

//===========================================================================================================

void num_of_chars(Processor* proc, const char* filename)
{
      struct stat buf{};
      
      stat(filename, &buf); 

      if(buf.st_size <= 0)
      {
            printf("Error code: %d\n", ERROR_STAT_SIZE);
            exit(ERROR_STAT_SIZE);
      }

      proc->chars_num = buf.st_size + 1;
}

//===========================================================================================================

void num_of_strings(Processor* proc)
{
      for(size_t i = 0; i <= proc->chars_num; i++)
      {
            if(proc->buffer_ptr[i] == '\n')
            {
                  proc->buffer_ptr[i] = '\0';
                  proc->strings_num++;
            }
            else if(proc->buffer_ptr[i] == '\r')
            {
                  proc->buffer_ptr[i] = '\0';
                  proc->strings_num--;
            }
      }
      proc->strings_num++;
}

//===========================================================================================================

void fill_structs_arr(Processor* proc)
{
      proc->structs_arr = (String*)calloc(proc->strings_num, sizeof(String));
      proc->tags_arr    = (String*)calloc((proc->strings_num)/2, sizeof(String));

      if(proc->structs_arr == nullptr || proc->tags_arr == nullptr)
      {
            printf("Error code: %d\n", ERROR_STRUCTS_ARRAY_NULLPTR);
            exit(ERROR_STRUCTS_ARRAY_NULLPTR);
      }

      for (size_t j = 0; j < (proc->strings_num)/2; j++)
      {
            proc->tags_arr[j].str_ptr = PSN;
      }

      size_t counter = 0;
      size_t k = 0;

      for(size_t i = 0; i < proc->strings_num; i++)
      {
            proc->structs_arr[i].str_ptr = proc->buffer_ptr + counter;
            proc->structs_arr[i].str_len = strlen(proc->buffer_ptr + counter);

            if(strcmp(proc->structs_arr[i].str_ptr + proc->structs_arr[i].str_len - 1, ":") == 0)
            {
                  size_t flag = 0;
                  for (size_t j = 0; j < (proc->strings_num)/2; j++)
                  {
                        if (strcmp(proc->tags_arr[j].str_ptr, proc->structs_arr[i].str_ptr) == 0)
                        {
                              proc->tags_arr[j].num_tag++;
                              flag = 1;
                        }
                  }

                  if (flag == 0)
                  {
                        proc->tags_arr[k].str_ptr = proc->buffer_ptr + counter;
                        proc->tags_arr[k].str_len = strlen(proc->buffer_ptr + counter);
                        proc->tags_arr[k].num_tag++;
                        k++;
                  }
            }

            counter += proc->structs_arr[i].str_len + 1;
      }
}

//===========================================================================================================

void proc_dtor(Processor* proc)
{
      proc->chars_num       = 0;
      proc->strings_num     = 0;
      proc->error_code      = 0;

      for(size_t i = 0; i <= proc->strings_num; i++)
      {
            proc->structs_arr[i].str_ptr = nullptr;
            proc->structs_arr[i].str_len = 0;
      }

      free(proc->buffer_ptr);
      proc->buffer_ptr = nullptr;

      free(proc->structs_arr);
      proc->structs_arr = nullptr;

      fclose(proc->mainfile);
      proc->mainfile = nullptr;
}

//===========================================================================================================
