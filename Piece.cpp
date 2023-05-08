#include "Piece.h"

using namespace std;

void Piece::newPiece() {
	int idx = rand() % pieceBank.size();
	pieceChar = pieceBank[idx].first;
	pieceIdx = idx;
	occupiedSpaces = pieceBank[idx].second;
}

void Piece::newPiece(int idx, int r, int v, int h) {
	pieceChar = pieceBank[idx].first;
	pieceIdx = idx;
	occupiedSpaces = pieceBank[idx].second;

	for (int i = 0; i < occupiedSpaces.size(); i++) {
		occupiedSpaces[i].first += v - 1;
		occupiedSpaces[i].second += h - 6;
	}
	moveR(r);
}

void Piece::moveV(int dv) {
	for (auto& coord : occupiedSpaces) {
		coord.first += dv;
	}
}

void Piece::moveH(int dh) {
	for (auto& coord : occupiedSpaces) {
		coord.second += dh;
	}
}

void Piece::moveR(int turns) {
	pair<int, int>& pivot = occupiedSpaces[0];

	// 1st value is pivot, so it gets skipped
	if (turns % 4 == 1) {
		for (int i = 1; i < occupiedSpaces.size(); i++) {
			pair<int, int>& cell = occupiedSpaces[i];

			int tempV = cell.first;
			cell.first = pivot.first - (cell.second - pivot.second);
			cell.second = pivot.second + (tempV - pivot.first);
		}
	}
	else if (turns % 4 == 2) {
		for (int i = 1; i < occupiedSpaces.size(); i++) {
			pair<int, int>& cell = occupiedSpaces[i];

			// FIXME: This looks impossible
			cell.first = pivot.first - (cell.first - pivot.first);
			cell.second = pivot.second - (cell.second - pivot.second);
		}
	}
	else if (turns % 4 == 3) {
		for (int i = 1; i < occupiedSpaces.size(); i++) {
			pair<int, int>& cell = occupiedSpaces[i];

			int tempV = cell.first;
			cell.first = pivot.first + (cell.second - pivot.second);
			cell.second = pivot.second - (tempV - pivot.first);
		}
	}
}

/*

Piece::Piece() {
	// Nothing (intentional)
}

// 0 is I, 1 is L, 2 is T, 3 is S, 4 is Z, 5 is B
void Piece::newPiece(int randomNum) {
	currentPiece = randomNum;
	currentVertical = 1;
	currentHorizontal = 6;
	currentRotation = 0;
	
	//Stored in <v><h><v><h><v><h> form
	string initOccupiedSpaces;
	if (currentPiece == 0) {
		initOccupiedSpaces = "06162636";
	}
	else if (currentPiece == 1) {
		initOccupiedSpaces = "06162627";
	}
	else if (currentPiece == 2) {
		initOccupiedSpaces = "06161517";
	}
	else if (currentPiece == 3) {
		initOccupiedSpaces = "06071615";
	}
	else if (currentPiece == 4) {
		initOccupiedSpaces = "06051617";
	}
	else if (currentPiece == 5) {
		initOccupiedSpaces = "06071617";
	}

	for (int i = 0; i < PIECE_SIZE; i++) {
		for (int j = 0; j < 2; j++) {
			// Gets the right position in the string and makes it an int
			occupiedSpaces[i][j] = (int)(initOccupiedSpaces.at(2 * i + j) - '0');
		}
	}

	return;
}


void Piece::moveLeft() {
	for (int i = 0; i < PIECE_SIZE; i++) {
		occupiedSpaces[i][1] = occupiedSpaces[i][1] - 1;
	}
	currentHorizontal--;
	return;
}


void Piece::moveRight() {
	for (int i = 0; i < PIECE_SIZE; i++) {
		occupiedSpaces[i][1] = occupiedSpaces[i][1] + 1;
	}
	currentHorizontal++;
	return;
}


void Piece::moveDown() {
	for (int i = 0; i < PIECE_SIZE; i++) {
		occupiedSpaces[i][0] = occupiedSpaces[i][0] + 1;
	}
	currentVertical++;
	return;
}


void Piece::rotate() {
	for (int i = 0; i < PIECE_SIZE; i++) {
		// Rotating the tile clockwise 90 degrees about the piece's current Horiz/Vert
		int tempX = currentHorizontal + currentVertical - occupiedSpaces[i][0];
		int tempY = currentVertical + occupiedSpaces[i][1] - currentHorizontal;
		occupiedSpaces[i][1] = tempX;
		occupiedSpaces[i][0] = tempY;
	}
	currentRotation = (currentRotation + 1) % 4;
	return;
}

char Piece::getPieceChar() {
	if (currentPiece == 0) {
		return 'I';
	}
	else if (currentPiece == 1) {
		return 'L';
	}
	else if (currentPiece == 2) {
		return 'T';
	}
	else if (currentPiece == 3) {
		return 'S';
	}
	else if (currentPiece == 4) {
		return 'Z';
	}
	else if (currentPiece == 5) {
		return 'B';
	}
	return ' ';
}

int Piece::getOccupiedSpaceY(int i) {
	return occupiedSpaces[i][0];
}

int Piece::getOccupiedSpaceX(int i) {
	return occupiedSpaces[i][1];
}

int Piece::getCurrentVertical() {
	return currentVertical;
}

int Piece::getCurrentHorizontal() {
	return currentHorizontal;
}

*/