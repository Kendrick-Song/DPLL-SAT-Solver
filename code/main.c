/**
 * 文件名称：main.c
 * 文件描述：主界面的显示，函数调用
 */
 
#include "head.h"

int main()
{
    int op = 1;
    while (op)
    {
        system("cls");
        printf("\n\n");
        printf("%20cSAT<\n", '>');
        printf("%10c***********************\n", ' ');
        printf("%16c1.  SAT\n", ' ');
        printf("%16c2.  Puzzle\n", ' ');
        printf("%16c0.  Exit\n", ' ');
        printf("%10c***********************\n", ' ');
        scanf("%d", &op);
        switch (op)
        {
        case 1:
            sat();
            break;
        case 2:
            puzzle();
            break;
        case 0:
            break;
        default:
            break;
        }
    }
    return 0;
}
