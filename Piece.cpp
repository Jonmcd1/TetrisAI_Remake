#include "Piece.h"

using namespace std;

/*** INITIALIZERS ***/
void Piece::newPiece() {
	int idx = rand() % pieceBank.size();
	pieceChar = pieceBank[idx].first;
	pieceIdx = idx;
	occupiedSpaces = pieceBank[idx].second;
	currRotation = 0;
}

void Piece::newPiece(int idx, int r, int v, int h) {
	pieceChar = pieceBank[idx].first;
	pieceIdx = idx;
	occupiedSpaces = pieceBank[idx].second;
	currRotation = r;

	// The -1 and -6 are so v and h are accurate
	// (the pieceBank starts pieces in the middle of the board already)
	move(v-1, h-6, r);
}


/*** MOVER ***/
void Piece::move(int dv, int dh, int turns) {
	moveCoords(occupiedSpaces, dv, dh, turns);
	currRotation = (currRotation + turns) % 4;
}