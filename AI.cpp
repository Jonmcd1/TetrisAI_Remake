#include "AI.h"
using namespace std;

struct BfsCoords {
	int d;
	int v;
	int h;

	BfsCoords(int dI, int vI, int hI) : d(dI), v(vI), h(hI) {}
};

// FIXME: make sure topWithCell gets updated
// FIXME: I think I can save computation time by labeling impossible positions with 'X'
	// (So as to prevent repeated collision checks)
void AI::findDrops() {
	// TODO: Track which final state is best and backtrack to get
	// the input route to it

	BfsCoords currBest(0, 1, 6);
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

	// Load start pos into queue
	queue<BfsCoords> bfsQueue;

	if (topWithCell > 3) {
		currBest = { 0, topWithCell - 3, 6 };

		// Efficiency booster -- waste less time with unneeded paths
		bfsQueue.push(BfsCoords(0, topWithCell - 3, 6));
		bfsMap[0][topWithCell - 3][6] = 'S';
	}
	else {
		bfsQueue.push(BfsCoords(0, 1, 6));
		bfsMap[0][1][6] = 'S';
	}



	Piece& piece = game->currPiece;
	while (!bfsQueue.empty()) {
		BfsCoords& coords = bfsQueue.front();
		piece.newPiece(piece.pieceIdx, coords.d, coords.v, coords.h);

		if (!(game->moveVAllowed(1))) {
			double thisCost = evaluatePosition();
			// Lower cost is better
			if (thisCost < currBestCost) {
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


	/* TESTING */
	// print3DMap(bfsMap);



	// Find the route to the best position via backtracking
	// FIXME: The code below fails to account for S being offset down
	// FIXME: The code below fails to make the final waiting move to 
		// finalize the piece position
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
			currPos.d -= 1;
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
}

// TODO: Write
// Assumes piece is in position but hasn't been added to the board yet
double AI::evaluatePosition() {
	// Load in the piece
	for (auto coord : game->currPiece.occupiedSpaces) {
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
}

// TODO: Finalize
char AI::makeMove() {
	if (moves.empty()) {
		if (game->currPiece.occupiedSpaces[0].first != 1
			|| game->currPiece.occupiedSpaces[0].second != 6) {
			cout << "Finding drops for a piece not in the start position"
				<< " isn't allowed.\n";
			exit(1);
		}

		findDrops();
	}

	char move = moves.top();
	moves.pop();
	return move;
}

/*

AI::AI() {
	// Setting possibleMoveCombos and possibleMoveCombosLens
	for (int i = 48; i < POSSIBLE_MOVE_COUNT; i++) {
		possibleMoveCombos[i][possibleMoveCombosLens[i]] = 's';
		possibleMoveCombosLens[i]++;
	}
	for (int i = 0; i < POSSIBLE_MOVE_COUNT; i++) {
		for (int j = 0; j < (i % 4); j++) {
			possibleMoveCombos[i][possibleMoveCombosLens[i]] = 't';
			possibleMoveCombosLens[i]++;
		}
	}
	for (int i = 0; i < 28; i++) {
		for (int j = 0; j < (i / 4); j++) {
			possibleMoveCombos[i][possibleMoveCombosLens[i]] = 'l';
			possibleMoveCombosLens[i]++;
		}
	}
	for (int i = 28; i < 48; i++) {
		for (int j = 0; j < ((i / 4) - 6); j++) {
			possibleMoveCombos[i][possibleMoveCombosLens[i]] = 'r';
			possibleMoveCombosLens[i]++;
		}
	}
	for (int i = 48; i < 76; i++) {
		for (int j = 0; j < ((i - 48) / 4); j++) {
			possibleMoveCombos[i][possibleMoveCombosLens[i]] = 'l';
			possibleMoveCombosLens[i]++;
		}
	}
	for (int i = 76; i < POSSIBLE_MOVE_COUNT; i++) {
		for (int j = 0; j < (((i - 48)/ 4) - 6); j++) {
			possibleMoveCombos[i][possibleMoveCombosLens[i]] = 'r';
			possibleMoveCombosLens[i]++;
		}
	}

	// Setting genVals (Temporary)
	for (int i = 0; i < GENERATION_POPULATION; i++) {
		for (int j = 0; j < COST_FACTOR_COUNT; j++) {
			genVals[i][j] = 50;
		}

		/*
		genVals[i][0] = 0;
		genVals[i][1] = 0;
		genVals[i][2] = 0;
		genVals[i][3] = 0;
		genVals[i][4] = 0;
		genVals[i][5] = 0;
		genVals[i][6] = 0;
		genVals[i][7] = 0;
		genVals[i][8] = 0;
		genVals[i][9] = 0;
		*\/
		
		
		genVals[i][0] = 45.1949;
		genVals[i][1] = 169.689;
		genVals[i][2] = 50.3321;
		genVals[i][3] = 97.8057;
		genVals[i][4] = 42.116;
		genVals[i][5] = 6.58193;
		genVals[i][6] = 19.0269;
		genVals[i][7] = 19.3061;
		genVals[i][8] = 68.2907;
		genVals[i][9] = 78;
		
		
		
	}

	/*
	Best:
		genVals[i][0] = 45.1949;
		genVals[i][1] = 169.689;
		genVals[i][2] = 50.3321;
		genVals[i][3] = 97.8057;
		genVals[i][4] = 42.116;
		genVals[i][5] = 6.58193;
		genVals[i][6] = 19.0269;
		genVals[i][7] = 19.3061;
		genVals[i][8] = 68.2907;
		genVals[i][9] = 78;
	*\/

	// Scored 109400
	/*
	genVals[i][0] = 23.985;
	genVals[i][1] = 164.986;
	genVals[i][2] = 108.094;
	genVals[i][3] = 82.6877;
	genVals[i][4] = 43.1157;
	genVals[i][5] = 72.8097;
	genVals[i][6] = 14.6162;
	genVals[i][7] = 5.24222;
	genVals[i][8] = 37.807;
	genVals[i][9] = 113.075;
	*\/

	// 10000-test high scorer
	/*
	genVals[i][0] = 42.9394;
	genVals[i][1] = 169.083;
	genVals[i][2] = 56.2331;
	genVals[i][3] = 93.3902;
	genVals[i][4] = 40.3476;
	genVals[i][5] = 17.3001;
	genVals[i][6] = 22.2975;
	genVals[i][7] = 13.2417;
	genVals[i][8] = 61.6814;
	genVals[i][9] = 126.93;
	*\/
}

double AI::randomGen() {
	double tempRand = (rand() % 1000) * 1.0;

	if (tempRand < 700) {
		tempRand = tempRand / 7000;
	}
	else if (tempRand < 800) {
		tempRand = (tempRand / 1000) - 0.6;
	}
	else if (tempRand < 850) {
		tempRand = (tempRand / 500) - 1.4;
	}
	else if (tempRand < 885) {
		tempRand = (tempRand / 350) - 2.13;
	}
	else if (tempRand < 915) {
		tempRand = (tempRand / 300) - 2.55;
	}
	else if (tempRand < 940) {
		tempRand = (tempRand / 250) - 3.16;
	}
	else if (tempRand < 960) {
		tempRand = (tempRand / 200) - 4.1;
	}
	else if (tempRand < 975) {
		tempRand = (tempRand / 150) - 5.7;
	}
	else if (tempRand < 990) {
		tempRand = (tempRand / 150) - 5.7;
	}
	else {
		tempRand = (tempRand / 100) - 9;
	}

	return tempRand * (((rand() % 2) * 2) - 1);
}


void AI::createNewGen() {
	// Copying the current generation
	double currGenCopy[GENERATION_POPULATION][COST_FACTOR_COUNT] = {0};
	copyGenVals(currGenCopy);

	// Finding its highest scorers
	int bestList[GENERATION_POPULATION] = {0};
	findWinners(bestList);

	// Crossing every pair of winners including each winner paired with itself
	// This fills in genVals' individuals 0-35
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			
			// Decide if random variation will be thrown in
			int randomVarWeight = 0;
			int randomVarDecider = rand() % 26;
			if (randomVarDecider >= 20) {
				randomVarWeight = 1;
			}
			if (randomVarDecider >= 23) {
				randomVarWeight = 2;
			}
			if (randomVarDecider >= 25) {
				randomVarWeight = 3;
			}
			
			// For each weight in an individual's cost weight list
			for (int k = 0; k < COST_FACTOR_COUNT; k++) {
				// Combine the parent pair's weights in a randomized fashion with
				// random variation thrown in as well
				double parent1Weight = 0.5 + (randomGen() / 2);
				double parent2Weight = 1.0 - parent1Weight;
				genVals[(6 * i) + j][k] = parent1Weight * currGenCopy[bestList[i]][k]
										  + parent2Weight * currGenCopy[bestList[j]][k]
										  + 5.0 * randomVarWeight * randomGen();
				// Setting max/min for each cost weight
				if (genVals[(6 * i) + j][k] < 0) {
					genVals[(6 * i) + j][k] = 0;
				}
				if (genVals[(6 * i) + j][k] > MAX_COST_FACTOR) {
					genVals[(6 * i) + j][k] = MAX_COST_FACTOR;
				}
			}

		}
	}

	// Copies the winning 6 individuals into the next generation; they live
	// This fills in genVals' individuals 36-41
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < COST_FACTOR_COUNT; j++) {
			genVals[i + 36][j] = currGenCopy[bestList[i]][j];
		}
	}
	
	// Copies the best indiv 5 times, 2nd best 4 times, etc, and creates some variation of them
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < (5 - i); j++) {
			for (int k = 0; k < COST_FACTOR_COUNT; k++) {
				// Formula for current indiv index was derived from 
				// 42 + (15 - ((5 - i) + 1)(5 - i) / 2) + j
				int currIndivIndex = 57 + j - ((6 - i) * (5 - i) / 2);

				int randomVarDecider = rand() % 25;
				if (randomVarDecider < 21) {
					genVals[currIndivIndex][k] = currGenCopy[bestList[i]][k];
				}
				else if (randomVarDecider < 24) {
					genVals[currIndivIndex][k] = currGenCopy[bestList[i]][k] + 5.0 * randomGen();
				}
				else {
					genVals[currIndivIndex][k] = static_cast<double>(rand() % 20000) / 100;
				}
			}
		}
	}

	// Adds 3 totally random individuals into the next generation
	// This fills in the last 3 individuals in genVals
	for (int i = 57; i < GENERATION_POPULATION; i++) {
		for (int j = 0; j < COST_FACTOR_COUNT; j++) {
			genVals[i][j] = static_cast<double>(rand() % 20000) / 100;
		}
	}

	//cout genVals
	/*for (int i = 0; i < GENERATION_POPULATION; i++) {
		for (int j = 0; j < COST_FACTOR_COUNT; j++) {
			cout << genVals[i][j] << " ";
		}
		cout << endl;
	}

	//cout bestList
	cout << endl;
	for (int i = 0; i < 6; i++) {
		cout << bestList[i] << " ";
	}
	cout << endl;

	//cout genScores
	cout << endl;
	for (int i = 0; i < GENERATION_POPULATION; i++) {
		cout << genScores[i] << " ";
	}
	cout << endl << endl;*\/

	//cout highscore
	int highScore = 0;
	for (int i = 0; i < GENERATION_POPULATION; i++) {
		if (highScore < genScores[i]) {
			highScore = genScores[i];
		}
	}
	cout << endl << highScore << endl;

	return;
}


void AI::findWinners(int winnerList[GENERATION_POPULATION]) {
	int valsAdded = 0;
	for (int i = 0; i < GENERATION_POPULATION; i++) {
		int betterThanMe = 0;
		
		for (int j = 0; j < GENERATION_POPULATION; j++) {
			if (genScores[j] > genScores[i]) {
				betterThanMe++;
			}
		}

		if (betterThanMe <= 5) {
			winnerList[valsAdded] = i;
			valsAdded++;
		}
	}

	// Ordering the list (1st in list = highest scorer)
	int switches = 1;
	
	while (switches != 0) {
		switches = 0;
		for (int i = 0; i < 5; i++) {
			if (genScores[winnerList[i]] < genScores[winnerList[i + 1]]) {
				int temp = winnerList[i];
				winnerList[i] = winnerList[i + 1];
				winnerList[i + 1] = temp;
				switches++;
			}
		}
	}

}


void AI::saveGen(ofstream& outs, int genNum, int genWinners[6]) {
	outs << "GENERATION NUMBER: " << genNum << endl;
	
	outs << "Highscore: " << genScores[genWinners[0]] << endl;
	outs << "Highscore vals: ";
	for (int i = 0; i < COST_FACTOR_COUNT; i++) {
		outs << genVals[genWinners[0]][i] << " ";
	}
	outs << endl << "------------------------" << endl << endl;
	

	outs << "Generation Cost Values:" << endl;
	for (int i = 0; i < COST_FACTOR_COUNT; i++) {
		for (int j = 0; j < GENERATION_POPULATION; j++) {
			outs << genVals[j][i] << " ";
		}
		outs << endl;
	}
	outs << endl << "Generation Scores: " << endl;
	for (int i = 0; i < GENERATION_POPULATION; i++) {
		outs << genScores[i] << " ";
	}
	outs << endl << endl << "Generation Winners: " << endl;
	for (int i = 0; i < 6; i++) {
		outs << genWinners[i] << " ";
	}
	outs << endl << endl << endl;
}


void AI::copyGenVals(double genValsCopy[GENERATION_POPULATION][COST_FACTOR_COUNT]) {
	for (int i = 0; i < GENERATION_POPULATION; i++) {
		for (int j = 0; j < COST_FACTOR_COUNT; j++) {
			genValsCopy[i][j] = genVals[i][j];
		}
	}
}


double AI::evaluatePosition(Game game, double bumpCostW, double holeCostW, 
							double nonTetW, double tetW, double blocksOverHoleCostW,
							double blocksInRightLaneW, double topHeightW,
							double oneWideCostW, double holdIW, 
							double wasteIW, char pieceUsed, int tempTest) {

	double bumpCost = findBumpCost(game);
	double holeCost = findHoleCost(game);
	double nonTet = findNonTet(game);
	double tet = findTet(game);
	double blocksOverHoleCost = findBlocksOverHoleCost(game);
	double blocksInRightLane = findBlocksInRightLaneCost(game);
	double topHeight = findTopHeightCost(game);
	double oneWideCost = findOneWideCost(game);
	double holdI = findHoldI(game);
	double wasteI = findWasteI(game, pieceUsed);

	if (tempTest == 1) {
		cout << "bumpCost: " << bumpCost << endl;
		cout << "holeCost: " << holeCost << endl;
		cout << "nonTet: " << nonTet << endl;
		cout << "tet: " << tet << endl;
		cout << "blocksOverHoleCost: " << blocksOverHoleCost << endl;
		cout << "blocksInRightLane: " << blocksInRightLane << endl;
		cout << "topHeight: " << topHeight << endl;
		cout << "oneWideCost: " << oneWideCost << endl;
		cout << "holdI: " << holdI << endl;
		cout << "wasteI: " << wasteI << endl;
	}


	return (bumpCostW * bumpCost) + 
		   (holeCostW * holeCost) + 
		   (nonTetW * nonTet) - 
		   (tetW * tet) + 
		   (blocksOverHoleCostW * blocksOverHoleCost) +
		   (blocksInRightLaneW * blocksInRightLane) + 
		   (topHeightW * topHeight) +
		   (oneWideCostW * oneWideCost) -
		   (holdIW * holdI) +
		   (wasteIW * wasteI);
}


double AI::findBumpCost(Game game) {
	double currCost = 0;
	int topHeights[BOARD_WIDTH] = {0};
	assignTopHeights(topHeights, game);
	for (int i = 0; i < (BOARD_WIDTH - 2); i++) {
		currCost += abs(topHeights[i] - topHeights[i + 1]);
	}
	
	return currCost;
}


double AI::findHoleCost(Game game) {
	double currCost = 0;
	int topHeights[BOARD_WIDTH] = {0};
	assignTopHeights(topHeights, game);

	int gameBoardCopy[BOARD_HEIGHT][BOARD_WIDTH] = {0};
	game.assignGameBoard(gameBoardCopy);

	for (int i = 0; i < BOARD_WIDTH; i++) {
		for (int j = 0; j < topHeights[i]; j++) {
			if (gameBoardCopy[(BOARD_HEIGHT - 1) - j][i] == 0) {
				currCost++;
			}
		}
	}

	return currCost;
}


double AI::findNonTet(Game game) {
	int rowsCleared = game.getRowsJustCleared();
	if (rowsCleared > 0 && rowsCleared < 4) {
		return 1.0;
	}
	return 0.0;
}


double AI::findTet(Game game) {
	int rowsCleared = game.getRowsJustCleared();
	if (rowsCleared == 4) {
		return 1.0;
	}
	return 0.0;
}


double AI::findBlocksOverHoleCost(Game game) {
	int copyBoard[24][12];
	game.assignGameBoard(copyBoard);
	
	double sumTilesOverAir = 0;
	for (int i = 0; i < BOARD_WIDTH; i++) {
		int airTileBelow = 0;
		for (int j = (BOARD_HEIGHT - 1); j >= 0; j--) {
			if (copyBoard[j][i] == 0) {
				airTileBelow = 1;
			}
			else {
				sumTilesOverAir += airTileBelow;
			}
		}
	}

	return sumTilesOverAir;
}


double AI::findBlocksInRightLaneCost(Game game) {
	int copyBoard[BOARD_HEIGHT][BOARD_WIDTH];
	game.assignGameBoard(copyBoard);

	double rightLaneBlockCount = 0;
	for (int i = 0; i < BOARD_HEIGHT; i++) {
		if (copyBoard[i][BOARD_WIDTH - 1] != 0) {
			rightLaneBlockCount++;
		}
	}

	return rightLaneBlockCount;
}


double AI::findOneWideCost(Game game) {
	double currCost = 0;
	int topHeights[BOARD_WIDTH] = {0};
	assignTopHeights(topHeights, game);

	for (int i = 0; i < 11; i++) {
		if (i == 0) {
			// check the col on the right
			if (topHeights[i + 1] - topHeights[i] > 2) {
				int diffLevel = topHeights[i + 1] - topHeights[i];
				currCost += diffLevel * diffLevel;
			}
		}
		else if (i == 10) {
			// check the col on the left
			if (topHeights[i - 1] - topHeights[i] > 2) {
				int diffLevel = topHeights[i - 1] - topHeights[i];
				currCost += diffLevel * diffLevel;
			}
		}
		else {
			// check the cols on each side
			if (topHeights[i + 1] - topHeights[i] > 2
				&& topHeights[i - 1] - topHeights[i] > 2) {
				int diffLevel = topHeights[i + 1] - topHeights[i];
				if (topHeights[i - 1] - topHeights[i] < diffLevel) {
					diffLevel = topHeights[i - 1] - topHeights[i];
				}
				currCost += diffLevel * diffLevel;
			}
		}
	}

	return currCost;
}


double AI::findHoldI(Game game) {
	if (game.getHeldPiece().getPieceChar() == 'I') {
		return 1.0;
	}
	return 0.0;
}


double AI::findWasteI(Game game, char pieceUsed) {
	if (pieceUsed == 'I' && game.getRowsJustCleared() != 4) {
		return 1.0;
	}
	return 0.0;
}


double AI::findTopHeightCost(Game game) {
	int currTopHeight = 0;
	
	int topHeights[BOARD_WIDTH] = {0};
	assignTopHeights(topHeights, game);

	for (int i = 0; i < BOARD_WIDTH; i++) {
		if (topHeights[i] > currTopHeight) {
			currTopHeight = topHeights[i];
		}
	}

	if (currTopHeight > 8) {
		currTopHeight -= 8;
		return currTopHeight * currTopHeight;
	}
	return 0;
}




int AI::findBestMove(Game game, int currIndivIndex) {
	int currBestMoveCombo = -1;
	double currLowestCost = 10000000;

	// Checks all possible hypothetical moves
	for (int i = 0; i < POSSIBLE_MOVE_COUNT; i++) {
		// Setting the hypothetical game for the hypothetical move
		Game hypGame;
		hypGame.setGame(game);
		// Making the hypothetical move
		hypGame.setMoveEnded(false);
		// Getting the kind of piece used
		char pieceUsed;
		if (possibleMoveCombosLens[i] == 0 || possibleMoveCombos[i][0] != 's') {
			pieceUsed = hypGame.getPiece().getPieceChar();
		}
		else {
			pieceUsed = hypGame.getHeldPiece().getPieceChar();
		}
		// Making the hypothetical move
		for (int j = 0; j < possibleMoveCombosLens[i]; j++) {

			if (hypGame.getMoveValid() && !hypGame.getMoveEnded()) {
				hypGame.gameTick(1, 1, possibleMoveCombos[i][j]);
			}
		}
		while (!hypGame.getMoveEnded()) {
			hypGame.gameTick(1, 1, ' ');
		}

		// Scoring the hypothetical position
		hypGame.takePieceOffBoard();

		int tester = 0;
		if (i == 0) {
			//tester = 1;
		}
		double hypMoveScore = evaluatePosition(hypGame, genVals[currIndivIndex][0], 
														genVals[currIndivIndex][1], 
														genVals[currIndivIndex][2], 
														genVals[currIndivIndex][3],
														genVals[currIndivIndex][4],
														genVals[currIndivIndex][5],
														genVals[currIndivIndex][6],
														genVals[currIndivIndex][7],
														genVals[currIndivIndex][8],
														genVals[currIndivIndex][9],
														pieceUsed, tester);
		hypGame.putPieceOnBoard();
		if (hypMoveScore < currLowestCost) {
			currLowestCost = hypMoveScore;
			currBestMoveCombo = i;
		}
	}

	return currBestMoveCombo;
}

// Note: make sure there is no active piece when this function is run
void AI::assignTopHeights(int topHeights[BOARD_WIDTH], Game game) {
	int gameBoardCopy[BOARD_HEIGHT][BOARD_WIDTH] = {0};
	game.assignGameBoard(gameBoardCopy);

	for (int i = 0; i < BOARD_WIDTH; i++) {
		for (int j = (BOARD_HEIGHT - 1); j >= 0; j--) {
			if (gameBoardCopy[j][i] > 0) {
				topHeights[i] = BOARD_HEIGHT - j;
			}
		}
	}

	return;
}

void AI::assignPossibleMoveCombos(char possMoves[POSSIBLE_MOVE_COUNT][MAX_MOVE_LENGTH]) {
	for (int i = 0; i < POSSIBLE_MOVE_COUNT; i++) {
		for (int j = 0; j < MAX_MOVE_LENGTH; j++) {
			possMoves[i][j] = possibleMoveCombos[i][j];
		}
	}
	return;
}

void AI::assignPossibleMoveCombosLens(int possMovesLens[POSSIBLE_MOVE_COUNT]) {
	for (int i = 0; i < POSSIBLE_MOVE_COUNT; i++) {
		possMovesLens[i] = possibleMoveCombosLens[i];
	}
	return;
}

double AI::getAGenVal(int i, int j) {
	return genVals[i][j];
}

void AI::setScoreVal(int score, int currIndivIndex) {
	genScores[currIndivIndex] = score;
	return;
}

void AI::resetScores() {
	for (int i = 0; i < GENERATION_POPULATION; i++) {
		genScores[i] = 0;
	}
}

*/