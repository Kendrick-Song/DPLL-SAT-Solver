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
void show_answer(LiteralList literals[], clock_t cost, int result, char filename[])
{
    FILE *fp = NULL;

    char *suffix = strrchr(filename, '.'); //找到'.'最后一次出现的位置
    suffix[1] = 'r';
    suffix[2] = 'e';
    suffix[3] = 's';
    //文件名后缀更改

    fp = fopen(filename, "w");
    fprintf(fp, "s%6d\n", result);
    printf("s%6d\n", result);
    //写入/输出求解结果

    fprintf(fp, "v");
    printf("v");
    if (result == TRUE)
    {
        for (int i = 1; i <= ltr_num; i++)
        {
            fprintf(fp, "%6d", literals[i].value * i);
            printf("%6d", literals[i].value * i);
            if (i % 20 == 0)
            {
                fprintf(fp, "\n ");
                printf("\n ");
            }//每20个换行
        }
    } //写入/输出解

    fprintf(fp, "\nt%6ld", cost);
    printf("\nt%6ld\n", cost);
    //写入/输出求解时间
    return;
}

int main()
{
    LiteralList literals[Max_Ltr_Num];
    clock_t begin = 0, end = 0, cost = 0; //计时相关变量
    char filename[100];
    int result = 0;

    printf("Please input the file path:\n");
    scanf("%s", filename);
    load_file(literals, filename);
    printf("Load File Successfully!\n");

    begin = clock();
    result = dpll(literals);
    end = clock();
    cost = (end - begin);

    show_answer(literals, cost, result, filename);

    system("pause");
    return 0;
}
