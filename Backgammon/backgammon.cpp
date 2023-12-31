﻿#include "conio2.h"
#include <stdio.h>
#include <stdlib.h>
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

// players
#define WHITEPLAYER 0
#define REDPLAYER 1


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


struct PlayerMove
{
	Field* sourceField;
	Field* destinationField;
};

struct Board
{
	Band band;
	Court court;
	Field fieldTable[FIELDTABLESIZE];
};


struct PossibleMove
{
	PlayerMove playerMove;
	PossibleMove* next;
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
	Player whitePlayer;
	Player redPlayer;
	int whoseTurn; // 0 - white, 1 - red
	CursorState cursorState;
};


// inicjacja nowego elementu listy
PossibleMove* createPossibleMove(PlayerMove move)
{
	PossibleMove* newMove = (PossibleMove*)malloc(sizeof(PossibleMove));

	if (newMove == NULL)
	{
		exit(EXIT_FAILURE);
	}

	newMove->playerMove = move;
	newMove->next = NULL;
	return newMove;
}


void printAuthor()
{
	gotoxy(RIGHTVERTICALBORDER + 1, TOPHORIZONTALBORDER);
	cputs("Szymon Orynczak, s191722");
}


void printBand(Band band)
{
	gotoxy(RIGHTVERTICALBORDER + 1, TOPHORIZONTALBORDER + 2);
	textcolor(WHITE);
	cputs("Number of white pawns on the band: ");
	putch(band.nOfWhitePawns + '0');

	gotoxy(RIGHTVERTICALBORDER + 1, TOPHORIZONTALBORDER + 3);
	textcolor(RED);
	cputs("Number of red pawns on the band: ");
	putch(band.nOfRedPawns + '0');
}


int returnColorOfCurrentPlayer(Game* game)
{
	if (game->whoseTurn == WHITEPLAYER)
	{
		return WHITE;
	}

	else if (game->whoseTurn == REDPLAYER)
	{
		return RED;
	}
}


void printRollResult(Game* game)
{
	gotoxy(RIGHTVERTICALBORDER + 1, TOPHORIZONTALBORDER + 5);
	textcolor(WHITE);
	printf("First dice: %d, Second dice: %d", game->dice1Value, game->dice2Value);
}


void rollDices(Game* game)
{
	game->dice1Value = (rand() % 6) + 1;
	game->dice2Value = (rand() % 6) + 1;
}


void changePlayerTurn(Game* game)
{
	game->whoseTurn = (game->whoseTurn + 1) % 2;
}


void swapPlayers(Game* game)
{
	Player tempPlayer = game->whitePlayer;

	game->whitePlayer = game->redPlayer;
	game->redPlayer = tempPlayer;
}


void rollStartPlayer(Game* game)
{
	rollDices(game);

	if (game->dice2Value > game->dice1Value)
	{
		swapPlayers(game);
	}

	else if (game->dice1Value == game->dice2Value)
	{
		rollStartPlayer(game);
	}

	game->whoseTurn = WHITEPLAYER;
}


void initPlayers(Game* game)
{
	cputs("Podaj nazwy graczy: ");

	int character = getche();
	game->whitePlayer.name[0] = character;
	game->whitePlayer.name[1] = '/0';

	int character1 = getche();
	game->redPlayer.name[0] = character1;
	game->redPlayer.name[1] = '/0';
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


// tutaj niech printuje nazwę gracza
void printWhoseTurn(Game* game)
{
	gotoxy(RIGHTVERTICALBORDER + 1, TOPHORIZONTALBORDER + 7);
	textcolor(returnColorOfCurrentPlayer(game));

	if (game->whoseTurn == WHITEPLAYER)
	{
		printf("White player turn");
	}

	else if (game->whoseTurn == REDPLAYER)
	{
		printf("Red player turn");
	}
}


void printGame(Game* game)
{
	textcolor(WHITE);
	printAuthor();
	drawBoardBoundaries(&game->board);
	drawPawnsOnFields(game);
	printBand(game->board.band);
	printRollResult(game);
	printWhoseTurn(game);
}


void initGame(Game* game)
{
	initEmptyBoard(&game->board);
	initFields(game->board.fieldTable);
	initPawnPosition(&game->board);
	initPlayers(game);
	rollStartPlayer(game);
	game->cursorPositionX = game->board.fieldTable[0].positionX;
	game->cursorPositionY = game->board.fieldTable[0].firstPositionY - game->board.fieldTable[0].nOfPawns;
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


void moveLeftOnTop(Game* game, Field* currentField, int cursorPivot)
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
		moveLeftOnTop(game, currentField, cursorPivot);
	}
}


