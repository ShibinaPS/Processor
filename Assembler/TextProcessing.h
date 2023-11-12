#ifndef TEXT_PROCESSING_H
#define TEXT_PROCESSING_H
//===========================================================================================================

#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

//===========================================================================================================

const int MAX_WORD_LEN = 32;

//===========================================================================================================

      static const char* cmd[] = {"in", "out", "push", "pop", "hlt", 
                        "add", "sub", "mul", "div", "sqrt", "minus"};

      static const char* reg[] = {"ax", "bx", "cx", "dx"};

      static const char* st_reg[] = {"[ax]", "[bx]", "[cx]", "[dx]"};

      static const char* jmp[] = {"jmp", "je", "jge", "jne", "jg", "jl", "jle", "call"};

//===========================================================================================================

struct Flag
{
      char word[MAX_WORD_LEN] = {0};
      
      size_t index = 0;
};

//===========================================================================================================
struct FlagProcessing
{
      size_t jmps_num   = 0;
      size_t tags_num   = 0;
      size_t flags_num  = 0;
      size_t cur_tag    = 0;
      size_t cur_flag   = 0;

      Flag* tags_arr         = nullptr;
      Flag* flags_arr        = nullptr;
      Flag* new_tags_arr     = nullptr;

};

//===========================================================================================================
struct TextProcessing
{
      char asmbl_filename[MAX_WORD_LEN] = {0};

      FILE* mainfile    = nullptr;
      char* buffer_ptr  = nullptr;
      char** tok_arr    = nullptr;

      size_t chars_num  = 0;
      size_t str_num    = 0;
      size_t tok_num    = 0;
      size_t cur_n      = 0;
      size_t hlt_count  = 0;

      size_t error      = 0;
};

//===========================================================================================================

enum ProcErrors
{
      ERROR_MAINFILE_OPEN           = 1,
      ERROR_EMPTY_MAINFILE          = 2,
      ERROR_BUFFER_NULLPTR          = 3,
      ERROR_TOK_ARR_NULLPTR         = 4,
      ERROR_LEXICAL_ANALYSIS        = 5,
      ERROR_HLT_COUNT               = 6,
      ERROR_JMP_FLG_NUM             = 7,
      ERROR_NUM_TAGS                = 8,
      ERROR_FLAG                    = 9,
      ERROR_TAG_CALLOC              = 10,
      ERROR_FLAG_CALLOC             = 11,
      ERROR_NEW_TAG_CALLOC          = 12,
};

//===========================================================================================================

void tp_ctor(TextProcessing* tp, FlagProcessing* fp, const char* filename);

//===========================================================================================================

void file_open(TextProcessing* tp, const char* filename);

//===========================================================================================================

void num_of_chars(TextProcessing* tp, const char* filename);

//===========================================================================================================

void chars_buffer(struct TextProcessing* tp);

//===========================================================================================================

void num_of_tok(struct TextProcessing* tp);

//===========================================================================================================

void fill_tok_arr(struct TextProcessing* tp);

//===========================================================================================================

void lexical_analysis(struct TextProcessing* tp, struct FlagProcessing* jump);

//===========================================================================================================

void tags_analysis(struct TextProcessing* tp, struct FlagProcessing* jump);

//===========================================================================================================

void bin_file(struct AsmblProcessing* asmbl, struct TextProcessing* tp);

//===========================================================================================================

void HLT_count(struct TextProcessing* tp);

//===========================================================================================================

void tp_dtor(struct TextProcessing* tp, struct FlagProcessing* jump);

//===========================================================================================================

#endif //TEXT_PROCESSING_H