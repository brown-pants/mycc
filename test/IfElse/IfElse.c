void func(int n, int m)
{
    if (n == 1)
    {
        if (m == 1)
        {
            mycc_putchar('1');
            mycc_putchar('1');
            mycc_putchar('\n');
        }
        else if (m == 2)
        {
            mycc_putchar('1');
            mycc_putchar('2');
            mycc_putchar('\n');
        }
        else if (m == 3)
        {
            mycc_putchar('1');
            mycc_putchar('3');
            mycc_putchar('\n');
        }
        else
        {
            mycc_putchar('1');
            mycc_putchar('$');
            mycc_putchar('\n');
        }
    }
    else if (n == 2)
    {
        if (m == 1)
        {
            mycc_putchar('2');
            mycc_putchar('1');
            mycc_putchar('\n');
        }
        else if (m == 2)
        {
            mycc_putchar('2');
            mycc_putchar('2');
            mycc_putchar('\n');
        }
        else if (m == 3)
        {
            mycc_putchar('2');
            mycc_putchar('3');
            mycc_putchar('\n');
        }
        else
        {
            mycc_putchar('2');
            mycc_putchar('$');
            mycc_putchar('\n');
        }
    }
    else if (n == 3)
    {
        if (m == 1)
        {
            mycc_putchar('3');
            mycc_putchar('1');
            mycc_putchar('\n');
        }
        else if (m == 2)
        {
            mycc_putchar('3');
            mycc_putchar('2');
            mycc_putchar('\n');
        }
        else if (m == 3)
        {
            mycc_putchar('3');
            mycc_putchar('3');
            mycc_putchar('\n');
        }
        else
        {
            mycc_putchar('3');
            mycc_putchar('$');
            mycc_putchar('\n');
        }
    }
    else
    {
        if (m == 1)
        {
            mycc_putchar('$');
            mycc_putchar('1');
            mycc_putchar('\n');
        }
        else if (m == 2)
        {
            mycc_putchar('$');
            mycc_putchar('2');
            mycc_putchar('\n');
        }
        else if (m == 3)
        {
            mycc_putchar('$');
            mycc_putchar('3');
            mycc_putchar('\n');
        }
        else
        {
            mycc_putchar('$');
            mycc_putchar('$');
            mycc_putchar('\n');
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