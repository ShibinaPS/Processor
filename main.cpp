#include "ProcessorDebug.h"

//===========================================================================================================

int main()
{
      ProcDebug proc {};

      switch(proc_ctor(&proc, "Input.txt"))
      {
            case(0):
            {
                  proc_dtor(&proc);
                  return 0;
            }

            case(ERROR_MAINFILE_OPEN):
            {
                  printf("Error: %d\n", ERROR_MAINFILE_OPEN);
                  return 0;
            }

            case(ERROR_STAT_SIZE):
            {
                  printf("Error: %d\n", ERROR_STAT_SIZE);
                  return 0;
            }

            case(ERROR_BUFFER_NULLPTR):
		{
			printf("Error: %d\n", ERROR_BUFFER_NULLPTR);
			return 0;
		}

		case(ERROR_FREAD_COUNT):
		{
			printf("Error: %d\n", ERROR_FREAD_COUNT);
			return 0;
		}

		case(ERROR_STRUCTS_ARRAY_NULLPTR):
		{
			printf("Error: %d\n", ERROR_STRUCTS_ARRAY_NULLPTR);
			return 0;
		}

            case(ERROR_BRACKET_SEQUENCE):
            {
                  printf("Error: %d\n", ERROR_BRACKET_SEQUENCE);
                  return 0;
            }

            case(ERROR_MISSPELL):
            {
                  printf("Error: %d\n", ERROR_MISSPELL);
                  return 0;
            }

            case(ERROR_HLT_ABSENCE):
            {
                  printf("Error: %d\n", ERROR_HLT_ABSENCE);
                  return 0;
            }

		default:
		{
			printf("Undefined error!\n");
			return 0;
		}
      }
}