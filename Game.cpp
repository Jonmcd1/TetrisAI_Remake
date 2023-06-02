#include "Game.h"

using namespace std;

Game::Game() {
	reset();
}


void Game::reset() {
	gameBoard = vector<vector<int>>(BOARD_HEIGHT, vector<int>(BOARD_WIDTH, 0));
	blocksPerRow = vector<int>(BOARD_HEIGHT, 0);
	topRowWithBlock = BOARD_HEIGHT; // 1-past-the-end

	currPiece.newPiece();
	heldPiece.newPiece();

	userScore = 0;

	/* TESTING */
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

// alreadyOnBoard refers ot the currPiece already being on the board
void Game::print(bool alreadyOnBoard) {
	if (!alreadyOnBoard) {
		for (auto coord : currPiece.occupiedSpaces) {
			if (gameBoard[coord.vert][coord.horiz]) {
				cout << "Print statement falsely thinks the piece "
					<< "isn't on the board. Invalid.\n";
				exit(1);
			}

			gameBoard[coord.vert][coord.horiz] = 2;
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
			gameBoard[coord.vert][coord.horiz] = 0;
		}
	}

	// Debugging
	cout << piecesPlaced << endl;

	return;
}

void Game::processFilledLines() {
	// The piece position is accurate. Use it to check filled lines.
	// Assumes the piece has already been placed on the board.

	// Get rows that were added to
	vector<int> rClear = getRowsToClear();
	clearRows(rClear);

	userScore += rClear.size() * rClear.size() * 100;
}


void Game::placePiece() {
	// Debugging
	piecesPlaced++;
	
	for (auto coord : currPiece.occupiedSpaces) {
		// Sanity check
		if (gameBoard[coord.vert][coord.horiz] == 1) {
			cout << "Loaded piece on occupied square. Invalid.\n";
			gameBoard[-1][-1];
			exit(1);
		}
		if (blocksPerRow[coord.vert] == BOARD_WIDTH) {
			cout << "Loaded piece on full row. Invalid.\n";
			exit(1);
		}

		
		gameBoard[coord.vert][coord.horiz] = 1;
		blocksPerRow[coord.vert]++;

		// Update topRowWithBlock
		if (coord.vert < topRowWithBlock) {
			topRowWithBlock = coord.vert;
		}
	}
}

// Assumes the piece has already been placed on the board.
vector<int> Game::getRowsToClear() {
	vector<int> output;
	for (auto cell : currPiece.occupiedSpaces) {
		// Second condition prevents duplicates
		if (blocksPerRow[cell.vert] == BOARD_WIDTH
			&& find(output.begin(), output.end(), cell.vert) == output.end()) {
			output.push_back(cell.vert);
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


	// Prep the data
	int nClears = rowsToClear.size();
	sort(rowsToClear.begin(), rowsToClear.end(), greater<int>());

	// Start from the bottom and move up one layer at a time
	for (int rLower = BOTTOM_ROW_HEIGHT, cleared = 0;
		rLower >= 0;
		rLower--) {
		// If this row is to be cleared
		if (cleared < nClears
			&& rLower == rowsToClear[cleared]) {
			// Clear it
			gameBoard[rLower] = vector<int>(BOARD_WIDTH, 0);
			blocksPerRow[rLower] = 0;
			cleared++;
		}
		else {
			// Move the layer down by how many clears are done
			swap(gameBoard[rLower], gameBoard[rLower + cleared]);
			swap(blocksPerRow[rLower], blocksPerRow[rLower + cleared]);
		}
		
	}
}


// Assumes the movement is in order of 
// vertical, then horizontal, then rotational;
// Only moving on 1 axis per call to this function is strongly suggested
bool Game::moveAllowed(int dv, int dh, int turns) {
	vector<Coord> testCoords = currPiece.occupiedSpaces;
	
	// Make sure starting position is valid
	if (!checkValidPos(testCoords)) {
		return false;
	}

	// Check vertical movement validity
	for (int i = 0; i < dv; i++) {
		moveCoords(testCoords, 1, 0, 0);
		if (!checkValidPos(testCoords)) {
			return false;
		}
	}

	// Check horizontal movement validity (negative movement is possible)
	int dhSign = (2 * (dh >= 0)) - 1; // -1 if dh < 0 // 1 if dh >= 0
	for (int i = 0; i < abs(dh); i++) {
		moveCoords(testCoords, 0, dhSign, 0);
		if (!checkValidPos(testCoords)) {
			return false;
		}
	}

	// Check rotational movement validity
	for (int i = 0; i < (turns % 4); i++) {
		moveCoords(testCoords, 0, 0, 1);
		if (!checkValidPos(testCoords)) {
			return false;
		}
	}

	// Move is valid if no invalidity was detected
	return true;
}

// Returns true if the positions of the cells in testCoords are valid
// (On the board and empty)
bool Game::checkValidPos(vector<Coord>& testCoords) {
	for (auto& cell : testCoords) {
		if (cell.vert >= BOARD_HEIGHT || cell.vert < 0
			|| cell.horiz >= BOARD_WIDTH || cell.horiz < 0
			|| gameBoard[cell.vert][cell.horiz] != 0) {
			return false;
		}
	}

	return true;
}