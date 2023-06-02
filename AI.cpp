#include "AI.h"
using namespace std;

char AI::makeMove() {
	// This means a piece has been placed
	if (moves.empty()) {
		// Sanity check
		if (game->currPiece.occupiedSpaces[0].vert != 1
			|| game->currPiece.occupiedSpaces[0].horiz != 6) {
			cout << "Finding drops for a piece not in the start position"
				<< " isn't allowed.\n";
			exit(1);
		}

		// Find a new move sequence for the new piece
		getMovePath();
	}

	char move = moves.top();
	moves.pop();
	return move;
}


// makeMove() helper
void AI::getMovePath() {
	// Find path and cost without swapping pieces
	stack<char> pathWithoutSwap;
	double costWithoutSwap = findBestDrop(pathWithoutSwap);

	// Find path and cost with swapping pieces
	swap(game->currPiece, game->heldPiece);
	stack<char> pathWithSwap;
	double costWithSwap = findBestDrop(pathWithSwap);
	pathWithSwap.push('S');
	swap(game->currPiece, game->heldPiece);

	// Set the moves path
	if (costWithoutSwap < costWithSwap) {
		moves = pathWithoutSwap;
	}
	else {
		moves = pathWithSwap;
	}
}

// findDrops() helper
double AI::findBestDrop(stack<char>& pathOfDrop) {
	// max(topWithCell - 3, 1) is efficiency booster -- avoids unneeded paths
	int startHeight = max(topWithCell - 3, 1);

	// Create starting "best" positions -- filler values to be replaced
	BfsCoords currBest = { 0, startHeight, 6 };
	double currBestCost = std::numeric_limits<double>::infinity();

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

	// Create search queue for the map above
	queue<BfsCoords> bfsQueue;

	// Load start pos into search queue and map
	bfsQueue.push(BfsCoords(0, startHeight, 6));
	bfsMap[0][startHeight][6] = 'S';



	Piece& piece = game->currPiece;
	while (!bfsQueue.empty()) {
		BfsCoords& coords = bfsQueue.front();

		// Set piece to specified position on the map
		piece.newPiece(piece.pieceIdx, coords.d, coords.v, coords.h);

		// If the current position is a possible final position for the piece
		if (!(game->moveAllowed(1, 0, 0))) {
			// Put the piece on the board
			game->placePiece();
			vector<int> clears = game->getRowsToClear();
			game->clearRows(clears);

			// Evaluate the position
			int rowsCleared = clears.size();
			double thisCost = evaluatePosition(rowsCleared);

			// Remove the piece from the board
			fillRows(clears);
			removePiece();

			// See if this position is better than all previously found ones
			if (thisCost <= currBestCost) {
				// Save it if so
				currBestCost = thisCost;
				currBest = coords;
			}
		}

		// Test all possible moves from this position
		for (auto& move : moveSet) {
			BfsCoords& moveCoords = move.second;

			BfsCoords newCoords = {
				(coords.d + moveCoords.d) % 4,
				coords.v + moveCoords.v,
				coords.h + moveCoords.h
			};

			if (game->moveAllowed(moveCoords.v, moveCoords.h, moveCoords.d)
				&& bfsMap[newCoords.d][newCoords.v][newCoords.h] == '-') {
				bfsQueue.push({ newCoords.d, newCoords.v, newCoords.h });
				bfsMap[newCoords.d][newCoords.v][newCoords.h] = move.first;
			}
		}


		bfsQueue.pop();
	}

	// Reset the piece after testing
	piece.newPiece(piece.pieceIdx, 0, 1, 6);


	// Find the path to the best drop spot
	backtrackPath(bfsMap, currBest, startHeight, pathOfDrop);

	// Return the cost of the best drop spot
	return currBestCost;
}

// findBestDrop() helper
void AI::backtrackPath(vector<vector<vector<char>>>& bfsMap, 
						BfsCoords& end, int startHeight,
						stack<char>& path) {
	// Final waiting move to trigger piece placement
	path.push('W');

	// Find the route to the best position via backtracking
	BfsCoords currPos = end;
	while (true) {
		char currChar = bfsMap[currPos.d][currPos.v][currPos.h];

		if (currChar == 'S') {
			break;
		}

		path.push(currChar);

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
		path.push('D');
		startOffset -= 2;
	}
	if (startOffset == 1) {
		path.push('W');
	}
}



