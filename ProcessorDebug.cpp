#include "ProcessorDebug.h"

//===========================================================================================================

int proc_ctor(ProcDebug* proc, const char* filename)
{
      if(file_open(proc, filename))
      {
            return ERROR_MAINFILE_OPEN;
      }

      if(num_of_chars(proc, filename))
      {
            return ERROR_STAT_SIZE;
      }

      switch(chars_buffer(proc, proc->mainfile))
      {
            case(0):
            {
                  break;
            }

            case(ERROR_BUFFER_NULLPTR):
            {
                  return ERROR_BUFFER_NULLPTR;
            }

            case(ERROR_FREAD_COUNT):
            {
                  return ERROR_FREAD_COUNT;
            }

            default:
            {
                  return -1;
            }
      }

      if(fill_structs_arr(proc))
      {
            return ERROR_STRUCTS_ARRAY_NULLPTR;
      }

      switch(lexical_analysis(proc))
      {
            case(0):
            {
                  break;
            }

            case(ERROR_BRACKET_SEQUENCE):
            {
                  return ERROR_BRACKET_SEQUENCE;
            }
            
            case(ERROR_MISSPELL):
            {
                  return ERROR_MISSPELL;
            }

            case(ERROR_HLT_ABSENCE):
            {
                  return ERROR_HLT_ABSENCE;
            }

            default:
            {
                  return -2;
            }
      }

      return 0;
}

//===========================================================================================================
int lexical_analysis(ProcDebug* proc)
{
      size_t hlt_count = 0;

      size_t i = 0;
      size_t k = 0;
      size_t j = 0;

      char word[10];

      while(k < proc->chars_num && i < proc->strings_num)
      {
            while(proc->structs_arr[i].str_ptr[k] == ' ')
            {
                  k++;
            }

            if(proc->structs_arr[i].str_ptr[k] == '\0')
            {
                  while(proc->structs_arr[i].str_ptr[k] == '\0')
                  {
                        k++;
                  }

                  i++;
                  k = 0;
            }

            size_t open_b      = 0;
            size_t close_b     = 0;
            size_t open_count  = 0;
            size_t close_count = 0;

            while(proc->structs_arr[i].str_ptr[k] != '\0')
            {
                  word[j] = proc->structs_arr[i].str_ptr[k];

                  if (proc->structs_arr[i].str_ptr[k] == '[')
                  {
                        open_b = k;
                        open_count++;
                  }

                  else if (proc->structs_arr[i].str_ptr[k] == ']')
                  {
                        close_b = k;
                        close_count++;
                  }

                  if (close_b != 0 && open_b != 0 && (close_b < open_b || open_count != 1 || close_count != 1))
                  {
                        proc->error_code = ERROR_BRACKET_SEQUENCE;
                        proc_dtor(proc);
                        return ERROR_BRACKET_SEQUENCE;
                  }

                  k++;
                  j++;
            }

            if (strstr(word, "pop") == 0 && strstr(word, "push") == 0 && 
                strstr(word, "hlt") == 0 && strstr(word, "in") == 0 && strstr(word, "out") == 0 && 
                strstr(word, "mul") == 0 && strstr(word, "add") == 0 && strstr(word, "sub") == 0 && 
                strstr(word, "div") == 0 && strstr(word, "je") == 0)
            {
                  printf("The lexical error in %d string: %s\n", i, word);
                  proc->error_code = ERROR_MISSPELL;
                  proc_dtor(proc);
                  return ERROR_MISSPELL;
            }

            else if (strstr(word, "hlt") != 0)
            {
                  hlt_count++;
            }
            
            i++;

            k = 0;
            j = 0;

            strcpy(word, "        ");  
      }
      
      if(hlt_count == 0)
      {
            proc->error_code = ERROR_HLT_ABSENCE;
            proc_dtor(proc);
            return ERROR_HLT_ABSENCE;
      }

      return 0;
}

