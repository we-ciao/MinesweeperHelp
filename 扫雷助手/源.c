#include <stdio.h>
#include <Windows.h>

int height = 0;
int width = 0;
int reheight = 9;
int rewidth = 9;
int windowsH = 30;
int windowsL = 15;
int selmenu_Num = 0;
unsigned char chess[24][32];
char shell_cmd[20];
RECT WindowXy;
HWND thishwnd;
HWND hwnd = 0;
DWORD processid;
HANDLE hProcess = 0;
WORD yx[2];

void gotoxy(int x,int y) //移动光标位置
{
	COORD xy;
	HANDLE hstdout;
	hstdout = GetStdHandle(STD_OUTPUT_HANDLE);
	xy.X = x;
	xy.Y = y;
	SetConsoleCursorPosition(hstdout, xy);
}

void SetPos()
{
	GetWindowThreadProcessId(hwnd, &processid);
	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processid);
	GetWindowRect(hwnd, &WindowXy); // getwindowrect()获得整个窗口的范围矩形，窗口的边框、标题栏、滚动条及菜单等都在这个矩形内 
	thishwnd = FindWindow(NULL, L"扫雷助手");
	//MoveWindow(thishwnd, WindowXy.right-12, WindowXy.top, 260, WindowXy.bottom- WindowXy.top, FALSE);//MoveWindow()改变指定窗口的位置和大小 
	SetWindowPos(-1, HWND_TOPMOST, WindowXy.right - 12, WindowXy.top, 260, 1000, SWP_SHOWWINDOW);//	SetWindowPos 为窗口指定一个新位置和状态 

}

void getChess()
{
	ReadProcessMemory(hProcess, (LPVOID)0x010056A8,&height,4, NULL);
	//if (height > reheight) {
	//	windowsL += (height - reheight);
	//	reheight = height;
	//}
	ReadProcessMemory(hProcess, (LPVOID)0x010056AC, &width, 4, NULL);
	//if (width > rewidth)
	//{
	//	windowsH += (width - rewidth) * 3;
	//	rewidth = width;
	//}
	//sprintf(shell_cmd, "mode con cols=%d lines=%d", windowsH, windowsL);
	//system(shell_cmd);
	ReadProcessMemory(hProcess, (LPVOID)0x01005361, chess, 32*24, NULL);
	//gotoxy(2,4);
	//for (int x = 0; x <height;x++)
	//{
	//	for (int y = 0; y < 32; y++)
	//	{
	//		if (chess[x][y]==0x10)
	//		{
	//			break;
	//		}
	//		printf("%02X ", chess[x][y]);
	//	}
	//	printf("\n  ");
	//}
	//printf("0F=空白 8F=炸弹");
}

void changeChess()
{
	ReadProcessMemory(hProcess, (LPVOID)0x01005361, chess, 32 * 24, NULL);
	for (int y = 0, i = 0; y < height || i < 30; y++, i++) {
		for (int x = 0; x < 32; x++)
		{
			if (chess[y][x] == 0x10)
			{
				break;
			}
			if (chess[y][x] == 0X0F || chess[y][x] == 0x8E)
				chess[y][x] = 0x8F;
		}
	}
	WriteProcessMemory(hProcess, (LPVOID)0x01005361, chess, 32 * 24, NULL);
}

