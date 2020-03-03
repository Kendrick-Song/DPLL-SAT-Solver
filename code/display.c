/**
 * 文件名称：display.c
 * 文件描述：SAT界面的显示，函数调用
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
            if (i % 10 == 0 && i != ltr_num)
            {
                fprintf(fp, "\n ");
                printf("\n ");
            } //每10个换行
        }
    } //写入/输出解

    fprintf(fp, "\nt%6ld", cost);
    printf("\nt%6ld ms\n", cost);
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
    if (literals[1].value == UNKNOWN)
    {
        printf("There's no Answer to check!\n");
        return;
    }

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

    if (status == TRUE)
    {
        printf("The check result is TRUE!\n");
    }
    else
    {
        printf("The check result is FALSE!\n");
    }

    printf("The check report has been generated!\n");
}

void sat()
{
    LiteralList literals[Max_Ltr_Num];
    clock_t begin = 0, end = 0, cost = 0; //计时相关变量
    char filename[100];
    int result = 0;

    int op = 1;
    while (op)
    {
        printf("\n\n");
        printf("%20cSAT<\n", '>');
        printf("%10c***********************\n", ' ');
        printf("%13c1.  New SAT\n", ' ');
        printf("%13c2.  Check Answer\n", ' ');
        printf("%13c0.  Back\n", ' ');
        printf("%10c***********************\n", ' ');
        scanf("%d", &op);
        system("cls");
        switch (op)
        {
        case 1:
            printf("Please input the file path:\n");
            scanf("%s", filename);
            if (load_file(literals, filename) == FALSE)
            {
                printf("Failed to Load File !\n");
            }
            else
            {
                printf("Load File Successfully!\n");
            } //文件读取

            begin = clock();
            result = dpll(literals);
            end = clock();
            cost = (end - begin);
            //dpll求解并计时

            show_answer(literals, cost, result, filename); //输出答案到终端和文件
            system("pause");
            break;
        case 2:
            check_answer(literals);
            system("pause");
            break;
        case 0:
            return;
        default:
            break;
        }
    }
}
