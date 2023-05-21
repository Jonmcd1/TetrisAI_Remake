#include "AI.h"
using namespace std;

struct BfsCoords {
	int d;
	int v;
	int h;

	BfsCoords(int dI, int vI, int hI) : d(dI), v(vI), h(hI) {}
};


// FIXME: I think I can save computation time by labeling impossible positions with 'X'
	// (So as to prevent repeated collision checks)
void AI::findDrops() {
	/* Map key: 
	* S is "start"
	* R is "got here by moving right" 
	* L is "got here by moving left"
	* T is "got here by tilting"
	* D is "got here by moving down"
	* W is "got here by waiting"
	*/
	// [Depth][Vert][Horiz]
	// V and H match the V and H of the gameBoard
	// D is the current rotation of the piece, from 0 - 3
	vector<vector<vector<char>>> bfsMap(4, 
		vector<vector<char>>(BOARD_HEIGHT, vector<char>(BOARD_WIDTH, '-')));

	queue<BfsCoords> bfsQueue;

	// max(topWithCell - 3, 1) is efficiency booster -- avoids unneeded paths
	int startHeight = max(topWithCell - 3, 1);

	BfsCoords currBest = { 0, startHeight, 6 };
	double currBestCost = std::numeric_limits<double>::infinity();

	bfsQueue.push(BfsCoords(0, startHeight, 6)); // Load start pos into queue
	bfsMap[0][startHeight][6] = 'S';



	Piece& piece = game->currPiece;
	while (!bfsQueue.empty()) {
		BfsCoords& coords = bfsQueue.front();
		piece.newPiece(piece.pieceIdx, coords.d, coords.v, coords.h);

		if (!(game->moveVAllowed(1))) {

			double thisCost = evaluatePosition();

			// Lower cost is better
			if (thisCost <= currBestCost) {
				currBestCost = thisCost;
				currBest = coords;
			}
		}

		// R
		if (game->moveHAllowed(1)
			&& (coords.h + 1) < BOARD_WIDTH
			&& bfsMap[coords.d][coords.v][coords.h + 1] == '-') {
			bfsQueue.push({ coords.d, coords.v, coords.h + 1 });
			bfsMap[coords.d][coords.v][coords.h + 1] = 'R';
		}
		// L
		if (game->moveHAllowed(-1) 
			&& (coords.h - 1) >= 0
			&& bfsMap[coords.d][coords.v][coords.h - 1] == '-') {
			bfsQueue.push({ coords.d, coords.v, coords.h - 1 });
			bfsMap[coords.d][coords.v][coords.h - 1] = 'L';
		}
		// T
		if (game->moveRAllowed(1) 
			&& bfsMap[(coords.d + 1) % 4][coords.v][coords.h] == '-') {
			bfsQueue.push({ (coords.d + 1) % 4, coords.v, coords.h });
			bfsMap[(coords.d + 1) % 4][coords.v][coords.h] = 'T';
		}
		// D
		if (game->moveVAllowed(1)
			&& game->moveVAllowed(2) 
			&& (coords.v + 2) < BOARD_HEIGHT
			&& bfsMap[coords.d][coords.v + 2][coords.h] == '-') {
			bfsQueue.push({ coords.d, coords.v + 2, coords.h });
			bfsMap[coords.d][coords.v + 2][coords.h] = 'D';
		}
		// Waiting
		if (game->moveVAllowed(1) 
			&& (coords.v + 1) < BOARD_HEIGHT
			&& bfsMap[coords.d][coords.v + 1][coords.h] == '-') {
			bfsQueue.push({ coords.d, coords.v + 1, coords.h });
			bfsMap[coords.d][coords.v + 1][coords.h] = 'W';
		}

		bfsQueue.pop();
	}
	// Put it back after testing
	piece.newPiece(piece.pieceIdx, 0, 1, 6);


	// Final waiting move to trigger piece placement
	moves.push('W');

	// Find the route to the best position via backtracking
	BfsCoords currPos = currBest;
	while (true) {
		char& currChar = bfsMap[currPos.d][currPos.v][currPos.h];

		if (currChar == 'S') {
			break;
		}

		moves.push(currChar);

		if (currChar == 'R') {
			currPos.h -= 1;
		}
		else if (currChar == 'L') {
			currPos.h += 1;
		}
		else if (currChar == 'T') {
			// Subtracting 1, but not going negative
			currPos.d = (currPos.d + 3) % 4;
		}
		else if (currChar == 'D') {
			currPos.v -= 2;
		}
		else if (currChar == 'W') {
			currPos.v -= 1;
		}
		else {
			cout << "This is impossible, how did you get here?\n";
			exit(1);
		}
	}
	
	// Accounts for the start offset in the move stack
	int startOffset = startHeight - 1;
	while (startOffset > 1) {
		moves.push('D');
		startOffset -= 2;
	}
	if (startOffset == 1) {
		moves.push('W');
	}
}

