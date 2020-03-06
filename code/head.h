/**
 * 文件名称：head.h
 * 文件描述：SAT求解器相关常量，数据结构定义，相关操作函数声明
 */

#ifndef _HEAD_H
#define _HEAD_H

/*头文件引用*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

/*常量定义*/
//变元相关
#define TRUE 1           //变元为真
#define FALSE -1         //变元为假
#define UNKNOWN 0        //变元未赋值
#define NONE 2           //变元不存在
#define Max_Ltr_Num 4000 //最大文字数
//范式相关
#define SATISFIABLE 1   //可满足
#define UNSATISFIABLE 0 //不可满足
#define CONFLICT 0      //存在冲突
#define OTHERS 2        //其他情况
#define UNIT -1         //单子句

/*数据结构定义*/
//变元结点定义
typedef struct varNode
{
    int var;                      //变元
    struct varNode *next_varNode; //指向下一个变元结点
} VarNode;

//子句结点定义
typedef struct clauseNode
{
    VarNode *vn;                        //指向文字结点
    struct clauseNode *next_clauseNode; //指向下一个子句结点
} ClauseNode;

//文字邻接表定义
typedef struct literalList
{
    int value;       //TRUE/FALSE/UNKNOWN/NONE
    int blevel;      //赋值时的决策级
    int assigned;    //被赋值的次数
    int unit_clause; //标记是否存在该变元的单子句
    ClauseNode *pos; //正文字邻接表
    ClauseNode *neg; //负文字邻接表
} LiteralList;

/*变量定义*/
typedef int status;
int ltr_num;   //全部文字数
int cls_num;   //全部子句数
int ltr_known; //已知文字数
ClauseNode *clist; //子句链表用于检查

/*函数声明*/
//cnfparser.c中函数声明
void add_clause(LiteralList literals[], ClauseNode *ctemp, int val);
void init_cnf(LiteralList literals[]);
status load_file(LiteralList literals[], char filename[]);
//solver.c中函数声明
void decide_next_branch(LiteralList literals[], int *val, int *blevel);
status deduce(LiteralList literals[], ClauseNode *root, int blevel);
status unit_clause_deduce(LiteralList literals[], ClauseNode **cp, int blevel);
int back_track(LiteralList literals[], int *blevel, int val);
status dpll(LiteralList literals[]);
//diplay.c中函数声明
void show_answer(LiteralList literals[], clock_t cost, int result, char filename[]);
void check_answer(LiteralList literals[]);
void sat();



#endif