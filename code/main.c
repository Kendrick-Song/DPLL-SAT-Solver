/**
 * 文件名称：main.c
 * 文件描述：主界面的显示，函数调用
**/

#include "head.h"

int main()
{
    ClauseNode *S = NULL;
    Answer *ans = NULL;
    LiteralList literals[Max_Ltr_Num];
    char filename[15];
    int result = 0;
    clock_t start = 0, finish = 0, cost = 0;
    printf("Please input the file path:\n");
    scanf("%s", filename);
    LoadCnfFile(&S, &ans, literals, filename);
    start = clock();
    result = dpll(ans, literals);
    finish = clock();
    cost = (finish - start);
    if (result == SATISFIABLE)
    {
        show_answer(ans);
        printf("Cost time: %ld ms\n", cost);
    }
    system("pause");
    return 0;
}
