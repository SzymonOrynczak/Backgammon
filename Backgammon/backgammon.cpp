#include "conio2.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h> //to potem usunąć
#include <time.h>

#define _CRT_SECURE_NO_WARNINGS

#define MAXPAWNS 15
#define MAXCHARLENGTH 16
#define FIELDTABLESIZE 24

// borders of a board
#define TOPHORIZONTALBORDER 1
#define LEFTVERTIVALBORDER 1
#define HORIZONTALMIDDLE 11
#define BANDX 14
#define BOTHORIZONTALBORDER 21
#define RIGHTVERTICALBORDER 29

// indexes of fields
#define WHITEHOMESTART 0
#define WHITEHOMEEND 5
#define REDHOMESTART 18
#define REDHOMEEND 23

// symbols
#define CURSORCHAR '*'
#define PAWNCHAR 'o'


enum CursorState
{
	PICK_PAWN,
	PLACE_PAWN
};


enum MoveDirection
{
	LEFT,
	RIGHT,
	UP,
	DOWN,
	NONE
};


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
	int index;
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
	int cursorPositionX;
	int cursorPositionY;
	Player* whitePlayer;
	Player* redPlayer;
	Player* whoseTurn;
};


struct PlayerMove
{
	int startFieldIndex;
	int endFieldIndex;
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
		board->fieldTable[i].color = -1;
	}
}


void drawBoardBoundaries(Board* board)
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

	for (int i = 0; i < FIELDTABLESIZE; i++)
	{
		if (board->fieldTable[i].nOfPawns == 0)
		{
			gotoxy(board->fieldTable[i].positionX, board->fieldTable[i].firstPositionY);
			putch('|');
		}
	}
}


Field* identifyFieldByCursorPosition(Game* game)
{
	// górne pola
	if (game->cursorPositionY < HORIZONTALMIDDLE)
	{
		for (int fieldIndex = FIELDTABLESIZE/2; fieldIndex < FIELDTABLESIZE; fieldIndex++)
		{
			if (game->cursorPositionX == game->board.fieldTable[fieldIndex].positionX)
			{
				return &game->board.fieldTable[fieldIndex];
			}
		}
	}

	// dolne pola
	else
	{
		for (int fieldIndex = 0; fieldIndex < FIELDTABLESIZE/2; fieldIndex++)
		{
			if (game->cursorPositionX == game->board.fieldTable[fieldIndex].positionX)
			{
				return &game->board.fieldTable[fieldIndex];
			}
		}
	}

	return NULL;
}


