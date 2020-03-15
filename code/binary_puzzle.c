/**
 * 文件名称：binary_puzzle
 * 文件描述：二进制数独模块,包括游戏格局生成、归约、求解
 */

#include "head.h"

/**
 * 函数名称：rules_generate
 * 函数功能：生成二进制数独规则的cnf文件
 * 返回值：void
 */
void rules_generate(FILE *fp)
{
    char filename[] = "puzzle.cnf";
    fp = fopen(filename, "w");
    fprintf(fp, "p cnf 30000 30000\n");

    rule_1(fp);
    rule_2(fp);
    rule_3(fp);

    fclose(fp);
    return;
}

/**
 * 函数名称：rule_1
 * 函数功能：规则1子句生成（在每一行、每一列中不允许有连续的3个1或3个0出现）
 * 返回值：void
 */
void rule_1(FILE *fp)
{
    int var = 0;

    //行生成
    for (int i = 1; i <= 6; i++)
    {
        for (int j = 1; j <= 4; j++)
        {
            var = 10 * i + j;
            fprintf(fp, "%d %d %d 0\n", var, var + 1, var + 2);
            fprintf(fp, "%d %d %d 0\n", -var, -(var + 1), -(var + 2));
        }
    }

    //列生成
    for (int j = 1; j <= 6; j++)
    {
        for (int i = 1; i <= 4; i++)
        {
            var = 10 * i + j;
            fprintf(fp, "%d %d %d 0\n", var, var + 10, var + 20);
            fprintf(fp, "%d %d %d 0\n", -var, -(var + 10), -(var + 20));
        }
    }
}

/**
 * 函数名称：rule_2
 * 函数功能：规则2子句生成（在每一行、每一列中1与0的个数相同）
 * 返回值：void
 */
void rule_2(FILE *fp)
{
    int j = 0;
    int var = 0;
    int exhaustion[] = {1, 2, 3, 4, 0, 1, 2, 3, 5, 0, 1, 2, 3, 6, 0, 1, 2, 4, 5, 0, 1, 2, 4, 6, 0, 1, 2, 6, 5, 0, 1, 3, 4, 5, 0, 1, 3, 4, 6, 0, 1, 3, 5, 6, 0, 1, 4, 5, 6, 0, 2, 3, 4, 5, 0, 2, 3, 4, 6, 0, 2, 3, 5, 6, 0, 2, 4, 5, 6, 0, 3, 4, 5, 6, -1};

    //行生成
    for (int i = 1; i <= 6; i++)
    {
        //任选4个至少有一个1
        while (exhaustion[j] != -1)
        {
            var = 10 * i + exhaustion[j];

            if (exhaustion[j] == 0)
            {
                //数组到零换行
                fprintf(fp, "0\n");
            }
            else
            {
                //写入文件
                fprintf(fp, "%d ", var);
            }
            j++;
        }

        fprintf(fp, "0\n");
        j = 0;

        //任选4个至少有一个0
        while (exhaustion[j] != -1)
        {
            var = 10 * i + exhaustion[j];

            if (exhaustion[j] == 0)
            {
                //数组到零换行
                fprintf(fp, "0\n");
            }
            else
            {
                //写入文件
                fprintf(fp, "%d ", -var);
            }
            j++;
        }

        fprintf(fp, "0\n");
        j = 0;
    }

    //列生成
    for (int i = 1; i <= 6; i++)
    {
        //任选4个至少有一个1
        while (exhaustion[j] != -1)
        {
            var = i + 10 * exhaustion[j];

            if (exhaustion[j] == 0)
            {
                //数组到零换行
                fprintf(fp, "0\n");
            }
            else
            {
                //写入文件
                fprintf(fp, "%d ", var);
            }
            j++;
        }

        fprintf(fp, "0\n");
        j = 0;

        //任选4个至少有一个0
        while (exhaustion[j] != -1)
        {
            var = i + 10 * exhaustion[j];

            if (exhaustion[j] == 0)
            {
                //数组到零换行
                fprintf(fp, "0\n");
            }
            else
            {
                //写入文件
                fprintf(fp, "%d ", -var);
            }
            j++;
        }

        fprintf(fp, "0\n");
        j = 0;
    }
}

/**
 * 函数名称：rule_3
 * 函数功能：规则3子句生成（不存在重复的行与重复的列）
 * 返回值：void
 */
