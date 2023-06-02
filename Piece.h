#ifndef _PIECE_H_
#define _PIECE_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "Utility.h"
using namespace std;


class Piece {
private:
	friend class AI;
	friend class Game;


	char pieceChar = 'I';
	int pieceIdx = 0;

	// (0, 0) is top-left
	// The 4 tiles occupied in {vertical, horizontal} form
	vector<Coord> occupiedSpaces;

	// For debugging
	int currRotation = 0;

	// Piece bank
	// A vector of piece data, each datum of which is an ID char
	// paired with a vector of starting cell positions
	// The first cell position is the pivot cell
	vector<pair<char, vector<Coord>>> pieceBank = {
		{'I', { {1,6}, {0,6}, {2,6}, {3,6} }},
		{'L', { {1,6}, {0,6}, {2,6}, {2,7} }},
		{'T', { {1,6}, {0,6}, {0,5}, {0,7} }},
		{'S', { {1,6}, {0,6}, {0,7}, {1,5} }},
		{'Z', { {1,6}, {0,6}, {0,5}, {1,7} }},
		{'B', { {1,6}, {0,6}, {0,7}, {1,7} }}
	};

public:

	// Resets this piece with a random one
	void newPiece();

	// Resets this piece with a specified one (by index in pieceBank)
	// Places the pivot at the specified coords
	void newPiece(int idx, int r, int v, int h);


	void move(int dv, int dh, int turns);
};

#endif