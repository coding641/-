#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <stdbool.h>

#define WIDTH 60
#define HEIGHT 20
#define INIT_LENGTH 5

// 方向枚举
enum DIRECTION { UP, DOWN, LEFT, RIGHT };

// 蛇身结构体
typedef struct {
    int x;
    int y;
} Point;

// 游戏全局变量
Point snake[WIDTH * HEIGHT]; // 蛇身数组
Point food;                  // 食物位置
int length;                  // 蛇长度
int score;                   // 当前分数
int highScore;               // 最高分
enum DIRECTION dir;          // 当前方向
int sleeptime = 200;         // 休眠时间(控制速度)
bool gameOver;               // 游戏结束标志

// 函数声明
void printsnake();
void welcometogame();
void createMap();
void scoreandtips();
void File_out();
void File_in();
void intsnake();
void createfood();
int biteself();
void cantcrosswall();
void speedup();
void speeddown();
void snakemove();
void keyboardControl();
void gameover();
void explation();

// 设置光标位置
void gotoxy(int x, int y) {
    COORD pos = { x, y };
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hOut, pos);
}

// 隐藏光标
void HideCursor() {
    CONSOLE_CURSOR_INFO cursor_info = { 1, 0 };
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}

// 设置颜色
void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// 打印蛇字符画
void printsnake() {
    setColor(10);
    gotoxy(35, 5); printf("/^\\/^\\");
    gotoxy(34, 6); printf("|_| O|");
    gotoxy(33, 7); printf("\\/   \\/");
    gotoxy(32, 8); printf(" |     |");
    gotoxy(31, 9); printf(" /     \\");
    gotoxy(30, 10); printf(" |     |");
    gotoxy(29, 11); printf(" \\     \\_");
    gotoxy(28, 12); printf(" /      \\/\\");
    gotoxy(27, 13); printf(" /         \\");
    gotoxy(26, 14); printf(" \\        /");
    gotoxy(25, 15); printf(" \\      /");
    gotoxy(24, 16); printf(" /     /");
    gotoxy(23, 17); printf(" /     /");
    gotoxy(22, 18); printf(" |    |");
    gotoxy(21, 19); printf(" |    |");
    gotoxy(20, 20); printf(" |    |");
    setColor(7);
}

// 欢迎界面
void welcometogame() {
    system("cls");
    printsnake();
    
    setColor(11);
    gotoxy(35, 22); printf("贪吃蛇游戏");
    setColor(14);
    gotoxy(33, 24); printf("1. 开始游戏");
    gotoxy(33, 25); printf("2. 游戏说明");
    gotoxy(33, 26); printf("3. 退出游戏");
    setColor(7);
    
    char ch = _getch();
    while (1) {
        if (ch == '1') {
            system("cls");
            createMap();
            File_in();
            intsnake();
            createfood();
            score = 0;
            scoreandtips();
            dir = RIGHT;
            gameOver = false;
            while (!gameOver) {
                snakemove();
                keyboardControl();
                Sleep(sleeptime);
            }
            gameover();
            break;
        } else if (ch == '2') {
            explation();
            break;
        } else if (ch == '3') {
            exit(0);
        } else {
            ch = _getch();
        }
    }
}

// 创建地图
void createMap() {
    int i, j;
    
    // 绘制上边界
    for (i = 0; i < WIDTH; i += 2) {
        gotoxy(i, 0);
        printf("■");
    }
    
    // 绘制下边界
    for (i = 0; i < WIDTH; i += 2) {
        gotoxy(i, HEIGHT);
        printf("■");
    }
    
    // 绘制左边界
    for (i = 1; i < HEIGHT; i++) {
        gotoxy(0, i);
        printf("■");
    }
    
    // 绘制右边界
    for (i = 1; i < HEIGHT; i++) {
        gotoxy(WIDTH - 2, i);
        printf("■");
    }
}

// 显示分数和提示
void scoreandtips() {
    gotoxy(WIDTH + 10, 4);
    printf("最高分: %d", highScore);
    gotoxy(WIDTH + 10, 6);
    printf("当前得分: %d", score);
    gotoxy(WIDTH + 10, 8);
    printf("方向键控制移动");
    gotoxy(WIDTH + 10, 10);
    printf("F1 加速");
    gotoxy(WIDTH + 10, 12);
    printf("F2 减速");
    gotoxy(WIDTH + 10, 14);
    printf("ESC 退出游戏");
}

// 从文件读取最高分
void File_in() {
    FILE *fp;
    if ((fp = fopen("save.txt", "r")) == NULL) {
        highScore = 0;
        return;
    }
    fscanf(fp, "%d", &highScore);
    fclose(fp);
}

// 保存最高分到文件
void File_out() {
    FILE *fp;
    if ((fp = fopen("save.txt", "w")) == NULL) {
        return;
    }
    fprintf(fp, "%d", highScore);
    fclose(fp);
}

// 初始化蛇
void intsnake() {
    length = INIT_LENGTH;
    
    // 初始化蛇身
    for (int i = 0; i < length; i++) {
        snake[i].x = WIDTH / 2 - i * 2;
        snake[i].y = HEIGHT / 2;
    }
    
    // 绘制蛇
    for (int i = 0; i < length; i++) {
        gotoxy(snake[i].x, snake[i].y);
        if (i == 0) {
                      setColor(14);
            printf("●"); // 蛇头
        } else {
            setColor(10);
            printf("★"); // 蛇身
        }
    }
    setColor(7);
}

