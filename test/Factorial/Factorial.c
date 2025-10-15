void putchar(int c);

void printInt(int a)
{
    if (a) 
    {
        printInt(a / 10);
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
    else printInt(a);
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


int main()
{
    int defualt_var;
    printSignInt(defualt_var);
    printSignInt(factorial(5));
    printSignInt(-factorial(5));
    printSignInt(factorial_recursion(10));
    printSignInt(-factorial_recursion(10));
}