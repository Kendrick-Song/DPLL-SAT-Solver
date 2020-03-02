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
    ClauseNode *ctemp = NULL; //子句操作指针
    FILE *fp = NULL;
    int val = 0;            //变元值
    int clause_var_num = 0; //每个字句的变元数
    char string[20];

    if ((fp = fopen(filename, "r")) == NULL)
    {
        return FALSE;
    } //打开文件失败则返回

    while (1)
    {
        fscanf(fp, "%s", string);
        if (strcmp(string, "cnf") == 0) //读到cnf退出循环
            break;
    } //过滤无用字符

    fscanf(fp, "%d", &ltr_num); //读取文字数量
    fscanf(fp, "%d", &cls_num); //读取子句数量

    init_cnf(literals); //初始化

    fscanf(fp, "%d", &val); //读取第一个变元
    while (1)
    {
        ctemp = (ClauseNode *)malloc(sizeof(ClauseNode));
        ctemp->vn = NULL;
        ctemp->next_clauseNode = NULL;
        //初始化子句临时结点

        clause_var_num = 0; //初始化当前子句变元数
        while (val)
        {
            clause_var_num++;
            literals[abs(val)].value = UNKNOWN; //文字值状态更改

            vtemp = (VarNode *)malloc(sizeof(VarNode));
            vtemp->var = val; //结点赋值
            vtemp->next_varNode = NULL;
            //初始化变元临时结点

            if (clause_var_num == 1)
            {
                vp = vtemp;
                ctemp->vn = vp;
            } //存储子句中首个文字
            else
            {
                vp->next_varNode = vtemp;
                vp = vp->next_varNode;
            } //存储子句中非首个文字

            add_clause(literals, ctemp, val); //将子句添加到对应文字的邻接表

            fscanf(fp, "%d", &val);
        } //创建子句

        if (clause_var_num == 1)
        {
            literals[abs(vp->var)].value = vp->var / abs(vp->var);
            ltr_known++;
        } //该子句为单子句，则该变元为真，该文字值已知

        fscanf(fp, "%d", &val); //读取下一个变元

        if (feof(fp))
        {
            break;
        } //读取结束
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
    for (int i = 1; i <= ltr_num; i++)
    {
        literals[i].value = NONE;
        literals[i].assigned = 0;
        literals[i].blevel = 0;
        literals[i].unit_clause = 0;
        //文字相关参数初始化

        if (literals[i].pos != NULL)
        {
            free_clause(literals[i].pos); //释放正文字相关子句
            literals[i].pos = NULL;

            free_clause(literals[i].neg); //释放负文字相关子句
            literals[i].neg = NULL;
        } //若该文字头结点存在，说明有子句，应释放

        literals[i].pos = (ClauseNode *)malloc(sizeof(ClauseNode));
        literals[i].pos->vn = NULL;
        literals[i].pos->next_clauseNode = NULL;
        //正文字头结点初始化

        literals[i].neg = (ClauseNode *)malloc(sizeof(ClauseNode));
        literals[i].neg->vn = NULL;
        literals[i].neg->next_clauseNode = NULL;
        //负文字头结点初始化

    } //文字数组清空
}

/**
 * 函数名称：free_clause
 * 函数功能：释放存在的子句内存
 * 返回值：void
 */
void free_clause(ClauseNode *cfront)
{
    VarNode *vfront = NULL, *vrear = NULL;       //变元结点操作指针
    ClauseNode *crear = cfront->next_clauseNode; //指向首结点

    while (crear != NULL)
    {
        vfront = crear->vn;
        while (vfront != NULL)
        {
            vrear = vfront->next_varNode;
            free(vfront);
            vfront = vrear;
        } //释放变元结点内存

        crear->vn = NULL;
        crear = crear->next_clauseNode; //指向下一个子句结点
        free(cfront->next_clauseNode);
        cfront->next_clauseNode = crear;
    }
    return;
}

/**
 * 函数名称：add_clause
 * 函数功能：将子句添加到对应文字的邻接表
 * 返回值：void
 */
void add_clause(LiteralList literals[], ClauseNode *ctemp, int val)
{
    ClauseNode *cp = NULL;

    if (val > 0)
    {
        cp = literals[val].pos;
    }
    else
    {
        cp = literals[-val].neg;
    } //按正负文字分类该文字对应子句

    while (cp->next_clauseNode)
    {
        cp = cp->next_clauseNode;
    } //找到最后一个子句结点

    cp->next_clauseNode = ctemp; //创建新子句结点
    return;
}