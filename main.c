/**
 * 文件名称：main.c
 * 文件描述：主界面的显示，函数调用
**/

#include "head.h"

int main()
{
    ClauseNode *S = NULL;
    Answer *ans = NULL;
    LiteralList *literals = NULL;
    char filename[15];
    printf("Please input the file path:\n");
    scanf("%s", filename);
    LoadCnfFile(&S, ans, literals, filename);
    return 0;
}