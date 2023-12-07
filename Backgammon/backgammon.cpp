#include "conio2.h"
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


struct Board
{
	Band band;
	Court court;
	Field fieldTable[FIELDTABLESIZE];
};


struct PossibleMove
{
	int fieldIndex;
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


struct PlayerMove
{
	int startFieldIndex;
	int endFieldIndex;
};


void printBand(Band band)
{
	gotoxy(RIGHTVERTICALBORDER + 1, TOPHORIZONTALBORDER);
	textcolor(WHITE);
	cputs("Number of white pawns on the band: ");
	putch(band.nOfWhitePawns + '0');

	gotoxy(RIGHTVERTICALBORDER + 1, TOPHORIZONTALBORDER + 1);
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
	gotoxy(RIGHTVERTICALBORDER + 1, TOPHORIZONTALBORDER + 2);
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


void printGame(Game* game)
{
	textcolor(WHITE);
	drawBoardBoundaries(&game->board);
	drawPawnsOnFields(game);
	printBand(game->board.band);
	printRollResult(game);
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


// ta funkcja zawierać będzie funkcje, które obsługują poruszanie się na planszy, klawisze wywołujące jakieś działanie
// na początek chodzić będziemy CURSORCHARem, gra będzie trwała do momentu, aż sami jej nie wyłączymy - tzn.
// nic nie będzie znikało dopóki nie wciśniemy np. 'q'.
// TA FUNKCJA BEDZIE ZMIENIONA
// zwraca czy kontynuować grę
int cursorMovement(Game* game)
{
	//inicjalizacja początkowej pozycji kursora
	char charFromUser = '0';
	PossibleMove* possibleMove;
	_setcursortype(_NOCURSOR);

	while (charFromUser != 'q' && charFromUser != 10 && charFromUser != 13)
	{
		clrscr();

		printGame(game);

		//gotoxy(30, 4);
		//textcolor(WHITE);
		//printf("x: %d, y: %d - index: %d", game->cursorPositionX, game->cursorPositionY, identifyFieldByCursorPosition(game)->index);

		gotoxy(game->cursorPositionX, game->cursorPositionY);
		textcolor(returnColorOfCurrentPlayer(game));

		if (game->cursorState == PICK_PAWN)
		{
			putch(CURSORCHAR);
		}

		else if (game->cursorState == PLACE_PAWN)
		{
			putch(PAWNCHAR);
		}

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


void movePawn(Game* game, int sourceFieldIndex, int destinationFieldIndex)
{
	if (game->board.fieldTable[sourceFieldIndex].nOfPawns > 0 && game->board.fieldTable[sourceFieldIndex].color == returnColorOfCurrentPlayer(game))
	{
		game->board.fieldTable[sourceFieldIndex].nOfPawns -= 1;
	}

	if (game->board.fieldTable[destinationFieldIndex].nOfPawns == 0)
	{
		game->board.fieldTable[destinationFieldIndex].nOfPawns = 1;
		game->board.fieldTable[destinationFieldIndex].color = returnColorOfCurrentPlayer(game);
	}

	else if (game->board.fieldTable[destinationFieldIndex].color == returnColorOfCurrentPlayer(game))
	{
		game->board.fieldTable[destinationFieldIndex].nOfPawns += 1;
	}

	else if (game->board.fieldTable[destinationFieldIndex].nOfPawns == 1)
	{
		captureEnemyPawn(game);
		game->board.fieldTable[destinationFieldIndex].nOfPawns = 1;
		game->board.fieldTable[destinationFieldIndex].color = returnColorOfCurrentPlayer(game);
	}

	else
	{
		cputs("Nie mozna wykonac ruchu");
	}
}


// ta funkcja będzie rozpoznawać, że gracz biały może ruszyć się na mniejsze indeksy, gracz czerwony na większe
void addPossibleMove(Game* game)
{
	PossibleMove* possibleMove = (PossibleMove*)malloc(sizeof(PossibleMove));
	possibleMove->fieldIndex = 2; // tymczasowo
	possibleMove->next = NULL;
}


// usuwa ruch z listy możliwych ruchów - został on zweryfikowany jako niemożliwy
void removeMove()
{

}


void verifyPossibleMoves(Game* game, PossibleMove* possibleMove)
{
	while (possibleMove != NULL)
	{
		// używać struct PlayerMove
		// if (playerMove.desinationField.nOfPawns > 1 || inny kolor pionka na destinationField)
		// { removeMove(); }

		possibleMove->next;
	}
}


void displayPossibleMoves(Game* game, PossibleMove* possibleMove)
{
	while (possibleMove != NULL)
	{


		possibleMove->next;
	}
}



int makePlayerMove(Game* game)
{
	rollDices(game);
	// addPossibleMove(); // dodaje do listy jednokierunkowej pola, które są w odległości dice1, dice2 od indexu na którym znajduje sie kursor
	// verifyPossibleMoves(game, possibleMove);
	// displayPossibleMoves(game, possibleMove); // jeśli można bić to trzeba bić - podkreśla na zielono możliwe pola po weryfikacji
	game->cursorState = PICK_PAWN;
	int ifContinue = cursorMovement(game); //do wcisniecia entera

	if (!ifContinue)
	{
		return 0;
	}

	int sourceFieldIndex = identifyFieldByCursorPosition(game)->index;

	game->cursorState = PLACE_PAWN;

	ifContinue = cursorMovement(game); // do wcisniecia entera

	if (!ifContinue)
	{
		return 0;
	}

	int destinationFieldIndex = identifyFieldByCursorPosition(game)->index;
	movePawn(game, sourceFieldIndex, destinationFieldIndex);
	printGame(game);

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
			changePlayerTurn(game); // kto wykonuje ruch bedzie widac po kolorze kursora
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

//TODO: naprawić kolory po kolei
//TODO: dopracować kursory - biały może podnieść tylko białego pionka itd
//TODO: na zielono wyświetla się gdzie można postawić pionek
//TODO: zrobic liste jednokierunkowa mozliwych ruchow
//TODO: postępowanie w razie dubletu
//TODO: użyć struktury PlayerMove