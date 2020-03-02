/**
 * 文件名称：dpll.c
 * 文件描述：dpll求解过程相关函数定义
**/
#include "head.h"

/**
 * 函数名称：decide_next_branch
 * 函数功能：选择搜索分支
 * 返回值：
**/
status decide_next_branch(Answer *ans, int *ltr_val, int *blevel)
{
    for (int i = 1; i <= ltr_num; i++)
    {
        if (ans->value[i] == UNKNOWN)
        {
            *ltr_val = i;
            (*blevel)++;              //决策级加一
            ans->value[*ltr_val] = 1; //选择文字赋值为真
            ans->branchLevel[*ltr_val] = *blevel;
            ans->searched[*ltr_val]++;
            ltr_known++;
            return TRUE;
        }
    } //顺序查找答案数组，选择第一个值未知的文字
    return FALSE;
}

/**
 * 函数名称：deduce
 * 函数功能；使用单子句规则进行搜索化简
 * 返回值；SATISIABLE CONFLICT OTHERS
**/
status deduce(Answer *ans, LiteralList literals[], ClauseList *root, int blevel)
{
    ClauseList *stack[Max_Ltr_Num]; //子句链指针栈
    ClauseList *clp = NULL;
    int top = 0; //栈顶指针
    int status;

    stack[top++] = root; //根结点进栈
    while (top)
    {
        clp = stack[top - 1]; //访问栈顶元素
        status = UNIT;
        while (status == UNIT && clp)
        {
            status = unit_clause_deduce(ans, literals, &clp, blevel);
            stack[top] = clp;
            top++;
        } //广度搜索

        top--; //空指针退栈

        if (status == CONFLICT)
        {
            return CONFLICT;
        } //存在冲突

        if (top)
        {
            top--;
            clp = stack[top];
            if (clp->next)
            {
                stack[top] = clp->next;
                top++;
            }
        } //深度搜索
    }

    if (ltr_known < ltr_num)
    {
        return OTHERS;
    }
    else
    {
        return SATISFIABLE;
    } //栈空时根据已知文字数判断是否满足
}

/**
 * 函数名称：unit_clause_deduce
 * 函数功能：检查单子句并返回其指针
 * 返回值：SATISFIABLE CONFLICT UNIT OTHERS
**/
status unit_clause_deduce(Answer *ans, LiteralList literals[], ClauseList **clp, int blevel)
{
    ClauseNode *cp = NULL;  //子句结点指针
    LiteralNode *lp = NULL; //文字结点指针
    int unknown_num = 0, first_unknown = 0, satisfiable = 0;

    cp = (*clp)->p;
    lp = cp->p;

    while (lp)
    {
        if (ans->value[abs(lp->x)] * lp->x > 0)
        {
            satisfiable = 1;
            break;
        } //该子句有值为真的文字
        if (ans->value[abs(lp->x)] == UNKNOWN)
        {
            unknown_num++;
            if (first_unknown == 0)
            {
                first_unknown = lp->x; //记录第一个未知的文字
            }
        } //该子句有未赋值的文字
        lp = lp->next_literalNode;
    } //扫描该子句下的文字

    if (unknown_num == 0 && satisfiable == 0)
    {
        return CONFLICT;
    } //该子句文字都已知且都为假

    if (unknown_num == 1 && satisfiable == 0)
    {
        ans->unitClause[abs(first_unknown)] = 1;                             //标记单子句位置
        ans->value[abs(first_unknown)] = first_unknown / abs(first_unknown); //该变量赋值为真
        ltr_known++;                                                         //已知文字数加一
        ans->branchLevel[abs(first_unknown)] = blevel;
        if (first_unknown > 0)
        {
            *clp = literals[first_unknown].neg->next;
        }
        else
        {
            *clp = literals[-first_unknown].pos->next;
        } //切换子句链指针
        return UNIT;
    } //该子句为单子句

    if (ltr_known < ltr_num)
    {
        *clp = NULL;
        return OTHERS;
    } //该子句为真

    return SATISFIABLE; //该子句为真
}

/**
 * 函数名称：back_track
 * 函数功能：回溯
 * 返回值：文字值
 */
int back_track(int *blevel, int ltr_val, Answer *ans)
{
    int parent = abs(ltr_val);
    while (*blevel != 0)
    {
        for (int i = 1; i <= ltr_num; i++)
        {
            if (i != parent && ans->branchLevel[i] == *blevel)
            {
                ans->value[i] = UNKNOWN;
                ans->branchLevel[i] = 0;
                ans->searched[i] = 0;
                ans->unitClause[i] = 0;
                ltr_known--;
            }
        } //扫描出该判定级的文字并初始化

        if (*blevel != 1)
        {
            if (ans->searched[parent] == 2)
            {
                (*blevel)--;
                ans->value[parent] = UNKNOWN;
                ans->branchLevel[parent] = 0;
                ans->searched[parent] = 0;
                ltr_known--;
                for (int j = 1; j <= ltr_num; j++)
                {
                    if (ans->branchLevel[j] == *blevel && ans->unitClause[j] == 0)
                    {
                        parent = j;
                        break;
                    }
                } //搜索上一决策层

            } //当前结点T/F都搜索过
            else
                break;
        }
        else if (ans->searched[parent] == 2)
        {
            (*blevel)--;
        }
        else
            break;
    }
    return parent;
}

/**
 * 函数名称：dpll
 * 函数功能：dpll算法求解
 * 返回值：SATISFIABLE UNSATISFIABLE
 */
status dpll(Answer *ans, LiteralList literals[])
{
    int status = 0, ltr_val = 0, blevel = 0;
    ClauseList *clp = NULL;
    while (1)
    {
        decide_next_branch(ans, &ltr_val, &blevel); //分支选择，返回选择为真的文字
        while (TRUE)
        {
            if (ltr_val > 0)
            {
                clp = literals[ltr_val].neg->next;
            }
            else
            {
                clp = literals[-ltr_val].pos->next;
            } //真文字为正文字，搜索其负文字相关子句，反之亦然

            status = deduce(ans, literals, clp, blevel);

            if (status == SATISFIABLE)
            {
                return SATISFIABLE;
            }
            else if (status == CONFLICT)
            {
                ltr_val = back_track(&blevel, ltr_val, ans);
                if (blevel == 0)
                {
                    return UNSATISFIABLE;
                }
                else
                {
                    ans->value[ltr_val] *= -1; //答案反转
                    ans->searched[ltr_val]++;
                    if (ans->value[ltr_val] < 0)
                    {
                        ltr_val *= -1;
                    }
                }
            } //冲突回溯
            else if (status == OTHERS)
            {
                break;
            }
        }
    }
}

/**
 * 函数名称：show_answer
 * 函数功能：打印结果
 * 返回值：无
 */
void show_answer(Answer *ans)
{
    for (int i = 1; i <= ltr_num; i++)
    {
        if (ans->value[i] > 0)
            printf("%d : TRUE\n", i);
        else if (ans->value[i] < 0)
            printf("%d : FALSE\n", i);
        else
            printf("%d : BOTH\n", i);
    }
    return;
}

/**
 * 函数名称：check_answer
 * 函数功能：检查结果
 * 返回值：TRUE FALSE
 */
// status check_answer(Answer *ans, ClauseNode *S)
// {
//     ClauseNode *cp = S;
//     for (int i = 1; i <= ltr_num; i++)
//     {
//         if (ans->value[i] > 0)
//         {
//             while (cp)
//             {

//             }
//         }
//     }
// }
