/**
 * 文件名称：dpll.c
 * 文件描述：dpll求解过程相关函数定义
 */

#include "head.h"

/**
 * 函数名称：dpll
 * 函数功能：dpll算法求解
 * 返回值：SATISFIABLE UNSATISFIABLE
 */
status dpll(LiteralList literals[])
{
    int status = 0, val = 0, blevel = 0;
    ClauseNode *cp = NULL;
    while (1)
    {
        decide_next_branch(literals, &val, &blevel); //分支选择，返回选择为真的变元
        while (TRUE)
        {
            if (val > 0)
            {
                cp = literals[val].neg->next_clauseNode;
            }
            else
            {
                cp = literals[-val].pos->next_clauseNode;
            } //正文字为真，指向负文字子句链，反之亦然

            status = deduce(literals, cp, blevel);

            if(status == SATISFIABLE)
            {
                return SATISFIABLE;
            }
        }
    }
}

/**
 * 函数名称：decide_next_branch
 * 函数功能：选择搜索分支
 * 返回值：void
 */
void decide_next_branch(LiteralList literals[], int *val, int *blevel)
{
    for (int i = 1; i <= ltr_num; i++)
    {
        if (literals[i].value == UNKNOWN)
        {
            *val = i;
            (*blevel)++;
            literals[*val].value = 1;
            literals[*val].blevel = *blevel;
            literals[*val].assigned++;
            ltr_known++;
            return;
        }
    }
    return;
}

/**
 * 函数名称：deduce
 * 函数功能；使用单子句规则进行搜索化简
 * 返回值；SATISIABLE/CONFLICT/OTHERS
 */
status deduce(LiteralList literals[], ClauseNode *root, int blevel)
{
    ClauseNode *stack[Max_Ltr_Num]; //子句链指针栈
    ClauseNode *cp = NULL;
    int top = 0; //栈顶指针
    int status;

    stack[top] = root;
    top++;
    //根结点进栈

    while (top)
    {
        cp = stack[top - 1]; //访问栈顶元素
        status = UNIT;
        while (status == UNIT && cp != NULL)
        {
            status = unit_clause_deduce(literals, &cp, blevel);
            stack[top] = cp;
            top++;
        } //广度搜索

        top--;

        if (status == CONFLICT)
        {
            return CONFLICT;
        }

        if (top)
        {
            top--;
            cp = stack[top];
            if (cp->next_clauseNode)
            {
                stack[top] = cp->next_clauseNode;
                top++;
            }
        } //深度搜索

    } //搜索所有影响的变元

    if(ltr_known <ltr_num)
    {
        return OTHERS;
    }
    else
    {
        return SATISFIABLE;
    }
    
}

/**
 * 函数名称：unit_clause_deduce
 * 函数功能：检查单子句并返回其指针
 * 返回值：SATISFIABLE/CONFLICT/UNIT/OTHERS
 */
status unit_clause_deduce(LiteralList literals[], ClauseNode **cp, int blevel)
{
    VarNode *vp = (*cp)->vn;
    int unknown_num = 0, first_unknown = 0, satisfiable = 0;

    while (vp)
    {
        if (literals[abs(vp->var)].value * vp->var > 0)
        {
            satisfiable = 1;
            break;
        } //该子句有值为真的变元

        if (literals[abs(vp->var)].value == UNKNOWN)
        {
            unknown_num++;
            if (first_unknown == 0)
            {
                first_unknown = vp->var;
            } //记录第一个未知的变元

        } //该子句有未赋值的变元
        vp = vp->next_varNode;
    } //扫描该子句下的变元

    if (unknown_num == 0 && satisfiable == 0)
    {
        return CONFLICT;
    } //该子句变元都已知且子句不满足

    if (unknown_num == 1 && satisfiable == 0)
    {
        literals[abs(first_unknown)].unit_clause = 1;
        literals[abs(first_unknown)].value = first_unknown / abs(first_unknown);
        literals[abs(first_unknown)].blevel = blevel;
        ltr_known++;
        //推理出的变元赋值

        if (first_unknown > 0)
        {
            *cp = literals[first_unknown].neg->next_clauseNode;
        }
        else
        {
            *cp = literals[-first_unknown].pos->next_clauseNode;
        } //返回推理出的变元所影响的子句链

        return UNIT;
    }

    if (ltr_known < ltr_num)
    {
        *cp = NULL; //返回空，选择下一分支
        return OTHERS;
    } //推理结束且仍有未知变元

    return SATISFIABLE;
}