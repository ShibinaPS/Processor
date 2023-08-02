#ifndef FILE_PROCESSING_H
#define FILE_PROCESSING_H
//===========================================================================================================

#include <stdio.h>
#include <sys\stat.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

//===========================================================================================================

typedef struct String
{
      char* str_ptr  = nullptr;
      size_t str_len = 0;
      size_t num_tag = 0;
}str;

//===========================================================================================================

typedef struct Processor
{
      FILE* mainfile    = nullptr;
      char* buffer_ptr  = nullptr;

      size_t strings_num = 0;
      size_t chars_num   = 0;

      String* structs_arr = nullptr;
      String* tags_arr    = nullptr;

      int error_code     = 0;

}proc;

//===========================================================================================================

enum ProcErrors
{
      ERROR_MAINFILE_OPEN           = 1,
      ERROR_BUFFER_NULLPTR          = 2,
      ERROR_FREAD_COUNT             = 3,
      ERROR_STAT_SIZE               = 4,
      ERROR_STRUCTS_ARRAY_NULLPTR   = 5,
      ERROR_ONE_STRING              = 6,
      ERROR_POP_NUM                 = 7,
      ERROR_LEXICAL_UNIT            = 8,
      ERROR_HLT_COUNT               = 9,
      ERROR_REG_PART                = 10,
      ERROR_SIGN_PART               = 11,
      ERROR_UNKNOWN_SIGN            = 12,
      ERROR_NUM_PART                = 13,
      ERROR_TAG_AGAIN               = 14,
      ERROR_TAG                     = 15,
};

//===========================================================================================================

int proc_ctor(Processor* proc, const char* filename);

//===========================================================================================================

void file_open(Processor* proc, const char* filename);

//===========================================================================================================

void chars_buffer(Processor* proc, FILE* filename);

//===========================================================================================================

void num_of_chars(Processor* proc, const char* filename);

//===========================================================================================================

void num_of_strings(Processor* proc);

//===========================================================================================================

void fill_structs_arr(Processor* proc);

//===========================================================================================================

void lexical_analysis(Processor* proc);

//===========================================================================================================

int tag_analysis(Processor* proc, const char* token, size_t string);

//===========================================================================================================

void proc_dtor(Processor* proc);

//===========================================================================================================

#endif //FILE_PROCESSING_H