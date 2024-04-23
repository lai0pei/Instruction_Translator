#include "Common.h"
#include "Parser/Parser.h"
#include "CodeWriter/CodeWrite.h"

char *FILE_NAME;
int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        fprintf(stderr, "File expected\n");
        exit(0);
    }

    FILE_NAME = argv[1];
    init();
    while (set_fp())
    {
        while (hasMoreLines())
        {
            if (advance() > 0)
            {
                expression();
                translate_instruction();
            }
        }
    }
    end_line();
    clean();
    return 0;
}