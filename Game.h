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

	// Top left is (0,0), as in (vert, horiz)
	vector<vector<int>> gameBoard;
	Piece currPiece;
	Piece heldPiece;
	int userScore;

	// Useful for improving line-clear efficiency
	vector<int> blocksPerRow;

	// Extra data
	int topRowWithBlock;

	// dv is delta v
	bool moveVAllowed(int dv);
	
	// dh is delta h (horizontal)
	bool moveHAllowed(int dh);

	// Rotation counterclockwise 90 degrees
	bool moveRAllowed(int turns);

	void placePiece();

	void reset();

	void print(bool alreadyOnBoard = false);

	void checkFilledLines();

	void clearRows(vector<int>& rowsToClear);

	vector<int> getRowsToClear();


public:
	Game();

	template <typename Mover>
	void run(bool autoMode, Mover* autoMover = nullptr) {
		reset();

		while (true) {
			// New piece setup
			currPiece.newPiece();

			// Exit if game is lost
			if (!moveVAllowed(0)) {
				if (!autoMode) {
					cout << "\n\nGame Over!\n"
						<< "-=+ Your final score: " << userScore
						<< " +=-\n\n\n\n";
				}
				return;
			}

			while (true) {
				string move;
				if (!autoMode) {
					print();
					cout << "Your Move (S, L, R, D, T, or <anything else> to idle): ";
					getline(cin, move);
				}
				else {
					// TODO: get move from outside
					//print();
					move = autoMover->makeMove();

					//string junk;
					//getline(cin, junk);
				}

				// Cover selected moves
				if (move == "S") {
					swap(currPiece, heldPiece);
					heldPiece.newPiece(heldPiece.pieceIdx, 0, 1, 6);
					continue;
				}

				if (move == "L" && moveHAllowed(-1)) {
					currPiece.moveH(-1);
					continue;
				}

				if (move == "R" && moveHAllowed(1)) {
					currPiece.moveH(1);
					continue;
				}

				if (move == "T" && moveRAllowed(1)) {
					currPiece.moveR(1);
					continue;
				}

				if (move == "D" && moveVAllowed(1)) {
					currPiece.moveV(1);
				}

				// In case of idle or D selection, move the piece down
				// and check if this ends the piece's turn
				if (moveVAllowed(1)) {
					currPiece.moveV(1);
				}
				else {
					placePiece();
					print(true);
					string junk;
					getline(cin, junk);

					break;
				}
			} // finish this piece

			checkFilledLines();

		} // finish this game
	}
};

#endif