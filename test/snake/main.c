extern void putchar(char ch);
extern void system(char *cmd);
extern void usleep(int usec);
extern int kbhit();
extern int getch();
extern int rand();
extern int srand(int seed);
extern int time(int *seconds);

extern int Width;
extern int Height;
extern char Map[100 * 50];
extern int dir;
extern int snake_x[10000];
extern int snake_y[10000];
extern int snake_len;
extern int exit;
extern int grown;

void new_food()
{
    int x = (rand() + 1) % (Width - 1);
    int y = (rand() + 1) % (Height - 1);
    if (Map[y * Width + x] == ' ')
    {
        Map[y * Width + x] = '*';
    }
    else
    {
        new_food();
    }
}

void init()
{
    int i;
    int j;
    for (i = 0; i < snake_len; i = i + 1)
    {
        snake_x[i] = Width / 2 + i;
        snake_y[i] = Height / 2;
    }
    for (i = 0; i < Height; i = i + 1)
    {
        for (j = 0; j < Width; j = j + 1)
        {
            char *p = &Map[i * Width + j];
            if (j == 0 || j == Width - 1 || i == 0 || i == Height - 1)
            {
                *p = '#';
            }
            else *p = ' ';
        }
    }
    for (i = 0; i < snake_len; i = i + 1)
    {
        int x = snake_x[i];
        int y = snake_y[i];
        if (i == 0)
        {
            Map[y * Width + x] = '@';
        }
        else Map[y * Width + x] = '+';
    }
    srand(time(0));
    new_food();
}

void draw()
{
    system("TERM=xterm clear");
    int i;
    int j;

    for (i = 0; i < Height; i = i + 1)
    {
        for (j = 0; j < Width; j = j + 1)
        {
            char *p = &Map[i * Width + j];
            putchar(*p);
        }
        putchar('\n');
    }
}

void update()
{
    int i;
    int tail_x = snake_x[snake_len - 1];
    int tail_y = snake_y[snake_len - 1];
    int head_x = snake_x[0];
    int head_y = snake_y[0];

    if (grown)
    {
        grown = 0;
        snake_len = snake_len + 1;
    }
    else
    {
        Map[tail_y * Width + tail_x] = ' ';
    }
    Map[head_y * Width + head_x] = '+';

    for (i = snake_len - 1; i > 0; i = i - 1)
    {
        snake_x[i] = snake_x[i - 1];
        snake_y[i] = snake_y[i - 1];
    }

    if (dir == 0)
    {
        snake_x[0] = snake_x[0] - 1;
    }
    else if (dir == 1)
    {
        snake_x[0] = snake_x[0] + 1;
    }
    else if (dir == 2)
    {
        snake_y[0] = snake_y[0] - 1;
    }
    else
    {
        snake_y[0] = snake_y[0] + 1;
    }

    head_x = snake_x[0];
    head_y = snake_y[0];

    char *p = &Map[head_y * Width + head_x];

    if (*p == '#' || *p == '+')
    {
        exit = 1;
        return;
    }
    else if (*p == '*')
    {
        grown = 1;
        new_food();
    }
    *p = '@';

    if (kbhit())
    {
        char input = getch();
        if (input == 'a' && dir != 1) dir = 0;
        else  if (input == 'd' && dir != 0) dir = 1;
        else  if (input == 'w' && dir != 3) dir = 2;
        else  if (input == 's' && dir != 2) dir = 3;
    }
}

int main()
{
    init();
    while(1)
    {
        if (exit)
        {
            break;
        }
        draw();
        update();
        usleep(100000);
    }
    return 0;
}