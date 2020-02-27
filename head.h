/**
 * 文件名称：head.h
 * 文件描述：SAT求解器相关常量，数据结构定义，相关操作函数声明
**/
#ifndef _HEAD_H
#define _HEAD_H

/*头文件引用*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
/*常量定义*/
typedef int status;
int ltr_num;   //全部文字数
int cls_num;   //全部子句数
int ltr_known; //已知文字数
//变量相关
#define TRUE 1           //变量为真
#define FALSE -1         //变量为假
#define UNKNOWN 0        //变量未赋值
#define NONE 2           //变量不存在
#define Max_Ltr_Num 4000 //最大文字数
//范式相关
#define SATISFIABLE 1   //可满足
#define UNSATISFIABLE 0 //不可满足
#define CONFLICT 0      //存在冲突
#define OTHERS 2        //其他情况

/*数据结构定义*/
//文字结点定义
typedef struct literalNode
{
    int x;                                //文字的值
    struct literalNode *next_literalNode; //指向下一个文字结点
} LiteralNode;

//子句结点定义
typedef struct clauseNode
{
    LiteralNode *p;                     //指向子句的文字链
    struct clauseNode *next_caluseNode; //指向下一个子句结点
} ClauseNode;

//子句链表定义
typedef struct clauseList
{
    ClauseNode *p;           //指向包含该文字的一个子句
    struct clauseList *next; //指向包含该文字的下一个子句
} ClauseList;

//文字邻接表定义
typedef struct literalList
{
    ClauseList *pos; //正文字邻接表
    ClauseList *neg; //负文字邻接表
} LiteralList;

//答案结构定义
typedef struct answer
{
    int *branchLevel; //赋值时的决策树高度
    int *value;       //TRUE or FALSE or UNKNOWN or NONE
    int *searched;    //已被搜索的情况数
    int *unitClause;  //标记是否存在该变量的单子句
} Answer;

/*函数声明*/
//cnf.c中相关函数声明
status InitCnf(ClauseList **G, Answer **ans, LiteralList literals[]);
status AddClause(ClauseNode *ctemp, int var, LiteralList literals[]);
status LoadCnfFile(ClauseNode **G, Answer *ans, LiteralList literals[], char *filename);
#endif