/**
 * 文件名称：main.c
 * 文件描述：主界面的显示，函数调用
 */

#include "head.h"

int main()
{
    LiteralList literals[Max_Ltr_Num];
    for (int i = 1; i < Max_Ltr_Num; i++)
    {
        literals[i].pos = NULL;
        literals[i].neg = NULL;
    }
    // clock_t begin = 0, end = 0, cost = 0; //计时相关变量
    char filename[30];
    // int status = 0;

    printf("Please input the file path:\n");
    scanf("%s", filename);
    load_file(literals, filename);

    return 0;
}