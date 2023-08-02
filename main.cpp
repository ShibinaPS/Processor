#include "FileProcessing.h"

//===========================================================================================================

int main()
{
      Processor proc {};

      if(proc_ctor(&proc, "Input.txt") == 0)
      {
            proc_dtor(&proc);
      }
}