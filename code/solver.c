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

    //DPLL求解过程
    while (1)
    {
        //分支决策
        switch (op)
        {
        case 1:
            //随机分支选择，返回选择为真的变元
            rand_decide(literals, &val, &blevel);
            break;
        case 2:
            //VSIDS分支选择，返回选择为真的变元
            VSIDS_decide(literals, &val, &blevel);
            break;
        default:
            break;
        }

        while (TRUE)
        {
            //指向受影响的子句链
            if (val > 0)
            {
                //正文字为真，指向负文字子句链
                cp = literals[val].neg->next_clauseNode;
            }
            else
            {
                //负文字为真，指向正文字子句链
                cp = literals[-val].pos->next_clauseNode;
            }

            status = deduce(literals, cp, blevel); //BCP传播

            if (status == SATISFIABLE)
            {
                return SATISFIABLE;
            }
            else if (status == CONFLICT)
            {
                //冲突回溯,返回需要更改决策的变元
                val = back_track(literals, &blevel, val);

                if (blevel == 0)
                {
                    //回溯至决策级0，则问题无解
                    return UNSATISFIABLE;
                }
                else
                {
                    //反转决策变元的值，被赋值次数加一
                    literals[val].value *= -1;
                    literals[val].assigned++;
                    if (literals[val].value < 0)
                    {
                        val *= -1;
                    }
                }
            }
            else if (status == OTHERS)
            {
                //无法得出结论，继续选择决策变元
                break;
            }
        }
    }
}

/**
 * 函数名称：rand_decide
 * 函数功能：随机选择搜索分支
 * 返回值：void
 */
void rand_decide(LiteralList literals[], int *val, int *blevel)
{
    //扫描文字邻接表，将第一个未赋值的变元赋值为真并返回
    for (int i = 1; i <= ltr_num; i++)
    {
        if (literals[i].value == UNKNOWN)
        {
            //决策级，被赋值次数，已知文字数加一
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
 * 函数名称：VSIDS_decide
 * 函数功能：选择搜索分支
 * 返回值：void
 */
void VSIDS_decide(LiteralList literals[], int *val, int *blevel)
{
    int temp = 0;
    int var = 0;

    //找到子句最多的变元
    for (int i = 1; i <= ltr_num; i++)
    {
        //简单的预处理，将只有正子句或负子句存在的文字赋值
        if (*blevel == 0)
        {
            if (literals[i].pos_cls_num == 0 || literals[i].neg_cls_num == 0)
            {
                if (literals[i].neg_cls_num == 0)
                {
                    //负文字对应子句数为0，则该文字值必为正
                    literals[i].value = 1;
                }
                else
                {
                    //正文字对应子句数为0，则该文字值必为负
                    literals[i].value = -1;
                }
                ltr_known++;
                continue;
            }
        }

        if (literals[i].value != UNKNOWN)
        {
            //该文字值已知，跳过本次循环
            continue;
        }

        //暂存子句最多的文字
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

    //决策变元赋值，决策级，赋值次数，已知文字数加一
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

    //根结点进栈
    stack[top] = root;
    top++;

    //栈空退出循环
    while (top)
    {
        cp = stack[top - 1]; //访问栈顶元素
        status = UNIT;

        //广度搜索进栈
        while (status == UNIT && cp != NULL)
        {
            status = unit_clause_deduce(literals, &cp, blevel);
            stack[top] = cp;
            top++;
        }

        top--; //空指针退栈

        if (status == CONFLICT)
            return CONFLICT;

        //栈不为空，深度搜索进栈
        if (top)
        {
            //访问栈顶元素并退栈
            top--;
            cp = stack[top];

            //深度搜索，不为空则进栈
            if (cp->next_clauseNode)
            {
                stack[top] = cp->next_clauseNode;
                top++;
            }
        }
    }

    if (ltr_known < ltr_num)
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
 * 函数功能：检查单子句，若为单子句，返回推导出的文字影响的子句链
 * 返回值：SATISFIABLE/CONFLICT/UNIT/OTHERS
 */
status unit_clause_deduce(LiteralList literals[], ClauseNode **cp, int blevel)
{
    VarNode *vp = (*cp)->vn;
    int unknown_num = 0, first_unknown = 0, satisfiable = 0;

    //扫描该子句下的变元
    while (vp)
    {
        //该子句有值为真的变元
        if (literals[abs(vp->var)].value * vp->var > 0)
        {
            satisfiable = 1;
            break;
        }

        //该子句有未赋值的变元
        if (literals[abs(vp->var)].value == UNKNOWN)
        {
            //未知文字数加一
            unknown_num++;

            //记录第一个未知的变元
            if (first_unknown == 0)
            {
                first_unknown = vp->var;
            }
        }
        vp = vp->next_varNode;
    }

    //该子句无未知变元且子句不满足
    if (unknown_num == 0 && satisfiable == 0)
    {
        return CONFLICT;
    }

    //该子句仅有一个未知变元且子句不满足（单子句）
    if (unknown_num == 1 && satisfiable == 0)
    {
        //推理出的变元赋值
        literals[abs(first_unknown)].unit_clause = 1;
        literals[abs(first_unknown)].value = first_unknown / abs(first_unknown);
        literals[abs(first_unknown)].blevel = blevel;
        ltr_known++;

        //返回推理出的变元所影响的子句链
        if (first_unknown > 0)
        {
            *cp = literals[first_unknown].neg->next_clauseNode;
        }
        else
        {
            *cp = literals[-first_unknown].pos->next_clauseNode;
        }
        return UNIT;
    }

    //推理结束且仍有未知变元
    if (ltr_known < ltr_num)
    {
        *cp = NULL; //返回空，选择下一分支
        return OTHERS;
    }

    return SATISFIABLE;
}

/**
 * 函数名称：back_track
 * 函数功能：回溯
 * 返回值：需要反转的决策变量
 */
int back_track(LiteralList literals[], int *blevel, int val)
{
    int parent = abs(val);

    //初始化当前决策级被推理出的文字的相关参数
    for (int i = 1; i <= ltr_num; i++)
    {
        if (i != parent && literals[i].blevel == *blevel)
        {
            literals[i].value = UNKNOWN;
            literals[i].blevel = 0;
            literals[i].assigned = 0;
            literals[i].unit_clause = 0;
            ltr_known--;
        }
    }

    //决策变元被赋值次数为2，初始化该变元，回退上一决策级
    while (*blevel != 1 && literals[parent].assigned == 2)
    {
        (*blevel)--;
        literals[parent].value = UNKNOWN;
        literals[parent].blevel = 0;
        literals[parent].assigned = 0;
        ltr_known--;

        //找到上一级的决策变元,初始化上一决策级推理出的文字信息
        for (int j = 1; j <= ltr_num; j++)
        {
            //找到上一级的决策变元
            if (literals[j].blevel == *blevel && literals[j].unit_clause == 0)
            {
                parent = j;
            }
            //初始化上一决策级被推理出的文字的相关参数
            if (j != parent && literals[j].blevel == *blevel)
            {
                literals[j].value = UNKNOWN;
                literals[j].blevel = 0;
                literals[j].assigned = 0;
                literals[j].unit_clause = 0;
                ltr_known--;
            }
        }
    }

    //回退到的决策级是1且被赋值次数为2
    if (*blevel == 1 && literals[parent].assigned == 2)
    {
        //决策级减一变为0，无解
        (*blevel)--;
    }

    //返回要回退到的决策级对应的决策变量
    return parent;
}

