#include "TextProcessing.h"
#include "AsmProcessing.h"

//===========================================================================================================

int main()
{
      struct TextProcessing tp      = {};
      struct FlagProcessing fp      = {};
      struct AsmblProcessing  asmbl = {};

      tp_ctor(&tp, &fp, "input.txt");

      if(tp.error != 0)
      {
            tp_dtor(&tp, &fp);
            return tp.error;
      }

      asm_ctor(&asmbl, &tp, &fp);

      if(asmbl.error != 0)
      {
            asm_dtor(&asmbl, &tp);
            return asmbl.error;
      }
      
      tp_dtor(&tp, &fp);
      asm_dtor(&asmbl, &tp);
}