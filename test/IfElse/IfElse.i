# 1 "../test/IfElse/IfElse.c"

# 1 "../stl/sys.h"
extern void putchar(char ch);

extern void puts(char *str);

extern void system(char *cmd);

extern void sleep(int sec);

extern void usleep(int usec);

extern int kbhit();

extern int getch();

extern int rand();

extern int srand(int seed);

extern int time(int *seconds);
# 3 "../test/IfElse/IfElse.c"
void func(int n, int m)
{
    if (n == 1)
    {
        if (m == 1)
        {
            puts("n == 1 m == 1");
        }
        else if (m == 2)
        {
            puts("n == 1 m == 2");
        }
        else if (m == 3)
        {
            puts("n == 1 m == 3");
        }
        else
        {
            puts("n == 1 m == ?");
        }
    }
    else if (n == 2)
    {
        if (m == 1)
        {
            puts("n == 2 m == 1");
        }
        else if (m == 2)
        {
            puts("n == 2 m == 2");
        }
        else if (m == 3)
        {
            puts("n == 2 m == 3");
        }
        else
        {
            puts("n == 2 m == ?");
        }
    }
    else if (n == 3)
    {
        if (m == 1)
        {
            puts("n == 3 m == 1");
        }
        else if (m == 2)
        {
            puts("n == 3 m == 2");
        }
        else if (m == 3)
        {
            puts("n == 3 m == 3");
        }
        else
        {
            puts("n == 3 m == ?");
        }
    }
    else
    {
        if (m == 1)
        {
            puts("n == ? m == 1");
        }
        else if (m == 2)
        {
            puts("n == ? m == 2");
        }
        else if (m == 3)
        {
            puts("n == ? m == 3");
        }
        else
        {
            puts("n == ? m == ?");
        }
    }

}

int main()
{
    int n;
    int m;
    for (n = 1; n <= 4; n = n + 1)
    {
        for (m = 1; m <= 4; m = m + 1)
        {
            func(n, m)
        ;}
    }

    return 0;
}
