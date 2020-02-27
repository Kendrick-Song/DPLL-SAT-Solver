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
    printf("Please input the file path:\n");
    scanf("%s", filename);
    LoadCnfFile(&S, &ans, literals, filename);
    return 0;
}

/**
 * 文件名称：cnf.c
 * 文件描述：CNF相关操作函数定义
**/

/**
 * 函数名称：InitCnf
 * 函数功能：初始化cnf相关结构
 * 返回值：TRUE
**/
status InitCnf(ClauseNode **G, Answer **ans, LiteralList literals[])
{
    //子句初始化
    ClauseNode *cfront = *G, *crear = NULL;    //子句链表操作指针
    LiteralNode *lfront = NULL, *lrear = NULL; //文字链表操作指针
    while (cfront)
    {
        crear = cfront->next_caluseNode;
        lfront = cfront->p;
        while (lfront)
        {
            lrear = lfront->next_literalNode;
            free(lfront);
            lfront = lrear;
        } //文字结点初始化
        free(cfront);
        cfront = crear;
    } //子句结点初始化
    *G = NULL;

    //解初始化
    ltr_known = 0;
    *ans = (Answer *)malloc(sizeof(Answer));

    for (int i = 0; i <= (ltr_num + 1); i++)
    {
        (*ans)->value[i] = NONE;
        (*ans)->branchLevel[i] = 0;
        (*ans)->searched[i] = 0;
        (*ans)->unitClause[i] = 0;
        //文字邻接表初始化
        literals[i].pos = (ClauseList *)malloc(sizeof(ClauseList));
        literals[i].pos->p = NULL;
        literals[i].pos->next = NULL;
        literals[i].neg = (ClauseList *)malloc(sizeof(ClauseList));
        literals[i].neg->p = NULL;
        literals[i].neg->next = NULL;
    }
    return TRUE;
}
/**
 * 函数名称：AddClause
 * 函数功能：将子句添加到对应文字的邻接表中
 * 返回值：TRUE
**/
status AddClause(ClauseNode *ctemp, int var, LiteralList literals[])
{
    ClauseList *clp = NULL;
    if (var > 0)
    {
        clp = literals[var].pos;
    }
    else
    {
        clp = literals[-var].neg;
    } //按正负文字分类该文字对应子句
    while (clp->next)
    {
        clp = clp->next;
    } //找到最后一个子句链

    clp->next = (ClauseList *)malloc(sizeof(ClauseList)); //创建新子句链
    clp = clp->next;
    clp->p = ctemp;
    clp->next = NULL;
    return TRUE;
}

/**
 * 函数名称：LoadCnfFile
 * 函数功能：读取cnf文件中的字符进行预处理
 * 返回值：TRUE or FALSE
**/
status LoadCnfFile(ClauseNode **G, Answer **ans, LiteralList literals[], char *filename)
{
    char string[20];
    ClauseNode *ctemp = NULL, *cp = NULL;  //子句操作指针
    LiteralNode *ltemp = NULL, *lp = NULL; //文字操作指针
    int var = 0;                           //变量的值
    int numClauseVar = 0;                  //每个字句的变量数
    FILE *fp = NULL;

    if ((fp = fopen(filename, "r")) == NULL) //打开文件
        return FALSE;
    while (1)
    {
        fscanf(fp, "%s", string);
        if (strcmp(string, "cnf") == 0) //读到cnf退出循环
            break;
    }

    fscanf(fp, "%d", &ltr_num); //读取文字数量
    fscanf(fp, "%d", &cls_num); //读取子句数量

    InitCnf(G, ans, literals); //初始化

    fscanf(fp, "%d", &var); //变量赋值
    while (1)
    {
        numClauseVar = 0;
        ctemp = (ClauseNode *)malloc(sizeof(ClauseNode)); //初始化子句临时结点
        ctemp->p = NULL;
        ctemp->next_caluseNode = NULL;
        lp = ctemp->p;
        while (var)
        {
            ++numClauseVar;
            if ((*ans)->value[abs(var)] == NONE)
            {
                (*ans)->value[abs(var)] = UNKNOWN;
            } //答案文字状态更改

            ltemp = (LiteralNode *)malloc(sizeof(LiteralNode)); //初始化文字临时结点
            ltemp->x = var;                                     //结点赋值
            ltemp->next_literalNode = NULL;
            if (numClauseVar == 1)
            {
                lp = ltemp;
                ctemp->p = lp;
            } //存储子句中首个文字
            else
            {
                lp->next_literalNode = ltemp;
                lp = lp->next_literalNode;
            } //存储子句中非首个文字

            AddClause(ctemp, var, literals); //将子句添加到对应文字的邻接表
            fscanf(fp, "%d", &var);
        } //创建子句

        if (numClauseVar == 1)
        {
            (*ans)->value[abs(lp->x)] = lp->x / abs(lp->x); //单子句该变量解已知
            ++ltr_known;                                    //已知文字数加一
        }
        else if ((*G) == NULL)
        {
            cp = ctemp;
            *G = cp;
            cp->next_caluseNode = NULL;
        } //创建子句链表首结点
        else
        {
            cp->next_caluseNode = ctemp;
            cp = cp->next_caluseNode;
        } //创建子句链表非首结点
        fscanf(fp, "%d", &var);
        if (feof(fp))
        {
            break;
        } //读取结束
    }
    fclose(fp);
    return TRUE;
}
