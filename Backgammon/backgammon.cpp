#include "conio2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> //to potem usunąć
#include <time.h>

#define _CRT_SECURE_NO_WARNINGS

#define BOTHORIZONTALBORDER 21
#define RIGHTVERTICALBORDER 29
#define MAXPAWNS 15
#define MAXCHARLENGTH 16
#define FIELDTABLESIZE 24
#define BANDX 14

#define TOPHORIZONTALBORDER 1
#define LEFTVERTIVALBORDER 1
#define HORIZONTALMIDDLE 11

// indexes of fields
#define WHITEHOMESTART 0
#define WHITEHOMEEND 5
#define REDHOMESTART 18
#define REDHOMEEND 23

#define CURSORCHAR '*'
#define PAWNCHAR 'o'


struct Band
{
	int nOfWhitePawns;
	int nOfRedPawns;
};


struct Field
{
	int nOfPawns;
	int color;
	int positionX;
	int firstPositionY;
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

	//printRollResult(game);
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

	strcpy(game->whitePlayer->name, "Szymon"); //napisać własne strcpy
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


	for (int i = 0; i < FIELDTABLESIZE; i++)
	{
		if (board->fieldTable[i].nOfPawns == 0)
		{
			board->fieldTable[i].nOfPawns = 1;
			board->fieldTable[i].color = WHITE;
		}
	}
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
	for (int j = LEFTVERTIVALBORDER; j <= BOTHORIZONTALBORDER; j++)
	{
		for (int i = TOPHORIZONTALBORDER; i <= RIGHTVERTICALBORDER; i++)
		{
			gotoxy(i, j);

			if (i == TOPHORIZONTALBORDER || i == RIGHTVERTICALBORDER || i == RIGHTVERTICALBORDER - 2 || i == BANDX - 1||
				i == BANDX + 1)
			{
				cputs("|");
			}

			else if(j == HORIZONTALMIDDLE || j == LEFTVERTIVALBORDER || j == BOTHORIZONTALBORDER)
			{
				cputs("-");
			}
		}
	}
}


//na podstawie x znajduje field
Field* identifyFieldByX(Game* game, int positionX)
{
	for (int fieldIndex = 0; fieldIndex < FIELDTABLESIZE; fieldIndex++)
	{
		if (game->board.fieldTable[fieldIndex].positionX == positionX)
		{
			return &game->board.fieldTable[fieldIndex];
		}
	}

	return nullptr;
}


void initFields(Game* game)
{
	// starting positions of field's x in particular quadrants
	int positionXRightBot = RIGHTVERTICALBORDER - 3;
	int positionXLeftBot = BANDX - 2;
	int positionXLeftTop = LEFTVERTIVALBORDER + 1;
	int positionXRightTop = BANDX + 2;


	for (int fieldIndex = 0; fieldIndex < FIELDTABLESIZE; fieldIndex++)
	{
		Field* currentField = &game->board.fieldTable[fieldIndex]; //czy to moze byc definiowane w tym miejscu?

		//prawy-dolny
		if (fieldIndex < 6) //ostatni patyk w prawym-dolnym ma indeks 6
		{
			currentField->positionX = positionXRightBot;
			positionXRightBot -= 2; //idziemy o dwa w lewo 
			currentField->firstPositionY = BOTHORIZONTALBORDER - 1;
		}

		//lewy-dolny
		else if (fieldIndex >= 6 && fieldIndex < 12)
		{
			currentField->positionX = positionXLeftBot;
			positionXLeftBot -= 2;
			currentField->firstPositionY = BOTHORIZONTALBORDER - 1;
		}

		//lewy-górny
		else if (fieldIndex >= 12 && fieldIndex < 18)
		{
			currentField->positionX = positionXLeftTop;
			positionXLeftTop += 2;
			currentField->firstPositionY = TOPHORIZONTALBORDER + 1;
		}

		//prawy górny
		else if (fieldIndex >= 18)
		{
			currentField->positionX = positionXRightTop;
			positionXRightTop += 2;
			currentField->firstPositionY = TOPHORIZONTALBORDER + 1;
		}

		//printf("Index: %d - x: %d, y: %d\n",fieldIndex, game->board.fieldTable[fieldIndex].positionX, game->board.fieldTable[fieldIndex].firstPositionY);
	}
}


//funkcja sprawdza czy ruch nie wychodzi poza granice planszy
int ifMoveOnBoard(int currentX, int currentY)
{
	if (currentX + 1 > RIGHTVERTICALBORDER || currentX - 1 < LEFTVERTIVALBORDER)
	{
		return 0;
	}

	if (currentY + 1 > BOTHORIZONTALBORDER || currentY - 1 < TOPHORIZONTALBORDER)
	{
		return 0;
	}

	return 1;
}


//funkcja stworzona by nie powielać kodu
void drawSinglePawn(Game* game, int currentX, int currentY, int playerColor)
{
	//printf("x: %d, y: %d \n", currentX, currentY);
	gotoxy(currentX, currentY);
	textcolor(playerColor);
	putch(PAWNCHAR);
}


void drawPawnsOnFields(Game* game)
{
	//printf("%d", game->board.fieldTable[0].color);

	for (int fieldIndex = 0; fieldIndex < FIELDTABLESIZE; fieldIndex++)
	{
		Field* currentField = &game->board.fieldTable[fieldIndex];

		//printf("Index: %d - starting y: %d \n", fieldIndex, currentField.firstPositionY);

		for (int pawnIndex = 0; pawnIndex < currentField->nOfPawns; pawnIndex++)
		{
			if (currentField->firstPositionY == TOPHORIZONTALBORDER + 1)
			{
				drawSinglePawn(game, currentField->positionX, currentField->firstPositionY + pawnIndex, currentField->color);
			}

			else if (currentField->firstPositionY == BOTHORIZONTALBORDER - 1)
			{
				drawSinglePawn(game, currentField->positionX, currentField->firstPositionY - pawnIndex, currentField->color);
			}

			else
			{
				printf("blad startowego y %d", currentField->firstPositionY);
			}
		}
	}
}


void printBoard(Game* game)
{
	drawBoardBoundaries();
	drawPawnsOnFields(game);
}


int pickField(Game game)
{
	int cursorX = RIGHTVERTICALBORDER / 2;
	int cursorY = BOTHORIZONTALBORDER / 2;
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


void initGame(Game* game)
{
	initEmptyBoard(&game->board);
	initFields(game);
	initPawnPosition(&game->board);
	initPlayers(game);
	rollStartPlayer(game);
}


int main()
{
	Game game;
	initGame(&game);
	printBoard(&game);

	srand(time(0));

	_setcursortype(_NOCURSOR);
	_setcursortype(_NORMALCURSOR);	
	return 0;
}

//TODO: w funkcji initPawnPosition() są pozycje startowe pionków na danych polach, ale nie znamy współrzędnych tych pól.
//TODO: czy współrzędna x dla pól powinna być z góry zdefiniowana?