/*
    贪吃蛇全局变量定义与初始化
*/

//地图长宽
int Width = 100;
int Height = 50;

//地图数组 第y行x列：Map[y * Width + x]
char Map[100 * 50];

//方向 0:左; 1:右; 2:上; 3:下
int dir = 0;

//蛇身位置数组
int snake_x[10000];
int snake_y[10000];

//蛇身长度
int snake_len = 4;

//程序退出标志
int exit = 0;

//蛇身增长标志
int grown = 0;