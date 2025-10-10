char arr[13];

void puts(char *s)
{
    while(*s != '\0')
    {
        mycc_putchar(*s);
        s = s + 1;
    }
}

void HelloWorld_1()
{
    char arr[13];
    arr[0] = 'H';
    arr[1] = 'e';
    arr[2] = 'l';
    arr[3] = 'l';
    arr[4] = 'o';
    arr[5] = ' ';
    arr[6] = 'W';
    arr[7] = 'o';
    arr[8] = 'r';
    arr[9] = 'l';
    arr[10] = 'd';
    arr[11] = '\n';
    arr[12] = '\0';
    puts(arr);
}

void HelloWorld_2()
{
    arr[0] = 'H';
    arr[1] = 'e';
    arr[2] = 'l';
    arr[3] = 'l';
    arr[4] = 'o';
    arr[5] = ' ';
    arr[6] = 'W';
    arr[7] = 'o';
    arr[8] = 'r';
    arr[9] = 'l';
    arr[10] = 'd';
    arr[11] = '\n';
    arr[12] = '\0';
    puts(arr);
}

char *S = "Hello World\n";

int main()
{
    HelloWorld_1();
    HelloWorld_2();
    puts("Hello World\n");
    char *s = "Hello World\n";
    puts(S);
    puts(s);
}