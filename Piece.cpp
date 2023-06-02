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

	move(v-1, h-6, r);
}


// Makes each move in order of the argument 
// (vertical, then horizontal, then rotation)
void Piece::move(int dv, int dh, int turns) {
	moveCoords(occupiedSpaces, dv, dh, turns);
	currRotation = (currRotation + turns) % 4;
}