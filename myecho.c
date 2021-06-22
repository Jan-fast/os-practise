#include<stdio.h>
int main(int argc,char*argv[])
{
    int x;
    for(x = 1;x < argc;x ++)
        printf("%s ",argv[x]);
    puts("");
    return 0 ;
}
