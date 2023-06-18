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

	/*** METADATA ***/
	int pieceIdx = 0;
	int currRotation = 0;
	char pieceChar = 'I';


	/*** OCCUPIED CELL COORDS ***/
	// Coordinate (0, 0) is the top-left
	vector<Coord> occupiedSpaces;

	/*** PIECE BANK ***/
	// Each element is an ID char
	// paired with a vector of starting cell coordinates
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

	/*** INITIALIZERS ***/
	/*
	* REQUIRES: Nothing
	* MODIFIES: pieceIdx, currRotation, pieceChar, occupiedSpaces
	* EFFECTS:	Re-initializes this Piece to a random piece from 
	*			pieceBank in its starting position
	*/
	void newPiece();

	/*
	* REQUIRES: 0 <= idx < pieceBank.size()
	* MODIFIES:	pieceIdx, currRotation, pieceChar, occupiedSpaces
	* EFFECTS:	Re-initializes this Piece to a specified piece from 
	*			pieceBank in a specified position/rotation; the pivot
	*			is the cell that gets set to the specified coords
	* CALLS:	move
	*/
	void newPiece(int idx, int r, int v, int h);

	/*** MOVER ***/
	/*
	* REQUIRES: Nothing
	* MODIFIES: currRotation, occupiedSpaces
	* EFFECTS:	Moves the Piece's occupiedSpaces by dv vertically,
	*			dh horizontally, and rotates it 90 degrees counterclockwise
	*			<turns> times
	* CALLS:	moveCoords (see Utility.cpp)
	*/
	void move(int dv, int dh, int turns);
};

#endif