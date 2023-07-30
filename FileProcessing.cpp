#include "FileProcessing.h"

//===========================================================================================================

int proc_ctor(ProcDebug* proc, const char* filename)
{
      file_open(proc, filename);
      
      num_of_chars(proc, filename);
      
      chars_buffer(proc, proc->mainfile);

      fill_structs_arr(proc);

      lexical_analysis(proc);

      return 0;
}

//===========================================================================================================

void file_open(ProcDebug* proc, const char* filename)
{
      proc->mainfile = fopen(filename, "r+");

      if (proc->mainfile == nullptr)
      {
            printf("Error code: %d\n", ERROR_MAINFILE_OPEN);
            exit(ERROR_MAINFILE_OPEN);
      }
}

//===========================================================================================================

void chars_buffer(ProcDebug* proc, FILE* filename)
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

void num_of_chars(ProcDebug* proc, const char* filename)
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

void num_of_strings(ProcDebug* proc)
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

void fill_structs_arr(ProcDebug* proc)
{
      proc->structs_arr = (String*)calloc(proc->strings_num, sizeof(String));

      if(proc->structs_arr == nullptr)
      {
            printf("Error code: %d\n", ERROR_STRUCTS_ARRAY_NULLPTR);
            exit(ERROR_STRUCTS_ARRAY_NULLPTR);
      }

      size_t counter = 0;

      for(size_t i = 0; i < proc->strings_num; i++)
      {
            proc->structs_arr[i].str_ptr = proc->buffer_ptr + counter;
            proc->structs_arr[i].str_len = strlen(proc->buffer_ptr + counter);

            counter += proc->structs_arr[i].str_len + 1;
      }
}

//===========================================================================================================

void proc_dtor(ProcDebug* proc)
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