void initFields(Field fieldTable[FIELDTABLESIZE])
{
	// starting positions of field's x in particular quadrants
	int positionXRightBot = RIGHTVERTICALBORDER - 3;
	int positionXLeftBot = BANDX - 2;
	int positionXLeftTop = LEFTVERTIVALBORDER + 1;
	int positionXRightTop = BANDX + 2;


	for (int fieldIndex = 0; fieldIndex < FIELDTABLESIZE; fieldIndex++)
	{
		Field* currentField = &fieldTable[fieldIndex];

		//prawy-dolny
		if (fieldIndex < 6) //ostatni patyk w prawym-dolnym ma indeks 6
		{
			currentField->positionX = positionXRightBot;
			positionXRightBot -= 2;
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

		currentField->index = fieldIndex;
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
	drawBoardBoundaries(&game->board);
	drawPawnsOnFields(game);
}


// ta funkcja nie wiem po co jest i piszę jakby jej nie było - nie jest używana
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
	initFields(game->board.fieldTable);
	initPawnPosition(&game->board);
	initPlayers(game);
	rollStartPlayer(game);
}


void moveLeftOnBottom(Game* game, Field* currentField, int cursorPivot)
{
	game->cursorPositionX = game->board.fieldTable[currentField->index + 1].positionX;

	// cursorPositionX przed ifem ma już pocyzję następnego pola
	if (identifyFieldByCursorPosition(game)->nOfPawns == 0)
	{
		game->cursorPositionY = game->board.fieldTable[currentField->index + 1].firstPositionY;
	}

	else
	{
		game->cursorPositionY = game->board.fieldTable[currentField->index + 1].firstPositionY - game->board.fieldTable[currentField->index + 1].nOfPawns + cursorPivot;
	}
}


void moveLeft(Game* game, Field* currentField, int cursorPivot)
{
	// dolne pola
	if (currentField->firstPositionY == BOTHORIZONTALBORDER - 1)
	{
		moveLeftOnBottom(game, currentField, cursorPivot);
	}

	// górne pola
	else if (currentField->firstPositionY == TOPHORIZONTALBORDER + 1)
	{
		game->cursorPositionX = game->board.fieldTable[currentField->index - 1].positionX;

		if (identifyFieldByCursorPosition(game)->nOfPawns == 0)
		{
			game->cursorPositionY = game->board.fieldTable[currentField->index - 1].firstPositionY;
		}

		else
		{
			game->cursorPositionY = game->board.fieldTable[currentField->index - 1].firstPositionY + game->board.fieldTable[currentField->index - 1].nOfPawns - cursorPivot;
		}
	}
}


// realizuje chodzenie po planszych po patykach
void moveCursor(Game* game, MoveDirection moveDirection)
{
	CursorState cursorState = PICK_PAWN;
	Field* currentField = identifyFieldByCursorPosition(game);
	int cursorPivot = 0; // zapewnia odpowiednie przesunięcie kursora w PICKPAWN i PLACEPAWN

	// zaimplementować cursorPivot zamiast - 1 lub + 1
	if (cursorState == PICK_PAWN)
	{
		cursorPivot = 1;
	}

	else if (cursorState == PLACE_PAWN)
	{
		cursorPivot = 0;
	}


	if (moveDirection == LEFT)
	{
		moveLeft(game, currentField, cursorPivot);
	}

	else if (moveDirection == RIGHT)
	{
		if (currentField->firstPositionY == BOTHORIZONTALBORDER - 1)
		{
			game->cursorPositionX = game->board.fieldTable[currentField->index - 1].positionX;
		}

		if (currentField->firstPositionY == TOPHORIZONTALBORDER + 1)
		{
			game->cursorPositionX = game->board.fieldTable[currentField->index + 1].positionX;
		}
	}

	else if (moveDirection == UP)
	{
		game->cursorPositionY = game->board.fieldTable[FIELDTABLESIZE - 1 - currentField->index].firstPositionY + currentField->nOfPawns - 1;
	}

	else if (moveDirection == DOWN)
	{
		game->cursorPositionY = game->board.fieldTable[currentField->index].firstPositionY - currentField->nOfPawns + 1;
	}
}


void saveCourtToFile(Court court, FILE* writeFile)
{
	fprintf(writeFile, "%d %d\n", court.nOfWhitePawns, court.nOfRedPawns);
}


void saveBandToFile(Band band, FILE* writeFile)
{
	fprintf(writeFile, "%d %d\n", band.nOfWhitePawns, band.nOfRedPawns);
}


void saveFieldsToFile(Field fieldTable[FIELDTABLESIZE], FILE* writeFile)
{
	for (int fieldIndex = 0; fieldIndex < FIELDTABLESIZE; fieldIndex++)
	{
		fprintf(writeFile, "%d %d\n", fieldTable[fieldIndex].nOfPawns, fieldTable[fieldIndex].color);
	}
}


void saveBoardToFile(Board* board, FILE* writeFile)
{
	saveBandToFile(board->band, writeFile);
	saveCourtToFile(board->court, writeFile);
	saveFieldsToFile(board->fieldTable, writeFile);
}


void saveGameToFile(Game* game)
{
	FILE* writeFile = fopen("backgammon.txt", "w");

	if (writeFile == NULL)
	{
		printf("Nie udalo sie otworzyc pliku do zapisu");
		return;
	}

	saveBoardToFile(&game->board, writeFile);

	fprintf(writeFile, "%d %d\n", game->dice1Value, game->dice2Value);
	fprintf(writeFile, "%d %d\n", game->cursorPositionX, game->cursorPositionY);
	fprintf(writeFile, "%s %s\n", game->whitePlayer->name, game->redPlayer->name);
	fprintf(writeFile, "%s\n", game->whoseTurn->name);

	fclose(writeFile);

	gotoxy(30, 2);
	printf("Gra zapisana");
	gotoxy(game->cursorPositionX, game->cursorPositionY);
	putch(CURSORCHAR);
}


void readBandFromFile(Band* band, FILE* readFile)
{
	fscanf(readFile, "%d%d", &band->nOfWhitePawns, &band->nOfRedPawns);
}


void readCourtFromFile(Court* court, FILE* readFile)
{
	fscanf(readFile, "%d%d", &court->nOfWhitePawns, &court->nOfRedPawns);
	printf("Court wczytany\n");
}


void readFieldsFromFile(Field fieldTable[FIELDTABLESIZE], FILE* readFile)
{
	for (int fieldIndex = 0; fieldIndex < FIELDTABLESIZE; fieldIndex++)
	{
		fscanf(readFile, "%d%d", &fieldTable[fieldIndex].nOfPawns, &fieldTable[fieldIndex].color);
	}

	printf("Pola wczytane\n");
	initFields(fieldTable);
	printf("Pola zainicjowane\n");
}

void readBoardFromFile(Board* board, FILE* readFile)
{
	readBandFromFile(&board->band, readFile);
	readCourtFromFile(&board->court, readFile);
	readFieldsFromFile(board->fieldTable, readFile);
}


void readGameFromFile(Game* game)
{
	FILE* readFile = fopen("backgammon.txt", "r");

	if (readFile == NULL)
	{
		printf("Nie udalo sie otworzyc pliku do odczytu");
		return;
	}
	
	readBoardFromFile(&game->board, readFile);
	
	fscanf(readFile, "%d%d", &game->dice1Value, &game->dice2Value);
	fscanf(readFile, "%d%d", &game->cursorPositionX, &game->cursorPositionY);
	fscanf(readFile, "%s%s", &game->whitePlayer->name, &game->redPlayer->name);
	fscanf(readFile, "%s", &game->whoseTurn->name);
	
	fclose(readFile);
}


// ta funkcja zawierać będzie funkcje, które obsługują poruszanie się na planszy, klawisze wywołujące jakieś działanie
// na początek chodzić będziemy CURSORCHARem, gra będzie trwała do momentu, aż sami jej nie wyłączymy - tzn.
// nic nie będzie znikało dopóki nie wciśniemy np. 'q'.
void gameFlow(Game* game)
{
	//inicjalizacja początkowej pozycji kursora
	game->cursorPositionX = game->board.fieldTable[0].positionX;
	game->cursorPositionY = game->board.fieldTable[0].firstPositionY - game->board.fieldTable[0].nOfPawns;
	
	char charFromUser = '0';

	_setcursortype(_NOCURSOR);

	while (charFromUser != 'q')
	{
		clrscr();

		printBoard(game);

		gotoxy(30, 1);
		printf("x: %d, y: %d - index: %d", game->cursorPositionX, game->cursorPositionY, identifyFieldByCursorPosition(game)->index);

		gotoxy(game->cursorPositionX, game->cursorPositionY);
		putch(CURSORCHAR);

		MoveDirection moveDirection = NONE;
		charFromUser = getch();

		if (charFromUser == 0)
		{
			charFromUser = getch();

			if (charFromUser == 0x48) moveDirection = UP;
			else if (charFromUser == 0x50) moveDirection = DOWN;
			else if (charFromUser == 0x4b) moveDirection = LEFT;
			else if (charFromUser == 0x4d) moveDirection = RIGHT;
		}

		else if (charFromUser == 's') saveGameToFile(game);
		else if (charFromUser == 'r') readGameFromFile(game);

		if (moveDirection != NONE)
		{
			moveCursor(game, moveDirection);
		}
	}
}


int main()
{
	Game game;
	srand(time(0));
	initGame(&game);
	gameFlow(&game);
	_setcursortype(_NORMALCURSOR);	
	return 0;
}

//TODO: w funkcji initPawnPosition() są pozycje startowe pionków na danych polach, ale nie znamy współrzędnych tych pól.
//TODO: czy współrzędna x dla pól powinna być z góry zdefiniowana?

//TODO: Zrobić poruszanie po planszy
//TODO: wszędzie gdzie są wspołrzędne kursora zamienić na game.cursorx albo y