void rule_3(FILE *fp)
{
    int var1 = 0, var2 = 0; //31 51

    //附加变元存储数组
    int pos_addition[7] = {'\0'}; //13511...13561
    int neg_addition[7] = {'\0'}; //13510...13560
    int addition[7] = {'\0'};     //1351...1356

    //行生成
    for (int std_line = 1; std_line <= 6; std_line++) //基准行
    {
        for (int cmp_line = std_line + 1; cmp_line <= 6; cmp_line++) //比较行
        {
            //两行的每一列比较
            for (int column = 1; column <= 6; column++)
            {
                pos_addition[column] = 10000 + std_line * 1000 + cmp_line * 100 + column * 10 + 1; //13511
                neg_addition[column] = 10000 + std_line * 1000 + cmp_line * 100 + column * 10;     //13510
                addition[column] = 1000 + std_line * 100 + cmp_line * 10 + column;                 //1351
                var1 = std_line * 10 + column;                                                     //31
                var2 = cmp_line * 10 + column;                                                     //51

                //13511= 31∧51转化为CNF生成3个子句：31∨¬13511；51∨¬13511；¬31∨¬51∨13511
                fprintf(fp, "%d %d 0\n", var1, -pos_addition[column]);
                fprintf(fp, "%d %d 0\n", var2, -pos_addition[column]);
                fprintf(fp, "%d %d %d 0\n", -var1, -var2, pos_addition[column]);

                //13510= ¬31∧¬51转化为CNF生成3个子句：¬31∨¬13510；¬51∨¬13510；31∨51∨13510
                fprintf(fp, "%d %d 0\n", -var1, -neg_addition[column]);
                fprintf(fp, "%d %d 0\n", -var2, -neg_addition[column]);
                fprintf(fp, "%d %d %d 0\n", var1, var2, neg_addition[column]);

                //1351= 13511∨13510转化为CNF生成3个子句：1351∨¬13511；1351∨¬13510；13511∨13510∨¬1351
                fprintf(fp, "%d %d 0\n", addition[column], -pos_addition[column]);
                fprintf(fp, "%d %d 0\n", addition[column], -neg_addition[column]);
                fprintf(fp, "%d %d %d 0\n", pos_addition[column], neg_addition[column], -addition[column]);
            }

            //135= ¬[1351∧1352∧…∧1356]转化为CNF生成7个子句：（¬135∨¬1351∨¬1352∨…∨¬1356）∧（1351∨135）∧（1352∨135）…（1356∨135）

            int var3 = 100 + std_line * 10 + cmp_line; //135
            fprintf(fp, "%d 0\n", var3);               //135不能为假

            //（¬135∨¬1351∨¬1352∨…∨¬1356）
            fprintf(fp, "%d %d %d %d %d %d %d 0\n", -var3, -addition[1], -addition[2], -addition[3], -addition[4], -addition[5], -addition[6]);

            //（1351∨135）∧（1352∨135）…（1356∨135）
            for (int i = 1; i <= 6; i++)
            {
                fprintf(fp, "%d %d 0\n", var3, addition[i]);
            }
        }
    }

    //列生成
    for (int std_col = 1; std_col <= 6; std_col++) //基准列
    {
        for (int cmp_col = std_col + 1; cmp_col <= 6; cmp_col++) //比较列
        {
            //两行的每一列比较
            for (int line = 1; line <= 6; line++)
            {
                pos_addition[line] = 20000 + std_col * 1000 + cmp_col * 100 + line * 10 + 1; //23511
                neg_addition[line] = 20000 + std_col * 1000 + cmp_col * 100 + line * 10;     //23510
                addition[line] = 2000 + std_col * 100 + cmp_col * 10 + line;                 //2351
                var1 = std_col + line * 10;                                                  //13
                var2 = cmp_col + line * 10;                                                  //15

                //23511= 13∧15转化为CNF生成3个子句：13∨¬23511；15∨¬13511；¬13∨¬15∨23511
                fprintf(fp, "%d %d 0\n", var1, -pos_addition[line]);
                fprintf(fp, "%d %d 0\n", var2, -pos_addition[line]);
                fprintf(fp, "%d %d %d 0\n", -var1, -var2, pos_addition[line]);

                //23510= ¬13∧¬15转化为CNF生成3个子句：¬13∨¬23510；¬15∨¬23510；13∨15∨23510
                fprintf(fp, "%d %d 0\n", -var1, -neg_addition[line]);
                fprintf(fp, "%d %d 0\n", -var2, -neg_addition[line]);
                fprintf(fp, "%d %d %d 0\n", var1, var2, neg_addition[line]);

                //2351= 23511∨23510转化为CNF生成3个子句：2351∨¬23511；2351∨¬23510；23511∨23510∨¬2351
                fprintf(fp, "%d %d 0\n", addition[line], -pos_addition[line]);
                fprintf(fp, "%d %d 0\n", addition[line], -neg_addition[line]);
                fprintf(fp, "%d %d %d 0\n", pos_addition[line], neg_addition[line], -addition[line]);
            }

            //235= ¬[2351∧2352∧…∧2356]转化为CNF生成7个子句：（¬235∨¬2351∨¬2352∨…∨¬2356）∧（2351∨235）∧（2352∨235）…（2356∨235）
            int var3 = 200 + std_col * 10 + cmp_col; //235
            // fprintf(fp, "%d 0\n", var3);             //235不能为假

            //（¬235∨¬2351∨¬2352∨…∨¬2356）
            fprintf(fp, "%d %d %d %d %d %d %d 0\n", -var3, -addition[1], -addition[2], -addition[3], -addition[4], -addition[5], -addition[6]);

            //（2351∨235）∧（2352∨235）…（2356∨235）
            for (int i = 1; i <= 6; i++)
            {
                fprintf(fp, "%d %d 0\n", var3, addition[i]);
            }
        }
    }
}

/**
 * 函数名称：show_puzzle
 * 函数功能：打印数独
 * 返回值：void
 */
void show_puzzle(LiteralList literalList[])
{
    int x = 0;

    //行打印
    for (int i = 1; i <= 6; i++)
    {
        //列打印
        for (int j = 1; j <= 6; j++)
        {
            x = 10 * i + j;
            if (literalList[x].value == UNKNOWN)
            {
                //文字值未知，打印' _ '
                printf("%4c", '_');
            }
            else if (literalList[x].value > 0)
            {
                //文字值为正，打印' 1 '
                printf("%4d", literalList[x].value);
            }
            else
            {
                //文字值为负，打印' 0 '
                printf("%4d", 0);
            }
        }
        printf("\n");
    }
}