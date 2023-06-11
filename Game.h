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
	int gameCount = 0;

	friend class AI;
	friend class AITrainer;

	// Top left is (0,0), as in (vert, horiz)
	vector<vector<int>> gameBoard;
	Piece currPiece;
	Piece heldPiece;
	int userScore;

	// Useful for improving line-clear efficiency
	vector<int> blocksPerRow;

	// Extra data
	int topRowWithBlock;

	// Debugging
	int piecesPlaced = 0;

	void placePiece();

	void reset();

	void print(bool alreadyOnBoard = false);

	void processFilledLines();

	void clearRows(vector<int>& rowsToClear);

	vector<int> getRowsToClear();

	// dv = delta vertical, dh = delta horizontal, turns = turns
	bool moveAllowed(int dv, int dh, int turns);

	bool swapAllowed();

	bool checkValidPos(vector<Coord>& testCoords);


public:
	Game();

	template <typename Mover>
	void run(bool autoMode, Mover* autoMover = nullptr) {
		reset();

		while (true) {
			// New piece setup
			currPiece.newPiece();

			// Exit if game is lost
			if (!moveAllowed(0, 0, 0) || piecesPlaced > 2000) {
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
					move = autoMover->makeMove();
				}

				// Cover selected moves
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

					break;
				}
			} // finish this piece

			processFilledLines();

		} // finish this game
	}
};

#endif