// findBestDrop() helper
// TODO: Write
double AI::evaluatePosition(int rowsCleared) {
	/* Factors to weigh:
	* top height
	* bumpiness (height diff between each column)
	* # of holes
	* # of tiles above holes
	* # of lines cleared
	* # of cells in "holes" (bumps) that are 1 cell wide (hard to clear)
	* I-block held or not
	*/
	vector<int> features(AI_FEATURE_COUNT, 0);

	// Helpful names
	int& topHeight = features[0];
	int& numLinesCleared = features[1];
	int& tetrisClear = features[2];
	int& heldIBlock = features[3];
	int& totalHoles = features[4];
	int& totalBlocksAboveHoles = features[5];
	int& ditchCount = features[6];
	int& bumpCount = features[7];

	/* 
	Note: A "ditch" is a column with a lower height than the
	adjacent columns. It's hard to fill in. ditchCount tracks 
	the total number of cells in ditches.

	Example:
	@ @
	@ @
	@@@

	is a ditch with 2 cells in it.


	Note: The rightmost column is immune to ditch and bump costs.
	This is to allow for I-block stacking on the right.
	*/


	// COMPLETES: topHeight, numLinesCleared, tetrisClear, heldIBlock
	// Handles trivial features
	topHeight = topWithCell;
	numLinesCleared = rowsCleared;
	// Cost reducers since these are good
	tetrisClear = (rowsCleared == 4) * -1;
	heldIBlock = (game->heldPiece.pieceChar == 'I') * -1;


	// +2 is so the edges can be filled with max height border cols
	vector<int> tops(BOARD_WIDTH + 2, BOARD_HEIGHT);

	// COMPLETES: totalHoles, totalBlocksAboveHoles
	// Tracks top height for each col
	for (int col = 0; col < BOARD_WIDTH; col++) {
		int emptiesSoFar = 0;
		int nHoles = 0;
		int nBlocksAboveHoles = 0;

		for (int row = BOTTOM_ROW_HEIGHT; row >= topWithCell; row--) {
			if (board[row][col] == 0) {
				emptiesSoFar++;
			}
			else {
				nHoles = emptiesSoFar;
				nBlocksAboveHoles += emptiesSoFar;
				tops[col] = row;
			}
		}

		totalHoles += nHoles;
		totalBlocksAboveHoles += nBlocksAboveHoles;
	}

	// COMPLETES: ditchCount, bumpCount
	// Indices are a bit weird due to the borders/right-edge immunity
	for (int idx = 1; idx < BOARD_WIDTH; idx++) {
		bumpCount += abs(tops[idx] - tops[idx - 1]);

		// Finding if there is a ditch
		int lowerAdj = min(tops[idx - 1], tops[idx + 1]);
		if (lowerAdj > tops[idx]) {
			ditchCount += lowerAdj - tops[idx];
		}
	}
	// I don't really want to count the left edge's diff from
	// BOARD_HEIGHT towards the bump cost
	bumpCount -= abs(tops[1] - tops[0]);


	// Calculate the final cost
	double cost = 0;
	for (int i = 0; i < AI_FEATURE_COUNT; i++) {
		cost += features[i] * weights[i];
	}

	return cost;
}

// evaluatePosition() helper; reverses a piece drop
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

// evaluatePosition() helper; reverses a piece drop
void AI::removePiece() {
	// Debugging
	game->piecesPlaced--;

	for (auto coord : game->currPiece.occupiedSpaces) {
		// Sanity check
		if (board[coord.vert][coord.horiz] == 0) {
			cout << "Unloaded piece on empty square. Invalid.\n";
			exit(1);
		}
		if (blocksPerRow[coord.vert] == 0) {
			cout << "Unloaded piece on empty row. Invalid.\n";
			exit(1);
		}

		board[coord.vert][coord.horiz] = 0;
		blocksPerRow[coord.vert]--;
	}


	while (topWithCell < BOARD_HEIGHT
			&& blocksPerRow[topWithCell] == 0) {
		topWithCell++;
	}
}

