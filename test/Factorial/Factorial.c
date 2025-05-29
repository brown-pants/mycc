int mod(int a)
{
    return a - (a/10) * 10;
}

void printInt(int a)
{
    if (a) 
    {
        printInt(a / 10);
        mycc_putchar(mod(a) + '0');
    }
}

void printSignInt(int a)
{
    if (a < 0)
    {
        mycc_putchar('-');
        a = 0-a;
    }
    if (a == 0)
    {
        mycc_putchar('0');
    }
    else printInt(a);
    mycc_putchar('\n');
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
    printSignInt(0-factorial(5));
    printSignInt(factorial_recursion(10));
    printSignInt(0-factorial_recursion(10));
}