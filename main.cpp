#include "TextProcessing.h"
#include "Tokenization.h"

//===========================================================================================================

int main()
{
      struct TextProcessing tp   = {};
      struct FlagProcessing fp   = {};
      struct TokProcessing  tokp = {};

      if(tp_ctor(&tp, &fp, "input.txt") == 0)
      {
            tp_dtor(&tp, &fp);
      }

      if(tok_ctor(&tokp, &tp, &fp) == 0)
      {
            tok_dtor(&tokp, &tp);
      }
}