//===========================================================================================================

int file_open(ProcDebug* proc, const char* filename)
{
      proc->mainfile = fopen(filename, "r+");

      if (proc->mainfile == nullptr)
      {
            proc->error_code = ERROR_MAINFILE_OPEN;
            proc_dtor(proc);
            return ERROR_MAINFILE_OPEN;
      }
      return 0;
}

//===========================================================================================================

int chars_buffer(ProcDebug* proc, FILE* filename)
{
      proc->buffer_ptr = (char*)calloc(proc->chars_num, sizeof(char));

      if (proc->buffer_ptr == nullptr)
      {
            proc->error_code = ERROR_BUFFER_NULLPTR;
            proc_dtor(proc);
            return ERROR_BUFFER_NULLPTR;
      }

      size_t fread_count = fread(proc->buffer_ptr, sizeof(char), proc->chars_num - 1, filename);

      num_of_strings(proc);

      if(fread_count != (proc->chars_num - proc->strings_num))
      {
            proc->error_code = ERROR_FREAD_COUNT;
            proc_dtor(proc);
            return ERROR_FREAD_COUNT;
      }

      proc->buffer_ptr[proc->chars_num - 1] = '\0';

      return 0;
}

//===========================================================================================================

int num_of_chars(ProcDebug* proc, const char* filename)
{
      struct stat buf{};
      
      stat(filename, &buf); 

      if(buf.st_size <= 0)
      {
            proc->error_code = ERROR_STAT_SIZE;
            proc_dtor(proc);
            return ERROR_STAT_SIZE;
      }

      proc->chars_num = buf.st_size + 1;
      
      return 0;
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

int fill_structs_arr(ProcDebug* proc)
{
      proc->structs_arr = (String*)calloc(proc->strings_num, sizeof(String));

      if(proc->structs_arr == nullptr)
      {
            proc->error_code = ERROR_STRUCTS_ARRAY_NULLPTR;
            proc_dtor(proc);
            return ERROR_STRUCTS_ARRAY_NULLPTR;
      }

      size_t counter = 0;

      for(size_t i = 0; i < proc->strings_num; i++)
      {
            proc->structs_arr[i].str_ptr = proc->buffer_ptr + counter;
            proc->structs_arr[i].str_len = strlen(proc->buffer_ptr + counter);

            counter += proc->structs_arr[i].str_len + 1;
      }
      return 0;
}

//===========================================================================================================

void proc_dtor(ProcDebug* proc)
{
      switch(proc->error_code)
      {
            case(0):
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

                  break;
            }

            case(ERROR_MAINFILE_OPEN):
            {
                  proc->error_code = 0;
                  break;
            }

            case(ERROR_STAT_SIZE):
            {
                  proc->error_code = 0;

                  fclose(proc->mainfile);
                  proc->mainfile = nullptr;

                  break;
            }

            case(ERROR_BUFFER_NULLPTR):
		{
			proc->chars_num       = 0;
                  proc->error_code      = 0;

                  fclose(proc->mainfile);
                  proc->mainfile = nullptr;

                  break;
		}

		case(ERROR_FREAD_COUNT):
		{
			proc->chars_num       = 0;
                  proc->strings_num     = 0;
                  proc->error_code      = 0;

                  free(proc->buffer_ptr);
                  proc->buffer_ptr = nullptr;

                  fclose(proc->mainfile);
                  proc->mainfile = nullptr;

                  break;
		}

		case(ERROR_STRUCTS_ARRAY_NULLPTR):
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

                  break;
		}

            case(ERROR_BRACKET_SEQUENCE):
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

                  break;
            }

            case(ERROR_MISSPELL):
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

                  break;
            }

            case(ERROR_HLT_ABSENCE):
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

                  break;
            }


		default:
		{
			printf("Undefined error!\n");
                  break;
		}


      }
}

//===========================================================================================================
