/**
 * 文件名称：main.c
 * 文件描述：主界面的显示，函数调用
 */

#include "head.h"

/**
 * 函数名称：show_answer
 * 函数功能：输出结果到终端并写入同名文件
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
            if (i % 20 == 0 && i != ltr_num)
            {
                fprintf(fp, "\n ");
                printf("\n ");
            } //每20个换行
        }
    } //写入/输出解

    fprintf(fp, "\nt%6ld", cost);
    printf("\nt%6ld\n", cost);
    //写入/输出求解时间

    fclose(fp);
    return;
}

/**
 * 函数名称：check_answer
 * 函数功能：检查解是否正确
 * 返回值：void
 */
void check_answer(LiteralList literals[])
{
    FILE *fp = NULL;
    char filename[] = "check report.txt";
    ClauseNode *cp = clist;
    VarNode *vp = NULL;
    int status = 0; //子句状态

    fp = fopen(filename, "w");
    while (cp != NULL)
    {
        vp = cp->vn;
        while (vp != NULL)
        {
            fprintf(fp, "%4d", vp->var);
            if (literals[abs(vp->var)].value * vp->var > 0)
            {
                status = TRUE;
            } //有变元为真，则该子句为真
            vp = vp->next_varNode;
        } //子句文字扫描

        if (status == TRUE)
        {
            fprintf(fp, "\tT\n");
        }
        else
        {
            fprintf(fp, "\tF\n");
        }

        cp = cp->next_clauseNode; //扫描下一子句
    }
    //检查结果写入文件

    printf("The check result is TRUE!\n");
    printf("The check report has been generated!\n");
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
    check_answer(literals);

    system("pause");
    return 0;
}
