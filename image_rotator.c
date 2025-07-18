/* 이미지 RAW 데이터는 기본적으로 왼쪽 상단이 (0,0)이지만 PC마다 데이터의 시작하는 위치가 다른것 같다.
   왼쪽 상단이 데이터의 시작으로 생각하여 프로그램을 구성했지만 CW로 회전하니 CCW로 회전된 이미지가
   생성되었다. */
   
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <windows.h>
#include <time.h>

#define ORGINAL_IMG_PATH    "Original_Raw_IMG\\l0.img"    // 원본 이미지 파일 경로
#define OUTPUT_IMG_NAME_PATH "Rotation_Raw_IMG\\rotation_img_%Y%m%d_%H%M%S.img" // 출력 이미지 파일 파일명 및 경로

#define WIDTH  512
#define HEIGHT 512

#define UP_KEY      72
#define DOWN_KEY    80
#define ENTER_KEY   13
#define EXIT        3

void Photo_Rotation(int select);
unsigned char *Rotation(unsigned char *inImg, unsigned char *outImg, int num);
FILE *CheckFileOpen(FILE *file);    // file open check
int ShowOption(int *select);
void hideCursor(int cursor);

int main(void)
{
    int menuSelect = 0;
    hideCursor(0);
    SetConsoleOutputCP(CP_UTF8);    // 각도기호(°) 표시를 위해 콘솔창을 UTF-8로 설정. 
    while(1)
    {
        while(!ShowOption(&menuSelect)) {}
        if(menuSelect == EXIT)
            exit(1);
        Photo_Rotation(menuSelect);
    }
    return 0;
}

void Photo_Rotation(int select)
{
    FILE *orgImg_f = CheckFileOpen(fopen(ORGINAL_IMG_PATH, "rb"));
    unsigned char *orgImg = (unsigned char *)malloc(WIDTH * HEIGHT);
    unsigned char *outputImg = (unsigned char *)malloc(WIDTH * HEIGHT);
    char fileName[50];
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
 
    if(orgImg == NULL || outputImg == NULL)
    {
        printf("\n   Memory allocation failed. \n");
        getch();
        exit(1);
    }

    fread(orgImg, sizeof(unsigned char), WIDTH*HEIGHT, orgImg_f);   // 이미지 읽기
    fclose(orgImg_f);
    if(orgImg == NULL)
    {
        printf("\n   Failed to read file. \n");
        getch();
        exit(1);
    }
    outputImg = Rotation(orgImg, outputImg, select);    // 이미지 회전

    strftime(fileName, sizeof(fileName), OUTPUT_IMG_NAME_PATH, t);   // 현재시간을 문자로 변환
    
    FILE *outputImg_f = CheckFileOpen(fopen(fileName, "wb"));
    fwrite(outputImg, sizeof(unsigned char), WIDTH*HEIGHT, outputImg_f);    // 이미지 쓰기
    fclose(outputImg_f);

    printf("\n   Success  \n");
    getch();
}

unsigned char *Rotation(unsigned char *inImg, unsigned char *outImg, int num)
{
    for(int n=0 ; n<(num+1) ; n++)    // 90도 회전 횟수
    {
        for(int h=0 ; h<HEIGHT ; h++)
        {
            for(int w=0 ; w<WIDTH ; w++)
            {
                *(outImg+(h*WIDTH+w)) = *(inImg+(w*WIDTH+(WIDTH-h-1))); // CW
                // *(outImg+(h*WIDTH+w)) = *(inImg+(HEIGHT-w)*HEIGHT+h);   // CCW
            }
        }
        memcpy(inImg, outImg, HEIGHT * WIDTH);   // 90도 회전 후 저장
    }
    return outImg;
}

FILE *CheckFileOpen(FILE *file)
{
    if(file == NULL)
    {
        printf("\n   Failed to open file. \n");
        getch();
        return NULL;
    }
    else
        return file;
}

int ShowOption(int *select)
{
    char *menu[] = {
    "90°",
    "180°",
    "270°",
    "Exit"
    };
    int menuLen = sizeof(menu) / sizeof(menu[0]);
    int input;
    
    system("cls");
    
    for(int idx = 0 ; idx < menuLen ; idx++)    // 메뉴 출력
    {
        if(idx == *select)
            printf(" > %s \n", menu[idx]);
        else
            printf("   %s \n", menu[idx]);
    }
    
    input = getch();
    if(input == ENTER_KEY)  return 1;

    else if(input == 0 || input == 224)
    {
        input = getch();
        if(input == UP_KEY && *select > 0)
            (*select)--;
        if(input == DOWN_KEY && *select < 3)
            (*select)++;
        return 0;
    }

    else return 0;

}

void hideCursor(int cursor)
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = cursor;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}