int selmenu(void)
{ //0,1,2
  //如果按下↓selmenu_Num++; 如果>2  selmenu_Num=0;
  //如果按下↑selmenu_Num--; 如果<0  selmenu_Num=2;
	int ch = 0;
	while (1)
	{
		ch = getch();
		//gotoxy(0,0);
		//printf("%d ",ch);
		if (ch == 13)
		{
			return selmenu_Num;
		}
		switch (ch)
		{
		case 72://上光标
			selmenu_Num--;
			if (selmenu_Num<0)
			{
				selmenu_Num = 2;
			}
			break;
		case 80: //下光标
			selmenu_Num++;
			if (selmenu_Num>2)
			{
				selmenu_Num = 0;
			}
			break;
		}
		gotoxy(0, 0);
		// printf("selmenu_num=%d",selmenu_Num);
		//动态菜单
		switch (selmenu_Num)
		{
		case 0:
			gotoxy(windowsL - 9, windowsH - 26);
			printf("1->");
			gotoxy(windowsL - 9, windowsH - 24);
			printf("   ");
			gotoxy(windowsL - 9, windowsH - 22);
			printf("   ");
			gotoxy(0, 0);
			break;
		case 1:
			gotoxy(windowsL - 9, windowsH - 26);
			printf("   ");
			gotoxy(windowsL - 9, windowsH - 24);
			printf("2->");
			gotoxy(windowsL - 9, windowsH - 22);
			printf("   ");
			gotoxy(0, 0);
			break;
		case 2:
			gotoxy(windowsL - 9, windowsH - 26);
			printf("   ");
			gotoxy(windowsL - 9, windowsH - 24);
			printf("   ");
			gotoxy(windowsL - 9, windowsH - 22);
			printf("3->");
			gotoxy(0, 0);
			break;
		}
	} //end while

	return selmenu_Num;
}

void breakdownWhite(int x)
{
	if (x==0)
		for (int y = 0; y < height; y++)
			for (int x = 0; x < 32; x++)
			{
				if (chess[y][x] == 0x10)
				{
					break;
				}
				if (chess[y][x] == 0x8f)
				{
					continue;
				}
				yx[0] = 11 + 6 + x * 16;
				yx[1] = 54 + 6 + y * 16;
				SendMessage(hwnd, WM_LBUTTONDOWN, 1, *(int*)yx);
				SendMessage(hwnd, WM_LBUTTONUP, 0, *(int*)yx);
			}
	else
	for (int y = 0; y < height; y++)
		for (int x = 0; x < 32; x++)
		{
			if (chess[y][x] == 0x10)
			{
				break;
			}
			if (chess[y][x] == 0x8f)
			{
				continue;
			}
			yx[0] = 11 + 6 + x * 16;
			yx[1] = 54 + 6 + y * 16;
			SendMessage(hwnd, WM_LBUTTONDOWN, 1, *(int*)yx);
			SendMessage(hwnd, WM_LBUTTONUP, 0, *(int*)yx);
			Sleep(x);
		}
}

void breakdownBlack()
{
	for (int y = 0,i=0; y < height||i<30; y++,i++) {
		for (int x = 0; x < 32; x++)
		{
			if (chess[y][x] == 0x10)
			{
				break;
			}
			if (chess[y][x] == 0x8f)
			{
				yx[0] = 11 + 6 + x * 16;
				yx[1] = 54 + 6 + y * 16;
				SendMessage(hwnd, WM_RBUTTONDOWN, 0, *(int*)yx);
				SendMessage(hwnd, WM_RBUTTONUP, 1, *(int*)yx);
			}
		}
		gotoxy(y, 11);
		printf(">");
		Sleep(20);
	}
}

void main()
{
begin:
	system("title=扫雷助手");
	system("mode con cols=30 lines=15");
	gotoxy(9, 6);
	printf("等待扫雷运行");
	do
	{
		hwnd = FindWindow(NULL, L"扫雷");
		Sleep(1000);
	}while (!hwnd);
	SetPos();
	gotoxy(9, 6);
	system("color 2f");
	printf("检测到扫雷运行\n");
	gotoxy(20, 10);
	Sleep(300);
	printf("解密Loding\n");
	gotoxy(0, 11);
	printf("==============================");
	getChess();
	breakdownBlack();
	system("cls");
	system("color 3f");
	gotoxy(windowsL-9, windowsH-26);
	printf("1->  一键扫雷");
	gotoxy(windowsL - 9, windowsH - 24);
	printf("     延时扫雷");
	gotoxy(windowsL - 9, windowsH - 22);
	printf("     作死选项");
	switch (selmenu())
	{
	case 0:
		getChess();
		breakdownWhite(0);
		break;
	case 1:
		getChess();
		breakdownWhite(50);
		break;
	case 2:
		changeChess();
		break;
	default:
		break;
	} 
	selmenu_Num = 0;
	system("cls");
	system("color 2f");
	gotoxy(9, 6);
	printf("->重开一局");
	while (1)
	{
		int ch = getch();
		if (ch == 13)
		goto begin;
	}
}