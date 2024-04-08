#include "common.h"
#include "Parser/Parser.h"
#include "CodeWriter/CodeWrite.h"

char* FILE_NAME;

int main(int argc, char *argv[]){

    if(argc < 2){
        fprintf(stderr,"File expected\n");
        exit(0);
    }

   FILE_NAME = argv[1];
   parser_init();
   code_init();
   CommandType t;
   SEG_TYPE s;
   INDEX_TYPE index;
   OP_TYPE op;
    while (hasMoreLines())
    {
        if (advance() > 0)
        {
            expression();
            t = commandType();
            // #if DEBUG == 1
            // printOpType();
            // printSegType();
            // printIndex();
            // #endif
            s = arg1();
            index = arg2();
            op = getOp();
            if(t != UNDEFINED_COMMAND_TYPE){
                writePushPop(t,op,s,index);
            }
            
            if(op != UNDEFINED_OPTYPE){
                writeArithematic(op);
            }
        }
    }
    end_line();
    parser_clean();
    code_clean();
    return 1;
}