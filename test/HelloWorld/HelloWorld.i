# 1 "../test/HelloWorld/HelloWorld.c"
char arr[12];

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
# 4 "../test/HelloWorld/HelloWorld.c"

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
    arr[11] = '\0';
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
    arr[11] = '\0';
    puts(arr);
}

char *S = "Hello World";

int main()
{
    HelloWorld_1();
    HelloWorld_2();
    puts("Hello World");
    char *s = "Hello World";
    puts(S);
    puts(s);
}
