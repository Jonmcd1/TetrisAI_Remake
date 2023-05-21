#include "Game.h"

using namespace std;

Game::Game() {
	gameBoard.resize(BOARD_HEIGHT, vector<int>(BOARD_WIDTH, 0));
	currPiece.newPiece();
	heldPiece.newPiece();
	userScore = 0;

	blocksPerRow = vector<int>(BOARD_HEIGHT, 0);
	topRowWithBlock = BOTTOM_ROW_HEIGHT;
	/* TESTING */
	// Remove Game.run()'s reset() line if using this
	/*
	gameBoard = {
		{0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,0,0,0,0,0},
		{0,0,0,0,0,0,0,1,0,0,0,0},
		{1,1,1,1,0,0,1,1,0,0,1,1},
		{1,1,1,1,0,1,1,1,0,0,1,1},
		{1,1,1,0,1,0,1,1,0,0,1,1}
	};

	for (int row = 0; row < BOARD_HEIGHT; row++) {
		int count = 0;
		for (int col = 0; col < BOARD_WIDTH; col++) {
			blocksPerRow[row] += gameBoard[row][col];
		}
	}

	for (int row = BOTTOM_ROW_HEIGHT; row >= 0; row--) {
		if (blocksPerRow[row] != 0) {
			topRowWithBlock = row;
		}
	}
	*/
	
}

void Game::reset() {
	for (int i = 0; i < BOARD_HEIGHT; i++) {
		for (int j = 0; j < BOARD_WIDTH; j++) {
			gameBoard[i][j] = 0;
		}
	}

	currPiece.newPiece();
	heldPiece.newPiece();
	userScore = 0;
}

// alreadyOnBoard refers ot the currPiece already being on the board
void Game::print(bool alreadyOnBoard) {
	if (!alreadyOnBoard) {
		for (auto coord : currPiece.occupiedSpaces) {
			gameBoard[coord.first][coord.second] = 2;
		}
	}

	cout << "\nHeld Piece: " << heldPiece.pieceChar << "\n";

	for (int i = 0; i < BOARD_WIDTH + 2; i++) {
		cout << "#";
	}
	cout << "\n";

	for (int i = 0; i < BOARD_HEIGHT; i++) {
		cout << "#";

		for (int j = 0; j < BOARD_WIDTH; j++) {
			if (gameBoard[i][j] == 0) {
				cout << "-";
			}
			else if (gameBoard[i][j] == 1)  {
				cout << "@";
			}
			else {
				cout << "X";
			}
		}

		cout << "#\n";
	}

	for (int i = 0; i < BOARD_WIDTH + 2; i++) {
		cout << "#";
	}
	cout << "\nScore: " << userScore << "\n";

	if (!alreadyOnBoard) {
		for (auto coord : currPiece.occupiedSpaces) {
			gameBoard[coord.first][coord.second] = 0;
		}
	}

	return;
}

void Game::checkFilledLines() {
	// The piece position is accurate. Use it to check filled lines.
	// Assumes the piece has already been placed on the board.

	// Get rows that were added to
	vector<int> rClear = getRowsToClear();
	clearRows(rClear);

	userScore += rClear.size() * rClear.size() * 100;
}

bool Game::moveVAllowed(int dv) {
	for (auto coord : currPiece.occupiedSpaces) {
		if (coord.first + dv >= BOARD_HEIGHT
			|| coord.first + dv < 0
			|| gameBoard[coord.first + dv][coord.second] != 0) {
			return false;
		}
	}
	return true;
}

bool Game::moveHAllowed(int dh) {
	for (auto coord : currPiece.occupiedSpaces) {
		if (coord.second + dh >= BOARD_WIDTH
			|| coord.second + dh < 0
			|| gameBoard[coord.first][coord.second + dh] != 0) {
			return false;
		}
	}
	return true;
}

bool Game::moveRAllowed(int turns) {
	pair<int, int>& pivot = currPiece.occupiedSpaces[0];

	vector<pair<int, int>> 
		testCoords(currPiece.occupiedSpaces.size(), { 0,0 });

	// 1st value is pivot, so it gets skipped
	if (turns % 4 == 1) {
		for (int i = 1; i < currPiece.occupiedSpaces.size(); i++) {
			pair<int, int>& cell = currPiece.occupiedSpaces[i];

			testCoords[i] = {
				pivot.first - (cell.second - pivot.second),
				pivot.second + (cell.first - pivot.first)
			};
		}
	}
	else if (turns % 4 == 2) {
		for (int i = 1; i < currPiece.occupiedSpaces.size(); i++) {
			pair<int, int>& cell = currPiece.occupiedSpaces[i];

			testCoords[i] = {
				pivot.first - (cell.first - pivot.first),
				pivot.second - (cell.second - pivot.second)
			};
		}
	}
	else if (turns % 4 == 3) {
		for (int i = 1; i < currPiece.occupiedSpaces.size(); i++) {
			pair<int, int>& cell = currPiece.occupiedSpaces[i];

			testCoords[i] = {
				pivot.first + (cell.second - pivot.second),
				pivot.second - (cell.first - pivot.first)
			};
		}
	}

	// Checking
	for (int i = 1; i < currPiece.occupiedSpaces.size(); i++) {
		pair<int, int>& cell = testCoords[i];
		if (cell.first >= BOARD_HEIGHT || cell.first < 0
			|| cell.second >= BOARD_WIDTH || cell.second < 0
			|| gameBoard[cell.first][cell.second] != 0) {
			return false;
		}
	}

	return true;
}

void Game::placePiece() {
	for (auto coord : currPiece.occupiedSpaces) {
		gameBoard[coord.first][coord.second] = 1;
		blocksPerRow[coord.first]++;

		// Update topRowWithBlock
		if (coord.first < topRowWithBlock) {
			topRowWithBlock = coord.first;
		}
	}
}

// Assumes the piece has already been placed on the board.
vector<int> Game::getRowsToClear() {
	vector<int> output;
	for (auto cell : currPiece.occupiedSpaces) {
		if (blocksPerRow[cell.first] == BOARD_WIDTH
			&& find(output.begin(), output.end(), cell.first) == output.end()) {
			output.push_back(cell.first);
		}
	}
	return output;
}

void Game::clearRows(vector<int>& rowsToClear) {
	// Time-saver
	if (rowsToClear.empty()) {
		return;
	}

	// Update topRowWithBlock
	topRowWithBlock += rowsToClear.size();

	// Move the rows
	// Row width has almost no effect on this timing
	sort(rowsToClear.begin(), rowsToClear.end(), greater<int>());
	for (int rMove = BOARD_HEIGHT - 1,
		rCurr = BOARD_HEIGHT - 1,
		compI = 0;
		rMove >= 0; rMove--) {
		if (compI >= rowsToClear.size() || rMove != rowsToClear[compI]) {
			swap(gameBoard[rMove], gameBoard[rCurr]);
			swap(blocksPerRow[rMove], blocksPerRow[rCurr]);
			rCurr--;
		}
		else {
			gameBoard[rMove] = vector<int>(BOARD_WIDTH, 0);
			blocksPerRow[rMove] = 0;
			compI++;
		}
	}
}