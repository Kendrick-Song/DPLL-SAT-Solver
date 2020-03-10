/**
 * 文件名称：solver.c
 * 文件描述：dpll求解过程相关函数定义
 */

#include "head.h"

/**
 * 函数名称：dpll
 * 函数功能：dpll算法求解
 * 返回值：SATISFIABLE/UNSATISFIABLE
 */
status dpll(LiteralList literals[], int op)
{
    int status = 0, val = 0, blevel = 0;
    ClauseNode *cp = NULL;
    while (1)
    {
        switch (op)
        {
            case 1:
                decide_next_branch(literals, &val, &blevel); //分支选择，返回选择为真的变元
                break;
            case 2:
                VSIDS(literals, &val, &blevel); //分支选择，返回选择为真的变元
            default:
                break;
        }
        while (TRUE)
        {
            if (val > 0)
            {
                cp = literals[val].neg->next_clauseNode;
            } else
            {
                cp = literals[-val].pos->next_clauseNode;
            } //正文字为真，指向负文字子句链，反之亦然

            status = deduce(literals, cp, blevel); //BCP传播

            if (status == SATISFIABLE)
            {
                return SATISFIABLE;
            } else if (status == CONFLICT)
            {
                val = back_track(literals, &blevel, val); //回溯,返回需要更改决策的变元

                if (blevel == 0)
                {
                    return UNSATISFIABLE;
                } else
                {
                    literals[val].value *= -1;
                    literals[val].assigned++;
                    if (literals[val].value < 0)
                    {
                        val *= -1;
                    }
                }
            } else if (status == OTHERS)
            {
                break;
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
 * 函数名称：VSIDS
 * 函数功能：选择搜索分支
 * 返回值：void
 */
void VSIDS(LiteralList literals[], int *val, int *blevel)
{
    int temp = 0;
    int var = 0;
    for (int i = 1; i <= ltr_num; i++)
    {
        if (*blevel == 0)
        {
            if (literals[i].pos_cls_num == 0 || literals[i].neg_cls_num == 0)
            {
                if (literals[i].neg_cls_num == 0)
                {
                    literals[i].value = 1;//负文字对应子句数为0，则该文字必为负
                } else
                {
                    literals[i].value = -1;//正文字对应子句数为0，则该文字必为正
                }
                ltr_known++;
                continue;
            }
        }//简单的预处理

        if (literals[i].value != UNKNOWN)
        {
            continue;//该文字值已知，跳过本次循环
        }

        if (literals[i].pos_cls_num >= temp)
        {
            temp = literals[i].pos_cls_num;
            var = i;
        }
        if (literals[i].neg_cls_num >= temp)
        {
            temp = literals[i].neg_cls_num;
            var = -i;
        }
    }

    *val = var;
    (*blevel)++;
    literals[abs(*val)].value = abs(*val) / (*val);
    literals[abs(*val)].blevel = *blevel;
    literals[abs(*val)].assigned++;
    ltr_known++;
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

        top--; //空指针退栈

        if (status == CONFLICT)
            return CONFLICT;

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

    if (ltr_known < ltr_num)
    {
        return OTHERS;
    } else
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
        } else
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

/**
 * 函数名称：back_track
 * 函数功能：回溯
 * 返回值：文字值
 */
int back_track(LiteralList literals[], int *blevel, int val)
{
    int parent = abs(val);
    while (*blevel != 0)
    {
        for (int i = 1; i <= ltr_num; i++)
        {
            if (i != parent && literals[i].blevel == *blevel)
            {
                literals[i].value = UNKNOWN;
                literals[i].blevel = 0;
                literals[i].assigned = 0;
                literals[i].unit_clause = 0;
                ltr_known--;
            } //初始化该决策级的文字相关参数
        }

        if (*blevel != 1)
        {
            if (literals[parent].assigned == 2)
            {
                (*blevel)--;
                literals[parent].value = UNKNOWN;
                literals[parent].blevel = 0;
                literals[parent].assigned = 0;
                ltr_known--;
                //被赋值次数为2，回退上一决策级，初始化该变元

                for (int j = 1; j <= ltr_num; j++)
                {
                    if (literals[j].blevel == *blevel && literals[j].unit_clause == 0)
                    {
                        parent = j;
                        break;
                    }
                }
            } else
                break;
        } else if (literals[parent].assigned == 2)
        {
            (*blevel)--;
        } else
        {
            break;
        }
    }
    return parent;
}