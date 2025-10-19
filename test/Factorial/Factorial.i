# 1 "../test/Factorial/Factorial.c"
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
# 2 "../test/Factorial/Factorial.c"


void printPositiveInt(int a)
{
    if (a) 
    {
        printPositiveInt(a / 10);
        putchar(a % 10 + '0');
    }
}


void printSignInt(int a)
{
    if (a < 0)
    {
        putchar('-');
        a = 0-a;
    }
    if (a == 0)
    {
        putchar('0');
    }
    else printPositiveInt(a);
    putchar('\n');
}


int factorial(int n)
{
    int s;
    int i;
    s = 1;
    
    for (i = 2; i <= n; i = i + 1)
    {
        s = s * i;
    }
    return s;
}


int factorial_recursion(int n)
{
    if (n == 1) return 1;
    int t;
    return n * factorial_recursion(n - 1);
}



int fib(int n)
{
    if (n == 1 || n == 2) return 1;
    return fib(n - 1) + fib(n - 2);
}

int main()
{
    printSignInt(factorial(5));
    printSignInt(-factorial(5));
    printSignInt(factorial_recursion(10));
    printSignInt(-factorial_recursion(10));
    printSignInt(fib(5));
    printSignInt(fib(8));
}
