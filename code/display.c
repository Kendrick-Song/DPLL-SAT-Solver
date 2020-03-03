/**
 * 文件名称：main.c
 * 文件描述：主界面的显示，函数调用
 */

#include "head.h"

/**
 * 函数名称：show_answer
 * 函数功能：打印结果
 * 返回值：无
 */
void show_answer(LiteralList literals[])
{
    for (int i = 1; i <= ltr_num; i++)
    {
        if (literals[i].value > 0)
            printf("%d : TRUE\n", i);
        else
            printf("%d : FALSE\n", i);
    }
    return;
}

int main()
{
    LiteralList literals[Max_Ltr_Num];
    clock_t begin = 0, end = 0, cost = 0; //计时相关变量
    char filename[30];
    int status = 0;

    printf("Please input the file path:\n");
    scanf("%s", filename);
    load_file(literals, filename);
    printf("Load File Successfully!\n");

    begin = clock();
    status = dpll(literals);
    end = clock();
    cost = (end - begin);

    if (status == SATISFIABLE)
    {
        show_answer(literals);
        printf("Cost time: %ld ms\n", cost);
    }
    system("pause");
    return 0;
}
