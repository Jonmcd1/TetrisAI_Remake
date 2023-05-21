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

