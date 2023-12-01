#include "conio2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> //to potem usunąć
#include <time.h>

#define _CRT_SECURE_NO_WARNINGS

#define BOARDSIZEY 21
#define BOARDSIZEX 29
#define MAXPAWNS 15
#define MAXCHARLENGTH 16
#define FIELDTABLESIZE 24

#define WHITEHOMESTART 0
#define WHITEHOMEEND 5

#define REDHOMESTART 18
#define REDHOMEEND 23

#define CURSORCHAR '*'


struct Band
{
	int nOfWhitePawns;
	int nOfRedPawns;
};


struct Field
{
	int nOfPawns;
	int color;
};


struct Court
{
	int nOfWhitePawns;
	int nOfRedPawns;
};


struct Board
{
	Band band;
	Court court;
	Field fieldTable[FIELDTABLESIZE];
};


struct Player
{
	char name[MAXCHARLENGTH];
	int score;
};


struct Game
{
	Board board;
	int dice1Value;
	int dice2Value;
	Player* whitePlayer;
	Player* redPlayer;
	Player* whoseTurn;
};


void printRollResult(Game* game)
{
	gotoxy(1, 1);
	putch(game->dice1Value + '0');

	gotoxy(1, 2);
	putch(game->dice2Value + '0');
}


void rollDices(Game* game)
{
	game->dice1Value = (rand() % 6) + 1;
	game->dice2Value = (rand() % 6) + 1;

	printRollResult(game);
}


void rollStartPlayer(Game* game)
{
	rollDices(game);

	if (game->dice1Value > game->dice2Value)
	{
		game->whoseTurn = game->whitePlayer;
	}

	else if (game->dice2Value > game->dice1Value)
	{
		game->whoseTurn = game->redPlayer;
	}

	else
	{
		rollStartPlayer(game);
	}
}


void initPlayers(Game* game)
{
	game->whitePlayer = (Player*)malloc(sizeof(Player));
	game->redPlayer = (Player*)malloc(sizeof(Player));

	strcpy(game->whitePlayer->name, "Szymon");
	strcpy(game->redPlayer->name, "Alicja");
}


void initPawnPosition(Board* board)
{
	board->fieldTable[0].nOfPawns = 2;
	board->fieldTable[0].color = RED;

	board->fieldTable[5].nOfPawns = 5;
	board->fieldTable[5].color = WHITE;

	board->fieldTable[7].nOfPawns = 3;
	board->fieldTable[7].color = WHITE;

	board->fieldTable[11].nOfPawns = 5;
	board->fieldTable[11].color = RED;

	board->fieldTable[12].nOfPawns = 5;
	board->fieldTable[12].color = WHITE;

	board->fieldTable[16].nOfPawns = 3;
	board->fieldTable[16].color = RED;

	board->fieldTable[18].nOfPawns = 5;
	board->fieldTable[18].color = RED;

	board->fieldTable[23].nOfPawns = 2;
	board->fieldTable[23].color = WHITE;
}


void initEmptyBoard(Board* board)
{
	board->band.nOfWhitePawns = 0;
	board->band.nOfRedPawns = 0;
	
	board->court.nOfWhitePawns = 0;
	board->court.nOfRedPawns = 0;

	for (int i = 0; i < FIELDTABLESIZE; i++)
	{
		board->fieldTable[i].nOfPawns = 0;
	}
}


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


void initGame(Game* game)
{
	initEmptyBoard(&game->board);
	initPawnPosition(&game->board);
	initPlayers(game);
	rollStartPlayer(game);
}


Field identifyField()
{

}



int pickField(Game game)
{
	int cursorX = BOARDSIZEX / 2;
	int cursorY = BOARDSIZEY / 2;
	int textColor = CYAN;
	char charFromUser;

	gotoxy(cursorX, cursorY);
	textcolor(textColor);

	putch(CURSORCHAR);
	charFromUser = getch();

	while (charFromUser != 0x0d)
	{
		if (charFromUser == 0) {
			charFromUser = getch();
			if (charFromUser == 0x48) cursorY--;
			else if (charFromUser == 0x50) cursorY++;
			else if (charFromUser == 0x4b) cursorX--;
			else if (charFromUser == 0x4d) cursorX++;
		}

		textbackground(BLACK);
		clrscr();
		gotoxy(cursorX, cursorY);
		putch(CURSORCHAR);
		charFromUser = getch();
	}

	textbackground(BLACK);
	textcolor(RED);
	clrscr();
	gotoxy(cursorX, cursorY);
	putch(CURSORCHAR);
	putch(cursorX + '0');
	putch(cursorY + '0');

	return 0;
}


int main()
{
	Game game;
	srand(time(0));

	_setcursortype(_NOCURSOR);
	initGame(&game);
	pickField(game);

	_setcursortype(_NORMALCURSOR);	
	return 0;
}