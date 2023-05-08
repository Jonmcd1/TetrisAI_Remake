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


	// int currentPiece = -1;
	// int currentRotation = 0;

	// (0, 0) is top-left
	// int currentVertical = -1;
	// int currentHorizontal = -1;


	char pieceChar = 'I';
	int pieceIdx = 0;

	// The 4 tiles occupied in {vertical, horizontal} form
	vector<pair<int, int>> occupiedSpaces;

	// Piece bank
	// A vector of piece data, each datum of which is an ID char
	// paired with a vector of starting cell positions
	// The first cell position is the pivot cell
	vector<pair<char, vector<pair<int, int>>>> pieceBank = {
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

	void moveV(int dv);

	void moveH(int dh);

	// Counterclockwise by 90 degrees
	void moveR(int turns);
};

/*

class Piece {
private:
	int currentPiece = -1;
	int currentRotation = 0;

	// (0, 0) is top-left
	int currentVertical = -1;
	int currentHorizontal = -1;
	
	// The 4 tiles occupied in {vertical, horizontal} form
	int occupiedSpaces[PIECE_SIZE][2] = {0};

public:
	/**
	 * Requires: Nothing
	 * Modifies: Nothing
	 * Effects:  Constructor for Piece; Piece values get set by newPiece
	 *\/
	Piece();


	/**
	 * Requires: randomNum is >= 0 and < 6
	 * Modifies: all private data members
	 * Effects:  Sets the piece's position/type/tiles based on what type is selected
	 *\/
	void newPiece(int randomNum);


	/**
	 * Requires: Nothing
	 * Modifies: occupiedSpaces
	 * Effects:  Shifts the piece 1 tile to the left
	 *\/
	void moveLeft();


	/**
	 * Requires: Nothing
	 * Modifies: occupiedSpaces
	 * Effects:  Shifts the piece 1 tile to the right
	 *\/
	void moveRight();


	/**
	 * Requires: Nothing
	 * Modifies: occupiedSpaces
	 * Effects:  Shifts the piece 1 tile down
	 *\/
	void moveDown();


	/**
	 * Requires: Nothing
	 * Modifies: occupiedSpaces
	 * Effects:  Rotates(tilts) the piece 90 degrees around currentVertical/currentHorizontal
	 *\/
	void rotate();

	/**
	 * Requires: Nothing
	 * Modifies: Nothing
	 * Effects:  Returns the character representing the piece
	 *\/
	char getPieceChar();

	/**
	 * Requires: 0 <= i <= 3
	 * Modifies: Nothing
	 * Effects:  Returns an occupiedSpace Y coord
	 *\/
	int getOccupiedSpaceY(int i);
	
	/**
	 * Requires: 0 <= i <= 3
	 * Modifies: Nothing
	 * Effects:  Returns an occupiedSpace X coord
	 *\/
	int getOccupiedSpaceX(int i);

	/**
	 * Requires: Nothing
	 * Modifies: Nothing
	 * Effects:  Returns currentVertical
	 *\/
	int getCurrentVertical();

	/**
	 * Requires: Nothing
	 * Modifies: Nothing
	 * Effects:  Returns currentHorizontal
	 *\/
	int getCurrentHorizontal();
};

*/

#endif