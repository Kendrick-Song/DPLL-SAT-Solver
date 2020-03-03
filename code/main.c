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
        printf("\n\n");
        printf("%20cSAT<\n", '>');
        printf("%10c***********************\n", ' ');
        printf("%16c1.  SAT\n", ' ');
        printf("%16c2.  Sudku\n", ' ');
        printf("%16c0.  Exit\n", ' ');
        printf("%10c***********************\n", ' ');
        scanf("%d", &op);
        system("cls");
        switch (op)
        {
        case 1:
            sat();
            break;
        case 0:
            return 0;
        default:
            break;
        }
    }
}
