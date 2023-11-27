#include "conio2.h"
#include <stdio.h>

#define _CRT_SECURE_NO_WARNINGS

#define BOARDSIZEY 21
#define BOARDSIZEX 29
#define MAXPAWNS 16
#define N 64


struct Board
{
	int whiteInBase;
	int redInBase;
	int posX;
	int posY;
	int player;
};


struct Pawn
{
	int posX;
	int posY;
	Pawn* next;
	int color;
};


void drawBoardBoundaries()
{
	for (int j=1; j <= BOARDSIZEY; j++)
	{
		for (int i = 1; i <= BOARDSIZEX; i++)
		{
			gotoxy(i, j);

			if (i == 1 || i == BOARDSIZEX || i == BOARDSIZEX - 2 || i == BOARDSIZEX/2 - 1||
				i == BOARDSIZEX / 2 + 1)
			{
				cputs("|");
			}

			else if(j == BOARDSIZEY/2 + 1 || j == 1 || j == BOARDSIZEY)
			{
				cputs("-");
			}
		}
	}
}


void printPawns(Board &board)
{
	
}


int main()
{
	int zn = 0, x = 40, y = 12, attr = 7, back = 0, zero = 0;
	Board board;
	char txt[32];


	_setcursortype(_NOCURSOR);
	do {
		
		textbackground(BLACK);
		
		clrscr();

		drawBoardBoundaries();
		printPawns(board);

		gotoxy(x, y);
		textcolor(attr);
		textbackground(back);
		
		putch('*');

		zero = 0;
		zn = getch();
		
		if (zn == 0) {
			zero = 1;		
			zn = getch();		
			if (zn == 0x48) y--;	
			else if (zn == 0x50) y++;
			else if (zn == 0x4b) x--;
			else if (zn == 0x4d) x++;
		}
		else if (zn == ' ') attr = (attr + 1) % 16;

		else if (zn == 0x0d)
		{
			
		}

	} while (zn != 'q');

	_setcursortype(_NORMALCURSOR);	
	return 0;
}