void moveRightOnBottom(Game* game, Field* currentField, int cursorPivot)
{
	game->cursorPositionX = game->board.fieldTable[currentField->index - 1].positionX;

	if (identifyFieldByCursorPosition(game)->nOfPawns == 0)
	{
		game->cursorPositionY = game->board.fieldTable[currentField->index - 1].firstPositionY;
	}

	else
	{
		game->cursorPositionY = game->board.fieldTable[currentField->index - 1].firstPositionY - game->board.fieldTable[currentField->index - 1].nOfPawns + cursorPivot;
	}
}


void moveRightOnTop(Game* game, Field* currentField, int cursorPivot)
{
	game->cursorPositionX = game->board.fieldTable[currentField->index + 1].positionX;

	if (identifyFieldByCursorPosition(game)->nOfPawns == 0)
	{
		game->cursorPositionY = game->board.fieldTable[currentField->index + 1].firstPositionY;
	}

	else
	{
		game->cursorPositionY = game->board.fieldTable[currentField->index + 1].firstPositionY + game->board.fieldTable[currentField->index + 1].nOfPawns - cursorPivot;
	}
}


void moveRight(Game* game, Field* currentField, int cursorPivot)
{
	if (currentField->firstPositionY == BOTHORIZONTALBORDER - 1)
	{
		moveRightOnBottom(game, currentField, cursorPivot);
	}

	else if (currentField->firstPositionY == TOPHORIZONTALBORDER + 1)
	{
		moveRightOnTop(game, currentField, cursorPivot);
	}
}


void moveUp(Game* game, Field* currentField, int cursorPivot)
{
	if (identifyFieldByCursorPosition(game)->nOfPawns == 0)
	{
		game->cursorPositionY = game->board.fieldTable[FIELDTABLESIZE - 1 - currentField->index].firstPositionY;
	}

	else
	{
		game->cursorPositionY = game->board.fieldTable[FIELDTABLESIZE - 1 - currentField->index].firstPositionY + game->board.fieldTable[FIELDTABLESIZE - 1 - currentField->index].nOfPawns - cursorPivot;
	}
	
}


void moveDown(Game* game, Field* currentField, int cursorPivot)
{
	if (identifyFieldByCursorPosition(game)->nOfPawns == 0)
	{
		game->cursorPositionY = game->board.fieldTable[FIELDTABLESIZE - 1 - currentField->index].firstPositionY;
	}

	else
	{
		game->cursorPositionY = game->board.fieldTable[FIELDTABLESIZE - 1 - currentField->index].firstPositionY - game->board.fieldTable[FIELDTABLESIZE - 1 - currentField->index].nOfPawns + cursorPivot;
	}
}


int ifWrongMoveDirection(Game* game, MoveDirection moveDirection)
{
	if ((identifyFieldByCursorPosition(game)->firstPositionY == BOTHORIZONTALBORDER - 1 && moveDirection == DOWN) ||
		(identifyFieldByCursorPosition(game)->firstPositionY == TOPHORIZONTALBORDER + 1 && moveDirection == UP))
	{
		return 1;
	}

	else if (moveDirection == LEFT && (identifyFieldByCursorPosition(game)->index == 11 || identifyFieldByCursorPosition(game)->index == 12))
	{
		return 1;
	}

	else if (moveDirection == RIGHT && (identifyFieldByCursorPosition(game)->index == 0 || identifyFieldByCursorPosition(game)->index == 23))
	{
		return 1;
	}

	return 0;
}