// TODO: Write
// Assumes piece is in position but hasn't been added to the board yet
double AI::evaluatePosition() {
	// Load in the piece
	for (auto coord : game->currPiece.occupiedSpaces) {
		// Sanity check
		if (board[coord.first][coord.second] == 1) {
			cout << "Loaded piece on occupied square. Invalid.\n";
			exit(1);
		}
		if (blocksPerRow[coord.first] == BOARD_WIDTH) {
			cout << "Loaded piece on full row. Invalid.\n";
			exit(1);
		}

		board[coord.first][coord.second] = 1;
		blocksPerRow[coord.first]++;
	}
	vector<int> clears = game->getRowsToClear();
	game->clearRows(clears);


	int rowsCleared = clears.size();
	// TODO: eval

	/* Factors to weigh:
	* top height
	* bumpiness (height diff between each column)
	* # of holes
	* # of tiles above holes
	* # of lines cleared
	* # of cells in "holes" (bumps) that are 1 cell wide (hard to clear)
	* I-block held or not
	*/


	
	// Unload the piece
	fillRows(clears);
	for (auto coord : game->currPiece.occupiedSpaces) {
		// Sanity check
		if (board[coord.first][coord.second] == 0) {
			cout << "Unloaded piece on empty square. Invalid.\n";
			exit(1);
		}
		if (blocksPerRow[coord.first] == 0) {
			cout << "Unloaded piece on empty row. Invalid.\n";
			exit(1);
		}

		board[coord.first][coord.second] = 0;
		blocksPerRow[coord.first]--;
	}

	return 0.0; // Filler
}

void AI::fillRows(vector<int>& rowsToFill) {
	// Some basics
	if (rowsToFill.empty()) {
		return;
	}
	sort(rowsToFill.begin(), rowsToFill.end());

	// Go down the board from the top until all rows to fill have been filled
	// (All rows beyond those can stay unchanged)
	int fillCount = rowsToFill.size();
	for (int rHigher = 0, filled = 0;
		filled < fillCount;
		rHigher++) {
		// If we're currently at a row to be filled
		if (rHigher == rowsToFill[filled]) {
			// Fill it
			board[rHigher] = vector<int>(BOARD_WIDTH, 1);
			blocksPerRow[rHigher] = BOARD_WIDTH;
			filled++;
		}
		// Otherwise, bring the row that is X rows down up to the current row
		// X is the number of rows that have yet to be filled
		else {
			swap(board[rHigher], board[rHigher + (fillCount - filled)]);
			swap(blocksPerRow[rHigher], blocksPerRow[rHigher + (fillCount - filled)]);
		}
	}

	// Update topWithCell
	topWithCell -= fillCount;
}

// TODO: Finalize
char AI::makeMove() {
	// This means a piece has been placed
	if (moves.empty()) {
		// Sanity check
		if (game->currPiece.occupiedSpaces[0].first != 1
			|| game->currPiece.occupiedSpaces[0].second != 6) {
			cout << "Finding drops for a piece not in the start position"
				<< " isn't allowed.\n";
			exit(1);
		}

		// Find a new move sequence for the new piece
		findDrops();
	}

	char move = moves.top();
	moves.pop();
	return move;
}