// 创建食物
void createfood() {
    bool inSnake;
    srand((unsigned)time(NULL));
    
    do {
        inSnake = false;
        // 确保食物在地图内且不与蛇身重合
        food.x = (rand() % (WIDTH / 2 - 2) + 1) * 2;
        food.y = rand() % (HEIGHT - 1) + 1;
        
        for (int i = 0; i < length; i++) {
            if (snake[i].x == food.x && snake[i].y == food.y) {
                inSnake = true;
                break;
            }
        }
    } while (inSnake);
    
    setColor(12);
    gotoxy(food.x, food.y);
    printf("●");
    setColor(7);
}

// 判断是否咬到自己
int biteself() {
    for (int i = 1; i < length; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            return 1;
        }
    }
    return 0;
}

// 判断是否撞墙
void cantcrosswall() {
    if (snake[0].x <= 0 || snake[0].x >= WIDTH - 2 || 
        snake[0].y <= 0 || snake[0].y >= HEIGHT) {
        gameOver = true;
    }
}

// 加速
void speedup() {
    if (sleeptime >= 50) {
        sleeptime -= 30;
        score += 2;
    }
}

// 减速
void speeddown() {
    if (sleeptime <= 350) {
        sleeptime += 30;
        if (score > 2) score -= 2;
        else score = 1;
    }
}

// 蛇移动
void snakemove() {
    // 清除蛇尾
    gotoxy(snake[length - 1].x, snake[length - 1].y);
    printf("  ");
    
    // 蛇身移动
    for (int i = length - 1; i > 0; i--) {
        snake[i].x = snake[i - 1].x;
        snake[i].y = snake[i - 1].y;
    }
    
    // 蛇头移动
    switch (dir) {
        case UP:
            snake[0].y--;
            break;
        case DOWN:
            snake[0].y++;
            break;
        case LEFT:
            snake[0].x -= 2;
            break;
        case RIGHT:
            snake[0].x += 2;
            break;
    }
    
    // 绘制新蛇头
    gotoxy(snake[0].x, snake[0].y);
    setColor(14);
    printf("●");
    
    // 绘制蛇身
    for (int i = 1; i < length; i++) {
        gotoxy(snake[i].x, snake[i].y);
        setColor(10);
        printf("★");
    }
    setColor(7);
    
    // 检查是否吃到食物
    if (snake[0].x == food.x && snake[0].y == food.y) {
        length++;
        score += 5;
        createfood();
        scoreandtips();
    }
    
    // 检查游戏结束条件
    if (biteself() || snake[0].x <= 0 || snake[0].x >= WIDTH - 2 || 
        snake[0].y <= 0 || snake[0].y >= HEIGHT) {
        gameOver = true;
    }
}

// 键盘控制
void keyboardControl() {
    if (_kbhit()) {
        switch (_getch()) {
            case 'w':
            case 'W':
            case 72: // 上箭头
                if (dir != DOWN) dir = UP;
                break;
            case 's':
            case 'S':
            case 80: // 下箭头
                if (dir != UP) dir = DOWN;
                break;
            case 'a':
            case 'A':
            case 75: // 左箭头
                if (dir != RIGHT) dir = LEFT;
                break;
            case 'd':
            case 'D':
            case 77: // 右箭头
                if (dir != LEFT) dir = RIGHT;
                break;
            case 27: // ESC
                gameOver = true;
                break;
            case 59: // F1
                speedup();
                break;
            case 60: // F2
                speeddown();
                break;
        }
    }
}

// 游戏结束界面
void gameover() {
    system("cls");
    if (score > highScore) {
        highScore = score;
        File_out();
        gotoxy(30, 10);
        setColor(12);
        printf("创纪录啦！最高分数你刷新啦，真棒！！！");
    } else {
        gotoxy(30, 10);
        setColor(12);
        printf("继续努力吧～你离最高分还差 %d 分", highScore - score);
    }
    
    gotoxy(30, 12);
    printf("你的得分: %d", score);
    gotoxy(30, 14);
    printf("最高分: %d", highScore);
    
    setColor(14);
    gotoxy(30, 16);
    printf("1. 重新开始");
    gotoxy(30, 17);
    printf("2. 退出游戏");
    setColor(7);
    
    char ch = _getch();
    while (1) {
        if (ch == '1') {
            welcometogame();
            break;
        } else if (ch == '2') {
            exit(0);
        } else {
            ch = _getch();
        }
    }
}

// 游戏说明界面
void explation() {
    system("cls");
    setColor(11);
    gotoxy(30, 5);
    printf("游戏说明");
    setColor(14);
    gotoxy(10, 7);
    printf("1. 使用方向键或WASD控制蛇的移动");
    gotoxy(10, 9);
    printf("2. 吃到食物(红色)可以增加蛇的长度和得分");
    gotoxy(10, 11);
    printf("3. 撞到墙壁或自己的身体游戏结束");
    gotoxy(10, 13);
        printf("4. 按F1加速，F2减速");
    gotoxy(10, 15);
    printf("5. 按ESC键可以退出游戏");
    
    setColor(12);
    gotoxy(30, 18);
    printf("按任意键返回主菜单");
    setColor(7);
    
    _getch();
    welcometogame();
}

int main() {
    // 设置窗口大小
    system("mode con cols=100 lines=30");
    HideCursor();
    
    // 进入欢迎界面
    welcometogame();
    
    return 0;
}

