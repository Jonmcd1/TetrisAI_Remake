#ifndef _GAME_H_
#define _GAME_H_

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include "Utility.h"
#include "Piece.h"
using namespace std;


class Game {
private:
	friend class AI;
	friend class AITrainer;

	/*** METADATA ***/
	int gameCount = 0;
	int piecesPlaced = 0;


	/*** BOARD DATA ***/
	// Reminder: top left is (0,0), as in (vert, horiz)
	vector<vector<int>> gameBoard;
	Piece currPiece;
	Piece heldPiece;

	int userScore;

	// Useful for improving line-clear efficiency
	vector<int> blocksPerRow;
	int topRowWithBlock;


	/*** DISPLAY FUNCTION ***/
	/*
	* REQUIRES: Piece.occupiedSpaces does not contain an already occupied
	*			cell on gameBoard IF alreadyOnBoard == false
	* MODIFIES:	None
	* EFFECTS:	Prints the board, held piece, and score in formatted way;
	*			if currPiece has already been placed on the board, then
	*			alreadyOnBoard should be true
	* NOTES:	gameBoard is modified but reset when this function ends
	*/
	void print(bool alreadyOnBoard = false);


	/*** PIECE PLACEMENT FUNCTIONS ***/
	/*
	* REQUIRES: Piece.occupiedSpaces does not contain an already occupied 
	*			cell on gameBoard
	* MODIFIES: piecesPlaced, gameBoard, blocksPerRow, topRowWithBlock
	* EFFECTS:	Sets each coordinate of Piece.occupiedSpaces to 1 on gameBoard
	*/
	void placePiece();

	/*
	* REQUIRES: Nothing
	* MODIFIES: userScore
	* EFFECTS:	Analyzes the current state of gameBoard; finds any 
	*			lines that are full, clears them, and updates the score
	* CALLS:	getRowsToClear, clearRows
	*/
	void processFilledLines();

	/*
	* REQUIRES: 0 <= rowsToClear[i] < BOARD_HEIGHT
	* MODIFIES: gameBoard, topRowWithBlock, blocksPerRow
	* EFFECTS:	Removes each row to be cleared, and moves the rows above
	*			them down (like gravity)
	*/
	void clearRows(vector<int>& rowsToClear);

	/*
	* REQUIRES: currPiece is accurately positioned
	* MODIFIES: None
	* EFFECTS:	Returns a list containing the vertical positions of full
	*			rows on gameBoard
	*/
	vector<int> getRowsToClear();

	/*** MOVE VALIDITY CHECKERS ***/
	/*
	* REQUIRES: dv >= 0 (You can't move up in Tetris)
	* MODIFIES: None
	* EFFECTS:	Returns true if the specified movement is allowed on the board,
	*			and false otherwise. The entire path to the destination is
	*			checked for validity. The path is checked by moving vertically
	*			one step at a time, then horizontally one step at a time,
	*			then rotationally one turn at a time. If there is any collision
	*			with an occupied space or out-of-bounds movement, this returns
	*			false.
	* CALLS:	moveCoords (See Utility.cpp), checkValidPos
	* NOTES:	By the nature of the path checking, moving on only 1 axis per 
	*			call to this function is strongly suggested
	*/
	bool moveAllowed(int dv, int dh, int turns);

	/*
	* REQUIRES: Nothing
	* MODIFIES: None
	* EFFECTS:	Returns true if a current/held piece swap is allowed
	*			(no collision), and false otherwise
	* CALLS:	checkValidPos
	*/
	bool swapAllowed();

	/*
	* REQUIRES: Nothing
	* MODIFIES: None
	* EFFECTS:	Returns true if every coordinate in testCoords isn't occupied
	*			or out-of-bounds, and false otherwise
	*/
	bool checkValidPos(vector<Coord>& testCoords);


public:
	/*** INITIALIZERS ***/
	/*
	* REQUIRES: Nothing
	* MODIFIES: this
	* EFFECTS:	Initializes this Game object; see reset() RME
	* CALLS:	reset
	*/
	Game();

	/*
	* REQUIRES: Nothing
	* MODIFIES: Everything except gameCount
	* EFFECTS:	Resets the whole game (board emptied, score reset, etc)
	* CALLS:	Piece.newPiece
	*/
	void reset();


	/*** DRIVER ***/
	/*
	* REQUIRES: If autoMode == true, then autoMover points to an object that 
	*			returns a valid move character when autoMover->makeMove() 
	*			is called
	* MODIFIES: this (Everything)
	* EFFECTS:	Runs the game until it's lost
	* CALLS:	reset, moveAllowed, swapAllowed, move, placePiece,
	*			processFilledLines, print, Piece.newPiece
	*/
	template <typename Mover>
	void run(bool autoMode, Mover* autoMover = nullptr) {
		reset();

		// While the player hasn't lost yet
		while (true) {
			// Make a new piece
			currPiece.newPiece();

			// Exit if the game is lost
			if (!moveAllowed(0, 0, 0) || piecesPlaced > 2000) {
				if (!autoMode) {
					cout << "\n\nGame Over!\n"
						<< "-=+ Your final score: " << userScore
						<< " +=-\n\n\n\n";
				}
				return;
			}

			// While the current piece hasn't been placed yet
			while (true) {
				// Get the next move to make
				string move;
				if (!autoMode) {
					print();
					cout << "Your Move (S, L, R, D, T, or <anything else> to idle): ";
					getline(cin, move);
				}
				else {
					move = autoMover->makeMove();
				}

				// Run the selected move
				if (move == "S" && swapAllowed()) {
					swap(currPiece, heldPiece);
					heldPiece.newPiece(heldPiece.pieceIdx, 0, 1, 6);
					continue;
				}

				if (move == "L" && moveAllowed(0, -1, 0)) {
					currPiece.move(0, -1, 0);
					continue;
				}

				if (move == "R" && moveAllowed(0, 1, 0)) {
					currPiece.move(0, 1, 0);
					continue;
				}

				if (move == "T" && moveAllowed(0, 0, 1)) {
					currPiece.move(0, 0, 1);
					continue;
				}

				if (move == "D" && moveAllowed(1, 0, 0)) {
					currPiece.move(1, 0, 0);
				}

				// In case of idle or D selection, move the piece down
				// and check if this ends the piece's turn
				if (moveAllowed(1, 0, 0)) {
					currPiece.move(1, 0, 0);
				}
				else {
					placePiece();

					// Break out of this piece's loop if it gets placed
					break;
				}
			} // finish this piece

			processFilledLines();

		} // finish this game

		gameCount++;
	}
};

#endif