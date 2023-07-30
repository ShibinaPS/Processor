#include "FileProcessing.h"

//===========================================================================================================

int main()
{
      ProcDebug proc {};

      if(proc_ctor(&proc, "Input.txt") == 0)
      {
            

            proc_dtor(&proc);
      }
}