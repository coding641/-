#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <stdbool.h>

#define WIDTH 60
#define HEIGHT 20
#define INIT_LENGTH 5

// ����ö��
enum DIRECTION { UP, DOWN, LEFT, RIGHT };

// ����ṹ��
typedef struct {
    int x;
    int y;
} Point;

// ��Ϸȫ�ֱ���
Point snake[WIDTH * HEIGHT]; // ��������
Point food;                  // ʳ��λ��
int length;                  // �߳���
int score;                   // ��ǰ����
int highScore;               // ��߷�
enum DIRECTION dir;          // ��ǰ����
int sleeptime = 200;         // ����ʱ��(�����ٶ�)
bool gameOver;               // ��Ϸ������־

// ��������
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

// ���ù��λ��
void gotoxy(int x, int y) {
    COORD pos = { x, y };
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleCursorPosition(hOut, pos);
}

// ���ع��
void HideCursor() {
    CONSOLE_CURSOR_INFO cursor_info = { 1, 0 };
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}

// ������ɫ
void setColor(int color) {
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

// ��ӡ���ַ���
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

// ��ӭ����
void welcometogame() {
    system("cls");
    printsnake();
    
    setColor(11);
    gotoxy(35, 22); printf("̰������Ϸ");
    setColor(14);
    gotoxy(33, 24); printf("1. ��ʼ��Ϸ");
    gotoxy(33, 25); printf("2. ��Ϸ˵��");
    gotoxy(33, 26); printf("3. �˳���Ϸ");
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

// ������ͼ
void createMap() {
    int i, j;
    
    // �����ϱ߽�
    for (i = 0; i < WIDTH; i += 2) {
        gotoxy(i, 0);
        printf("��");
    }
    
    // �����±߽�
    for (i = 0; i < WIDTH; i += 2) {
        gotoxy(i, HEIGHT);
        printf("��");
    }
    
    // ������߽�
    for (i = 1; i < HEIGHT; i++) {
        gotoxy(0, i);
        printf("��");
    }
    
    // �����ұ߽�
    for (i = 1; i < HEIGHT; i++) {
        gotoxy(WIDTH - 2, i);
        printf("��");
    }
}

// ��ʾ��������ʾ
void scoreandtips() {
    gotoxy(WIDTH + 10, 4);
    printf("��߷�: %d", highScore);
    gotoxy(WIDTH + 10, 6);
    printf("��ǰ�÷�: %d", score);
    gotoxy(WIDTH + 10, 8);
    printf("����������ƶ�");
    gotoxy(WIDTH + 10, 10);
    printf("F1 ����");
    gotoxy(WIDTH + 10, 12);
    printf("F2 ����");
    gotoxy(WIDTH + 10, 14);
    printf("ESC �˳���Ϸ");
}

// ���ļ���ȡ��߷�
void File_in() {
    FILE *fp;
    if ((fp = fopen("save.txt", "r")) == NULL) {
        highScore = 0;
        return;
    }
    fscanf(fp, "%d", &highScore);
    fclose(fp);
}

// ������߷ֵ��ļ�
void File_out() {
    FILE *fp;
    if ((fp = fopen("save.txt", "w")) == NULL) {
        return;
    }
    fprintf(fp, "%d", highScore);
    fclose(fp);
}

// ��ʼ����
void intsnake() {
    length = INIT_LENGTH;
    
    // ��ʼ������
    for (int i = 0; i < length; i++) {
        snake[i].x = WIDTH / 2 - i * 2;
        snake[i].y = HEIGHT / 2;
    }
    
    // ������
    for (int i = 0; i < length; i++) {
        gotoxy(snake[i].x, snake[i].y);
        if (i == 0) {
                      setColor(14);
            printf("��"); // ��ͷ
        } else {
            setColor(10);
            printf("��"); // ����
        }
    }
    setColor(7);
}

// ����ʳ��
void createfood() {
    bool inSnake;
    srand((unsigned)time(NULL));
    
    do {
        inSnake = false;
        // ȷ��ʳ���ڵ�ͼ���Ҳ��������غ�
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
    printf("��");
    setColor(7);
}

// �ж��Ƿ�ҧ���Լ�
int biteself() {
    for (int i = 1; i < length; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
            return 1;
        }
    }
    return 0;
}

// �ж��Ƿ�ײǽ
void cantcrosswall() {
    if (snake[0].x <= 0 || snake[0].x >= WIDTH - 2 || 
        snake[0].y <= 0 || snake[0].y >= HEIGHT) {
        gameOver = true;
    }
}

// ����
void speedup() {
    if (sleeptime >= 50) {
        sleeptime -= 30;
        score += 2;
    }
}

// ����
void speeddown() {
    if (sleeptime <= 350) {
        sleeptime += 30;
        if (score > 2) score -= 2;
        else score = 1;
    }
}

// ���ƶ�
void snakemove() {
    // �����β
    gotoxy(snake[length - 1].x, snake[length - 1].y);
    printf("  ");
    
    // �����ƶ�
    for (int i = length - 1; i > 0; i--) {
        snake[i].x = snake[i - 1].x;
        snake[i].y = snake[i - 1].y;
    }
    
    // ��ͷ�ƶ�
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
    
    // ��������ͷ
    gotoxy(snake[0].x, snake[0].y);
    setColor(14);
    printf("��");
    
    // ��������
    for (int i = 1; i < length; i++) {
        gotoxy(snake[i].x, snake[i].y);
        setColor(10);
        printf("��");
    }
    setColor(7);
    
    // ����Ƿ�Ե�ʳ��
    if (snake[0].x == food.x && snake[0].y == food.y) {
        length++;
        score += 5;
        createfood();
        scoreandtips();
    }
    
    // �����Ϸ��������
    if (biteself() || snake[0].x <= 0 || snake[0].x >= WIDTH - 2 || 
        snake[0].y <= 0 || snake[0].y >= HEIGHT) {
        gameOver = true;
    }
}

// ���̿���
void keyboardControl() {
    if (_kbhit()) {
        switch (_getch()) {
            case 'w':
            case 'W':
            case 72: // �ϼ�ͷ
                if (dir != DOWN) dir = UP;
                break;
            case 's':
            case 'S':
            case 80: // �¼�ͷ
                if (dir != UP) dir = DOWN;
                break;
            case 'a':
            case 'A':
            case 75: // ���ͷ
                if (dir != RIGHT) dir = LEFT;
                break;
            case 'd':
            case 'D':
            case 77: // �Ҽ�ͷ
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

// ��Ϸ��������
void gameover() {
    system("cls");
    if (score > highScore) {
        highScore = score;
        File_out();
        gotoxy(30, 10);
        setColor(12);
        printf("����¼������߷�����ˢ���������������");
    } else {
        gotoxy(30, 10);
        setColor(12);
        printf("����Ŭ���ɡ�������߷ֻ��� %d ��", highScore - score);
    }
    
    gotoxy(30, 12);
    printf("��ĵ÷�: %d", score);
    gotoxy(30, 14);
    printf("��߷�: %d", highScore);
    
    setColor(14);
    gotoxy(30, 16);
    printf("1. ���¿�ʼ");
    gotoxy(30, 17);
    printf("2. �˳���Ϸ");
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

// ��Ϸ˵������
void explation() {
    system("cls");
    setColor(11);
    gotoxy(30, 5);
    printf("��Ϸ˵��");
    setColor(14);
    gotoxy(10, 7);
    printf("1. ʹ�÷������WASD�����ߵ��ƶ�");
    gotoxy(10, 9);
    printf("2. �Ե�ʳ��(��ɫ)���������ߵĳ��Ⱥ͵÷�");
    gotoxy(10, 11);
    printf("3. ײ��ǽ�ڻ��Լ���������Ϸ����");
    gotoxy(10, 13);
        printf("4. ��F1���٣�F2����");
    gotoxy(10, 15);
    printf("5. ��ESC�������˳���Ϸ");
    
    setColor(12);
    gotoxy(30, 18);
    printf("��������������˵�");
    setColor(7);
    
    _getch();
    welcometogame();
}

int main() {
    // ���ô��ڴ�С
    system("mode con cols=100 lines=30");
    HideCursor();
    
    // ���뻶ӭ����
    welcometogame();
    
    return 0;
}

