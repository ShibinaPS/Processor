#ifndef ASM_PROCESSING_H
#define ASM_PROCESSING_H
//===========================================================================================================

#include "TextProcessing.h"
#include <math.h>

//===========================================================================================================

const double EPS = 1e-6;

enum Comparison
{
    EQUAL   = 0,
    FIRST   = 1,
    SECOND  = 2,
};

//===========================================================================================================

struct Token
{
      char text[MAX_WORD_LEN] = {0};

      size_t asm_code = -1;         // Assembler code.
      size_t tok_type = 0;          // Type of the token.
      float num       = 0;          // Value of the token.
            
      int error       = 0;          // Token error.
};

//===========================================================================================================

struct AsmblProcessing
{
      FILE* lst_file = nullptr;                 // File for listing.
      FILE* bin_filename = nullptr;             // Binary file.

      Token* tok_arr = nullptr;                 // The array of tokens.
      Token* tok_arr_with_jmp_ind = nullptr;    // The array of tokens with indexes instead of labels.

      char signature[8] = "CHMONYA";            // Signature for the listing.

      size_t cur_n      = 0;                    // Current position.
      size_t tok_num    = 0;                    // The number of tokens.

      size_t error = 0;                         // Assembler errors.
};

//===========================================================================================================

enum AsmCodes
{
      // Command assembler codes.
      HLT   = 0,        // The end of the programm.
      IN    = 1,        // Accepts the number from the console.
      OUT   = 2,        // Enters the result in the console.
      PUSH  = 3,        // Pushes in stack.
      POP   = 4,        // Pops from stack.
      ADD   = 5,        // Adds two numbers in stack.
      SUB   = 6,        // Substracts from one number another in stack.
      MUL   = 7,        // Multiplies two numbers in stack.
      DIV   = 8,        // Divides one number on another in stack.
      SQRT  = 9,        // Takes sqrt from the number in stack.
      MINUS = 10,       // Makes a negative number in stack.

      // Register assembler codes.
      AX = 20,          // Register ax.
      BX = 21,          // Register bx.
      CX = 22,          // Register cx.
      DX = 23,          // Register dx.

      // Jump assembler codes.
      JMP   = 30,       // Jumps to the label without any conditions.
      JE    = 31,       // Jumps -//- if the numbers are equal.
      JGE   = 32,       // Jumps -//- if the first number >= the second one.
      JNE   = 33,       // Jumps -//- if the numbers are not equal.
      JG    = 34,       // Jumps -//- if the first number > the second one.
      JL    = 35,       // Jumps -//- if the first number < the second one.
      JLE   = 36,       // Jumps -//- if the first number >= the second one.
      CALL  = 37,       // Jumps to the label and after executing returns to the main code.
      FLAGS = 38,       // Flags.
};

//===========================================================================================================

enum CodeMask
{
      // Example: PUSH ...
      // ...num         --> 0000000 00000010 00000000 00000011
      // ...reg         --> 0000000 00000001 00000000 00000011
      // ...[reg]       --> 0000000 00000101 00000000 00000011
      // ...[num]       --> 0000000 00000110 00000000 00000011
      // ...[reg + num] --> 0000000 00000111 00000000 00000011

      REG_MASK = 1 << 16,
      NUM_MASK = 1 << 17,
      RAM_MASK = 1 << 18,
};

//===========================================================================================================

enum AsmCodeTypes
{
      CMD  = 1,   // Command
      REG  = 2,   // Register
      NUM  = 3,   // Number
      JUMP = 4,   // Jump
      FLAG = 5,   // Flag
};

//===========================================================================================================

enum AsmErrors
{
      ERROR_TOKEN                         = 1,
      ERROR_SEQUENCE_OF_ARGS              = 2,
      ERROR_CMD_ERROR                     = 4,
      DIV_BY_ZERO                         = 5,
      ERROR_LST_FILE_CREATE               = 6,
      ERROR_CALLOC_TOK_ARR                = 7,   
      ERROR_TOK_ARR_CALLOC                = 8, 
      ERROR_TOK_ARR_WITH_JMP_IND_CALLOC   = 9,
      ERROR_CALLOC_ASM_CODE               = 10,
};

//===========================================================================================================

/**
 * @brief Creates and initializes assembler.
 * 
 * @param asmbl The Assembler pointer.
 * @param tp    The TextProcessing pointer.
 * @param fp    The FlagProcessing pointer.
 */

void asm_ctor(struct AsmblProcessing* asmbl, struct TextProcessing* tp, struct FlagProcessing* fp);

//===========================================================================================================

/**
 * @brief Rebuilds the name of the file changing the extansion.
 * 
 * @param asmbl_filename  The name of assembler file.
 * @param extn    New extension for the file.
 * @return FILE* 
 */

FILE* new_extension(char* asmbl_filename, const char* extn);

//===========================================================================================================

/**
 * @brief Splits the code to each token and initializes parametres of each token.
 * 
 * @param asmbl The Assembler pointer.
 * @param tp    The TextProcessing pointer.
 * @param fp    The FlagProcessing pointer.
 */

void tokenization(struct AsmblProcessing* asmbl, struct TextProcessing* tp, struct FlagProcessing* fp);

//===========================================================================================================

/**
 * @brief Checks the correctness of the argument.
 * 
 * @param asmbl The Assembler pointer.
 * @param tp    The TextProcessing pointer.
 * @param asm_code Assembler code.
 * @param fp    The FlagProcessing pointer.
 */

void arg_check(struct AsmblProcessing* asmbl, struct TextProcessing* tp, int asm_code, struct FlagProcessing* fp);

//===========================================================================================================

/**
 * @brief Makes up array with indexes instead of labels.
 * 
 * @param asmbl The Assembler pointer.
 * @param fp    The FlagProcessing pointer.
 */

void arr_jmp_with_ind(struct AsmblProcessing* asmbl, struct FlagProcessing* fp);

//===========================================================================================================

/**
 * @brief Makes up listing.
 * 
 * @param asmbl The Assembler pointer.
 * @param fp    The FlagProcessing pointer.
 */

void listing(struct AsmblProcessing* asmbl, struct FlagProcessing* fp);

//===========================================================================================================

/**
 * @brief Encoding the signature from the wird to the integer.
 * 
 * @param asmbl The Assembler pointer.
 * @return int 
 */

int signature(struct AsmblProcessing* asmbl);

//===========================================================================================================

/**
 * @brief creates binary file.
 * 
 * @param asmbl The Assembler pointer.
 * @param tp    The TextProcessing pointer.
 * @param fp    The FlagProcessing pointer.
 */

void bin_file(struct AsmblProcessing* asmbl, struct TextProcessing* tp, struct FlagProcessing* fp);

//===========================================================================================================

/**
 * @brief Destructor of the assembler, frees memory.
 * 
 * @param asmbl The Assembler pointer.
 * @param tp    The TextProcessing pointer.
 */

void asm_dtor(struct AsmblProcessing* asmbl, struct TextProcessing* tp);

//===========================================================================================================

#endif // ASM_PROCESSING_H
