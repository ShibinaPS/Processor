#ifndef TOKENIZATION_H
#define TOKENIZATION_H
//===========================================================================================================

//===========================================================================================================

struct Token
{
      char text[MAX_WORD_LEN] = {0};

      size_t asm_code = -1;
      size_t tok_type = 0;
      float num       = 0;
            
      int error       = 0;      
};

//===========================================================================================================

struct TokProcessing
{
      FILE* asm_file = nullptr;

      Token* tok_arr = nullptr;

      size_t cur_n = 0;
      size_t tok_num = 0;
};

//===========================================================================================================

enum AsmCodes
{
      // CMD
      HLT   = 0,
      IN    = 1,
      OUT   = 2,
      PUSH  = 3,
      POP   = 4,
      ADD   = 5,
      SUB   = 6,
      MUL   = 7,
      DIV   = 8,

      // REG
      AX = 10,
      BX = 11,
      CX = 12,
      DX = 13,

      // [REG]
      ST_AX = 14,
      ST_BX = 15,
      ST_CX = 16,
      ST_DX = 17,

      // JMP
      JMP   = 20,
      JE    = 21,
      JGE   = 22,
      JNE   = 23,
      JG    = 24,
      JL    = 25,
      JLE   = 26,
      CALL  = 27,
};

//===========================================================================================================

enum CodeMask
{
      REG_MASK = 1 << 16,
      NUM_MASK = 1 << 17,
      RAM_MASK = 1 << 18,
};

//===========================================================================================================

enum AsmCodeTypes
{
      CMD  = 1,
      REG  = 2,
      NUM  = 3,
      JUMP = 4,
      FLAG = 5,
};

//===========================================================================================================

enum AsmErrors
{
      ERROR_TOKEN             = 1,
      ERROR_CMD_ARG_ERROR     = 2,
      ERROR_NUM_POP           = 3,
};

//===========================================================================================================

int tok_ctor(struct TokProcessing* tokp, struct TextProcessing* tp, struct FlagProcessing* fp);

//===========================================================================================================

void tokenization(struct TokProcessing* tokp, struct TextProcessing* tp, struct FlagProcessing* fp);

//===========================================================================================================

int cmd_check(struct TokProcessing* tokp);

//===========================================================================================================

void arg_check(struct TokProcessing* tokp, struct TextProcessing* tp);

//===========================================================================================================

void tok_dtor(struct TokProcessing* tokp, struct TextProcessing* tp);

//===========================================================================================================
#endif //TOKENIZATION_H