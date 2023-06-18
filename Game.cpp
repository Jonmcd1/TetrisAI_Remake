#include "Game.h"

using namespace std;

/*** INITIALIZERS ***/
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
	piecesPlaced = 0;

	/* TESTING */
	/*
	// Allows gameBoard to be manually initialized
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


/*** DISPLAY FUNCTION ***/
void Game::print(bool alreadyOnBoard) {
	// Update board if necessary
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

	// Print held piece
	cout << "\nHeld Piece: " << heldPiece.pieceChar << "\n";

	// Print top board-border
	for (int i = 0; i < BOARD_WIDTH + 2; i++) {
		cout << "#";
	}
	cout << "\n";

	// Print each row of board (bordered)
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

	// Print bottom board-border
	for (int i = 0; i < BOARD_WIDTH + 2; i++) {
		cout << "#";
	}

	// Print current score
	cout << "\nScore: " << userScore << "\n";

	// Reverse board update if necessary
	if (!alreadyOnBoard) {
		for (auto coord : currPiece.occupiedSpaces) {
			gameBoard[coord.vert][coord.horiz] = 0;
		}
	}

	return;
}


/*** PIECE PLACEMENT FUNCTIONS ***/
void Game::placePiece() {
	piecesPlaced++;

	for (auto coord : currPiece.occupiedSpaces) {
		// Sanity check
		if (gameBoard[coord.vert][coord.horiz] == 1) {
			cout << "Loaded piece on occupied square. Invalid.\n";
			exit(1);
		}
		if (blocksPerRow[coord.vert] == BOARD_WIDTH) {
			cout << "Loaded piece on full row. Invalid.\n";
			exit(1);
		}

		// Place the cell
		gameBoard[coord.vert][coord.horiz] = 1;

		// Update efficiency data
		blocksPerRow[coord.vert]++;
		if (coord.vert < topRowWithBlock) {
			topRowWithBlock = coord.vert;
		}
	}
}

void Game::processFilledLines() {
	// Get rows that are full, then clear them
	vector<int> rClear = getRowsToClear();
	clearRows(rClear);

	userScore += rClear.size() * rClear.size() * 100;
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
		// Otherwise
		else {
			// Move the layer down to its final position 
			// (AKA by how many clears have been done)
			swap(gameBoard[rLower], gameBoard[rLower + cleared]);
			swap(blocksPerRow[rLower], blocksPerRow[rLower + cleared]);
		}

	}
}

vector<int> Game::getRowsToClear() {
	vector<int> output;
	// Only considers currPiece.occupiedSpaces so as to avoid checking rows
	// that haven't been modified lately (efficiency)
	for (auto cell : currPiece.occupiedSpaces) {
		// Second condition prevents duplicates
		if (blocksPerRow[cell.vert] == BOARD_WIDTH
			&& find(output.begin(), output.end(), cell.vert) == output.end()) {
			output.push_back(cell.vert);
		}
	}
	return output;
}


/*** MOVE VALIDITY CHECKERS ***/
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

	// The move is valid if no invalidity was detected
	return true;
}

bool Game::swapAllowed() {
	// Simply collision checks the held-piece starting position
	vector<Coord> testCoords = heldPiece.occupiedSpaces;
	return checkValidPos(testCoords);
}

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