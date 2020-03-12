/**
 * 文件名称：cnf.c
 * 文件描述：CNF相关操作函数定义
 */

#include "head.h"

/**
 * 函数名称：load_file
 * 函数功能：读取cnf文件到内存
 * 返回值：TRUE/FALSE
 */
status load_file(LiteralList literals[], char filename[])
{
    VarNode *vtemp = NULL, *vp = NULL;    //变元结点操作指针
    ClauseNode *ctemp = NULL, *cp = NULL; //子句操作指针
    FILE *fp = NULL;
    int val = 0;            //变元值
    int clause_var_num = 0; //每个字句的变元数
    char string[20];

    //打开文件失败则返回
    if ((fp = fopen(filename, "r")) == NULL)
    {
        return FALSE;
    }

    //过滤无用字符
    while (1)
    {
        fscanf(fp, "%s", string);
        if (strcmp(string, "cnf") == 0) //读到cnf退出循环
            break;
    }

    fscanf(fp, "%d", &ltr_num); //读取文字数量
    fscanf(fp, "%d", &cls_num); //读取子句数量

    init_cnf(literals); //初始化

    fscanf(fp, "%d", &val); //读取第一个变元
    while (1)
    {
        //初始化子句临时结点
        ctemp = (ClauseNode *)malloc(sizeof(ClauseNode));
        ctemp->vn = NULL;
        ctemp->next_clauseNode = NULL;

        //创建子句链表用于检查
        if (clist == NULL)
        {
            clist = ctemp;
        }
        else
        {
            cp = clist;
            while (cp->next_clauseNode != NULL)
            {
                cp = cp->next_clauseNode;
            }
            cp->next_clauseNode = ctemp;
        }

        clause_var_num = 0; //初始化当前子句变元数

        //创建子句
        while (val)
        {
            clause_var_num++; //子句变元数加一

            //文字值状态更改
            if (literals[abs(val)].value == NONE)
            {
                literals[abs(val)].value = UNKNOWN;
            }

            //初始化变元临时结点
            vtemp = (VarNode *)malloc(sizeof(VarNode));
            vtemp->var = val; //结点赋值
            vtemp->next_varNode = NULL;

            if (clause_var_num == 1)
            {
                //存储子句中首个文字
                vp = vtemp;
                ctemp->vn = vp;
            }
            else
            {
                //存储子句中非首个文字
                vp->next_varNode = vtemp;
                vp = vp->next_varNode;
            }

            add_clause(literals, ctemp, val); //将子句添加到对应文字的邻接表

            fscanf(fp, "%d", &val);
        }

        //该子句为单子句，则该变元为真，该文字值已知
        if (clause_var_num == 1)
        {
            literals[abs(vp->var)].value = vp->var / abs(vp->var);
            ltr_known++;
        }

        fscanf(fp, "%d", &val); //读取下一个变元

        //读取结束
        if (feof(fp))
        {
            break;
        }
    }
    return TRUE;
}

/**
 * 函数名称：InitCnf
 * 函数功能：初始化cnf相关结构
 * 返回值：TRUE
 */
void init_cnf(LiteralList literals[])
{
    ltr_known = 0; //初始化已知变元数

    //子句链空间释放鸡初始化
    ClauseNode *cp = NULL;
    while (clist != NULL)
    {
        cp = clist->next_clauseNode;
        free(clist);
        clist = cp;
    }

    //文字数组初始化
    for (int i = 1; i <= ltr_num; i++)
    {
        //文字相关信息初始化
        literals[i].value = NONE;
        literals[i].assigned = 0;
        literals[i].blevel = 0;
        literals[i].unit_clause = 0;
        literals[i].pos_cls_num = 0;
        literals[i].neg_cls_num = 0;

        //释放空间
        if (literals[i].pos != NULL)
        {
            free(literals[i].pos);
            literals[i].pos = NULL;
        }
        if (literals[i].neg != NULL)
        {
            free(literals[i].neg);
            literals[i].neg = NULL;
        }

        //正文字头结点初始化
        literals[i].pos = (ClauseNode *)malloc(sizeof(ClauseNode));
        literals[i].pos->vn = NULL;
        literals[i].pos->next_clauseNode = NULL;

        //负文字头结点初始化
        literals[i].neg = (ClauseNode *)malloc(sizeof(ClauseNode));
        literals[i].neg->vn = NULL;
        literals[i].neg->next_clauseNode = NULL;
    }
}

/**
 * 函数名称：add_clause
 * 函数功能：将子句添加到对应文字的邻接表
 * 返回值：void
 */
void add_clause(LiteralList literals[], ClauseNode *ctemp, int val)
{
    ClauseNode *cp = NULL;

    //按正负文字分类该文字对应子句
    if (val > 0)
    {
        cp = literals[val].pos;
        literals[val].pos_cls_num++;
    }
    else
    {
        cp = literals[-val].neg;
        literals[-val].neg_cls_num++;
    }

    //找到最后一个子句结点
    while (cp->next_clauseNode)
    {
        cp = cp->next_clauseNode;
    }

    //创建新子句结点
    cp->next_clauseNode = (ClauseNode *)malloc(sizeof(ClauseNode));
    cp = cp->next_clauseNode;
    //新结点赋值
    cp->vn = ctemp->vn;
    cp->next_clauseNode = NULL;

    return;
}