// realizuje chodzenie po planszych po patykach
void moveCursor(Game* game, MoveDirection moveDirection)
{
	Field* currentField = identifyFieldByCursorPosition(game);
	int cursorPivot = 0; // zapewnia odpowiednie przesunięcie kursora w PICKPAWN i PLACEPAWN

	if (ifWrongMoveDirection(game, moveDirection))
	{
		return;
	}

	if (game->cursorState == PICK_PAWN)
	{
		cursorPivot = 1;
	}

	else if (game->cursorState == PLACE_PAWN)
	{
		cursorPivot = 0;
	}


	if (moveDirection == LEFT)
	{
		moveLeft(game, currentField, cursorPivot);
	}

	else if (moveDirection == RIGHT)
	{
		moveRight(game, currentField, cursorPivot);
	}

	else if (moveDirection == UP)
	{
		moveUp(game, currentField, cursorPivot);
	}

	else if (moveDirection == DOWN)
	{
		moveDown(game, currentField, cursorPivot);
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
	fprintf(writeFile, "%s %s\n", game->whitePlayer.name, game->redPlayer.name);
	fprintf(writeFile, "%d\n", game->whoseTurn);

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
	fscanf(readFile, "%s%s", game->whitePlayer.name, game->redPlayer.name);
	fscanf(readFile, "%d", &game->whoseTurn);
	
	fclose(readFile);
}


int ifOwnPawnPicked(Game* game, Field* sourceField)
{
	if (sourceField->color == returnColorOfCurrentPlayer(game))
	{
		return 1;
	}

	return 0;
}


void setProperCursorChar(Game* game)
{
	if (game->cursorState == PICK_PAWN)
	{
		putch(CURSORCHAR);
	}

	else if (game->cursorState == PLACE_PAWN)
	{
		putch(PAWNCHAR);
	}
}




// jeśli najedziemy na pionek w state == PICK_PAWN to na zielono podświetlą sie wszystkie możliwe destinationField z listy jednokierunkowej
// czyli: clrscr(), printGame i displayPossibleMoves
// dodatkowo jeśli podniesiemy pionek to pola wciąż powinny świecić się na zielono
void displayPossibleMoves(Game* game, PossibleMove* possibleMoveHead)
{
	clrscr();
	printGame(game);

	gotoxy(30, 10);
	printf("here");

	PossibleMove* current = possibleMoveHead;

	while (current != NULL)
	{
		//gotoxy dla dołu póki co
		gotoxy(current->playerMove.destinationField->positionX, current->playerMove.destinationField->firstPositionY - current->playerMove.destinationField->nOfPawns);
		textcolor(GREEN);
		putch('o');
		current = current->next;
	}
}


void captureEnemyPawn(Game* game)
{
	if (game->whoseTurn == WHITEPLAYER)
	{
		game->board.band.nOfRedPawns += 1;
	}

	else if (game->whoseTurn == REDPLAYER)
	{
		game->board.band.nOfWhitePawns += 1;
	}
}


// ta funckja chyba powinna korzystać z listy możliwych ruchów
void movePawn(Game* game, PlayerMove playerMove)
{
	if (playerMove.sourceField->nOfPawns > 0 && playerMove.sourceField->color == returnColorOfCurrentPlayer(game))
	{
		playerMove.sourceField->nOfPawns -= 1;
	}

	if (playerMove.destinationField->nOfPawns == 0)
	{
		playerMove.destinationField->nOfPawns = 1;
		playerMove.destinationField->color = returnColorOfCurrentPlayer(game);
	}

	else if (playerMove.destinationField->color == returnColorOfCurrentPlayer(game))
	{
		playerMove.destinationField->nOfPawns += 1;
	}

	else if (playerMove.destinationField->nOfPawns == 1)
	{
		captureEnemyPawn(game);
		playerMove.destinationField->nOfPawns = 1;
		playerMove.destinationField->color = returnColorOfCurrentPlayer(game);
	}

	else
	{
		cputs("Nie mozna wykonac ruchu");
	}
}


// zwolnienie pamięci
void freeList(PossibleMove* head)
{
	PossibleMove* current = head;
	PossibleMove* next;

	while (current != NULL)
	{
		next = current->next;
		free(current);
		current = next;
	}
}


// dodawanie do listy
void addPossibleMoveToList(PossibleMove** head, PlayerMove move)
{
	PossibleMove* newNode = createPossibleMove(move);
	if (*head == NULL)
	{
		*head = newNode;
	}

	else
	{
		PossibleMove* current = *head;
		while (current->next != NULL) {
			current = current->next;
		}
		current->next = newNode;
	}
}



void addNormalRollResult(Game* game, PossibleMove** head, Field* currentField)
{
	int dicesSum = game->dice1Value + game->dice2Value;

	// od razu musi być sprawdzenie czy index destinationField jest < 23 ponieważ może wyrzucić błąd
	PlayerMove move1 = { currentField, &game->board.fieldTable[currentField->index + dicesSum] };
	PlayerMove move2 = { currentField, &game->board.fieldTable[currentField->index + game->dice1Value] };
	PlayerMove move3 = { currentField, &game->board.fieldTable[currentField->index + game->dice2Value] };

	addPossibleMoveToList(head, move1);
	addPossibleMoveToList(head, move2);
	addPossibleMoveToList(head, move3);
}


void addDoubleRollResult(Game* game, PossibleMove** head, Field* currentField)
{
	PlayerMove move;

	for (int i = 0; i < game->dice1Value; i++)
	{
		//dodaj do listy jako index: dice*1, dice*2, dice*3, dice*4
		move = { currentField, &game->board.fieldTable[currentField->index + game->dice1Value * i] };
		addPossibleMoveToList(head, move);
	}
}


// ta funkcja będzie rozpoznawać, że gracz biały może ruszyć się na mniejsze indeksy, gracz czerwony na większe
// dodanie sourceField.index + dice1/dice2/suma
void addPossibleMoves(int nOfPossibleMoves, Game* game, PossibleMove** moveHead)
{
	Field* currentField = identifyFieldByCursorPosition(game);

	if (nOfPossibleMoves == 3)
	{
		addNormalRollResult(game, moveHead, currentField);
	}

	else if (nOfPossibleMoves == 4)
	{
		addDoubleRollResult(game, moveHead, currentField);
	}
}


// usuwa ruch z listy możliwych ruchów - został on zweryfikowany jako niemożliwy
void removeMoveFromList(PossibleMove** head, PlayerMove moveToRemove)
{
	PossibleMove* current = *head;
	PossibleMove* previous = NULL;

	// Szukanie elementu do usunięcia
	while (current != NULL && (current->playerMove.destinationField == moveToRemove.destinationField && current->playerMove.sourceField == moveToRemove.sourceField))
	{
		previous = current;
		current = current->next;
	}

	// Jeśli element został znaleziony
	if (current != NULL)
	{
		// Jeśli to pierwszy element listy
		if (previous == NULL)
		{
			*head = current->next;
		}
		else
		{
			previous->next = current->next;
		}

		// Zwolnienie pamięci zajmowanej przez usuwany element
		free(current);
	}
}

// sprawdza czy index jest <= 23, czy na destField są enemy pionki itd
// ta funkcja powinna sprawdzac więcej rzeczy ale to wymaga zmian w movePawn()
void verifyPossibleMoves(Game* game, PossibleMove* possibleMoveHead)
{
	PossibleMove* current = possibleMoveHead;

	while (current != NULL)
	{
		if (current->playerMove.destinationField->index > (FIELDTABLESIZE - 1))
		{
			removeMoveFromList(&possibleMoveHead, current->playerMove);
		}

		current = current->next;
	}
}


//tutaj jest duzo więcej możliwości
int numberOfPossibleDestination(Game* game)
{
	if (game->dice1Value == game->dice2Value)
	{
		return 4;
	}

	return 3;
}


int ifPawnCursorMoved(Game* game, PlayerMove playerMove)
{
	if (playerMove.sourceField->index == playerMove.destinationField->index)
	{
		return 0;
	}

	return 1;
}



int ifCursorOnOwnPawn(Game* game)
{
	Field* currentField = identifyFieldByCursorPosition(game);

	if (game->cursorState == PICK_PAWN || currentField->nOfPawns > 0 || currentField->color == game->whoseTurn)
	{
		return 1;
	}

	return 0;
}


// TA FUNKCJA BEDZIE ZMIENIONA
// zwraca czy kontynuować grę
int cursorMovement(Game* game, PossibleMove* moveHead, int nOfPossibleDest)
{
	char charFromUser = '0';
	_setcursortype(_NOCURSOR);

	while (charFromUser != 'q' && charFromUser != 10 && charFromUser != 13)
	{
		clrscr();
		printGame(game);
		freeList(moveHead);

		if (ifCursorOnOwnPawn(game))
		{
			addPossibleMoves(nOfPossibleDest, game, &moveHead); // dodaje do listy jednokierunkowej pola, które są w odległości dice1, dice2 od indexu na którym znajduje sie kursor
			verifyPossibleMoves(game, moveHead);
			displayPossibleMoves(game, moveHead);
		}
		
		gotoxy(game->cursorPositionX, game->cursorPositionY);
		textcolor(returnColorOfCurrentPlayer(game));
		setProperCursorChar(game);

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

	if (charFromUser == 'q')
	{
		return 0;
	}

	else
	{
		return 1;
	}
}


int makePlayerMove(Game* game)
{
	game->cursorState = PICK_PAWN;

	PlayerMove playerMove;
	PossibleMove* moveHead = NULL;

	rollDices(game);
	int nOfPossibleDest = numberOfPossibleDestination(game);

	int ifContinue = cursorMovement(game, moveHead, nOfPossibleDest); //do wcisniecia entera

	if (!ifContinue)
	{
		return 0;
	}

	playerMove.sourceField = identifyFieldByCursorPosition(game);

	while (!ifOwnPawnPicked(game, playerMove.sourceField))
	{
		ifContinue = cursorMovement(game, moveHead, nOfPossibleDest);
		playerMove.sourceField = identifyFieldByCursorPosition(game);
	}

	game->cursorState = PLACE_PAWN;

	ifContinue = cursorMovement(game, moveHead, nOfPossibleDest); // do wcisniecia entera

	playerMove.destinationField = identifyFieldByCursorPosition(game);

	while (!ifPawnCursorMoved(game, playerMove))
	{
		ifContinue = cursorMovement(game, moveHead, nOfPossibleDest); // do wcisniecia entera
		playerMove.destinationField = identifyFieldByCursorPosition(game);
	}

	if (!ifContinue)
	{
		return 0;
	}

	movePawn(game, playerMove);

	return 1;
}


// krok po kroku realizuje rozgrywkę
void performGame(Game* game)
{
	int ifContinue = 1;

	while (ifContinue)
	{
		ifContinue = makePlayerMove(game);

		if (ifContinue)
		{
			changePlayerTurn(game);
		}
	}
}


int main()
{
	Game game;
	srand(time(0));
	initGame(&game);
	performGame(&game);
	_setcursortype(_NORMALCURSOR);	
	return 0;
}

//TODO: na zielono wyświetla się gdzie można postawić pionek
//TODO: zrobic liste jednokierunkowa mozliwych ruchow
//TODO: postępowanie w razie dubletu
//TODO: jeżeli jesteśmy w pick_pawn to po najechaniu na pole pojawiają nam się możliwe ruchy i wtedy możemy podnieść pionek
//TODO: gracz idzie na podświetlone pole, jeśli jest ono w liście jednokierunkowej possibleMoves to ruch zostaje zatwierdzony
//TODO: wprowadzanie pionków do bazy: zmienić status np. gameState i wtedy pionki mogą przekraczać 23 indeks w ruchu
//TODO: przy wczytaniu pliku sie zmienia sie tura !!!
//TODO: wypisać informacje o autorze i zrobic menu opcji do wyboru (legendę)
//TODO: dopracowac losowanie gracza zaczynajacego
//TODO: jeśli podniesiemy czerwony pionek, nie postawimy go i zapiszemy gre to po wczytaniu jest tura białego - a nie czerwonego - naprawić

/*
*  Jest lista możliwych ruchów, zawiera ona ruchy nieprzekraczające 23 indeksu. Szukamy na liście ruchów, które są aktualnie możliwe.
*  Jeśli jakiś ruch na początku nie jest możliwy przez np. pionki przeciwnika to szukamy kolejnego ale go nie usuwamy. Usuwamy ruch z listy po jego wykonaniu.
*  Wyrzucam 3 i 5. Nie mogę iść na source+5 bo są tam pionki, mogę iść source+3 - program wybiera ruch source+3 i usuwa go z listy. Z pola source+3 mogę wykonać ruch
*  +5, który pozostał na liście.
*  WAŻNE: 3+5 daje 8, ale jeśli na polu source+3 i source+5 są pionki przeciwnika, to nie mogę sie ruszyć o 8.
*/

/*
*  Zająć się naprawą while'a w cursorMovement(). Listy działają natomiast dodawanie i usuwanie musi być realizowane w whilu, ponieważ elementy
*  listy zależą od pozycji kursora. 
*/