#ifndef TEXT_PROCESSING_H
#define TEXT_PROCESSING_H
//===========================================================================================================

#include <stdio.h>
#include <sys\stat.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

//===========================================================================================================

#define MAX_WORD_LEN 10

//===========================================================================================================

      static const char* cmd[] = {"in", "out", "push", "pop", "hlt", 
                        "add", "sub", "mul", "div"};

      static const char* reg[] = {"ax", "bx", "cx", "dx"};

      static const char* st_reg[] = {"[ax]", "[bx]", "[cx]", "[dx]"};

      static const char* jmp[] = {"jmp", "je", "jge", "call"};

//===========================================================================================================

struct Flag
{
      char word[MAX_WORD_LEN] = {0};
};

//===========================================================================================================
struct FlagProcessing
{
      size_t jmps_num   = 0;
      size_t tags_num   = 0;
      size_t flags_num  = 0;
      size_t cur_tag    = 0;
      size_t cur_flag   = 0;

      Flag* tags_arr  = nullptr;
      Flag* flags_arr = nullptr;
};

//===========================================================================================================
struct TextProcessing
{
      FILE* mainfile    = nullptr;
      char* buffer_ptr  = nullptr;
      char** str_arr    = nullptr;

      size_t chars_num  = 0;
      size_t str_num    = 0;
      size_t tok_num    = 0;
      size_t cur_tok    = 0;
      size_t hlt_count  = 0;
};

//===========================================================================================================

enum ProcErrors
{
      ERROR_MAINFILE_OPEN           = 1,
      ERROR_STAT_SIZE               = 2,
      ERROR_BUFFER_NULLPTR          = 3,
      ERROR_STR_ARR_NULLPTR         = 4,
      ERROR_LEXICAL_ANALYSIS        = 5,
      ERROR_HLT_COUNT               = 6,
      ERROR_JMP_FLG_NUM             = 7,
      ERROR_NUM_TAGS                = 8,
      ERROR_FLAG                    = 9,      
};

//===========================================================================================================

int tp_ctor(TextProcessing* tp, FlagProcessing* jump, const char* filename);

//===========================================================================================================

void file_open(TextProcessing* tp, const char* filename);

//===========================================================================================================

void num_of_chars(TextProcessing* tp, const char* filename);

//===========================================================================================================

void chars_buffer(struct TextProcessing* tp);

//===========================================================================================================

void num_of_tok(struct TextProcessing* tp);

//===========================================================================================================

int fill_str_arr(struct TextProcessing* tp);

//===========================================================================================================

void lexical_analysis(struct TextProcessing* tp, struct FlagProcessing* jump);

//===========================================================================================================

void tags_analysis(struct TextProcessing* tp, struct FlagProcessing* jump);

//===========================================================================================================

void HLT_count(struct TextProcessing* tp);

//===========================================================================================================

void tp_dtor(struct TextProcessing* tp, struct FlagProcessing* jump);

//===========================================================================================================

#endif //TEXT_PROCESSING_H