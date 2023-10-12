#include "TextProcessing.h"

//===========================================================================================================

int main()
{
      struct TextProcessing tp   = {};
      struct FlagProcessing fp = {};

      if(tp_ctor(&tp, &fp, "input.txt") == 0)
      {
            tp_dtor(&tp, &fp);
      }
}