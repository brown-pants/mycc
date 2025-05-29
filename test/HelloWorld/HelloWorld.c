int arr[12];

void HelloWorld_1()
{
    int arr[12];
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
    int a;
    for(a = 0;a < 12;a = a + 1)
    {
        mycc_putchar(arr[a]);
    }
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
    int a;
    for(a = 0;a < 12;a = a + 1)
    {
        mycc_putchar(arr[a]);
    }
}

int main()
{
    HelloWorld_1();
    HelloWorld_2();
}